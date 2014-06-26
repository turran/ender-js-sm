bin_PROGRAMS = \
src/bin/ender-js-sm-loader

src_bin_ender_js_sm_loader_LDADD = \
$(top_builddir)/src/lib/libender_js_sm.la \
@ENDER_JS_SM_LIBS@

src_bin_ender_js_sm_loader_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
@ENDER_JS_SM_CFLAGS@

src_bin_ender_js_sm_loader_SOURCES = \
src/bin/ender_js_sm_loader.c


