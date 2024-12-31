#include <stdlib.h>
#include <stdio.h>
#include "task.h"

// --------------- Example Tasks --------------- //

// A simple state for the task
struct SimpleCountingTask
{
    int curr, dst;
    char alotofthings[512];
};

// a task , it gets a pointer to the state ( you alloc it still ) , and a pointer to
// the task runner and this task , so we can do state changes .
// NOTE: a task should not block
void CountUpTask(struct TaskRunner *run, struct TaskInfo *self, void *task_state)
{
    struct SimpleCountingTask *t = task_state;

    if (t->dst <= t->curr) // once we count to the amount we want, we call complate task.
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

    if (t->dst >= t->curr)
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

    if (t->dst >= t->curr)
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
    struct TaskRunner r = {0};

    // where we store our task state
    struct TaskInfo countUpTaskInfo = {
        .method = CountUpTask,
        .task_state = &(struct SimpleCountingTask){.curr = 0, .dst = 10, .alotofthings = {0}},
    };

    struct TaskInfo countDownTaskInfo = {
        .method = CountDownTask,
        .task_state = &(struct SimpleCountingTask){.curr = 10, .dst = 0, .alotofthings = {0}},
    };

    struct TaskInfo countToErrorTaskInfo = {
        .method = CountToAnErrorTask,
        .task_state = &(struct SimpleCountingTask){.curr = 10, .dst = 0, .alotofthings = {0}},
    };

    // schedual our tasks
    TaskRunner_AddTask(&r, &countUpTaskInfo);
    TaskRunner_AddTask(&r, &countDownTaskInfo);
    TaskRunner_AddTask(&r, &countToErrorTaskInfo);

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

    TaskRunner_Free(&r);
}