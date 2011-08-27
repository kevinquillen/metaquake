#ifndef QCC_H
#define QCC_H

#include "cmdlib.h"
#include <stdio.h>
#include <setjmp.h>

#include "pr_comp.h"

#define STRCMP(s1,s2) (((*s1)!=(*s2)) || strcmp(s1,s2))
#define STRNCMP(s1,s2,n) (((*s1)!=(*s2)) || strncmp(s1,s2,n))

#ifdef WIN32
#pragma warning (disable:4244)
#endif

/*

TODO:

"stopped at 10 errors"

other pointer types for models and clients?

compact string heap?

allways initialize all variables to something safe

the def->type->type arrangement is really silly.

return type checking

parm count type checking

immediate overflow checking

pass the first two parms in call->b and call->c

*/

/*

comments
--------
// comments discard text until the end of line
/ *  * / comments discard all enclosed text (spaced out on this line because this documentation is in a regular C comment block, and typing them in normally causes a parse error)

code structure
--------------
A definition is:
	<type> <name> [ = <immediate>] {, <name> [ = <immediate>] };


types
-----
simple types: void, float, vector, string, or entity
	float		width, height;
	string		name;
	entity		self, other;

vector types:
	vector		org;	// also creates org_x, org_y, and org_z float defs
	
	
A function type is specified as: 	simpletype ( type name {,type name} )
The names are ignored except when the function is initialized.	
	void()		think;
	entity()	FindTarget;
	void(vector destination, float speed, void() callback)	SUB_CalcMove;
	void(...)	dprint;		// variable argument builtin

A field type is specified as:  .type
	.vector		origin;
	.string		netname;
	.void()		think, touch, use;
	

names
-----
Names are a maximum of 64 characters, must begin with A-Z,a-z, or _, and can continue with those characters or 0-9.

There are two levels of scoping: global, and function.  The parameter list of a function and any vars declared inside a function with the "local" statement are only visible within that function, 


immediates
----------
Float immediates must begin with 0-9 or minus sign.  .5 is illegal.
	
A parsing ambiguity is present with negative constants. "a-5" will be parsed as "a", then "-5", causing an error.  Seperate the - from the digits with a space "a - 5" to get the proper behavior.
	12
	1.6
	0.5
	-100

Vector immediates are three float immediates enclosed in single quotes.
	'0 0 0'
	'20.5 -10 0.00001'
	
String immediates are characters enclosed in double quotes.  The string cannot contain explicit newlines, but the escape character \n can embed one.  The \" escape can be used to include a quote in the string.
	"maps/jrwiz1.bsp"
	"sound/nin/pain.wav"
	"ouch!\n"

Code immediates are statements enclosed in {} braces.
statement:
	{ <multiple statements> }
	<expression>;
	local <type> <name> [ = <immediate>] {, <name> [ = <immediate>] };
	return <expression>;
	if ( <expression> ) <statement> [ else <statement> ];
	while ( <expression> ) <statement>;
	do <statement> while ( <expression> );
	<function name> ( <function parms> );
	
expression:
	combiations of names and these operators with standard C precedence:
	"&&", "||", "<=", ">=","==", "!=", "!", "*", "/", "-", "+", "=", ".", "<", ">", "&", "|"
	Parenthesis can be used to alter order of operation.
	The & and | operations perform integral bit ops on floats
	
A built in function immediate is a number sign followed by an integer.
	#1
	#12


compilation
-----------
Source files are processed sequentially without dumping any state, so if a 
defs file is the first one processed, the definitions will be available to 
all other files.

The language is strongly typed and there are no casts.

Anything that is initialized is assumed to be constant, and will have 
immediates folded into it.  If you change the value, your program will 
malfunction.  All uninitialized globals will be saved to savegame files.

Functions cannot have more than eight parameters.

Error recovery during compilation is minimal.  It will skip to the next 
global definition, so you will never see more than one error at a time in a 
given function.  All compilation aborts after ten error messages.

Names can be defined multiple times until they are defined with an 
initialization, allowing functions to be prototyped before their definition.

void()	MyFunction;			// the prototype

void()	MyFunction =		// the initialization
{
	dprint ("we're here\n");
};

entities and fields
-------------------

execution
---------
Code execution is initiated by C code in quake from two main places:  the 
timed think routines for periodic control, and the touch function when two 
objects impact each other.

There are three global variables that are set before beginning code execution:
	entity	world;		// the server's world object, which holds all global
					// state for the server, like the deathmatch flags
					// and the body ques.
	entity	self;		// the entity the function is executing for
	entity	other;		// the other object in an impact, not used for thinks
	float	time;		// the current game time.  Note that because the
					// entities in the world are simulated sequentially,
					// time is NOT strictly increasing.  An impact late
					// in one entity's time slice may set time higher
					// than the think function of the next entity. 
					// The difference is limited to 0.1 seconds.

Execution is also caused by a few uncommon events, like the addition of a 
new client to an existing server.
	
There is a runaway counter that stops a program if 100000 statements are 
executed, assuming it is in an infinite loop.

It is acceptable to change the system set global variables.  This is usually 
done to pose as another entity by changing self and calling a function.

The interpretation is fairly efficient, but it is still over an order of 
magnitude slower than compiled C code.  All time consuming operations should 
be made into built in functions.

A profile counter is kept for each function, and incremented for each 
interpreted instruction inside that function.  The "profile" console command 
in Quake will dump out the top 10 functions, then clear all the counters.
The "profile all" command will dump sorted stats for every function that has 
been executed.

afunc ( 4, bfunc(1,2,3));
will fail because there is a shared parameter marshaling area, which will 
cause the 1 from bfunc to overwrite the 4 allready placed in parm0.  When a 
function is called, it copies the parms from the globals into it's privately 
scoped variables, so there is no collision when calling another function.

total = factorial(3) + factorial(4);
Will fail because the return value from functions is held in a single global 
area.  If this really gets on your nerves, tell me and I can work around it 
at a slight performance and space penalty by allocating a new register for 
the function call and copying it out.


built in functions
------------------
void(string text)	dprint;
Prints the string to the server console.

void(entity client, string text)	cprint;
Prints a message to a specific client.

void(string text)	bprint;
Broadcast prints a message to all clients on the current server.

entity()	spawn;
Returns a totally empty entity.  You can manually set everything up, or just set the origin and call one of the existing entity setup functions.

entity(entity start, .string field, string match) find;
Searches the server entity list beginning at start, looking for an entity that has entity.field = match.  To start at the beginning of the list, pass world.  World is returned when the end of the list is reached.

<FIXME: define all the other functions...>


gotchas
-------

The && and || operators DO NOT EARLY OUT like C!

Don't confuse single quoted vectors with double quoted strings

The function declaration syntax takes a little getting used to.

Don't forget the ; after the trailing brace of a function initialization.

Don't forget the "local" before defining local variables.

There are no ++ / -- operators, or operate/assign operators.

*/

