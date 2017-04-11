/*
*smsh.h 声明相关处理函数
*/

#ifndef _SMSH_H_
#define _SMSH_H_

#include <stdio.h>
#include <stdlib.h>

#define YES 1
#define NO  0

//从文件中读取命令
char *next_cmd(char *prompt, FILE *fp);

//处理字符串 返回命令列表
char** splitline(char *);

//释放 splitline 返回的列表
void freelist(char **);

//内存分配, 相比malloc 分配失败直接退出程序
void *emalloc(size_t);
void *erealloc(void *, size_t);

#endif //_SMSH_H_