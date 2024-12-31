#ifndef TASK_H
#define TASK_H

struct TaskRunner;
struct TaskDeligate;
struct TaskInfo;

// this defines a func pointer that returns nothing, and takes in a TaskRunner, Taskinfo, and void *data
typedef void (*TaskDeligate)(struct TaskRunner *run, struct TaskInfo *self, void *data);

#define TASK(TNAME) void TNAME (struct TaskRunner *run, struct TaskInfo *self, void *data)

/*
    tasks / memory is allocated with the AddTask method, they are added into the task info list,
    the tasks then do there work intel the task Calls ResaultError() or ResaultSucess()

    Running happends in two stages, First, Task are itterated through, and there state changes
    tracked into the task compleate list
*/
struct TaskInfo
{
    TaskDeligate method;
    void *task_state;
    void *task_resault;
};


typedef int (*LLIST_COMPARISON_FUNC)(void *, void *);
typedef void (*LLIST_ITTERATOR_FUNC)(void *listdata, void *itterdata);
typedef struct LINKED_LIST_CONTAINER *LINKED_LIST_CONTAINERPTR;

typedef struct LINKED_LIST_CONTAINER
{
    LINKED_LIST_CONTAINERPTR next;
    void *data;
} LINKED_LIST_CONTAINER, *LINKED_LIST_CONTAINERPTR;

typedef struct LINKED_LIST
{
    int len;
    LINKED_LIST_CONTAINERPTR head;
} LLIST, *LLISTPTR;

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

void TaskRunner_AddTask(struct TaskRunner *t, struct TaskInfo *tinfo);
void TaskRunner_RunTasks(struct TaskRunner *t);
int TaskRunner_HasTasks(struct TaskRunner *t);
int TaskRunner_Itter_FreeCompleatedTask(struct TaskRunner *r, struct TaskInfo *t);
void TaskCompleate(struct TaskRunner *t, struct TaskInfo *tinfo, void *task_resault);
void TaskRunner_Free(struct TaskRunner *t);

#endif // TASK_H