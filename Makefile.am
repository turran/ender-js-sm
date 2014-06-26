AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4 -I common/m4

MAINTAINERCLEANFILES = \
Makefile.in \
aclocal.m4 \
build-stamp \
config.guess \
config.h \
config.h.in \
config.sub \
configure \
configure-stamp \
depcomp \
install-sh \
ltconfig \
ltmain.sh \
missing mkinstalldirs \
stamp-h.in

lib_LTLIBRARIES =
check_PROGRAMS =
EXTRA_DIST =

### Library

include src/lib/Makefile.mk

### Binaries

include src/bin/Makefile.mk

EXTRA_DIST += \
AUTHORS \
COPYING \
README.md \
ender_js_sm.pc.in \
m4/efl_dlfcn.m4 \
m4/ens_doxygen.m4
