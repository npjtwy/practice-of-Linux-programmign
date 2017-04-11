#include    <time.h>
#include    <stdio.h>
#include    <stdint.h>
#include    <stdlib.h>
#include    <unistd.h>
#include    <string.h>
#include    <dirent.h>
#include    <sys/stat.h>
#include    <sys/types.h>
#include    <sys/ioctl.h>

#define MAXPATHLEN  4096

typedef struct mc_col   mc_Col; /* 分栏算法所用结构 */
typedef struct mc_dir   mc_Dir; /* 存放一个目录所有信息的结构 */
typedef struct mc_file  mc_File; /* 存放目录下的每个文件的信息的结构 */

struct mc_col{
    int32_t cnt; /* 栏数 */
    int32_t *lsp; /* 每栏所占用的最大宽度 */
};

struct mc_dir{
    int32_t cnt; /* 目录下的文件总数 */
    int32_t iwid; /* inode的最大宽度 */
    int32_t lwid; /* links的最大宽度 */
    int32_t uwid; /* uid的最大宽度 */
    int32_t gwid; /* gid的最大宽度 */
    int32_t swid; /* size(dev)的最大宽度 */
    mc_File *fp; /* 指向各个文件的详细信息的数组 */
};

struct mc_file{
    mc_Dir  *dp;    /* 如果该文件是目录则此项不为NULL，否则此项总为NULL */
    int32_t wid; /* 文件名的长度，不包括'\0' */
    int32_t uid;
    int32_t gid;
    int32_t dev;
    int32_t mode;
    int32_t links;
    int64_t ino;
    int64_t size;
    int64_t atime;
    int64_t mtime;
    int64_t ctime;
    char    name[MAXNAMLEN + 1];
};

static int32_t  col;    /* 终端的宽度－列数 */
static int32_t  aflg;
static int32_t  cflg;
static int32_t  uflg;
static int32_t  Aflg;
static int32_t  iflg;
static int32_t  Lflg;
static int32_t  Rflg;
static int32_t  Uflg;
static int32_t  (*Printfunc)(mc_Dir *);
static int32_t  (*Cmpfunc)(const void *, const void *);

static int32_t
digitlen(int64_t n)
{
    char    buf[BUFSIZ];

    snprintf(buf, BUFSIZ, "%lld", n);
    return strlen(buf);
}

static void
modetostr(int32_t mode, char *buf)
{
    strcpy(buf, "----------");
    if(S_ISDIR(mode))
        buf[0] = 'd';
    if(S_ISCHR(mode))
        buf[0] = 'c';
    if(S_ISBLK(mode))
        buf[0] = 'b';
    if(S_ISLNK(mode))
        buf[0] = 'l';
    if(S_ISFIFO(mode))
        buf[0] = 'p';
    if(mode & S_IRUSR)
        buf[1] = 'r';
    if(mode & S_IWUSR)
        buf[2] = 'w';
    if(mode & S_IXUSR)
        buf[3] = 'x';
    if(mode & S_ISUID)
        buf[3] = 's';
    if(mode & S_IRGRP)
        buf[4] = 'r';
    if(mode & S_IWGRP)
        buf[5] = 'w';
    if(mode & S_IXGRP)
        buf[6] = 'x';
    if(mode & S_ISGID)
        buf[6] = 's';
    if(mode & S_IROTH)
        buf[7] = 'r';
    if(mode & S_IWOTH)
        buf[8] = 'w';
    if(mode & S_IXOTH)
        buf[9] = 'x';
    if(mode & S_ISVTX)
        buf[9] = 't';
}

static void
printtime(int32_t t)
{
    char    buf[BUFSIZ];
    struct tm   *tp;

    if((tp = gmtime((time_t *)&t)) == NULL){
        snprintf(buf, BUFSIZ, "Wrong time");
    }else
        snprintf(buf, BUFSIZ, "%d.%d.%d %d:%d:%d",
            tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday, tp->tm_hour, tp->tm_min, tp->tm_sec);
    printf("%-18s ", buf);
}

