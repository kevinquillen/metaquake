# Makefile for Metamorphism; (C) 2007 Lon Hohberger
# Updated by Patrick Baggett 2011
# Updated by Kevin Quillen 2021
SHELL := bash
BUILDROOT ?= ./build
BUILDDIR ?= $(BUILDROOT)/meta
QCC ?= qccx

all: $(QCC) progs.dat

$(QCC):
	make -C $(QCC)-src all
	ln -s $(QCC)-src/$(QCC) .

clean:
	make -C src/common clean
	make -C src/quake clean
	make -C src/qw clean

progs.dat: $(QCC) force_look
	make -C src/common CFLAGS="-I../../include -DQUAKE -DGAME_CTF"
	make -C src/quake QCC=../../$(QCC) CFLAGS="-I../../include -DQUAKE -DGAME_CTF"
	mkdir -p $(BUILDDIR)
	mv progs.dat $(BUILDDIR)

qwprogs.dat: $(QCC)
	make -C src/common QCC=../../$(QCC) CFLAGS="-I../../include -DQUAKEWORLD -DGAME_CTF -DNO_SYSOP"
	make -C src/qw QCC=../../$(QCC) CFLAGS="-I../../include -DQUAKEWORLD -DGAME_CTF -DNO_SYSOP"
	mkdir -p $(BUILDDIR)
	mv qwprogs.dat $(BUILDDIR)

release: $(QCC) force_look
	make clean
	make progs.dat
	make clean
	make qwprogs.dat
	cp CHANGELOG.txt $(BUILDDIR)
	cp LICENSE.txt $(BUILDDIR)
	tar -zcvf ./metaquake.tar.gz -C $(BUILDROOT) meta

force_look:
	@true
