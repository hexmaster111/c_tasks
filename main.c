#include <stdlib.h>
#include <stdio.h>

#include "link_list.h"

struct TaskRunner;
struct TaskDeligate;

typedef void (*TaskDeligate)(struct TaskRunner *run, struct TaskDeligate *self, void *data);

typedef struct TaskInfo
{
    TaskDeligate method;
    void *data;
} TaskInfo;

struct TaskRunner
{
    LLIST task_info_list;
};

void TaskRunner_AddTask(struct TaskRunner *t, TaskDeligate add, void *data)
{
    struct TaskInfo *tinfo = malloc(sizeof(struct TaskInfo));
    tinfo->data = data;
    tinfo->method = add;
    llist_append(&t->task_info_list, tinfo);
}

void TaskRunner_RemoveTask(struct TaskRunner *t, TaskInfo *tinfo)
{
}

void _RunTasks_itter(void *listdata, void *user)
{
    struct TaskInfo *tinfo = listdata;
    struct TaskRunner *tr = user;

    tinfo->method(tr, tinfo->method, tinfo->data);
}

void TaskRunner_RunTasks(struct TaskRunner *t)
{
    llist_itterate(&t->task_info_list, _RunTasks_itter, t);
}

int TaskRunner_HasTasks(struct TaskRunner *t) { return t->task_info_list.len > 0; }

void TaskRunner_TaskResaultDone(struct TaskRunner *t) {};
void TaskRunner_TaskResaultError(struct TaskRunner *t) {};

// --------------- USER IMPL --------------- //

struct SimpleCountingTask
{
    int curr, max;
};

void CountUpTask(struct TaskRunner *run, TaskDeligate self, void *data)
{
    struct SimpleCountingTask *t = data;

    if (t->max <= t->curr)
    {
        TaskRunner_RemoveTask(run, self);
    }
    else
    {
        t->curr += 1;
        printf("up  : %d\n", t->curr);
    }
}

void CountDownTask(struct TaskRunner *run, TaskDeligate self, void *data)
{
    struct SimpleCountingTask *t = data;

    if (t->max >= t->curr)
    {
        TaskRunner_RemoveTask(run, self);
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
    struct SimpleCountingTask countUpTaskCtx = {.curr = 0, .max = 10};
    struct SimpleCountingTask countDownTaskCtx = {.curr = 10, .max = 0};

    TaskRunner_AddTask(&r, CountUpTask, &countUpTaskCtx);
    TaskRunner_AddTask(&r, CountDownTask, &countDownTaskCtx);

    while (TaskRunner_HasTasks(&r))
    {
        TaskRunner_RunTasks(&r);
    }
}