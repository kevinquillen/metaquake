
#include "qcc.h"
#include "hash.h"

int		pr_source_line;

char		*pr_file_p;
char		*pr_line_start;		// start of current source line

int			pr_bracelevel;

char		pr_token[2048];
token_type_t	pr_token_type;
type_t		*pr_immediate_type;
eval_t		pr_immediate;
int			pr_immediate_index;

char	pr_immediate_string[2048];
int		pr_immediate_strlen;

int		pr_error_count;

char	*pr_punctuation[] =
// longer symbols must be before a shorter partial match
{";", "(", ")", "==", "=", ",", "...", ".", "{", "}",
"[", "]", "+", "*", "-", "!=", "!", "#", "&&", "&",
">=", ">", "<=", "<", "||", "|", "/", ":", "@", "^[", NULL};

int	pr_punct_length[] =
{1, 1, 1, 2, 1, 1, 3, 1, 1, 1,
1, 1, 1, 1, 1, 2, 1, 1, 2, 1,
2, 1, 2, 1, 2, 1, 1, 1, 1, 2, 0};

// simple types.  function types are dynamically allocated
type_t	type_void = {ev_void, &def_void};
type_t	type_string = {ev_string, &def_string};
type_t	type_float = {ev_float, &def_float};
type_t	type_vector = {ev_vector, &def_vector};
type_t	type_entity = {ev_entity, &def_entity};
type_t	type_field = {ev_field, &def_field};
type_t	type_function = {ev_function, &def_function,NULL,&type_void};
// type_function is a void() function used for state defs
type_t	type_pointer = {ev_pointer, &def_pointer};

type_t	type_floatfield = {ev_field, &def_field, NULL, &type_float};

int		type_size[8] = {1,1,1,3,1,1,1,1};

def_t	def_void = {&type_void, "temp"};
def_t	def_string = {&type_string, "temp"};
def_t	def_float = {&type_float, "temp"};
def_t	def_vector = {&type_vector, "temp"};
def_t	def_entity = {&type_entity, "temp"};
def_t	def_field = {&type_field, "temp"};
def_t	def_function = {&type_function, "temp"};
def_t	def_pointer = {&type_pointer, "temp"};

def_t	def_ret, def_parms[MAX_PARMS];

def_t	*def_for_type[8] = {&def_void, &def_string, &def_float, &def_vector, &def_entity, &def_field, &def_function, &def_pointer};

void PR_LexWhitespace (void);


/*
==============
PR_PrintNextLine
==============
*/
void PR_PrintNextLine (void)
{
	char	*t;

	printf ("%3i:",pr_source_line);
	for (t=pr_line_start ; *t && *t != '\n' ; t++)
		printf ("%c",*t);
	printf ("\n");
}

/*
==============
PR_NewLine

Call at start of file and when *pr_file_p == '\n'
==============
*/
void PR_NewLine (void)
{
	boolean	m;
	
	if (*pr_file_p == '\n')
	{
		pr_file_p++;
		m = true;
	}
	else
		m = false;

	pr_source_line++;
	pr_line_start = pr_file_p;

//	if (pr_dumpasm)
//		PR_PrintNextLine ();
	if (m)
		pr_file_p--;
}

/*
==============
PR_LexString

Parses a quoted string
==============
*/
void PR_LexString (void)
{
	int		c;
	int mask1 = 0;
	int mask2;
	
	pr_immediate_strlen = 0;
	pr_file_p++;
	do
	{
		mask2 = 128;
		c = *pr_file_p++;
		if (!c)
			PR_ParseError ("EOF inside quote");
		if (c=='\n')
			PR_ParseError ("newline inside quote");
		if (c=='\\')
		{	// escape char
			mask2 = 0;
			c = *pr_file_p++;
			if (!c)
				PR_ParseError ("EOF inside quote");
			if (c == 'n')
				c = '\n';
			else if (c == '"')
				c = '"' | mask1;
			else if (c == '\\')
				c = '\\' | mask1;
			else if (c == 'b')
			{
				mask1 ^= 128;
				continue;
			}
			else if (c == '[')
				c = 16;
			else if (c == ']')
				c = 17;
			else if (c == '.')
				c = 28 | mask1;
			else if (c == '<')
				c = 29;
			else if (c == '-')
				c = 30;
			else if (c == '>')
				c = 31;
			else if (c >= '0' && c <= '9')
				c = 18 + c - '0';
			else if (c == '(')
				c = 128;
			else if (c == '=')
				c = 129;
			else if (c == ')')
				c = 130;
			else if (c == '{')
			{
				int d;
				c = 0;
				while ((d = *pr_file_p++) != '}')
				{
					c = c * 10 + d - '0';
					if (d < '0' || d > '9' || c > 255)
						PR_ParseError("Bad character code");
				}
			}
			else
				PR_ParseError ("Unknown escape char");
		}
		else if (c=='\"')
		{
			mask1 = 0;
			PR_LexWhitespace ();
			if (*pr_file_p == ':')
			{
				pr_file_p++;
				pr_token[pr_immediate_strlen++] = 0;
				PR_LexWhitespace();
			}
			if (*pr_file_p != '\"')
			{
				pr_token[pr_immediate_strlen++] = 0;
				pr_token_type = tt_immediate;
				pr_immediate_type = &type_string;
				memcpy (pr_immediate_string, pr_token, pr_immediate_strlen);
				return;
			}
			pr_file_p++;
			continue;
		}
		pr_token[pr_immediate_strlen++] = c | (mask1 & mask2);
	} while (1);
}

