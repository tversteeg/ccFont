# Generic makefile for libraries

# Custom variables
NAME=ccFont
VERSIONMAYOR=1

SOURCEDIR:=src/$(NAME)
LIBDIR=lib
BINDIR=bin
INCDIR=include
TESTDIR=test
UTILDIR=utils

CFLAGS:=-I$(INCDIR) -g -fPIC -O3 -DCC_USE_ALL
LDLIBS=-lm
SRCS:=$(wildcard ./$(SOURCEDIR)/*.c)

# Generated
OBJS:=$(subst .c,.o,$(SRCS))
MAKEFILEDIR:=$(dir $(abspath $(lastword $(MAKEFILE_LIST))))

DLIBNAME:=lib$(NAME).so
DLIBFILE:=$(LIBDIR)/$(DLIBNAME)
SLIBNAME:=lib$(NAME).a
SLIBFILE:=$(LIBDIR)/$(SLIBNAME)
ILIBDIR:=$(DESTDIR)/usr/lib
ILIBFILE:=$(ILIBDIR)/$(DLIBNAME)
IINCDIR:=$(DESTDIR)/usr/include

DYNAR:=$(CC) $(LDFLAGS) -g -shared -Wl,-soname,$(DLIBNAME).$(VERSIONMAYOR) $(LDLIBS) -o
STATAR=ar rcs
RM=rm -f

all: $(DLIBFILE).$(VERSIONMAYOR) $(SLIBFILE) utils

$(SLIBFILE): $(OBJS)
	@mkdir -p $(LIBDIR)
	$(STATAR) $(SLIBFILE) $(OBJS)

$(DLIBFILE).$(VERSIONMAYOR) $(DLIBFILE): $(OBJS)
	@mkdir -p $(LIBDIR)
	$(DYNAR) $(DLIBFILE).$(VERSIONMAYOR) $(OBJS)
	ln -sf $(abspath $(DLIBFILE).$(VERSIONMAYOR)) $(DLIBFILE)

.PHONY: test
test: $(DLIBFILE).$(VERSIONMAYOR)
	@(cd $(TESTDIR); $(MAKE) INCDIR="$(MAKEFILEDIR)$(INCDIR)" LIBDIR="$(MAKEFILEDIR)$(LIBDIR)" LIBNAME="$(NAME)")

.PHONY: testrun
testrun: $(DLIBFILE).$(VERSIONMAYOR)
	@(cd $(TESTDIR); $(MAKE) run INCDIR="$(MAKEFILEDIR)$(INCDIR)" LIBDIR="$(MAKEFILEDIR)$(LIBDIR)" LIBNAME="$(NAME)")

.PHONY: utils
utils: $(DLIBFILE).$(VERSIONMAYOR)
	@(cd $(UTILDIR); $(MAKE) BINDIR="$(MAKEFILEDIR)$(BINDIR)" INCDIR="$(MAKEFILEDIR)$(INCDIR)" LIBDIR="$(MAKEFILEDIR)$(LIBDIR)" LIBNAME="$(NAME)")

.PHONY: clean
clean:
	$(RM) $(OBJS) $(DLIBFILE).$(VERSIONMAYOR) $(DLIBFILE)
	@(cd $(TESTDIR); $(MAKE) clean)
	@(cd $(UTILDIR); $(MAKE) clean)

.PHONY: install
install:
	mkdir -p $(IINCDIR)
	cp -R $(INCDIR)/* $(IINCDIR)
	mkdir -p $(ILIBDIR)
	cp $(SLIBFILE) $(ILIBDIR)
	cp $(DLIBFILE).$(VERSIONMAYOR) $(ILIBDIR)
	ln -sf $(DLIBNAME).$(VERSIONMAYOR) $(ILIBFILE)

.PHONY: dist-clean
dist-clean: clean
	$(RM) *~ .depend
	@(cd $(TESTDIR); $(MAKE) dist-clean)
	@(cd $(UTILDIR); $(MAKE) dist-clean)

.depend: $(SRCS)
	$(RM) ./.depend
	$(CC) $(CFLAGS) -MM $(SRCS) >>./.depend;

include .depend
