/*
 * =====================================================================================
 *
 *       Filename:  init.h
 *
 *    Description:  init
 *
 *        Version:  1.0
 *        Created:  06/06/2015 11:06:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  julyrain (RD), lzx1442@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __SYSTEM_INIT_H__
#define __SYSTEM_INIT_H__

#include "object.h"
#include "print.h"
#include "thread.h"
#include "io.h"

#include <signal.h>
#include <semaphore.h>

void app_init(int argc, char **argv);
int init(int argc, char **argv);
void loop(void);

struct object_information object_container[object_class_type_unknown];

#endif
