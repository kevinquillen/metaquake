all: progs.src qwprogs.dat

progs.qc:
	gcc -E -P -o progs.qc progs.c -DQUAKEWORLD -DGAME_CTF

progs.src: progs.qc
	mv progs.qc progs.src

qwprogs.dat: progs.src
	../qcc

clean:
	rm -f progs.src progs.qc qwprogs.dat
