
INCLUDES = 

lib_LTLIBRARIES = libavt.la

libavtincludedir = $(includedir)/avt
libavt_la_LDFLAGS = 	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)	\
			-release $(LT_RELEASE) 					\
			-export-dynamic

libavtinclude_HEADERS = avt.h
libavt_la_SOURCES = avt.c

DEPS = libavt.la
LDADDS = libavt.la

noinst_PROGRAMS = avt_test
avt_test_SOURCES = avt_test.c
avt_test_DEPENDENCIES = $(DEPS)
avt_test_LDADD = $(LDADDS)

.PHONY: test

test: avt_test
	for n in 2 10 100 1000 10000; do \
		builddir=`pwd`; cd $(top_builddir); top_builddir=`pwd`; \
		cd $$builddir; cd $(srcdir); \
		$(SHELL) $$top_builddir/libtool --mode=execute $$builddir/avt_test $$n; \
	done