/*
 * 分栏算法，算法尽可能的采用最少的行数
 * 成功返回指向分栏结构的指针，失败返回NULL
 */
static mc_Col   *
divcol(mc_Dir *dp)
{
    mc_Col  *cp;
    mc_File *fp;
    int32_t r, c; /* r为行数，c为列数 */
    int32_t i, j, w, l;

    if(dp == NULL || (fp = dp->fp) == NULL)
        return NULL;
        for(r = 1; ; r++){ /* 不断增加行数 */
                l = 0; /* 行长 */
                c = dp->cnt / r;
                for(i = 0; i < c; i++){
                        w = 0;
                        for(j = i; j < dp->cnt; j += c){ /* 得到该列长度最长的文件名的长度 */
                                if(fp[j].wid > w)
                                        w = fp[j].wid;
                        }
                        l += w + 1; /* 计算出最大的行长 */
                }
                if(l < col) /* 最大行长小于终端的行长度，满足退出 */
                        break;
        }
        if((cp = malloc(sizeof(mc_Col))) == NULL)
                return NULL;
        cp->cnt = c;
        if((cp->lsp = malloc(c * sizeof(int32_t))) == NULL){
                free(cp);
                return NULL;
        }
        for(i = 0; i < c; i++){ /* 记录每列最大的文件长度以方便打印 */
                w = 0;
                for(j = i; j < dp->cnt; j += c){
                        if(fp[j].wid > w)
                                w = fp[j].wid;
                }
                cp->lsp[i] = w;
        }
        return cp;
}

static int32_t
freecol(mc_Col *cp)
{
    if(cp == NULL)
        return -1;
    if(cp->lsp)
        free(cp->lsp);
    free(cp);
    return 0;
}

static int32_t
printone(mc_Dir *dp)
{
        int32_t    i;

    if(dp == NULL || dp->fp == NULL)
        return -1;
        if(dp->cnt == 0){
                putchar('\n');
                return 0;
        }
        for(i = 0; i < dp->cnt; i++){
                if(iflg)
                        printf("%-*lld ", dp->iwid, dp->fp[i].ino);
                printf("%s\n", dp->fp[i].name);
        }
        if(Rflg == 0)
                return 0;
        for(i = 0; i < dp->cnt; i++){ /* 如果存在-R选项则递归输出所有文件 */
                if(strcmp(dp->fp[i].name, ".") == 0 || strcmp(dp->fp[i].name, "..") == 0)
                        continue;
                if(S_ISDIR(dp->fp[i].mode)){
                        printf("%s:\n", dp->fp[i].name);
                        printone(dp->fp[i].dp);
                }
        }
        return 0;
}

static int32_t
printcol(mc_Dir *dp)
{
        int32_t i;
        mc_Col  *cp;

        if(iflg)
                return printone(dp);
    if(dp == NULL || dp->fp == NULL)
        return -1;
        if(dp->cnt == 0){
                putchar('\n');
                return 0;
        }
        if((cp = divcol(dp)) == NULL)
                return -1;
        for(i = 0; i < dp->cnt; i++){
                printf("%-*s", cp->lsp[i % cp->cnt], dp->fp[i].name);
                if((i == 0 && dp->cnt > 1) || (i + 1) % cp->cnt){
                        putchar(' ');
                }else
                        putchar('\n');
        }
        if(dp->cnt % cp->cnt)
            putchar('\n');
        freecol(cp);
        if(Rflg == 0)
                return 0;
        for(i = 0; i < dp->cnt; i++){ /* 如果存在-R选项则递归输出所有文件 */
                if(strcmp(dp->fp[i].name, ".") == 0 || strcmp(dp->fp[i].name, "..") == 0)
                        continue;
                if(S_ISDIR(dp->fp[i].mode)){
                        printf("%s:\n", dp->fp[i].name);
                        printcol(dp->fp[i].dp);
                }
        }
        return 0;
}

