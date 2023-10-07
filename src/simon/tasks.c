#include <stddef.h>
#include "tasks.h"
#include "systick.h"
#include "ch32v003fun.h"

TS_LOG_TAG("Tasks");

typedef void (*task_fn)();

#define TASK_FLAG_STARTED 0x01
#define TASK_FLAG_BLOCKED 0x02
#define TASK_FLAG_WAITING 0x04
#define TASK_FLAG_TIMER_TIMEOUT 0x06

//#define DEBUG_CONTEXT_SWITCH

typedef struct {
    uint32_t flags;
    uint32_t waiting_from;
    uint32_t sleep_delay;
    void* current_stack;
} TuxSays_Task_State;

struct TuxSays_Task_Handle {
    TuxSays_Task_State* state;
    void* stack_start;
    void* stack_end;
    uint8_t priority;
};

static volatile TuxSays_Task_Handle* current_task = NULL;

#define TASK(_name, _fn, _priority, _stack_size)                                             \
    void task_##_fn##_run();                                                                 \
    static uint32_t task_##_name##_stack[_stack_size / 4] __attribute__((aligned(64)));      \
    static TuxSays_Task_State task_##_name##_state = {                                       \
        .flags = 0,                                                                          \
        .waiting_from = 0,                                                                   \
        .sleep_delay = 0,                                                                    \
    };                                                                                       \
    const TuxSays_Task_Handle task_##_name = {                                               \
        .state = &task_##_name##_state,                                                      \
        .stack_start = task_##_name##_stack,                                                 \
        .stack_end =                                                                         \
            ((void*)task_##_name##_stack) + sizeof(task_##_name##_stack) - sizeof(uint32_t), \
        .priority = _priority,                                                               \
    };
#include "task_list_config.h"
#undef TASK

const TuxSays_Task_Handle* task_list[] = {
#define TASK(_name, _fn, _priority, _stack_size) &task_##_name,
#include "task_list_config.h"
#undef TASK
};

static void __attribute__((naked)) switch_context(void** old_ctx, void** new_ctx) {
    asm volatile(".option push \n"
                 ".option pop \n"
                 ".option norelax \n"
#if __GNUC__ > 10
                 ".option arch, +zicsr\n"
#endif
                 "    add	sp, sp,-56 \n"
                 "    sw    t0, 44(sp) \n" // store t0 to stack

                 "    csrr  t0, mstatus \n" // load IRQ state
                 "    and   t0, t0, 0x77 \n" // mask mstatus to disable IRQ
                 "    csrw  mstatus, t0 \n" // disable IRQ

                 "store_old_ctx: \n"

                 "    sw    sp, 0(%[old]) \n" // store current stack pointer

                 "    sw    ra, 16(sp) \n"
                 "    sw    gp, 12(sp) \n"
                 "    sw    tp, 8(sp) \n" // todo set proper thread pointer

                 "    sw    s0, 4(sp) \n"
                 "    sw    s1, 0(sp) \n"

                 "    sw    a5, 20(sp) \n"
                 "    sw    a4, 24(sp) \n"
                 "    sw    a3, 28(sp) \n"
                 "    sw    a2, 32(sp) \n"
                 "    sw    a1, 36(sp) \n"
                 "    sw    a0, 40(sp) \n"

                 "    sw    t1, 48(sp) \n"
                 "    sw    t2, 52(sp) \n"

                 "restore_new_ctx: \n"

                 "    lw    sp, 0(%[new]) \n" // store current stack pointer

                 "    lw    a5, 20(sp) \n"
                 "    lw    a4, 24(sp) \n"
                 "    lw    a3, 28(sp) \n"
                 "    lw    a2, 32(sp) \n"
                 "    lw    a1, 36(sp) \n"
                 "    lw    a0, 40(sp) \n"

                 "    lw    ra, 16(sp) \n"
                 "    lw    gp, 12(sp) \n"
                 "    lw    tp, 8(sp) \n" // todo set proper thread pointer

                 "    lw    s0, 4(sp) \n"
                 "    lw    s1, 0(sp) \n"

                 "    lw    t1, 48(sp) \n"
                 "    lw    t2, 52(sp) \n"

                 "    csrr  t0, mstatus \n"
                 "    or	t0, t0, 0x88 \n"
                 "    csrw  mstatus, t0 \n"

                 "    lw    t0, 44(sp) \n" // store t0 to stack
                 "    add	sp, sp,56 \n"

                 "ret \n"

                 :
                 : [new] "r"(new_ctx), [old] "r"(old_ctx)
                 : "t0");

    //TS_LOG_I("%08X", (unsigned int)old_task->state->current_stack_ptr);
}

