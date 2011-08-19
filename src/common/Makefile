include ../baserules.mk

SOURCE = $(wildcard *.c) $(wildcard class/*.c) $(wildcard games/ctf/*.c) $(wildcard magic/*.c) $(wildcard weapons/*.c)

QC = $(SOURCE:.c=.qc)

all: $(QC)


clean:
	rm -f *.qc class/*.qc games/ctf/*.qc magic/*.qc weapons/*.qc
