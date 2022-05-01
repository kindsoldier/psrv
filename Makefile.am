
AUTOMAKE_OPTIONS = foreign no-dependencies no-installinfo

SUFFIXES = .c .o
AM_CFLAGS = -Wall
AM_LDFLAGS = -pthread

.c.o:
	$(CC) -I./ -pthread $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

#config.h: config.h.in
#	./config.status $@

sbin_PROGRAMS = c2srv
c2srv_SOURCES = main.c service.c common.c wstring.c

c2srv_SOURCES += \
	service.h \
	wstring.h \
	common.h

clean-local:
	rm -rf autom4te.cache

install-data-local:
	test -z $(DESTDIR)$(APP_LIBDIR) || $(MKDIR_P) $(DESTDIR)$(APP_LIBDIR)
	test -z $(DESTDIR)$(APP_CONFDIR) || $(MKDIR_P) $(DESTDIR)$(APP_CONFDIR)
	test -z $(DESTDIR)$(APP_LOGDIR) || $(MKDIR_P) $(DESTDIR)$(APP_LOGDIR)
	test -z $(DESTDIR)$(APP_RUNDIR) || $(MKDIR_P) $(DESTDIR)$(APP_RUNDIR)
	test -z $(DESTDIR)$(APP_DATABASEDIR) || $(MKDIR_P) $(DESTDIR)$(APP_DATABASEDIR)
#EOF
