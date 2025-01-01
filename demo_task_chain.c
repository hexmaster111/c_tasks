#include "task.h"
#include <stdio.h>


/*



            THIS IS A WORK IN PROGRESS



*/


struct TState
{
    int times;
};

void T1(struct TaskRunner *run, struct TaskInfo *self, void *data);
void T2(struct TaskRunner *run, struct TaskInfo *self, void *data);
void T3(struct TaskRunner *run, struct TaskInfo *self, void *data);

void T3(struct TaskRunner *run, struct TaskInfo *self, void *data)
{
    struct TState *s = data;
    puts("T3");

    if (s->times)
    {
        /* do some task work */
        s->times -= 1;
    }
    else
    {
        TaskCompleate(run, self, 0);
        self->method = T1;
        struct TState *ts = self->task_state;
        ts->times = 5;
        TaskRunner_AddTask(run, self);
    }
}
void T2(struct TaskRunner *run, struct TaskInfo *self, void *data)
{
    struct TState *s = data;
    puts("T2");

    if (s->times)
    {
        /* do some task work */
        s->times -= 1;
    }
    else
    {
        TaskCompleate(run, self, 0);
        self->method = T3;
        struct TState *ts = self->task_state;
        ts->times = 5;
        TaskRunner_AddTask(run, self);
    }
}

void T1(struct TaskRunner *run, struct TaskInfo *self, void *data)
{
    struct TState *s = data;

    puts("T1");

    if (s->times)
    {
        /* do some task work */
        s->times -= 1;
    }
    else
    {
        TaskCompleate(run, self, 0);
        self->method = T2;
        struct TState *ts = self->task_state;
        ts->times = 5;
        TaskRunner_AddTask(run, self);
    }
}

int main()
{
    struct TaskRunner r = {0};
    struct TaskInfo s = {.method = T1, .task_state = &(struct TState){.times = 5}};

    TaskRunner_AddTask(&r, &s);

    while (TaskRunner_HasTasks(&r))
    {
        TaskRunner_RunTasks(&r);
    }
}

// --------------- WHAT IM KINDA THINKING ---------------


typedef struct TaskInfo TI;


int _main()
{
        
}