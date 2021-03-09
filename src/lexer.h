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
	TT_INT,
	TT_FLOAT,
	TT_PLUS,
	TT_MINUS,
	TT_MUL,
	TT_DIV,
	TT_LPAREN,
	TT_RPAREN,
	TT_EOF,
};

char *TokenTypeString[] = 
{
	"UNDEFINED",
	"INT",
	"FLOAT",
	"PLUS",
	"MINUS",
	"MUL",
	"DIV",
	"LPAREN",
	"RPAREN",
	"EOF",
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

struct token_list
{
	token *MemoryBase;
	uint32 TokenCount;
	uint32 MemorySize;
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
	token_list Tokens;
	position Pos;
	char CurrentChar;
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
