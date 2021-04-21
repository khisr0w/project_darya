/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  3/4/2021 6:45:32 AM                                           |
    |    Last Modified:                                                                |
    |                                                                                  |
    +=====================| Sayed Abid Hashimi, Copyright © All rights reserved |======+  */

#if !defined(LEXER_H)

enum token_type
{
	TT_UNDEFINED,
	TT_KEYWORD,
	TT_ID,
	TT_PLUS,
	TT_MINUS,
	TT_MUL,
	TT_DIV,
	TT_POW,
	TT_EQ,
	TT_EQEQ,
	TT_NEQ,
	TT_NOT,
	TT_GT,
	TT_GTE,
	TT_LT,
	TT_LTE,
	TT_AND,
	TT_OR,
	TT_INT,
	TT_REAL,
	TT_STRING,
	TT_LPAREN,
	TT_RPAREN,
	TT_COMMA,
	TT_LCBRACKET,
	TT_RCBRACKET,
	TT_NEWLINE,
	TT_EOF,
};

char *KEYWORDS[] =
{
	"var",
	"if",
	"other", // used instead of **else if**
	"else",
	"while",
	"out",
	"ignore",
};

struct position
{
	char *FileName;
	int32 Index;
	int32 Line;
	int32 Col;
};

// TODO(Khisrow): WARNING This should be changed to memory block soon
struct token
{
	char *Value;
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
	// memory_arena TokenMemory;
	dynamic_memory_arena TokenMemory;
	char *TextMemory;
	position Pos;
	char CurrentChar;
};

struct text_memory
{
	memory_arena Arena;
};

struct op_status
{
	bool32 Success;
	error Error;
};

#define LEXER_H
#endif
