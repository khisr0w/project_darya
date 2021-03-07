/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  3/6/2021 8:50:39 AM                                           |
    |    Last Modified:                                                                |
    |                                                                                  |
    +=====================| Sayed Abid Hashimi, Copyright © All rights reserved |======+  */

#include "parser.h"

typedef parser_result bin_op_delegate(parser_state *ParserState);

inline token
Advance(parser_state *ParserState)
{
	ParserState->Token_Id += 1;
	token_list *Tokens = ParserState->Tokens;

	if(ParserState->Token_Id < Tokens->TokenCount)
	{
		ParserState->CurrentToken = Tokens->MemoryBase[ParserState->Token_Id];
	}

	return ParserState->CurrentToken;
}

inline void
InitializeParser(parser_state *ParserState, token_list *Tokens)
{
	Assert(ParserState && Tokens);
	ParserState->ASTSize = 0;
	ParserState->Tokens = Tokens;
	ParserState->Token_Id = -1;
	Advance(ParserState);
}

#define PushNode(ParserState, Type) (node *)PushNode_ (ParserState, sizeof(Type), NT_##Type)
inline void *
PushNode_(parser_state *ParserState, uint32 Size, node_type Type)
{
	void *Result = 0;

	Size += sizeof(node);

	if((ParserState->ASTSize + Size) < ParserState->MaxASTSize)
	{
		node *Node = (node *)(ParserState->ASTMemory + ParserState->ASTSize);
		Node->Header.Type = Type;
		Result = (uint8 *)Node;
		ParserState->ASTSize += Size;
	}
	else
	{
		InvalidCodePath;
	}

	return Result;
}

inline token
OnParseRegister(parser_result *Result, token Token)
{
	return Token;
}

inline node *
OnParseRegister(parser_result *Result, parser_result ParseResult)
{
	if(ParseResult.Error.Type != NoError) Result->Error = ParseResult.Error;
	return ParseResult.Node;
}

inline parser_result
OnParseSuccess(parser_result *Result, node *Node)
{
	Result->Node = Node;
	return *Result;
}

inline parser_result
OnParseFailure(parser_result *Result, error Error)
{
	Result->Error = Error;

	return *Result;
}

inline bool32
IsTokenInList(token CurrentToken, token_type *TokenList, int32 TokenLength)
{
	for(int32 Index = 0;
		Index < TokenLength;
		++Index)
	{
		if(TokenList[Index] == CurrentToken.Type)
		{
			return true;
		}
	}

	return false;
}

#define MakeNode(ParserState, Token, First, Second, Type) MakeNode_(ParserState, Token, First, Second, NT_##Type)
inline node *
MakeNode_(parser_state *ParserState, token Token, node *First, node *Second, node_type Type)
{
	node *Result = 0;

	switch(Type)
	{
		case NT_number_node:
		{
			Result = PushNode(ParserState, number_node);
			number_node *NumberNode = (number_node *)(Result + 1);

			NumberNode->Token = Token;

			NumberNode->Pos.Start = Token.StartPos;
			NumberNode->Pos.End = Token.EndPos;
		} break;

		case NT_unary_node:
		{
			Assert(First);
			Result = PushNode(ParserState, unary_node);
			unary_node *UnaryNode = (unary_node *)(Result + 1);

			UnaryNode->OpToken = Token;
			UnaryNode->Node = First;

			node_pos *Pos = (node_pos *)(First + 1);
			UnaryNode->Pos.Start = Token.StartPos;
			UnaryNode->Pos.End = Pos->End;
		} break;

		case NT_binary_node:
		{
			Assert(First && Second);
			Result = PushNode(ParserState, binary_node);
			binary_node *BinaryNode = (binary_node *)(Result + 1);

			BinaryNode->LeftNode = First;
			BinaryNode->OpToken = Token;
			BinaryNode->RightNode = Second;

			node_pos *LeftPos = (node_pos *)(First + 1);
			node_pos *RightPos = (node_pos *)(Second + 1);

			BinaryNode->Pos.Start = LeftPos->Start;
			BinaryNode->Pos.End = RightPos->End;
		} break;

		default:
		{
			InvalidCodePath;
		} break;
	}

	return Result;
}

#define BinaryOperation(ParserState, DelegateFunc, OpTypes) BinaryOperation_(ParserState, DelegateFunc,\
																			 OpTypes, ArrayCount(OpTypes))
internal parser_result
BinaryOperation_(parser_state *ParserState, bin_op_delegate *DelegateFunc, token_type *OpTypes, int32 OpLength)
{
	parser_result Result = {};
	token OpToken = {};
	node *Left = OnParseRegister(&Result, DelegateFunc(ParserState));
	if (Result.Error.Type != NoError) return Result;


	while(IsTokenInList(ParserState->CurrentToken, OpTypes, OpLength))
	{
		OpToken = ParserState->CurrentToken;
		OnParseRegister(&Result, Advance(ParserState));
		node *Right = OnParseRegister(&Result, DelegateFunc(ParserState));
		if (Result.Error.Type != NoError) return Result;
		Left = MakeNode(ParserState, OpToken, Left, Right, binary_node);
	}

	return OnParseSuccess(&Result, Left);
}

#if 0
GetNode()
{
}
#endif

inline parser_result
Expression(parser_state *ParserState);

internal parser_result
Factor(parser_state *ParserState)
{
	parser_result Result = {};
	token CurrentToken = ParserState->CurrentToken;

	if((CurrentToken.Type == TT_PLUS) || (CurrentToken.Type == TT_MINUS))
	{
		OnParseRegister(&Result, Advance(ParserState));
		parser_result ParResult = Factor(ParserState);
		node *Factor = OnParseRegister(&Result, ParResult);
		if(Result.Error.Type != NoError) return Result;
		return OnParseSuccess(&Result, MakeNode(ParserState, CurrentToken, Factor, 0, unary_node));
	}
	else if((CurrentToken.Type == TT_INT) || (CurrentToken.Type == TT_FLOAT))
	{
		OnParseRegister(&Result, Advance(ParserState));
		return OnParseSuccess(&Result, MakeNode(ParserState, CurrentToken, 0, 0, number_node));
	}
	else if((CurrentToken.Type == TT_LPAREN))
	{
		OnParseRegister(&Result, Advance(ParserState));
		parser_result ParResult = Expression(ParserState);
		node * Expression = OnParseRegister(&Result, ParResult);
		if(Result.Error.Type != NoError) return Result;
		if(ParserState->CurrentToken.Type == TT_RPAREN)
		{
			OnParseRegister(&Result, Advance(ParserState));
			return OnParseSuccess(&Result, Expression);
		}
		else
		{
			OnParseFailure(&Result, MakeError(InvalidSyntaxError, "Expected ')'"));
		}
	}

	return OnParseFailure(&Result, MakeError(InvalidSyntaxError, "Expected int or float"));
}

inline parser_result
Term(parser_state *ParserState)
{
	token_type OpTypes[] = {TT_MUL, TT_DIV};
	return BinaryOperation(ParserState, Factor, OpTypes);
}

inline parser_result
Expression(parser_state *ParserState)
{
	token_type OpTypes[] = {TT_PLUS, TT_MINUS};
	return BinaryOperation(ParserState, Term, OpTypes);
}

inline parser_result
ParseTokens(parser_state *ParserState)
{
	parser_result Result = Expression(ParserState);
	if((Result.Error.Type != NoError) && (ParserState->CurrentToken.Type != TT_EOF))
	{
		return OnParseFailure(&Result, MakeError(InvalidSyntaxError, "Expected '+', '-', '*', '/'"));
	}

	return Result;
}
