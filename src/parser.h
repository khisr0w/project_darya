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
	dynamic_memory_arena AST;

	uint32 TokenLength;
	int32 Token_Id;
	token *CurrentToken;
};

enum node_type
{
	NodeType_undefined,
	NodeType_number_node,
	NodeType_unary_node,
	NodeType_binary_node,
	NodeType_var_assign_node,
	NodeType_var_access_node,
	NodeType_comparison_node,
	NodeType_compound_compare_node,
	NodeType_statements_node,
	NodeType_function_call_node,
	NodeType_function_def_node,
	NodeType_if_node,
	NodeType_other_node,
	NodeType_else_node,
	NodeType_string_node,

	// NodeType_end_of_file_node,
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

struct string_node
{
	node_pos Pos;

	char *Value;
};

struct unary_node
{
	node_pos Pos;

	token OpToken;
	node *Node;
};

struct comparison_node
{
	node_pos Pos;

	node *LeftNode;
	token OpToken;
	node *RightNode;
};

struct binary_node
{
	node_pos Pos;

	node *LeftNode;
	token OpToken;
	node *RightNode;
};

struct compound_compare_node
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

struct statements_node
{
	node_pos Pos;

	node **Statement;
	uint32 Length;
	uint32 MaxLength;
};

struct function_call_node
{
	node_pos Pos;

	token FuncName;
	node **Args;
	uint32 ArgLength;
	uint32 MaxLength;
};

struct if_node
{
	node_pos Pos;

	node *Body;
	node *Condition;
	node **Others;
	node *Else;
	uint32 OtherLength;
	uint32 MaxOtherLength;
};

struct other_node
{
	node_pos Pos;

	node *Body;
	node *Condition;
};

struct else_node
{
	node_pos Pos;

	node *Body;
};

struct function_def_node
{
	node_pos Pos;

	token FuncName;
	node **Args;
	node *Body;
	uint32 ArgLength;
	uint32 MaxArgLength;
};

struct end_of_file_node
{
	node_pos Pos;
};

#define PARSER_H
#endif
