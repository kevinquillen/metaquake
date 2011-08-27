include ../baserules.mk
SOURCE = $(wildcard *.c)
QC = $(SOURCE:.c=.qc)

ifeq ($(QCC),../../qcc)
	QCCFLAGS ?= -O2
endif

all: ../../progs.dat

progs.src: progs.c
	gcc -E -P -o progs.src progs.c $(CFLAGS)

../../progs.dat: progs.src $(QC)
	$(QCC) $(QCCFLAGS)

clean:
	rm -f progs.src *.qc ../../progs.dat
