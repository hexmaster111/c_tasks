#include <stdlib.h>
#include <stdio.h>

#include "link_list.h"

struct TaskRunner;
struct TaskDeligate;
struct TaskInfo;

typedef void (*TaskDeligate)(struct TaskRunner *run, struct TaskInfo *self, void *data);

struct TaskInfo
{
    TaskDeligate method;
    void *task_state;
    int task_resault;
};

/*
    tasks / memory is allocated with the AddTask method, they are added into the task info list,
    the tasks then do there work intel the task Calls ResaultError() or ResaultSucess()

    Running happends in two stages, First, Task are itterated through, and there state changes
    tracked into the task compleate list
*/
struct TaskRunner
{
    LLIST task_info_list;
    LLIST task_compleated_list;
};

void TaskRunner_AddTask(struct TaskRunner *t, TaskDeligate add, void *state)
{
    struct TaskInfo *tinfo = malloc(sizeof(struct TaskInfo));
    tinfo->task_state = state;
    tinfo->method = add;
    llist_append(&t->task_info_list, tinfo);
}

void _RunTasks_itter(void *listdata, void *user)
{
    struct TaskInfo *tinfo = listdata;
    struct TaskRunner *tr = user;

    tinfo->method(tr, tinfo, tinfo->task_state);
}

void TaskRunner_RunTasks(struct TaskRunner *t)
{
    llist_itterate(&t->task_info_list, _RunTasks_itter, t);
}

int TaskRunner_HasTasks(struct TaskRunner *t) { return t->task_info_list.len > 0; }

// returns 1 and puts the last compleated task into the value at t
// use it with a while() loop to itterate through all tasks that where compelated
// this loop through
int TaskRunner_Itter_FreeCompleatedTask(struct TaskRunner *r, struct TaskInfo *t)
{
    if (llist_is_empty(&r->task_compleated_list))
    {
        *t = (struct TaskInfo){.task_state = 0, .method = 0};
        return 0;
    }
 
    *t = *(struct TaskInfo *)llist_at(&r->task_compleated_list, r->task_compleated_list.len-1);

    llist_remove(&r->task_compleated_list, r->task_compleated_list.len-1);
    // TODO: Im pretty sure there should be a call freeing the (TaskInfo *) that was in the list...
    // unless we wanna do some kinda recycling of tasks so we arnt constantly malloc/freeing thease
    // little task structures
    return 1;
}

// Task compleation / failure api
void TaskCompleate(struct TaskRunner *t, struct TaskInfo *tinfo, int task_resault)
{
    tinfo->task_resault = task_resault;

    int runlistidx = llist_idxof(&t->task_info_list, tinfo);
    if (runlistidx == -1)
        abort();

    llist_remove(&t->task_info_list, runlistidx);
    llist_append(&t->task_compleated_list, tinfo);
};

// --------------- Example Tasks --------------- //


// A simple state for the task
struct SimpleCountingTask
{
    int curr, max;
};

// a task , it gets a pointer to the state ( you alloc it still ) , and a pointer to
// the task runner and this task , so we can do state changes . 
// NOTE: a task should not block
void CountUpTask(struct TaskRunner *run, struct TaskInfo *self, void *task_state)
{
    struct SimpleCountingTask *t = task_state;

    if (t->max <= t->curr) // once we count to the amount we want, we call complate task.
    {
        TaskCompleate(run, self, ""); 
    }
    else
    {
        t->curr += 1;
        printf("up  : %d\n", t->curr);
    }
}

void CountDownTask(struct TaskRunner *run, struct TaskInfo *self, void *task_state)
{
    struct SimpleCountingTask *t = task_state;

    if (t->max >= t->curr)
    {
        TaskCompleate(run, self, "");
    }
    else
    {
        t->curr -= 1;
        printf("down: %d\n", t->curr);
    }
}

void CountToAnErrorTask(struct TaskRunner *run, struct TaskInfo *self, void *task_state)
{
    struct SimpleCountingTask *t = task_state;

    if (t->max >= t->curr)
    {
        TaskCompleate(run, self, "Number Hit");
    }
    else
    {
        t->curr -= 1;
        printf("down: %d\n", t->curr);
    }
}



void main()
{
    struct TaskRunner r = {0}; // where we store our task state
    struct SimpleCountingTask countUpTaskCtx = {.curr = 0, .max = 10}; // our state
    struct SimpleCountingTask countDownTaskCtx = {.curr = 10, .max = 0};
    struct SimpleCountingTask countToErrorCtx = {.curr = 10, .max = 0};

    // schedual our tasks
    TaskRunner_AddTask(&r, CountUpTask, &countUpTaskCtx); 
    TaskRunner_AddTask(&r, CountDownTask, &countDownTaskCtx);
    TaskRunner_AddTask(&r, CountToAnErrorTask, &countToErrorCtx);

    // While we have things todo
    while (TaskRunner_HasTasks(&r))
    {
        // do the work that was added above
        TaskRunner_RunTasks(&r);

        // a place to store our task that was compleated
        struct TaskInfo ittr;

        // itterate through the tasks we finished this run through,
        // this call will remove the task
        while (TaskRunner_Itter_FreeCompleatedTask(&r, &ittr))
        {
            printf("Task %p done %s\n", ittr.method, ittr.task_resault ? ittr.task_resault : "");
        }
    }
}