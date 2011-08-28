#
# Makefile for Metamorphism; (C) 2007 Lon Hohberger
# Updated by Patrick Baggett 2011

COPYDIR ?= ~/Quake/meta/
QCC ?= qccx

all: $(QCC) progs.dat

$(QCC):
	make -C $(QCC)-src all
	ln -s $(QCC)-src/$(QCC) .

clean:
	make -C src/common clean
	make -C src/quake clean
	make -C src/qw clean
	rm -f qwprogs.dat progs.dat

qwprogs.dat: $(QCC)
	for f in `find Common -type f -name "*.c"`; do \
		gcc -E -P $$f -o $${f/.c/.qc}  -DGAME_CTF -DQUAKEWORLD ;\
	done
	make -C srcQW

progs.dat: $(QCC) force_look
	-rm progs.dat
	make -C src/common CFLAGS="-I../../include -DQUAKE -DGAME_CTF"
	make -C src/quake QCC=../../$(QCC) CFLAGS="-I../../include -DQUAKE -DGAME_CTF"
	cp progs.dat $(COPYDIR)

force_look:
	@true
