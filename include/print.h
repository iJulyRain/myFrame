/*
 * =====================================================================================
 *
 *       Filename:  print.h
 *
 *    Description:  debug interface 
 *
 *        Version:  1.0
 *        Created:  2014年09月19日 15时13分07秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (R&D), lzx1442@163.com
 *        Company:  wanwei-tech
 *
 * =====================================================================================
 */
#ifndef __PRINT_H__
#define __PRINT_H__

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <pthread.h>
#include <time.h>

//print level

#define DEBUG 0	///<print level debug
#define RELEASE 1 ///<print level release

/**
* @brief set print level to process
*
* @param plevel print level
*/
void set_print_level(int plevel);

/**
* @brief output information 
*
* @param plevel print level which set by user
* @param format format  
* @param ... ...
*
* @return always 0 
*/
int debug(int level, const char *format, ...); 

#endif