//=============================================================================

// offsets are allways multiplied by 4 before using
typedef int	gofs_t;				// offset in global data block
typedef struct function_s function_t;

#define	MAX_PARMS	8
#define MAX_NAME	64

typedef struct type_s
{
	etype_t			type;
	struct def_s	*def;		// a def that points to this type
	struct type_s	*next;
// function types are more complex
	struct type_s	*aux_type;	// return type or field type
	int				num_parms;	// -1 = variable args
	struct type_s	*parm_types[MAX_PARMS];	// only [num_parms] allocated
	//char			parm_names[MAX_PARMS][MAX_NAME];
} type_t;

typedef struct def_s
{
	type_t		*type;
	char		*name;
	struct def_s	*next;
	gofs_t		ofs;
	struct def_s	*scope;	// function the var was defined in, or NULL
	int	initialized;	// 1 when a declaration included "= immediate"

	etype_t		cast;	// used for type casting
} def_t;

//============================================================================

// pr_loc.h -- program local defs

#define	MAX_ERRORS		10

#define	MAX_NAME		64		// chars long

#define MAX_REGS                65536

//=============================================================================

typedef union eval_s
{
	string_t			string;
	float				_float;
	float				vector[3];
	func_t				function;
	int					_int;
	union eval_s		*ptr;
} eval_t;	

extern	int	type_size[8];
extern	def_t	*def_for_type[8];

extern	type_t	type_void, type_string, type_float, type_vector, type_entity, type_field, type_function, type_pointer, type_floatfield;

extern	def_t	def_void, def_string, def_float, def_vector, def_entity, def_field, def_function, def_pointer;

struct function_s
{
	int					builtin;	// if non 0, call an internal function
	int					code;		// first statement
	char				*file;		// source file with definition
	int					file_line;
	struct def_s		*def;
	int					parm_ofs[MAX_PARMS];	// allways contiguous, right?
};


//
// output generated by prog parsing
//
typedef struct
{
	char		*memory;
	int		max_memory;
	int		current_memory;
	type_t		*types;

	def_t		def_head;	// unused head of linked list
	def_t		*def_tail;	// add new defs after this and move it

	int		size_fields;
} pr_info_t;

extern	pr_info_t	pr;

typedef struct
{
	char	*name;
	char	*opname;
	int		op;
    int     priority;
	boolean	right_associative;
	def_t	*type_a, *type_b, *type_c;
} opcode_t;

//============================================================================


extern	opcode_t	pr_opcodes[99];		// sized by initialization

