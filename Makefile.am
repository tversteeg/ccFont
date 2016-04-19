#ACLOCAL_AMFLAGS = -I m4 --install
AM_CPPFLAGS += -I$(top_srcdir)/include

pkginclude_HEADERS = include/ccFont/ccFont.h

lib_LTLIBRARIES = libccFont.la
libccFont_la_LDFLAGS = -version-info $(CCFONT_VERSION)
libccFont_la_SOURCES = src/ccFont/ccFont.c
libccFont_la_SOURCES += src/ccFont/stb_truetype.h

if BUILD_TESTS
check_PROGRAMS = check_ccFont
check_ccFont_SOURCES = test/test.c
check_ccFont_CPPFLAGS = $(AM_CPPFLAGS) -D_DEBUG
check_ccFont_CFLAGS = @CHECK_CFLAGS@
check_ccFont_LDADD = @CHECK_LIBS@ $(top_builddir)/.libs/libccFont.la

TESTS = check_ccFont
endif

bin_PROGRAMS = bin/ccfconv
bin_ccfconv_SOURCES = utils/ccfconv.c
bin_ccfconv_LDADD = $(top_builddir)/.libs/libccFont.la
