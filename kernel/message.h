/*
 * =====================================================================================
 *
 *       Filename:  message.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年09月22日 16时40分23秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (R&D), lzx1442@163.com
 *        Company:  wanwei-tech
 *
 * =====================================================================================
 */
#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "def.h"
#include "types.h"
#include "object.h"

#include <pthread.h>
#include <semaphore.h>


#define MSG_TERM		0x0000

/*
 * brief Ready to initialize modules
 */
#define MSG_INIT		0x0001

/*
 * brief Indicates a timer has expired
 */
#define MSG_TIMER		0x0002

/*
 * brief The command message
 * 
 * \cmd
 * MSG_COMMAND
 * int cmd = wparam;
 */
#define MSG_COMMAND		0x0003

#define MSG_AIOIN		0x0004

#define MSG_AIOOUT		0x0005

#define MSG_AIOERR		0x0006

#define MSG_AIOCONN		0x0007

#define MSG_AIOBREAK	0x0008

#define MSG_STATE		0x0009

/*
 * brief Indicates a message has post
 */
#define QS_POSTMSG 		0x40000000

/**
* @brief 线程间消息结构
*/
typedef struct msg
{
	HMOD hmod;

	int message;

	WPARAM wparam;
	LPARAM lparam;
}*msg_t;

/**
* @brief 消息队列（环形）
*/
typedef struct msgqueue
{
	UINT dw_data;

	int read_pos;
	int write_pos;

	sem_t wait;
	pthread_mutex_t lock;
	struct msg msg[MSGQUEUE_MAX];
}*msgqueue_t;

////////////////////////////////////////////////////////////////////////
HMOD find_thread(const char *name);
int send_message(HMOD hmod, int message, WPARAM wparam, LPARAM lparam);
int post_message(HMOD hmod, int message, WPARAM wparam, LPARAM lparam);
int get_message(HMOD hmod, msg_t pmsg);
int dispatch_message(msg_t pmsg);

#endif
