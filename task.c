#include "task.h"
#include <stdlib.h>
#include <stddef.h>

#pragma region Link List Impl

LINKED_LIST_CONTAINERPTR newcontainer()
{
    LINKED_LIST_CONTAINERPTR new = malloc(sizeof(LINKED_LIST_CONTAINER));
    new->data = NULL;
    new->next = NULL;
    return new;
}

LINKED_LIST_CONTAINERPTR llist_container_at(LLISTPTR t, int idx)
{
    LINKED_LIST_CONTAINERPTR c = t->head;

    while (idx)
    {
        idx = idx - 1;
        c = c->next;
    }

    return c;
}

void llist_free(LLISTPTR t)
{
    LINKED_LIST_CONTAINERPTR c = t->head;

    for (size_t i = 0; i < t->len; i++)
    {
        LINKED_LIST_CONTAINERPTR tofree = c;
        c = c->next;
        free(tofree);
    }

    t->head = NULL;
    t->len = 0;
}

void llist_append(LLISTPTR t, void *data)
{
    if (t->head == NULL) /* this is a new list with no head */
    {
        t->head = newcontainer();
        t->head->data = data;
        t->len = 1;
        return;
    }

    LINKED_LIST_CONTAINERPTR tmp = t->head;

    while (tmp->next)
    {
        tmp = tmp->next;
    }

    LINKED_LIST_CONTAINERPTR new = newcontainer();
    t->len = t->len + 1;
    new->data = data;
    tmp->next = new;
}

void *llist_at(LLISTPTR t, int idx)
{
    if (idx > t->len)
        abort();

    LINKED_LIST_CONTAINERPTR c = t->head;

    while (idx)
    {
        idx = idx - 1;
        c = c->next;
    }

    return c->data;
}

int llist_is_empty(LLISTPTR t)
{
    return t->len == 0;
}

void llist_remove(LLISTPTR t, int idx)
{
    t->len -= 1;

    if (idx == 0)
    {
        LINKED_LIST_CONTAINERPTR f = t->head;
        t->head = f->next;
        free(f);
        return;
    }

    LINKED_LIST_CONTAINERPTR c = t->head;

    idx--;

    while (idx)
    {
        c = c->next;
        idx = idx - 1;
    }

    LINKED_LIST_CONTAINERPTR tofree = c->next;

    c->next = tofree->next;
    free(tofree);
}

int llist_idxof(LLISTPTR t, void *item)
{

    LINKED_LIST_CONTAINERPTR c = t->head;
    int idx = 0;

    while (c)
    {
        if (c->data == item)
            return idx;

        c = c->next;
        idx += 1;
    }

    return -1;
}

void llist_itterate(LLISTPTR t, LLIST_ITTERATOR_FUNC ittrf, void *user)
{
    // todo: have a way to check if the list was modifyed while itterating 
    for (size_t i = 0; i < t->len; i++)
    {
        ittrf(llist_at(t, i), user);
    }
}
#pragma endregion Link List Impl

void TaskRunner_AddTask(struct TaskRunner *t, struct TaskInfo *tinfo)
{
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

    *t = *(struct TaskInfo *)llist_at(&r->task_compleated_list, r->task_compleated_list.len - 1);

    llist_remove(&r->task_compleated_list, r->task_compleated_list.len - 1);
    // TODO: Im pretty sure there should be a call freeing the (TaskInfo *) that was in the list...
    // unless we wanna do some kinda recycling of tasks so we arnt constantly malloc/freeing thease
    // little task structures
    return 1;
}

// Task compleation API
void TaskCompleate(struct TaskRunner *t, struct TaskInfo *tinfo, void *task_resault)
{
    tinfo->task_resault = task_resault;

    int runlistidx = llist_idxof(&t->task_info_list, tinfo);
    if (runlistidx == -1)
        return; // TODO: Warning -- task done, and called task compleate, but we couldnt find it in our task list!

    llist_remove(&t->task_info_list, runlistidx);
    llist_append(&t->task_compleated_list, tinfo);
};

void TaskRunner_Free(struct TaskRunner *t)
{
    llist_free(&t->task_compleated_list);
    llist_free(&t->task_info_list);
}