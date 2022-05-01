/* $Id$ */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <limits.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#include <ctype.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>

#include "common.h"
#include "config.h"
#include "wstring.h"

#include "service.h"

srv_t* psrv = NULL;

const mode_t rundir_mode = 0777;
const mode_t pidfile_mode = 0666;
const char *pidfile = "pid";

const mode_t logdir_mode = 0777;
const mode_t logfile_mode = 0640;
const char *logfile = "log";

const char* conffile = "c2srv.conf";
#define MAX_LINE_SIZE 1024

error_t srv_init(srv_t *srv) {
    error_t error = false;
    srv->rundir = srv_rundir;
    srv->logdir = srv_logdir;
    srv->confdir = srv_confdir;
    srv->logfd = fcntl(STDOUT_FILENO, F_DUPFD, 0);
    return error;
}

error_t srv_print(srv_t *srv) {
    error_t error = false;
    printf("%s\n", srv->rundir);
    return error;
}

error_t srv_writepid(srv_t *srv) {
    error_t error = false;
    int res;

    char pfile[PATH_MAX];
    memset(pfile, 0, PATH_MAX);
    sprintf(pfile, "%s/%s", srv->rundir, pidfile);

    mkdir(srv->rundir, rundir_mode);

    pid_t pid = getpid();
    char pidstr[NAME_MAX];
    memset(pidstr, 0, NAME_MAX);
    sprintf(pidstr, "%d", pid);

    int fd = open(pfile, O_WRONLY|O_CREAT|O_TRUNC, pidfile_mode);
    if (fd < 0) {
        error = true;
        return error;
    }
    ssize_t wsize = write(fd, pidstr, strlen(pidstr));
    if (wsize != strlen(pidstr)) {
        error = true;
        close(fd);
        return error;
    }
    close(fd);
    return error;
}

error_t srv_openlog(srv_t *srv) {
    error_t error = false;
    int res = 0;

    mkdir(srv->logdir, logdir_mode);

    char pfile[PATH_MAX];
    memset(pfile, 0, PATH_MAX);
    sprintf(pfile, "%s/%s", srv->logdir, logfile);

    int fd = open(pfile, O_WRONLY|O_CREAT|O_TRUNC, logfile_mode);
    if (fd < 0) {
        error = true;
        return error;
    }
    srv->logfd = fd;

    return error;
}

error_t srv_closelog(srv_t *srv) {
    error_t error = false;
    close(srv->logfd);
    return error;
}

#define MAX_TS_LEN 256

const char* logsep = " ";
const char* loglim = "\n";

error_t srv_writelog(srv_t *srv, char * message) {
    error_t error = false;

    time_t now = time(NULL);
    //if (rawtime < 0) {
    //}

    struct tm *ptm = localtime(&now);
    //if (ptm == NULL) {
    //}

    struct timespec tv;
    clock_gettime(CLOCK_REALTIME_PRECISE, &tv);

    char timestamp[MAX_TS_LEN];
    memset(timestamp, 0, MAX_TS_LEN);
    sprintf(timestamp, "%04d-%02d-%02dT%02d:%02d:%02d.%ld+%s",
        ptm->tm_year + 1900,
        ptm->tm_mon + 1,
        ptm->tm_mday,
        ptm->tm_hour,
        ptm->tm_min,
        ptm->tm_sec,
        tv.tv_nsec,
        ptm->tm_zone);

    write(srv->logfd, timestamp, strlen(timestamp));
    write(srv->logfd, logsep, strlen(logsep));
    write(srv->logfd, message, strlen(message));
    write(srv->logfd, loglim, strlen(loglim));

    return error;
}


error_t srv_fork(srv_t *srv) {
    error_t error = false;
    int res;

    res = fork();

    if (res < 0) {
        srv_writelog(srv, "fork error");
        error = true;
        return error;
    }

    if (res != 0) {
        srv_writelog(srv, "fork srv");
        exit(success_exit);
    }

    res = setsid();
    if (res < 0) {
        srv_writelog(srv, "setsid error");
        error = true;
        return error;
    }

    dup2(srv->logfd, STDOUT_FILENO);
    dup2(srv->logfd, STDERR_FILENO);

    return error;
}

