#include <stdlib.h>
#include <stdlib.h>
void read_til_crnl(FILE *);
void process_rq(/* arguments */char * q, int fd);
void header(FILE *, char *);
void cannot_do(int);
void do_404(char * , int);
bool isadir(char*);
bool not_exist(char *);
void do_ls(char *, int );
char * file_type(char *);
bool ends_in_cgi(char *);
void do_exec(char*, int);
void do_cat(char *, int);