static int32_t
printlong(mc_Dir *dp)
{
        int32_t i;
        char    buf[10];
        char    tmp[BUFSIZ];

        if(dp == NULL || dp->fp == NULL)
                return -1;
    if(dp->cnt == 0){
        putchar('\n');
        return 0;
    }
        for(i = 0; i < dp->cnt; i++){
                modetostr(dp->fp[i].mode, buf);
                if(iflg)
                        printf("%-*lld ", dp->iwid, dp->fp[i].ino);
                printf("%s %-*u %-*d %-*d ", buf, dp->lwid, dp->fp[i].links, dp->uwid, dp->fp[i].uid, dp->gwid, dp->fp[i].gid);
                if(S_ISCHR(dp->fp[i].mode) || S_ISBLK(dp->fp[i].mode)){
                        snprintf(tmp, BUFSIZ, "%d,%d ", major(dp->fp[i].dev), minor(dp->fp[i].dev));
                        printf("%-*s ", dp->swid, tmp);
                }else
                        printf("%-*lld ", dp->swid, dp->fp[i].size);
                if(cflg){
                        printtime(dp->fp[i].ctime);
                }else if(uflg){
                        printtime(dp->fp[i].atime);
                }else
                        printtime(dp->fp[i].mtime);
                printf("%s\n", dp->fp[i].name);
        }
        if(Rflg == 0)
                return 0;
        for(i = 0; i < dp->cnt; i++){ /* 如果存在-R选项则递归输出所有文件 */
                if(strcmp(dp->fp[i].name, ".") == 0 || strcmp(dp->fp[i].name, "..") == 0)
                        continue;
                if(S_ISDIR(dp->fp[i].mode)){
                        printf("%s:\n", dp->fp[i].name);
                        printlong(dp->fp[i].dp);
                }
        }
    return 0;
}

static void
copyinfo(mc_File *fp, const struct stat *sp, const char *pathname)
{
    fp->dp = NULL;
    fp->ino = sp->st_ino;
        fp->uid = sp->st_uid;
        fp->gid = sp->st_gid;
        fp->dev = sp->st_rdev;
        fp->mode = sp->st_mode;
        fp->size = sp->st_size;
        fp->atime = sp->st_atime;
        fp->mtime = sp->st_mtime;
        fp->ctime = sp->st_ctime;
        fp->links = sp->st_nlink;
    fp->wid = strlen(pathname);
    strcpy(fp->name, pathname);
}

static int
SortDirs(mc_Dir *dp, int (*cmp)(const void *, const void *))
{
    int32_t i;

    if(dp == NULL || dp->fp == NULL)
        return -1;
    for(i = 0; i < dp->cnt; i++){ /* 对可能存在的子目录也进行排序 */
        if(dp->fp[i].dp)
            SortDirs(dp->fp[i].dp, cmp);
    }
    qsort(dp->fp, dp->cnt, sizeof(mc_File), cmp);
    return 0;
}

/*
 * 释放目录结构所包含的所有信息
 * 成功返回0，失败返回-1
 */
static int
FreeDirs(mc_Dir *dp)
{
    int32_t i;

    if(dp == NULL || dp->fp == NULL)
        return -1;
    for(i = 0; i < dp->cnt; i++){ /* 释放所有可能存在的子目录 */
        if(dp->fp[i].dp)
            FreeDirs(dp->fp[i].dp);
    }
    free(dp->fp);
    free(dp);
    return 0;
}

/*
 * 将目录中的所有文件读入到数据结构中
 * 如果读取成功则返回指向结构的指针，如果读取失败则返回NULL
 */
