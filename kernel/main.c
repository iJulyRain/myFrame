/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  main
 *
 *        Version:  1.0
 *        Created:  2014年09月22日 09时18分12秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (R&D), lzx1442@163.com
 *        Company:  wanwei-tech
 *
 * =====================================================================================
 */
#include "common.h"
#include "init.h"

int main(int argc, char **argv)
{
	int step = 0;

	fprintf(stdout, "version: %s build: %s %s @%s\n", VERSION, __DATE__, __TIME__, who(WHO));

#ifdef DAEMON_MODE
	daemon(1, 0);
#endif

	if((step = init(argc, argv)) != 0)
	{
		debug(RELEASE, "system initialize failed ont step: %d!\n", abs(step));
		return -1;
	}

	loop();	///<轮询
	
	return 0;
}
