all: build

build:
	scons

clean:
	scons -c

install:
	mkdir -p $(DESTDIR)/usr/include
	cp -R include/* $(DESTDIR)/usr/include
	mkdir -p $(DESTDIR)/usr/lib
	cp -R lib/* $(DESTDIR)/usr/lib
