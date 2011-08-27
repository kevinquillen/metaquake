#include "qcc.h"
#include "hash.h"

struct hash_element *htable[HSIZE1];
int optable[OPTABLESIZE];

int stats[HSIZE1];

void
inithash()
{
	int i=0;
	int index=0;

	for (i=0; i<HSIZE1; i++) 
	{
		htable[i] = NULL;
		stats[i] = 0;
	}

	// JPG - added optable
	for (i = 0 ; i < OPTABLESIZE ; i++)
		optable[i] = 0;
	for (i = 0 ; pr_opcodes[i].name ; i++)
	{
		if (pr_opcodes[i].priority > 0)
		{
			if (strcmp(pr_opcodes[i].name, pr_opcodes[i-1].name))
			{
				index = (pr_opcodes[i].name[0] + pr_opcodes[i].name[1]) & (OPTABLESIZE - 1);
				if (optable[index])
				{
					printf("conflict!\n");
					exit(1);
				}
				optable[index] = i;
			}
		}
	}
}

void HashImmediate (void)
{
	if (pr_immediate_type == &type_string) 
		pr_immediate_index = hash(pr_immediate_string);
	else if (pr_immediate_type == &type_float) 
	{
		char tmpchar[40];
		sprintf(tmpchar, "%d", pr_immediate._int);
		pr_immediate_index = hash(tmpchar);
	} 
	else if (pr_immediate_type == &type_vector) 
	{
		char tmpchar[80];
		sprintf(tmpchar, "%.4f,%.4f,%.4f", pr_immediate.vector[0], pr_immediate.vector[1], pr_immediate.vector[2]);
		pr_immediate_index = hash(tmpchar);
	} 
	else
		PR_ParseError ("weird immediate type");		
}

