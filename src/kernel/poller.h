#ifndef __POLLER_H__
#define __POLLER_H__

#include <stdlib.h>
#include <poll.h>
#include <string.h>

#include "print.h"

struct poller_event
{
    struct pollfd fd; 
    void *ptr;
};

struct poller 
{
    int maxfds;
    struct poller_event *ev_list;
};

int poller_create(int maxfds);
int poller_add(int pfd, int fd, short events, void *ptr);
int poller_mod(int pfd, int fd, short events, void *ptr);
int poller_del(int pfd, int fd);
int poller_wait(int pfd, struct poller_event *pv, int maxfds, int timeout);

#endif