error_t srv_readconf(srv_t *srv) {
    error_t error = false;
    int res = 0;

    char pfile[PATH_MAX];
    memset(pfile, 0, PATH_MAX);
    sprintf(pfile, "%s/%s", srv->confdir, conffile);

    int conffd = open(pfile, O_RDONLY, 0);
    if (conffd < 0) {
        return error;
    }

    char c = '\0';
    int pos = 0;
    char line[MAX_LINE_SIZE ];
    memset(line, '\0', MAX_LINE_SIZE);

    while ((res = read(conffd, &c, 1)) > 0) {
        line[pos++] = c;
        if (c == '\n') {
            char** words = NULL;
            int wcount = csplitstr(line, &words, 5, "=;#\r");

            char* key = NULL;
            char* val = NULL;
            if (words[0] != NULL) {
                key = sptrim(words[0]);
            }
            if (words[1] != NULL) {
                val = sptrim(words[1]);
            }
            if (key != NULL && val != NULL) {
                if (strcmp(key, "logdir") == 0) {
                    srv->logdir = val;
                }
            }

            free(key);
            free(val);
            for (int i = 0; i < wcount; i++) {
                if (words[i] != NULL) {
                    free(words[i]);
                }
            }
            free(words);

            memset(line, '\0', MAX_LINE_SIZE);
            pos = 0;
        };
    }
    return error;
}

typedef struct {
    int size;
    pthread_t* ref;
} parray_t;

parray_t parray_init(int size) {
    parray_t array;
    array.ref = malloc(sizeof(pthread_t*) * size);
    if (array.ref == NULL) return array;
    for (int i = 0; i < size; i++) {
        array.ref[i] = NULL;
    }
    array.size = size;
    return array;
}

void parray_add(parray_t* array, pthread_t* item) {
    for (int i = 0; i < array->size; i++) {
        if (array->ref[i] == NULL) {
            array->ref[i] = *item;
            return;
        }
    }
}

void parray_free(parray_t* array) {
    free(array->ref);
}


const int port = 5000;
const int backlog = 10;

typedef struct {
    int sock;
} hello_args_t;


void* hello(void* args) {

    hello_args_t* hargs = (hello_args_t*)args;
    srv_writelog(psrv, "thr start");

    struct timeval tv;
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    int res = setsockopt(hargs->sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));
    if (res < 0) {
        return NULL;
    }

    char message[MAX_LINE_SIZE];
    memset(message, '\0', MAX_LINE_SIZE);
    sprintf(message, "write to %d", hargs->sock);
    srv_writelog(psrv, message);

    char *hello_msg = "hello\n";
    write(hargs->sock, hello_msg, strlen(hello_msg));

    close(hargs->sock);


    return NULL;
}


error_t srv_run(srv_t *srv) {
    error_t error = false;

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
        return error;
    }

    int optval = 1;
    int res = 0;
    res = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if (res < 0) {
        return error;
    }

    struct sockaddr addr;
    struct sockaddr_in* paddr = (struct sockaddr_in*)&addr;
    paddr->sin_family = AF_INET;
    paddr->sin_addr.s_addr = INADDR_ANY;
    paddr->sin_port = htons(port);
    paddr->sin_len = sizeof(struct sockaddr_in);

    res = bind(sock, (struct sockaddr*)paddr, paddr->sin_len);
    if (res < 0) {
        return error;
    }
    res = listen(sock, backlog);
    if (res < 0) {
        return error;
    }

    while (true) {
        int newsock = 0;
        if ((newsock = accept(sock, NULL, 0)) > 3) {
            pthread_t thread;
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            hello_args_t args;
            args.sock = newsock;
            res = pthread_create(&thread, &attr, hello, &args);
            void* thres = NULL;
            res = pthread_join(thread, (void**)&thres);
        }
    }
    close(sock);

    return error;
}
