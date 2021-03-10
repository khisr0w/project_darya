/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  3/9/2021 6:23:18 AM                                           |
    |    Last Modified:                                                                |
    |                                                                                  |
    +=====================| Sayed Abid Hashimi, Copyright © All rights reserved |======+  */

#if !defined(INTERPRETER_H)

struct context
{
	char *DisplayName;
	context *Parent;
	position *ParentPos;
};

enum number_type
{
	NUM_DEFINED,
	NUM_FLOAT,
	NUM_INT,
};

struct number
{
	number_type Type;
	void *Number;
};

struct visit_result
{
	number *Number;
	error Error;
};

struct op_result
{
	number *Number;
	error Error;
};


#define INTERPRETER_H
#endif
