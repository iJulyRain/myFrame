/*
 * =====================================================================================
 *
 *       Filename:  tools.h
 *
 *    Description:  tools header file
 *
 *        Version:  1.0
 *        Created:  2014年02月28日 10时49分51秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (R&D), lzx1442@163.com
 *        Company:  wanwei-tech
 *
 * =====================================================================================
 */
#ifndef __TOOLS_H__
#define __TOOLS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>

#include <time.h>
#include <fcntl.h>
#include <poll.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

/**
* @brief 国标CRC校验 
*
* @param buf 参与校验的数据
* @param lenth 数据长度
*
* @return CRC校验码
*/
unsigned short crc16(char *buf, int lenth);

/**
* @brief CRC校验算法
*
* @param puchMsg 参与校验的数据
* @param usDataLen 参与校验的数据长度
*
* @return CRC校验码
*/
unsigned short modbus_crc(char *puchMsg, int usDataLen);

/**
* @brief 封装的read函数,Readn
*
* 用于读取固定的字节数，主要用于读取串口严重断包的情况
*
* @param fd 文件描述符，同read
* @param vptr 读缓冲区，同read
* @param n 缓冲区大小，同read
* @param data_len 预计读取的数据块大小
*
* @return 同read 
*/
int Readn(int fd, void *vptr, int n, int data_len);

/**
* @brief 获取时间 
*
* @param now unix时间
*
* @return 转化后的时间
*/
long long get_real_time(time_t now);

/**
* @brief 获取时间2  
*
* @param t 需要转化的格式化后的时间
*
*  20140101120000
*
* @return 转化后的time_t型的时间
*/
time_t get_real_time2(long long t);

/**
* @brief 读取指定结尾的数据
*
* @param fd 文件描述符
* @param vptr 读取缓冲
* @param maxlen 读取个数
*
* @return 成功返回读取个数，失败返回-1
*/
ssize_t Readline(int fd, void *vptr, size_t maxlen, const char tail);

/**
* @brief 获取系统当前分钟时间
*
* @param now 当前系统时间 
*
* @return 当前分钟时间
*/
time_t time_no_sec(time_t now);

/**
* @brief 获取系统当前小时时间
*
* @param now 当前系统时间
*
* @return 当前小时时间
*/
time_t time_no_min(time_t now);

/**
* @brief 获取系统当前日时间
*
* @param now 当前系统时间 
*
* @return 当前日时间 
*/
time_t time_no_hour(time_t now);

/**
* @brief 获取系统当前月时间 
*
* @param now 当前系统时间
*
* @return 当前月时间
*/
time_t time_no_day(time_t now);

/**
* @brief 获取系统整十分时间
*
* @param t 当前系统时间
*
* @return 整十分时间
*/
time_t time_10min(time_t t);

int get_sys_time(time_t now, char *systime);

int gbk_2_utf8(char *inbuf, size_t inlen, char *outbuf, size_t outlen);
int utf8_2_gbk(char *inbuf, size_t inlen, char *outbuf, size_t outlen);

#endif
