NAME=ccFont

SOURCEDIR=src/$(NAME)
LIBDIR=lib
TESTDIR=test

CC=gcc
RM=rm -f
AR=ar rcs
CFLAGS=-O3 -Iinclude/
LDLIBS=-lGL -lGLU -lGLEW -lm
TESTCFLAGS=-g -Wall -DCC_USE_ALL
TESTLDLIBS=-lccore -lX11 -lXrandr -lXinerama -lXi -lpthread

SRCS=$(shell find $(SOURCEDIR) -name '*.c')
OBJS=$(subst .c,.o,$(SRCS))

all: $(NAME)

$(NAME): $(OBJS)
	$(AR) $(LIBDIR)/lib$(NAME).a

test: SRCS += $(shell find $(TESTDIR) -name '*.c')
test: CFLAGS += $(TESTCFLAGS)
test: LDLIBS += $(TESTLDLIBS)
OBJS=$(subst .c,.o,$(SRCS))

test: $(OBJS)
	$(CC) $(LDFLAGS) -o $(TESTDIR)/test $(OBJS) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CC) $(CFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)
	$(RM) -r $(DESTDIR)/usr/include/$(NAME)
	$(RM) -r $(DESTDIR)/usr/lib/lib$(NAME).a

dist-clean: clean
	$(RM) *~ .depend

install:
	mkdir -p $(DESTDIR)/usr/include
	cp -R include/* $(DESTDIR)/usr/include
	mkdir -p $(DESTDIR)/usr/lib
	cp -R lib/* $(DESTDIR)/usr/lib

include .depend