static void prepare_task_for_start(const TuxSays_Task_Handle* handle, task_fn fn) {
    asm volatile(".option push \n"
                 ".option pop \n"
                 ".option norelax \n"

                 "prepare_ctx: \n"
                 "    lw    a0, 0(%[stack_end]) \n" // load stack space
                 "    add   a0, a0,-56 \n"
                 "    sw    a0, 0(%[curr]) \n" // store current stack pointer

                 "    sw    %[fn], 16(a0) \n" // store fn callback to sp-20
                 "    sw    gp, 12(a0) \n" // store global pointer to sp-16
                 "    sw    %[task], 8(a0) \n" // store thread pointer to sp-12

                 "    sw    zero, 4(a0) \n" // store s0 as zero
                 "    sw    zero, 0(a0) \n" // store s1 as zero
                 //"    sw    zero, 20(a0) \n"

                 :
                 : [fn] "r"(fn),
                   [stack_end] "r"(&handle->stack_end),
                   [task] "r"(handle),
                   [curr] "r"(&handle->state->current_stack)
                 : "a0");
}

#ifdef DEBUG_CONTEXT_SWITCH
static void dump_stack(TuxSays_Task_Handle* task) {
    TS_LOG_D("dump task  0x%08X stack", task);
    TS_LOG_D(" start = 0x%08X", task->stack_start);
    TS_LOG_D(" end = 0x%08X", task->stack_end);
    TS_LOG_D(" current = 0x%08X", task->state->current_stack);
    TS_LOG_D("");
    uint32_t* tmp = (uint32_t*)task->stack_end;
    for(uint16_t i = 0; i < 20; i++) {
        TS_LOG_D(" %c %08X > 0x%08X", task->state->current_stack == tmp ? '>' : ' ', tmp, *tmp);
        tmp--;
    }
}
#endif

static void fill_stack(const TuxSays_Task_Handle* task) {
    uint32_t* tmp = task->stack_start;
    while((uint32_t*)task->stack_end >= tmp) {
        *tmp = 0xDEADBEEF;
        tmp++;
    }
}

#if defined(TS_LOG_DEFALUT) && TS_LOG_DEFALUT
static uint32_t check_free_stack(const TuxSays_Task_Handle* task) {
    uint32_t* tmp = task->stack_start;
    uint32_t size = 0;
    while((uint32_t*)task->stack_end >= tmp) {
        if(*tmp != 0xDEADBEEF) {
            break;
        }
        tmp++;
        size += 4;
    }

    return size;
}
#endif

TuxSays_Error schedule() {
    TuxSays_Task_Handle* next = NULL;
    //TS_LOG_I("schedule");
    // --------------------------
    // ---- find next task ----

    uint32_t ms = TuxSays_SysTick_Milis();
    for(uint8_t id = 0; id < TuxSays_Task_Count - 1; id++) {
        TuxSays_Task_State* state = task_list[id]->state;

#ifdef DEBUG_CONTEXT_SWITCH
        TS_LOG_I(
            "loop task #%d (0x%08X) flags=0x%08X waiting_from=%lu sleep_delay=%lu stack_end=0x%08X current_stack=0x%08X",
            id,
            task_list[id],
            task_list[id]->state->flags,
            task_list[id]->state->waiting_from,
            task_list[id]->state->sleep_delay,
            task_list[id]->stack_end,
            task_list[id]->state->current_stack);
#endif
        // ignore blocked tasks
        if((state->flags & TASK_FLAG_BLOCKED) != 0) {
            continue;
        }

        // check waiting tasks
        if((state->flags & TASK_FLAG_WAITING) != 0) {
            if((ms - state->waiting_from) > state->sleep_delay) {
                state->flags &= ~TASK_FLAG_WAITING;
                state->flags |= TASK_FLAG_TIMER_TIMEOUT;
                state->waiting_from = 0;
                state->sleep_delay = 0;
            } else {
                continue;
            }
        }

        // use at least one task
        if(next == NULL) {
            next = (TuxSays_Task_Handle*)task_list[id];
            continue;
        }

        // try find task with higher priority (0 is most high prio, 255 is most low prio)
        if(task_list[id]->priority < next->priority) {
            next = (TuxSays_Task_Handle*)task_list[id];
        }
    }

    // --------------------------
    // ---- switch tasks ----

    // no any pending thread found => idle task missing or is blocked
    if(next == NULL) {
        // TODO: maybe go to hardfault
        return TuxSays_Error_NoPendingTask;
    }
#ifdef DEBUG_CONTEXT_SWITCH
    TS_LOG_I("switch to task (0x%08X)", next);
    dump_stack(next);
#endif

    // mark task as started
    next->state->flags |= TASK_FLAG_STARTED;

    // replace current_task
    TuxSays_Task_Handle* old = (TuxSays_Task_Handle*)current_task;
    current_task = next;

    // switch context to new task
    if(old != NULL) {
        switch_context(&old->state->current_stack, &next->state->current_stack);
    } else {
        // first task to run
        void* dummy;
        switch_context(&dummy, &next->state->current_stack);
    }

    return TuxSays_Ok;
}

