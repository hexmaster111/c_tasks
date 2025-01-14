# C Async

C Async is a libaray to make working with in a task based way easer in C

Go look at demo_tcp_echo_server for a good idea how to use this lib

## Example
``` c
void main(){
    // Define a task runner
    struct TaskRunner tr = {0};

    //Define the tasks you want your task runner todo
    struct TaskInfo simpleTask = {.method = MyTaskMethod, .task_state = NULL};
    struct TaskInfo otherTask = {.method = NeverEndingTask, .task_state = NULL};

    // Add your task to the task runner
    TaskRunner_AddTask(&tr, &simpleTask);

    // Run your tasks
    while (TaskRunner_HasTasks(&tr))
    {
        TaskRunner_RunTasks(&tr);


        struct TaskInfo itter;
        while (TaskRunner_Itter_FreeCompleatedTask(&tr, &itter))
        {
            // Clear out all your compleated tasks, if task_state is malloc'd nows the time to free it
        }
    }

    // free the task runner resources
    TaskRunner_Free(&tr);
}



// Sample Task Method
void MyTaskMethod(struct TaskRunner *run, struct TaskInfo *self, void *data){
    // the data pointer is the .task_state defined in your task info !

    // tasks add more tasks as they run
    TaskRunner_AddTask(run, &my_new_task_context);

    // Here your task can do any work it needs, when all work is done, call TaskCompleate
    TaskCompleate(run, self, NULL); // the NULL here can be anything, and is saved as your tasks resault
}

void NeverEndingTask(struct TaskRunner *run, struct TaskInfo *self, void *data){
    // This task never calls task compleate, so it will alwase get re-called
}
```