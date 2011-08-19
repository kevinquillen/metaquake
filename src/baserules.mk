# Unprocessed QC (*.c) to processed QC (*.qc)
%.qc: %.c
	gcc -E -P $< -o $@ $(CFLAGS)