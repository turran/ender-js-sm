
lib_LTLIBRARIES += src/lib/libender_js_sm.la

installed_headersdir = $(pkgincludedir)-$(VMAJ)
dist_installed_headers_DATA = \
src/lib/Ender_Js_Sm.h

src_lib_libender_js_sm_la_SOURCES = \
src/lib/ender_js_sm.c \
src/lib/ender_js_sm_lib.c \
src/lib/ender_js_sm_lib_private.h \
src/lib/ender_js_sm_string.c \
src/lib/ender_js_sm_string_private.h

src_lib_libender_js_sm_la_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
@ENDER_JS_SM_CFLAGS@

src_lib_libender_js_sm_la_LIBADD = \
@ENDER_JS_SM_LIBS@

src_lib_libender_js_sm_la_LDFLAGS = -no-undefined -version-info @version_info@
