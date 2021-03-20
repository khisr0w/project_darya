/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  3/4/2021 6:45:32 AM                                           |
    |    Last Modified:                                                                |
    |                                                                                  |
    +=====================| Sayed Abid Hashimi, Copyright © All rights reserved |======+  */

#if !defined(LEXER_H)

char DIGITS[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

enum token_type
{
	TT_UNDEFINED,
	TT_KEYWORD,
	TT_ID,
	TT_INT,
	TT_REAL,
	TT_PLUS,
	TT_MINUS,
	TT_MUL,
	TT_DIV,
	TT_POW,
	TT_EQ,
	TT_LPAREN,
	TT_RPAREN,
	TT_EOF,
};

#if 0
char *TokenTypeString[] = 
{
	"UNDEFINED",
	"KEYWORD",
	"ID",
	"INTEGER",
	"REAL",
	"PLUS",
	"MINUS",
	"MULTIPLICATION",
	"DIVISION",
	"POWER",
	"EQUAL",
	"LPAREN",
	"RPAREN",
	"EOF",
};
#endif

char *KEYWORDS[] =
{
	"var",
};

struct position
{
	char *FileName;
	char *TextMemory;
	int32 Index;
	int32 Line;
	int32 Col;
};

// TODO(Khisrow): WARNING This should be changed to memory block soon
struct token
{
	char Value[64];
	token_type Type;

	position StartPos;
	position EndPos;
};

enum error_type
{
	NoError,
	CharacterError,
	InvalidSyntaxError,
	VisitError,
};

struct error
{
	error_type Type;
	char Message[1024];
};

struct lexer_state
{
	memory_arena TokenMemory;
	position Pos;
	char CurrentChar;
};

struct text_memory
{
	memory_arena Arena;
};

struct runtime_state
{
};

struct op_status
{
	bool32 Success;
	error Error;
};

#define LEXER_H
#endif
