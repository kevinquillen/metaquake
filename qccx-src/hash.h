#ifndef HASH_H
#define HASH_H

//
// todo:
//
// - Try to reuse temps holding addresses/indirect loads within same function
//

struct hash_element 
{
	struct hash_element *next;
	def_t *def;
};

#define HSIZE1 16384

// JPG
#define OPTABLESIZE 128

extern struct hash_element *htable[HSIZE1];
extern void inithash();
extern int optable[OPTABLESIZE]; // JPG


extern int stats[HSIZE1];

inline static unsigned int hash(const char *string)
{
	unsigned int index = 0;
	int count;
	
	for (count = 0; string[count] != '\0' && count < 15; count++)
		index = (index << 1) ^ string[count];
	
	return index & (HSIZE1 - 1);
}

#endif

void HashImmediate (void);