extern	boolean	pr_dumpasm;

extern	def_t		*pr_global_defs[MAX_REGS];	// to find def for a global variable

typedef enum {
tt_eof,			// end of file reached
tt_name, 		// an alphanumeric name token
tt_punct, 		// code punctuation
tt_immediate,	// string, float, vector
} token_type_t;

extern	char		pr_token[2048];
extern	token_type_t	pr_token_type;
extern	type_t		*pr_immediate_type;
extern	eval_t		pr_immediate;
extern	int			pr_immediate_index;

void PR_PrintStatement (dstatement_t *s);

void PR_Lex (void);
// reads the next token into pr_token and classifies its type

type_t *PR_ParseType (void);
char *PR_ParseName (void);

void PR_ParseError (char *error, ...);


extern	jmp_buf		pr_parse_abort;		// longjump with this on parse error
extern	int			pr_source_line;
extern	char		*pr_file_p;

void *PR_Malloc (int size);


#define	OFS_NULL		0
#define	OFS_RETURN		1
#define	OFS_PARM0		4		// leave 3 ofs for each parm to hold vectors
#define	OFS_PARM1		7
#define	OFS_PARM2		10
#define	OFS_PARM3		13
#define	OFS_PARM4		16
#define OFS_PARM5		19
#define OFS_PARM6		22
#define OFS_PARM7		25
#define	RESERVED_OFS	28


extern	def_t	*pr_scope;
extern	int	pr_error_count;

void PR_NewLine (void);
def_t *PR_GetDef (type_t *type, char *name, def_t *scope, boolean allocate);

void PR_PrintDefs (void);

void PR_SkipToSemicolon (void);

extern	char pr_parm_names[MAX_PARMS][MAX_NAME];
extern	boolean	pr_trace;

#define	G_FLOAT(o) (pr_globals[o])
#define	G_INT(o) (*(int *)&pr_globals[o])
#define	G_VECTOR(o) (&pr_globals[o])
#define	G_STRING(o) (strings + *(string_t *)&pr_globals[o])
#define	G_FUNCTION(o) (*(func_t *)&pr_globals[o])

char *PR_ValueString (etype_t type, void *val);

void PR_ClearGrabMacros (void);

boolean	PR_CompileFile (char *string, char *filename);

extern	boolean	pr_dumpasm;

extern	string_t	s_file;			// filename for function definition

extern	def_t	def_ret, def_parms[MAX_PARMS];

//=============================================================================

#define	MAX_STRINGS		500000
#define MAX_GLOBALS             32768
#define	MAX_FIELDS		1024
#define	MAX_STATEMENTS	65536
#define	MAX_FUNCTIONS	8192
#define MAX_TEMPS		256

#define	MAX_SOUNDS		1024
#define	MAX_MODELS		1024
#define	MAX_FILES		1024
#define	MAX_DATA_PATH	64

extern	char	strings[MAX_STRINGS];
extern	int		strofs;

extern	dstatement_t	statements[MAX_STATEMENTS];
extern	int			numstatements;
extern	int			statement_linenums[MAX_STATEMENTS];

extern	dfunction_t	functions[MAX_FUNCTIONS];
extern	int			numfunctions;

extern	float		pr_globals[MAX_REGS];
extern	int			numpr_globals;

extern	char		pr_immediate_string[2048];
extern	int			pr_immediate_strlen;

extern	char		precache_sounds[MAX_SOUNDS][MAX_DATA_PATH];
extern	int			precache_sounds_block[MAX_SOUNDS];
extern	int			numsounds;

extern	char		precache_models[MAX_MODELS][MAX_DATA_PATH];
extern	int			precache_models_block[MAX_SOUNDS];
extern	int			nummodels;

extern	char		precache_files[MAX_FILES][MAX_DATA_PATH];
extern	int			precache_files_block[MAX_SOUNDS];
extern	int			numfiles;

extern int pr_optimize_eliminate_temps;
extern int pr_optimize_shorten_ifs;
extern int pr_optimize_nonvec_parms;
extern int pr_optimize_constant_names;
extern int pr_optimize_defs;
extern int pr_optimize_hash_strings;

extern int num_temps_removed;
extern int num_stores_shortened;
extern int num_ifs_shortened;
extern int num_nonvec_parms;
extern int num_constant_names;
extern int num_defs;
extern int num_strings;

extern int ofstemp;

int	CopyString (char *str, int len = 0);


inline static void PR_Expect (char *string)
{
	if (STRCMP(string, pr_token))
		PR_ParseError ("expected %s, found %s",string, pr_token);
	PR_Lex ();
} 

inline static boolean PR_Check (char *string)
{
	if (STRCMP(string, pr_token))
		return false;

	PR_Lex ();
	return true;
}


#endif

