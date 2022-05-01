
#ifndef APP_H
#define APP_H

#include "common.h"

typedef struct {
    const char *rundir;
    const char *logdir;
    const char *confdir;
    int logfd;
} srv_t;

extern srv_t* psrv;

error_t srv_init(srv_t *srv);
error_t srv_fork(srv_t *srv);

error_t srv_closelog(srv_t *srv);
error_t srv_openlog(srv_t *srv);
error_t srv_print(srv_t *srv);
error_t srv_write_pid(srv_t *srv);
error_t srv_writelog(srv_t *srv, char * message);
error_t srv_readconf(srv_t *srv);

srv_t* new_srv();

#endif
