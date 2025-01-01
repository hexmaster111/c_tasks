#define _GNU_SOURCE
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include <fcntl.h>

#include "task.h"

#define PORT (5050)
#define MAX_CONNECTIONS (10)

// Idk why intellasence cant find this... i can ctrl click it and it finds it...
// but it can find the other SO_
#ifndef SO_REUSEPORT
#define SO_REUSEPORT (15)
#endif

struct Connection
{
    int fd, len;
    struct sockaddr_in saddr;
    struct TaskInfo tinfo;
} g_connection_context[MAX_CONNECTIONS] = {0};

struct Connection *GetNextOpenConnection()
{
    for (int i = 0; i < MAX_CONNECTIONS; i++)
    {
        if (g_connection_context[i].fd == 0)
            return &g_connection_context[i];
    }

    puts("WARN: out of open connections!");
    return NULL;
}

struct Connection create_tcp_socket_fd();

void CheckClientTask(struct TaskRunner *run, struct TaskInfo *self, void *data)
{
    struct Connection *clinet = data; /* this connection is in the g_connection_context */

    char buff[1024] = {0};

    int n = read(clinet->fd, &buff, sizeof(buff));

    if (n == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
    {
        return; /* nothing to read */
    }

    if (n == 0 || 0 > n)
    {

        printf("n? %d\n", n);
        return;
    }

    write(clinet->fd, buff, n);

    if (buff[0] == 'q')
    {
        printf("clinet disconneced %d\n", clinet->fd);
        write(clinet->fd, "Good bye~!", 11);
        close(clinet->fd);
        TaskCompleate(run, self, NULL);
        return;
    }
}

void AcceptConnectionsTask(struct TaskRunner *run, struct TaskInfo *self, void *data)
{
    struct Connection *server = data; /* this connection is NOT in g_connection_context array */

    struct sockaddr_in connection_addr;
    int connection_addr_len = sizeof(connection_addr);

    memset(&connection_addr, 0, sizeof(connection_addr));

    int connection_fd = accept4(server->fd, (struct sockaddr *)&connection_addr, &connection_addr_len, SOCK_NONBLOCK);

    if (0 > connection_fd)
    {
        if (errno == EWOULDBLOCK || errno == EAGAIN)
        {
            return; // no work
        }

        perror("accept");
        return;
    }

    printf("clinet connected %d\n", connection_fd);

    struct Connection *newctx = GetNextOpenConnection();

    if (!newctx)
    {
        write(connection_fd, "SERVER FULL!\r\n", 15);
        close(connection_fd);
        TaskCompleate(run, self, NULL);
        return;
    }

    newctx->fd = connection_fd;
    newctx->saddr = connection_addr;
    newctx->len = connection_addr_len;

    newctx->tinfo = (struct TaskInfo){
        .method = CheckClientTask,
        .task_state = newctx};

    TaskRunner_AddTask(run, &newctx->tinfo);
}

int main()
{
    struct Connection server = create_tcp_socket_fd();
    struct TaskRunner tr = {0};

    struct TaskInfo acceptConnectionsTaskCtx = {.method = AcceptConnectionsTask, .task_state = &server};

    TaskRunner_AddTask(&tr, &acceptConnectionsTaskCtx);

    uint64_t lc = 0;
    while (TaskRunner_HasTasks(&tr))
    {
        lc += 1;
        TaskRunner_RunTasks(&tr);

        struct TaskInfo itter;
        while (TaskRunner_Itter_FreeCompleatedTask(&tr, &itter))
        {
            if (itter.task_state == &server)
            {
                // todo : handle closing down the server...
            }
            else
            {
                /* tasks are all done, and sockets are free'd we just need to mark thease tasks
                    as done so we can re-use our global buffer
                 */

                struct Connection *ts = itter.task_state;
                *ts = (struct Connection){
                    .fd = 0,
                    .len = 0,
                    .saddr = 0,
                    .tinfo = {
                        .method = NULL,
                        .task_state = NULL,
                        .task_resault = NULL}};
            }
        }
    }

    close(server.fd);
    TaskRunner_Free(&tr);
}

struct Connection create_tcp_socket_fd()
{
    int server_fd, len;
    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));

    server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    if (0 > server_fd)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if (0 > setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    if (0 > setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (0 > bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)))
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (0 > listen(server_fd, MAX_CONNECTIONS))
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    return (struct Connection){.fd = server_fd, .len = len, .saddr = server_addr};
}
