/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  3/9/2021 6:23:18 AM                                           |
    |    Last Modified:                                                                |
    |                                                                                  |
    +=====================| Sayed Abid Hashimi, Copyright © All rights reserved |======+  */

#if !defined(INTERPRETER_H)

struct interpreter_state
{
	memory_arena RuntimeMem;

	memory_arena Stack;
};

enum var_type
{
	VarType_undefined,
	VarType_number,
};

struct symbol_table
{
	uint32 SymbolSize;
	memory_arena Arena;

	symbol_table *Parent;
};

struct context
{
	char *DisplayName;
	context *Parent;
	position *ParentPos;

	symbol_table *SymbolTable;
};

struct var
{
	var_type Type;
};

struct symbol
{
	char *Name;
	var *Value;
};

struct number
{
	number_type Type;
	int32 Int;
	real32 Real;
};

struct visit_result
{
	var *Var;
	error Error;
};

#define INTERPRETER_H
#endif
