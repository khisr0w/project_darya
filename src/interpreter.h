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
};

enum var_type
{
	VarType_Number,
};

struct context
{
	char *DisplayName;
	context *Parent;
	position *ParentPos;
};

struct var
{
	var_type Type;
	void *Value;
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

struct symbol_table
{
	memory_arena Arena;

	symbol_table *Parent;
};

#define INTERPRETER_H
#endif
