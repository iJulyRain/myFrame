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

	INIT_LOCK(&poller->lock);
	
    for(i = 0; i < poller->maxfds; i++)
        poller->ev_list[i].fd.fd = -1;
    
    return (poller == NULL) ? -1 : (int)poller; 
}

int poller_add(int pfd, int fd, short events, void *ptr)
{
    int i;
    struct poller *poller;

	if(pfd == -1)
		return -1;

    poller = (struct poller *)pfd;

	ENTER_LOCK(&poller->lock);

    for(i = 0; i < poller->maxfds; i++)
    {
        if(poller->ev_list[i].fd.fd != -1)
            continue;

        poller->ev_list[i].fd.fd = fd;
        poller->ev_list[i].fd.events = events;
        poller->ev_list[i].ptr = ptr;

        break;
    }

	EXIT_LOCK(&poller->lock);

	return 0;
}

int poller_mod(int pfd, int fd, short events, void *ptr)
{
	int i;
	struct poller *poller;

	if(pfd == -1)
		return -1;

	poller = (struct poller *)pfd;

	ENTER_LOCK(&poller->lock);

	for(i = 0; i < poller->maxfds; i++)
	{
		if(poller->ev_list[i].fd.fd != fd)
			continue;

		poller->ev_list[i].fd.events = events;
		poller->ev_list[i].ptr = ptr;

		break;
	}

	EXIT_LOCK(&poller->lock);

	return 0;
}

int poller_del(int pfd, int fd)
{
    int i;
    struct poller *poller;

	if(pfd == -1)
		return -1;

    poller = (struct poller *)pfd;

	ENTER_LOCK(&poller->lock);

    for(i = 0; i < poller->maxfds; i++)
    {
        if(poller->ev_list[i].fd.fd != fd)
            continue;

        poller->ev_list[i].fd.fd = -1;
		poller->ev_list[i].ptr = (void *)-1;

        break;
    }

	EXIT_LOCK(&poller->lock);

	return 0;
}

int poller_wait(int pfd, struct poller_event *ev, int maxfds, int timeout)
{
    int i, nfds;
    struct poller *poller;

	if(pfd == -1)
		return -1;
    
    poller = (struct poller *)pfd;

    struct pollfd fds[poller->maxfds];
	memset(fds, 0, sizeof(struct pollfd) * poller->maxfds);

	ENTER_LOCK(&poller->lock);
    for(i = 0; i < poller->maxfds; i++)
        fds[i] = poller->ev_list[i].fd;
	EXIT_LOCK(&poller->lock);
    
    nfds = poll(fds, poller->maxfds, timeout);
    if(nfds <= 0)
        goto err;

	ENTER_LOCK(&poller->lock);
    for(i = 0; i < poller->maxfds && i < maxfds; i++)
    {
        if(fds[i].revents & POLLIN
		|| fds[i].revents & POLLOUT
		|| fds[i].revents & POLLERR
		|| fds[i].revents & POLLNVAL)
		{
			if(poller->ev_list[i].ptr == (void *)-1)
				continue;

			ev->fd = fds[i];
			ev->ptr = poller->ev_list[i].ptr;

			ev++;
		}
    }
	EXIT_LOCK(&poller->lock);

err:
    return nfds;
}