static mc_Dir   *
OpenDirs(const char *pathname)
{
    int32_t w;
    DIR *dp;
    mc_Dir  *rtv;
    mc_File *tmp;
    struct stat stbuf;
    struct dirent   *dirp;
    char    home[MAXPATHLEN + 1];

    if(getcwd(home, MAXPATHLEN + 1) == NULL) /* 获取程序的工作目录 */
        return NULL;
    if(stat(pathname, &stbuf) == -1)
        return NULL;
    if((rtv = malloc(sizeof(mc_Dir))) == NULL)
        return NULL;
    memset(rtv, 0, sizeof(mc_Dir));
    if((rtv->fp = malloc(sizeof(mc_File))) == NULL){
        free(rtv);
        return NULL;
    }
    if(!S_ISDIR(stbuf.st_mode)){ /* 如果不是目录则直接获取文件信息即可 */
        rtv->cnt = 1;
        copyinfo(rtv->fp, &stbuf, pathname);
        return rtv;
    }
    if(chdir(pathname) == -1){ /* 切换到特定的目录 */
        free(rtv->fp);
        free(rtv);
        return NULL;
    }
    if((dp = opendir(".")) == NULL){
        free(rtv->fp);
        free(rtv);
        chdir(home);
        return NULL;
    }
    for(rtv->cnt = 0; dirp = readdir(dp); rtv->cnt++){
        if(dirp->d_name[0] == '.' && Aflg == 0 && aflg == 0){
                        rtv->cnt--;
                        continue;
                }
                if((strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0) && aflg == 0){
                        rtv->cnt--;
                        continue;
                }
        if((tmp = realloc(rtv->fp, (rtv->cnt + 1) * sizeof(mc_File))) == NULL){
            FreeDirs(rtv);
            chdir(home);
            closedir(dp);
            return NULL;
        }
        rtv->fp = tmp;
        if(stat(dirp->d_name, &stbuf) == -1){
            FreeDirs(rtv);
            chdir(home);
            closedir(dp);
            return NULL;
        }
        copyinfo(rtv->fp + rtv->cnt, &stbuf, dirp->d_name);
        if(iflg && (w = digitlen(rtv->fp[rtv->cnt].ino)) > rtv->iwid) /* 获取最大的inode号的宽度 */
                        rtv->iwid = w;
                if(Printfunc == printlong){
                        if((w = digitlen(rtv->fp[rtv->cnt].links)) > rtv->lwid) /* 获取最大的links的宽度 */
                                rtv->lwid = w;
                        if((w = digitlen(rtv->fp[rtv->cnt].uid)) > rtv->uwid) /* 获取最大的uid的宽度 */
                                rtv->uwid = w;
                        if((w = digitlen(rtv->fp[rtv->cnt].gid)) > rtv->gwid) /* 获取最大的gid的宽度 */
                                rtv->gwid = w;
                        if(S_ISCHR(stbuf.st_mode) || S_ISBLK(stbuf.st_mode)){
                                w = digitlen(major(rtv->fp[rtv->cnt].dev));
                                w += digitlen(minor(rtv->fp[rtv->cnt].dev)) + 1; /* +1加的是主设备与次设备间的.的宽度 */
                                if(w > rtv->swid)
                                        rtv->swid = w;
                        }else{
                                if((w = digitlen(rtv->fp[rtv->cnt].size)) > rtv->swid)
                                        rtv->swid = w;
                        }
                }
        if(Rflg == 0)
            continue;
        if(strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
                        continue; /* 程序不需要读取父目录和自身，不然会陷入死循环 */
        if(S_ISDIR(stbuf.st_mode) && (rtv->fp[rtv->cnt].dp = OpenDirs(dirp->d_name)) == NULL){ /* 读取子目录信息 */
            FreeDirs(rtv);
            chdir(home);
            closedir(dp);
            return NULL;
        }
    }
    chdir(home);
    closedir(dp);
    return rtv;
}

static int32_t
cmpname(const void *s1, const void *s2)
{
    return strcmp(((mc_File *)s1)->name, ((mc_File *)s2)->name);
}

static int32_t
cmpatime(const void *s1, const void *s2)
{
    return ((mc_File *)s1)->atime - ((mc_File *)s2)->atime;
}

static int32_t
cmpmtime(const void *s1, const void *s2)
{
    return ((mc_File *)s1)->mtime - ((mc_File *)s2)->mtime;
}

static int32_t
cmpctime(const void *s1, const void *s2)
{
    return ((mc_File *)s1)->ctime - ((mc_File *)s2)->ctime;
}

/*
 * 显示目录的内容
 * 命令格式： 如果不存在目录名则显示当前目录
 *      ls [选项] [目录名]
 * 支持的参数如下： ls命令默认根据文件名排序
 *      a－列出目录下的所有文件
 *      A－列出目录下除.和..的所有文件
 *      c－输出文件的ctime(文件状态最后的更改时间)，然后文件按照ctime排序输出
 *      U－不排序
 *      i－输出每个文件的inode节点
 *      l－输出文件的详细信息
 *      L－如果是符号链接则显示符号链接所指的文件的信息而非符号链接的信息
 *      t－以文件修改时间排序(mtime)，ls默认的显示时间为mtime
 *      u－输出文件的atime(文件最后的访问时间)，然后文件按照atime排序输出
 *      R－递归的输出目录的所有文件(包括子目录)
 * 显示格式0：非详细信息的显示格式
 *      文件0   文件1   文件2   文件3
 *      ...
 *      文件n
 *      尽可能的让分栏做到最合理
 * 显示格式1：详细信息的显示格式
 *      [inode号] 文件类型和读写信息 引用数 用户 组 文件大小(或者设备号) 日期 文件名
 */
int
main(int argc, char *argv[])
{
    mc_Dir  *dp;
    int i, c;
    struct winsize  siz;

    if(argc < 1 || argv == NULL){
        printf("Usage: ls [-aciltuALRU] [dir1] [dir2] ... [dirn]\n");
        return -1;
    }
        if(ioctl(STDOUT_FILENO, TIOCGWINSZ, (char *)&siz) == -1)
                return -1;
        col = siz.ws_col;
    Cmpfunc = cmpname;
    Printfunc = printcol;
    aflg = cflg = uflg = iflg = Lflg = Rflg = Uflg = 0;
    for(i = 1; i < argc; i++){
        if(argv[i][0] == '-'){
            while(c = *++argv[i]){
                switch(c){
                case 'a':
                    aflg = 1;
                    break;
                case 'A':
                    Aflg = 1;
                    break;
                case 'c':
                    cflg = 1;
                    uflg = 0;
                    Cmpfunc = cmpctime;
                    break;
                case 'U':
                    Cmpfunc = NULL;
                    break;
                case 'i':
                    iflg = 1;
                    break;
                case 'l':
                    Printfunc = printlong;
                    break;
                case 'L':
                    Lflg = 1;
                    break;
                case 't':
                    Cmpfunc = cmpmtime;;
                    break;
                case 'u':
                    uflg = 1;
                    cflg = 0;
                    Cmpfunc = cmpatime;
                    break;
                case 'R':
                    Rflg = 1;
                    break;
                default:
                    break;
                }
            }
        }else
            break;
    }
    c = i < argc - 1 ? 1 : 0; /* c置1表示需要打印每个需要输出的目录 */
    if(i == argc){ /* ls当前目录 */
        if(dp = OpenDirs(".")){
            if(Cmpfunc)
                SortDirs(dp, Cmpfunc); /* 排序目录中的所有文件 */
            Printfunc(dp);
            FreeDirs(dp);
            return 0;
        }
        printf("Error: cannot ls '.'\n");
        return -1;
    }
    for( ; i < argc; i++){
        if(dp = OpenDirs(argv[i])){
            if(c)
                printf("%s:\n", argv[i]);
            if(Cmpfunc)
                SortDirs(dp, Cmpfunc);
            Printfunc(dp);
            FreeDirs(dp);
        }else
            printf("Error: cannot ls '%s'\n", argv[i]);
        if(c)
            putchar('\n');
    }
    return 0;
}
