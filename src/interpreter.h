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

struct context
{
	char *DisplayName;
	context *Parent;
	position *ParentPos;
};

struct visit_node
{
	number_type Type;
	void *Value;
};

struct visit_result
{
	node *Node;
	error Error;
};

struct op_result
{
	number Number;
	error Error;
};

struct symbol_table
{
	memory_arena Arena;

	symbol_table *Parent;
};

struct number
{
	number_type Type;
	int32 Int;
	real32 Real;
};

#define INTERPRETER_H
#endif
