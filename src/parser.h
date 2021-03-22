/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  3/6/2021 8:50:39 AM                                           |
    |    Last Modified:                                                                |
    |                                                                                  |
    +=====================| Sayed Abid Hashimi, Copyright © All rights reserved |======+  */

#if !defined(PARSER_H)
struct parser_state
{
	memory_arena AST;

	token *Tokens;
	uint32 TokenLength;
	int32 Token_Id;
	token CurrentToken;
};

enum node_type
{
	NodeType_undefined,
	NodeType_number_node,
	NodeType_unary_node,
	NodeType_binary_node,
	NodeType_var_assign_node,
	NodeType_var_access_node,
};

enum number_type
{
	NumberType_Undefined,
	NumberType_Int,
	NumberType_Real,
};

struct node_header
{
	node_type Type;
};

struct node
{
	node_header Header;
};

struct parser_result
{
	error Error;
	node *Node;
};

struct node_pos
{
	position Start;
	position End;
};

struct number_node
{
	node_pos Pos;

	number_type Type;
	int32 Int;
	real32 Real;
};

struct unary_node
{
	node_pos Pos;

	token OpToken;
	node *Node;
};

struct binary_node
{
	node_pos Pos;

	node *LeftNode;
	token OpToken;
	node *RightNode;
};

struct var_assign_node
{
	node_pos Pos;

	token Name;
	node *Value;
	bool32 New;
};

struct var_access_node
{
	node_pos Pos;

	token Name;
};

#define PARSER_H
#endif