/*
==============
PR_LexNumber
==============
*/
float PR_LexNumber (void)
{
	int		c;
	int		len;
	
	len = 0;
	c = *pr_file_p;
	do
	{
		pr_token[len] = c;
		len++;
		pr_file_p++;
		c = *pr_file_p;
	} while ((c >= '0' && c<= '9') || c == '.');
	pr_token[len] = 0;
	return atof (pr_token);
}

/*
==============
PR_LexInt
==============
*/
unsigned int PR_LexInt (void)
{
	int		c;
	int		len;
	
	len = 0;
	c = *++pr_file_p;
	do
	{
		pr_token[len] = c;
		len++;
		pr_file_p++;
		c = *pr_file_p;
	} while ((c >= '0' && c<= '9') || c == '-');
	pr_token[len] = 0;
	return atoi (pr_token);
}

/*
==============
PR_LexHex
==============
*/
unsigned int PR_LexHex (void)
{
	unsigned int		c;
	int		len;
	
	len = 0;
	pr_file_p += 2;
	c = *pr_file_p;
	do
	{
		pr_token[len] = c;
		len++;
		pr_file_p++;
		c = *pr_file_p;
	} while ((c >= '0' && c<= '9') || (c >= 'a' && c <= 'f'));
	pr_token[len] = 0;
	sscanf(pr_token, "%x", &c);
	return c;
}

/*
==============
PR_LexVector

Parses a single quoted vector
==============
*/
void PR_LexVector (void)
{
	int		i;
	
	pr_file_p++;
	pr_token_type = tt_immediate;
	pr_immediate_type = &type_vector;
	for (i=0 ; i<3 ; i++)
	{
		pr_immediate.vector[i] = PR_LexNumber ();
		PR_LexWhitespace ();
	}
	if (*pr_file_p != '\'')
		PR_ParseError ("Bad vector");
	pr_file_p++;
}

