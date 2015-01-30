#ifndef MONITOR_H
#define MONITOR_H

#include "common.h"

//在屏幕上输出一个字符
void monitor_put(char c);
//清屏
void monitor_clear();
//在屏幕上输出字符串
void monitor_str(char *c);
void monitor_hex(unsigned int c);
void monitor_dec(unsigned int c);
void printf(char *s,unsigned int c);
#endif
