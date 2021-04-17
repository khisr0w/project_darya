/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  3/9/2021 6:23:18 AM                                           |
    |    Last Modified:                                                                |
    |                                                                                  |
    +=====================| Sayed Abid Hashimi, Copyright © All rights reserved |======+  */

// Translatable Lang

#if !defined(INTERPRETER_H)

struct interpreter_state
{
	dynamic_memory_arena RuntimeMem;
};

enum var_type
{
	VarType_undefined,
	VarType_number,
	VarType_string,
	VarType_boolean_,
};

struct var
{
	var_type Type;
};

struct symbol
{
	char *Name;
	var *Value;

	symbol *Next;
};

struct symbol_table
{
	uint32 SymbolSize;
	symbol *Symbols;

	symbol_table *Parent;
};

#if 0
struct symbol_table
{
	uint32 SymbolSize;
	memory_arena Arena;

	symbol_table *Parent;
};
#endif

struct context
{
	char *DisplayName;
	context *Parent;
	position *ParentPos;

	symbol_table *SymbolTable;
};

struct number
{
	number_type Type;
	int32 Int;
	real32 Real;
};

struct string
{
	char *Value;
};

struct boolean_
{
	char Value;
};

struct visit_result
{
	var *Var;
	error Error;
};

#define INTERPRETER_H
#endif
