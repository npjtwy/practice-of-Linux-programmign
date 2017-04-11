/*
 * twordcount3.c
 *
 *  Created on: 2017年4月9日
 *      Author: yang
 * 使用结构体传递参数  避免对全局变量的依赖
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <ctype.h>

struct arg_set{
	char *fname;
	int count;
};

void *count_words(void *a);

int main(int argc, char **argv) {
	pthread_t 	t1,t2;
	struct arg_set args1, args2;
	if (argc != 3)
	{
		printf("usage: %s file1 file2", argv[0]);
		exit(1);
	}

	args1.fname = argv[1];
	args1.count = 0;
	pthread_create(&t1, NULL, count_words, (void*)& args1);

	args2.count = 0;
	args2.fname = argv[2];
	pthread_create(&t2, NULL, count_words, (void *) &args2);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	printf("%5d: %s\n",args1.count, argv[1] );
	printf("%5d: %s\n",args2.count, argv[2] );
	printf("%5d: total words\n", args1.count + args2.count);
}


void *count_words(void *a)
{
	struct arg_set *args = (struct arg_set *)a;
	FILE *fp;
	int c, prevc = '\0';

	if ((fp = fopen(args->fname, "r")) != NULL)
    {
        while( (c = getc(fp)) != EOF)
        {
            if (!isalnum(c) && isalnum(prevc))
            {
                args->count++;
            }
            prevc = c;
        }
        fclose(fp);
    }

    else
        perror(args->fname);
    return NULL;
}
