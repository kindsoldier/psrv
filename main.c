
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

#include <locale.h>
//#include <wchar.h>
//#include <wctype.h>

#include "common.h"
#include "config.h"
#include "wstring.h"
#include "service.h"


error_t logdebug(char *message) {
    error_t error;
    if (psrv != NULL) {
        return srv_writelog(psrv, message);
    }
    return error;
}

int main(int argc, char **argv) {
    error_t error = false;

    char* lctype = NULL;
    if ((lctype = getenv("LC_CTYPE")) != NULL) {
        setlocale(LC_CTYPE, lctype);
    }

    char* lcall = NULL;
    if ((lcall = getenv("LC_ALL")) != NULL) {
        setlocale(LC_ALL, lcall);
    }

    srv_t srv;
    psrv = &srv;

    srv_init(psrv);
    srv_readconf(psrv);

    error = srv_openlog(psrv);
    if (error != false) {
        return error_exit;
    }

    srv_fork(psrv);

    error = srv_writepid(psrv);
    if (error != false) {
        return error_exit;
    }

    error = srv_writepid(psrv);
    if (error != false) {
        return error_exit;
    }

    error = srv_run(psrv);
    if (error != false) {
        return error_exit;
    }

    error = srv_closelog(psrv);
    if (error != false) {
        return error_exit;
    }
    return success_exit;
}