TuxSays_Error TuxSays_Task_Init() {
#define TASK(_name, _fn, _priority, _stack_size)                \
    TS_LOG_I("init task '%s' (0x%08X)", #_name, &task_##_name); \
    fill_stack(&task_##_name);                                  \
    prepare_task_for_start(&task_##_name, task_##_fn##_run);
#include "task_list_config.h"
#undef TASK
    return TuxSays_Ok;
}

TuxSays_Error TuxSays_Task_DumpStackInfo() {
    TS_LOG_I("Dump stack info:");
#define TASK(_name, _fn, _priority, _stack_size) \
    TS_LOG_I(                                    \
        "  task '%s' (0x%08X) free stack = %lu", \
        #_name,                                  \
        &task_##_name,                           \
        check_free_stack(&task_##_name));
#include "task_list_config.h"
#undef TASK
    return TuxSays_Ok;
}

TuxSays_Error TuxSays_Task_StartScheduler() {
    TS_LOG_I("Start");

    schedule();

    return TuxSays_Ok;
}

TuxSays_Error TuxSays_Task_Yield() {
    schedule();
    return TuxSays_Ok;
}

TuxSays_Error TuxSays_Task_CalculateSleepTime(uint32_t* sleep_time) {
    uint32_t ms = TuxSays_SysTick_Milis();
    *sleep_time = (uint32_t)-1;
    for(uint8_t id = 0; id < TuxSays_Task_Count - 1; id++) {
        TuxSays_Task_State* state = task_list[id]->state;

        // check waiting tasks
        if((state->flags & TASK_FLAG_WAITING) != 0) {
            uint32_t diff = (ms - state->waiting_from);
            if(diff >= state->sleep_delay) {
                *sleep_time = 0;
                return TuxSays_Ok;
            }

            diff = state->sleep_delay - diff;

            if(*sleep_time > diff) {
                *sleep_time = diff;
            }
        }
    }

    return TuxSays_Ok;
}

TuxSays_Error TuxSays_Task_Delay(uint32_t ms) {
    if(current_task == NULL) {
        return TuxSays_Error_NotInTask;
    }

    TuxSays_Task_State* state = current_task->state;

    state->flags &= ~TASK_FLAG_TIMER_TIMEOUT;
    state->flags |= TASK_FLAG_WAITING;
    state->waiting_from = TuxSays_SysTick_Milis();
    state->sleep_delay = ms;

    schedule();

    state->flags &= ~TASK_FLAG_TIMER_TIMEOUT;

    return TuxSays_Ok;
}

TuxSays_Task_Handle* TuxSays_Task_GetCurrentTask() {
    return (TuxSays_Task_Handle*)current_task;
}

TuxSays_Error TuxSays_Task_WakeTask(const TuxSays_Task_Handle* handle) {
    if(handle == NULL) {
        return TuxSays_Error_InvalidArgument;
    }

    TuxSays_Task_State* state = handle->state;

    state->flags &= ~(TASK_FLAG_WAITING | TASK_FLAG_TIMER_TIMEOUT | TASK_FLAG_BLOCKED);
    state->waiting_from = 0;
    state->sleep_delay = 0;

    return TuxSays_Ok;
}

TuxSays_Error TuxSays_Task_WaitForEvent(uint32_t ms) {
    if(current_task == NULL) {
        return TuxSays_Error_NotInTask;
    }

    TuxSays_Task_State* state = current_task->state;

    if(ms == TS_TASK_WAIT_FOREVER) {
        state->flags |= TASK_FLAG_BLOCKED;
    } else {
        state->flags &= ~TASK_FLAG_TIMER_TIMEOUT;
        state->flags |= TASK_FLAG_WAITING;
        state->waiting_from = TuxSays_SysTick_Milis();
        state->sleep_delay = ms;
    }

    schedule();

    if((state->flags & TASK_FLAG_TIMER_TIMEOUT) != 0) {
        return TuxSays_Error_Timeout;
    }

    return TuxSays_Ok;
}