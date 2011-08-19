include ../baserules.mk
SOURCE = $(wildcard *.c)
QC = $(SOURCE:.c=.qc)

all: ../../progs.dat

progs.src: progs.c
	gcc -E -P -o progs.src progs.c $(CFLAGS)

../../progs.dat: progs.src $(QC)
	../../qcc 

clean:
	rm -f progs.src *.qc ../../progs.dat
