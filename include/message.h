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

/*
 * brief Indicates a message has post
 */
#define QS_POSTMSG 		0x40000000

HMOD find_thread(const char *name);
int send_message(HMOD hmod, int message, WPARAM wparam, LPARAM lparam);
int post_message(HMOD hmod, int message, WPARAM wparam, LPARAM lparam);
int get_message(HMOD hmod, msg_t pmsg);
int dispatch_message(msg_t pmsg);

#endif
