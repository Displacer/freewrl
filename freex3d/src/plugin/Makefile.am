# FreeWRL plugin
#

# Get global variables for Makefile defined by toplevel Makefile.globals
include $(top_srcdir)/Makefile.globals

# Just add variables tested and checked by toplevel configure

# General libraries used with all platforms
AM_CPPFLAGS += \
				$(PLUGIN_CFLAGS)

EXTERNAL_LIBS = \
				$(PLUGIN_LIBS)


# X11 flags specific to x11 target (default on Linux -- optional on Mac)
if WINDOW_USE_AQUA
ERR=Error: Build system does not currently support Mac Safari Target
echo $(ERR)
else
AM_CPPFLAGS += \
				$(X_CFLAGS) 
endif

_EV=$(top_srcdir)/versions/PLUGIN
export

templ=$(top_srcdir)/versions/template/version.c.in
component=freewrl_plugin
PLUGIN_VERSION=`cat $$_EV`

BUILD_TIMESTAMP=`date +%F_%T`
AM_CPPFLAGS += -DBUILD_TIMESTAMP=\"$(BUILD_TIMESTAMP)\"

# Generate the version source file
internal_version.c: $(_EV)
	$(top_srcdir)/vtempl $(component) \
	$(PLUGIN_VERSION) < $(templ) > $@
# And add it to the list of files to clean
CLEANFILES=internal_version.c

newversion:
	-rm -f internal_version.*
	$(MAKE)

LIBFREEWRL_SRCDIR=$(top_srcdir)/src/lib
LIBFREEWRL_BUILDDIR=$(top_builddir)/src/lib

# I'm removing external dependency from the plugin
# not needed: AM_CPPFLAGS += -I$(LIBFREEWRL_SRCDIR)

# libtool does not help much here, I'll end using gcc -shared
# if we don't find a smart way to instruct libtool...

plugin_LTLIBRARIES = libFreeWRLplugin.la

libFreeWRLplugin_la_LIBTOOLFLAGS = --tag=disable-static

# This makes the plugin installs in $libdir/mozilla/plugins by default...
plugindir=$(PLUGIN_DIR)

libFreeWRLplugin_la_LDFLAGS = -no-undefined -module -avoid-version $(AM_LDFLAGS)
# Im' removing external dependency from the plugin
# not needed: libFreeWRLplugin_la_LIBADD = $(LIBFREEWRL_BUILDDIR)/libFreeWRL.la $(EXTERNAL_LIBS)
# this last variable would be set with required libs for mozilla

include Makefile.sources

EXTRA_DIST = $(DIST_sources)

SOURCES_common = plugin_main.c \
		system.h \
		plugin_utils.h

libFreeWRLplugin_la_SOURCES = $(SOURCES_common)

# Mozilla UNIX support
if WINDOW_USE_X11
libFreeWRLplugin_la_SOURCES += npunix.c
else
if WINDOW_USE_MOTIF
libFreeWRLplugin_la_SOURCES += npunix.c
endif
endif

libFreeWRLplugin_la_SOURCES += internal_version.c

##
## This was here in order to attempt to work-around a "make distcheck" issue,
## and is no longer necessary
#register-plugin:
#	mkdir -p $(DESTDIR)$(libdir)
#	mkdir -p $(DESTDIR)$(PLUGIN_DIR)
#	if test -w $(DESTDIR)$(PLUGIN_DIR); then \
#		cd $(DESTDIR)$(PLUGIN_DIR); \
#		ln -s $(libdir)/$(FREEWRL_PLUGIN); \
#	fi
#
#install-exec-am: register-plugin
