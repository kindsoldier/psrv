
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

error_t srv_readconf(srv_t *srv);
error_t srv_init(srv_t *srv);
error_t srv_fork(srv_t *srv);
error_t srv_run(srv_t *srv);

error_t srv_openlog(srv_t *srv);
error_t srv_closelog(srv_t *srv);

error_t srv_writepid(srv_t *srv);
error_t srv_writelog(srv_t *srv, char * message);

srv_t* new_srv();

#endif
