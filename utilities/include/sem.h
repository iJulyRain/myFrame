/*
 * =====================================================================================
 *
 *       Filename:  sem.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年01月03日 16时02分18秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lifang (lf), chengxinaya@163.com
 *        Company:  WWYC
 *
 * =====================================================================================
 */

#ifndef __SEM_H_
#define __SEM_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

/**
 * @brief init_sem
 * init semaphore in sys
 * @param sem_id
 * current semaphore id value
 * @param init_value
 * semaphore value initialized
 * @return 
 * Successed：0 
 * Failed：-1
 */
int init_sem(int sem_id, int init_value);

/**
 * @brief del_sem
 * delete semaphore from sys
 * @param sem_id
 * current semaphore id value
 * @param init_value
 * 
 * @return 
 * succeed：0 
 * Failed：-1
 */
int del_sem(int sem_id);

/**
 * @brief sem_p
 * p operation
 * @param sem_id
 * current semaphore id value
 * @return 
 * succeed：0 
 * Failed：-1
 */
int sem_p(int sem_id);

/**
 * @brief sem_v
 * v operation
 * @param sem_id
 * current semaphore id value
 * @return 
 * succeed：0 
 * Failed：-1
 */
int sem_v(int sem_id);

#endif
