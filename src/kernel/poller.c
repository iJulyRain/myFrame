#include "poller.h"

static int poller_id;

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
	{
		poller->ev_list[i].fd.fd = -1;
        poller->ev_list[i].magic = 0;
	}

	poller_id = (int)poller;
    
    return (poller == NULL) ? -1 : (int)poller; 
}

int poller_add(int pfd, poller_event_t event)
{
    int i;
    struct poller *poller;

	if(pfd == 0)
		poller = (poller_t)poller_id;
	else
		poller = (struct poller *)pfd;

	ENTER_LOCK(&poller->lock);

    for(i = 0; i < poller->maxfds; i++)
    {
        if(poller->ev_list[i].magic != 0)
            continue;

        poller->ev_list[i] = *event;
        break;
    }

	EXIT_LOCK(&poller->lock);

	return 0;
}

int poller_mod(int pfd, poller_event_t event)
{
	int i;
	struct poller *poller;

	if(pfd == 0)
		poller = (poller_t)poller_id;
	else
		poller = (struct poller *)pfd;

	ENTER_LOCK(&poller->lock);

	for(i = 0; i < poller->maxfds; i++)
	{
		if(poller->ev_list[i].magic != event->magic)
			continue;

		poller->ev_list[i] = *event;

		break;
	}

	EXIT_LOCK(&poller->lock);

	return 0;
}

int poller_del(int pfd, poller_event_t event)
{
    int i;
    struct poller *poller;

	if(pfd == 0)
		poller = (poller_t)poller_id;
	else
		poller = (struct poller *)pfd;

	ENTER_LOCK(&poller->lock);

    for(i = 0; i < poller->maxfds; i++)
    {
        if(poller->ev_list[i].magic != event->magic)
            continue;

		poller->ev_list[i].magic = 0;
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

	if(pfd == 0)
		poller = (poller_t)poller_id;
	else
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

//////////////////////////poller event//////////////////////////////////////
poller_event_t poller_event_create(void *ptr)
{
	poller_event_t event;

	event = (poller_event_t)calloc(1, sizeof(struct poller_event));
	if(event == NULL)
		return event;

	event->magic = (int)event;
	event->fd.fd = -1;
	event->fd.events |= POLLIN;
	event->fd.events |= POLLERR | POLLNVAL;
	event->ptr = ptr;

	return event;
}

void poller_event_relase(poller_event_t event)
{
	free(event);
}

void poller_event_setfd(poller_event_t event, int fd)
{
	event->fd.fd = fd;
}

void poller_event_setev(poller_event_t event,  int ev)
{
	event->fd.events |= ev;
}

void poller_event_clrev(poller_event_t event, int ev)
{
	event->fd.events &= ~ev;
}
