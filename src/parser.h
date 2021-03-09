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
	uint8 *ASTMemory;
	uint32 MaxASTSize;
	uint32 ASTSize;

	token_list *Tokens;
	int32 Token_Id;
	token CurrentToken;
};

enum node_type
{
	NT_undefined,
	NT_number_node,
	NT_unary_node,
	NT_binary_node,
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

	token Token;
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


#define PARSER_H
#endif