/*
==============
PR_LexName

Parses an identifier
==============
*/
void PR_LexName (void)
{
	int		c;
	int		len;
	
	len = 0;
	c = *pr_file_p;
	do
	{
		pr_token[len] = c;
		len++;
		pr_file_p++;
		c = *pr_file_p;
	} while ( (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' 
	|| (c >= '0' && c <= '9'));
	pr_token[len] = 0;
	pr_token_type = tt_name;
}

/*
==============
PR_LexPunctuation
==============
*/
void PR_LexPunctuation (void)
{
	int	i;
	char	*p;
	int	len;

	pr_token_type = tt_punct;

	for (i=0; (p = pr_punctuation[i]) != NULL; i++)
	{
		len = pr_punct_length[i];
		if (!STRNCMP(p, pr_file_p, len) )
		{
			memcpy(pr_token, p, len+1);
			if (p[0] == '{')
				pr_bracelevel++;
			else if (p[0] == '}')
				pr_bracelevel--;
			pr_file_p += len;
			return;
		}
	}
	
	PR_ParseError ("Unknown punctuation");
}

		
/*
==============
PR_LexWhitespace
==============
*/
void PR_LexWhitespace (void)
{
	int		c;
	
	while (1)
	{
	// skip whitespace
		while ( (c = *pr_file_p) <= ' ')
		{
			if (c=='\n')
				PR_NewLine ();
			if (c == 0)
				return;		// end of file
			pr_file_p++;
		}
		
	// skip // comments
		if (c=='/' && pr_file_p[1] == '/')
		{
			if (pr_file_p[2] == '^')
				pr_file_p++;
			while (*pr_file_p && *pr_file_p != '\n')
				pr_file_p++;
			PR_NewLine();
			pr_file_p++;
			continue;
		}
		
	// skip /* */ comments
		if (c=='/' && pr_file_p[1] == '*')
		{
			do
			{
				pr_file_p++;
				if (pr_file_p[0]=='\n')
					PR_NewLine();
				if (pr_file_p[1] == 0)
					return;
			} while (pr_file_p[-1] != '*' || pr_file_p[0] != '/');
			pr_file_p++;
			continue;
		}
		
		break;		// a real character has been found
	}
}

//============================================================================

#define	MAX_FRAMES	256

char	pr_framemacros[MAX_FRAMES][16];
int		pr_nummacros;

void PR_ClearGrabMacros (void)
{
	pr_nummacros = 0;
}

void PR_FindMacro (void)
{
	int		i;
	
	for (i=0 ; i<pr_nummacros ; i++)
		if (!strcmp (pr_token, pr_framemacros[i]))
		{
			sprintf (pr_token,"%d", i);
			pr_token_type = tt_immediate;
			pr_immediate_type = &type_float;
			pr_immediate._float = i;
			return;
		}
	PR_ParseError ("Unknown frame macro $%s", pr_token);
}

// just parses text, returning false if an eol is reached
boolean PR_SimpleGetToken (void)
{
	int		c;
	int		i;
	
// skip whitespace
	while ( (c = *pr_file_p) <= ' ')
	{
		if (c=='\n' || c == 0)
			return false;
		pr_file_p++;
	}
	
	i = 0;
	while ( (c = *pr_file_p) > ' ' && c != ',' && c != ';')
	{
		pr_token[i] = c;
		i++;
		pr_file_p++;
	}
	pr_token[i] = 0;
	return true;
}

void PR_ParseFrame (void)
{
	while (PR_SimpleGetToken ())
	{
		strcpy (pr_framemacros[pr_nummacros], pr_token);
		pr_nummacros++;
	}
}

/*
==============
PR_LexGrab

Deals with counting sequence numbers and replacing frame macros
==============
*/
void PR_LexGrab (void)
{	
	pr_file_p++;	// skip the $
	if (!PR_SimpleGetToken ())
		PR_ParseError ("hanging $");
	
// check for $frame
	if (!STRCMP (pr_token, "frame"))
	{
		PR_ParseFrame ();
		PR_Lex ();
	}
// ignore other known $commands
	else if (!STRCMP (pr_token, "cd")
	|| !STRCMP (pr_token, "origin")
	|| !STRCMP (pr_token, "base")
	|| !STRCMP (pr_token, "flags")
	|| !STRCMP (pr_token, "scale")
	|| !STRCMP (pr_token, "skin") )
	{	// skip to end of line
		while (PR_SimpleGetToken ())
		;
		PR_Lex ();
	}
// look for a frame name macro
	else
		PR_FindMacro ();
}

//============================================================================

/*
==============
PR_Lex

Sets pr_token, pr_token_type, and possibly pr_immediate and pr_immediate_type
==============
*/
void PR_Lex (void)
{
	int		c;

	pr_token[0] = 0;
	pr_immediate_index = -1;


	if (!pr_file_p)
	{
		pr_token_type = tt_eof;
		return;
	}

	PR_LexWhitespace ();

	c = *pr_file_p;
		
	if (!c)
	{
		pr_token_type = tt_eof;
		return;
	}

// handle quoted strings as a unit
	if (c == '\"')
	{
		PR_LexString ();
		HashImmediate();
		return;
	}

// handle quoted vectors as a unit
	if (c == '\'')
	{
		if (pr_file_p[2] == '\'')
		{
			pr_token_type = tt_immediate;
			pr_immediate_type = &type_float;
			pr_immediate._int = pr_file_p[1];
			pr_file_p += 3;
		}
		else
			PR_LexVector ();
		HashImmediate();
		return;
	}

	if (c == '0' && pr_file_p[1] == 'x')
	{
		pr_token_type = tt_immediate;
		pr_immediate_type = &type_float;
		pr_immediate._int = PR_LexHex ();
		HashImmediate();
		return;
	}

// if the first character is a valid identifier, parse until a non-id
// character is reached
	if ( (c >= '0' && c <= '9') || ( c=='-' && pr_file_p[1]>='0' && pr_file_p[1] <='9') )
	{
		pr_token_type = tt_immediate;
		pr_immediate_type = &type_float;
		pr_immediate._float = PR_LexNumber ();
		HashImmediate();
		return;
	}

	if (c == '%')
	{
		pr_token_type = tt_immediate;
		pr_immediate_type = &type_float;
		pr_immediate._int = PR_LexInt ();
		HashImmediate();
		return;
	}
	
	if ( (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' )
	{
		PR_LexName ();
		return;
	}
	
	if (c == '$')
	{
		PR_LexGrab ();
		if (pr_token_type == tt_immediate)
			HashImmediate();
		return;
	}
	
// parse symbol strings until a non-symbol is found
	PR_LexPunctuation ();
}

//=============================================================================

/*
============
PR_ParseError

Aborts the current file load
============
*/
void PR_ParseError (char *error, ...)
{
	va_list		argptr;
	char		string[1024];

	va_start (argptr,error);
	vsprintf (string,error,argptr);
	va_end (argptr);

	printf ("%s:%i:%s\n", strings + s_file, pr_source_line, string);
	
	longjmp (pr_parse_abort, 1);
}


/*
============
PR_ParseName

Checks to see if the current token is a valid name
============
*/
char *PR_ParseName (void)
{
	static char	ident[MAX_NAME];
	
	if (pr_token_type != tt_name)
		PR_ParseError ("not a name");
	if (strlen(pr_token) >= MAX_NAME-1)
		PR_ParseError ("name too long");
	strcpy (ident, pr_token);
	PR_Lex ();
	
	return ident;
}

/*
============
PR_FindType

Returns a preexisting complex type that matches the parm, or allocates
a new one and copies it out.
============
*/
type_t *PR_FindType (type_t *type)
{
	def_t	*def;
	type_t	*check;
	int		i;
	
	for (check = pr.types ; check ; check = check->next)
	{
		if (check->type != type->type
		|| check->aux_type != type->aux_type
		|| check->num_parms != type->num_parms)
			continue;
	
		for (i=0 ; i< type->num_parms ; i++)
			if (check->parm_types[i] != type->parm_types[i])
				break;
			
		if (i == type->num_parms)
			return check;	
	}
	
// allocate a new one
	check = (struct type_s *) malloc (sizeof (*check));
	*check = *type;
	check->next = pr.types;
	pr.types = check;
	
// allocate a generic def for the type, so fields can reference it
	def = (struct def_s *) malloc (sizeof(def_t));
	def->name = "COMPLEX TYPE";
	def->type = check;
	check->def = def;
	return check;
}


/*
============
PR_SkipToSemicolon

For error recovery, also pops out of nested braces
============
*/
void PR_SkipToSemicolon (void)
{
	do
	{
		if (!pr_bracelevel && PR_Check (";"))
			return;
		PR_Lex ();
	} while (pr_token[0]);	// eof will return a null token
}


/*
============
PR_ParseType

Parses a variable type, including field and functions types
============
*/
char	pr_parm_names[MAX_PARMS][MAX_NAME];

type_t *PR_ParseType (void)
{
	type_t	newtype;
	type_t	*type;
	char	*name;
	static int recurse = 0;
	
	if (PR_Check ("."))
	{
		memset (&newtype, 0, sizeof(newtype));
		newtype.type = ev_field;
		newtype.aux_type = PR_ParseType ();
		return PR_FindType (&newtype);
	}
	
	if (!strcmp (pr_token, "float") )
		type = &type_float;
	else if (!strcmp (pr_token, "vector") )
		type = &type_vector;
	else if (!strcmp (pr_token, "float") )
		type = &type_float;
	else if (!strcmp (pr_token, "entity") )
		type = &type_entity;
	else if (!strcmp (pr_token, "string") )
		type = &type_string;
	else if (!strcmp (pr_token, "void") )
		type = &type_void;
	else
	{
		PR_ParseError ("\"%s\" is not a type", pr_token);
		type = &type_float;	// shut up compiler warning
	}
	PR_Lex ();
	
	if (!PR_Check ("("))
		return type;
	
// function type
	memset (&newtype, 0, sizeof(newtype));
	newtype.type = ev_function;
	newtype.aux_type = type;	// return type
	newtype.num_parms = 0;
	if (!PR_Check (")"))
	{
		do
		{
			if (PR_Check ("..."))
			{
				newtype.num_parms = -1 - newtype.num_parms;	// variable args
				break;
			}
			else
			{
				recurse++;
				type = PR_ParseType ();
				recurse--;
				name = PR_ParseName ();
				if (!recurse)
					strcpy (pr_parm_names[newtype.num_parms], name);
				newtype.parm_types[newtype.num_parms] = type;
				newtype.num_parms++;
			} 
		} while (PR_Check (","));
	
		PR_Expect (")");
	}
	
	return PR_FindType (&newtype);
}

