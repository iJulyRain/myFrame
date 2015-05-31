#include "poller.h"

int poller_create(int maxfds)
{
    int i;
    struct poller *poller;

    poller = (struct poller *)calloc(1, sizeof(struct poller));
    if(poller == NULL)
        return -1;
    
    poller->maxfds = maxfds;
    poller->ev_list = (struct poller_event *)calloc(poller->maxfds, sizeof(struct poller_event));
    if(poller->ev_list == NULL)
        return -1;
    
    for(i = 0; i < poller->maxfds; i++)
        poller->ev_list[i].fd.fd = -1;
    
    return (poller == NULL) ? -1 : (int)poller; 
}

void poller_add(int pfd, int fd, void *ptr)
{
    int i;
    struct poller *poller;

	if(pfd == -1)
		return;

    poller = (struct poller *)pfd;

    for(i = 0; i < poller->maxfds; i++)
    {
        if(poller->ev_list[i].fd.fd != -1)
            continue;

        poller->ev_list[i].fd.fd = fd;
        poller->ev_list[i].fd.events = POLLIN;
        poller->ev_list[i].ptr = ptr;

        break;
    }
}

void poll_del(int pfd, int fd)
{
    int i;
    struct poller *poller;

	if(pfd == -1)
		return;

    poller = (struct poller *)pfd;

    for(i = 0; i < poller->maxfds; i++)
    {
        if(poller->ev_list[i].fd.fd != fd)
            continue;

        poller->ev_list[i].fd.fd = -1;

        break;
    }
}

int poll_wait(int pfd, struct poller_event *ev, int timeout)
{
    int i, nfds;
    struct poller *poller;

	if(pfd == -1)
		return -1;
    
    poller = (struct poller *)pfd;

    struct pollfd fds[poller->maxfds];

    for(i = 0; i < poller->maxfds; i++)
        fds[i] = poller->ev_list[i].fd;
    
    nfds = poll(fds, poller->maxfds, timeout);
    if(nfds <= 0)
        goto err;
    for(i = 0; i < poller->maxfds; i++)
    {
        if(fds[i].revents & POLLIN)
        {
            debug(DEBUG, "fd: %d POLLIN\n", fds[i].fd);
            *ev++ = poller->ev_list[i];
        }
        else if(fds[i].revents & POLLERR)
        {
            debug(DEBUG, "fd: %d POLLERR\n", fds[i].fd);
            *ev++ = poller->ev_list[i];
        }
        else if(fds[i].revents & POLLNVAL)
        {
            debug(DEBUG, "fd: %d POLLNVAL\n", fds[i].fd);
            *ev++ = poller->ev_list[i];
        }
    }

err:
    return nfds;
}
