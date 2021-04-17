/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  3/6/2021 8:50:39 AM                                           |
    |    Last Modified:                                                                |
    |                                                                                  |
    +=====================| Sayed Abid Hashimi, Copyright © All rights reserved |======+  */

#include "parser.h"

typedef parser_result bin_op_delegate(parser_state *ParserState);

inline token *
Advance(parser_state *ParserState)
{
	dynamic_memory_block *Block = (dynamic_memory_block *)ParserState->CurrentToken - 1;

	if(Block->Next)
	{
		ParserState->CurrentToken = (token *)(Block->Next + 1);
		ParserState->Token_Id++;
	}

	return ParserState->CurrentToken;
}

inline token *
LookAheadToken(parser_state *ParserState, int32 Times = 1)
{
	token *Token = ParserState->CurrentToken;

	for(int32 Jump = 0;
		Jump < Times;
		++Jump)
	{
		dynamic_memory_block *Block = (dynamic_memory_block *)Token - 1;
		if(Block->Next) Token = (token *)(Block->Next + 1);
	}

	return Token;
}

inline void
InitializeParser(parser_state *ParserState, dynamic_memory_arena *Tokens)
{
	Assert(ParserState && Tokens && Tokens->Blocks);
	FreeDynamicArena(&ParserState->AST);
	ParserState->TokenLength = Tokens->Length;
	ParserState->CurrentToken = (token *)(Tokens->Blocks + 1);
	ParserState->Token_Id = 0;
}

#define PushNode(ParserState, Type) (node *)PushNode_ (ParserState, sizeof(Type), NodeType_##Type)
inline void *
PushNode_(parser_state *ParserState, uint32 Size, node_type Type)
{
	Size += sizeof(node);

#if 0
	node *Node = (node *)PushSize_(&ParserState->AST, Size);
#endif
	node *Node = (node *)PushDynamicSize_(&ParserState->AST, Size);
	Node->Header.Type = Type;

	return Node;
}

inline token *
OnParseRegister(parser_result *Result, token *Token)
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
IsTokenInList(token *CurrentToken, token_type *TokenList, int32 TokenLength)
{
	for(int32 Index = 0;
		Index < TokenLength;
		++Index)
	{
		if(TokenList[Index] == CurrentToken->Type)
		{
			return true;
		}
	}

	return false;
}

#define MakeNode(ParserState, Token, First, Second, Type) MakeNode_(ParserState, Token, First, Second, NodeType_##Type)
inline node *
MakeNode_(parser_state *ParserState, token *Token, node *First, node *Second, node_type Type)
{
	node *Result = 0;

	switch(Type)
	{
		case NodeType_number_node:
		{
			Result = PushNode(ParserState, number_node);
			number_node *NumberNode = (number_node *)(Result + 1);

			if(Token->Type == TT_REAL)
			{
				NumberNode->Real = ToReal(Token->Value).Value;
				NumberNode->Type = NumberType_Real;
			}
			else if(Token->Type == TT_INT)
			{
				NumberNode->Int = ToInt(Token->Value).Value;
				NumberNode->Type = NumberType_Int;
			}
			else InvalidCodePath;

			NumberNode->Pos.Start = Token->StartPos;
			NumberNode->Pos.End = Token->EndPos;
		} break;

		case NodeType_unary_node:
		{
			Assert(First);
			Result = PushNode(ParserState, unary_node);
			unary_node *UnaryNode = (unary_node *)(Result + 1);

			UnaryNode->OpToken = *Token;
			UnaryNode->Node = First;

			node_pos *Pos = (node_pos *)(First + 1);
			UnaryNode->Pos.Start = Token->StartPos;
			UnaryNode->Pos.End = Pos->End;
		} break;

		case NodeType_binary_node:
		{
			Assert(First && Second);
			Result = PushNode(ParserState, binary_node);
			binary_node *BinaryNode = (binary_node *)(Result + 1);

			BinaryNode->LeftNode = First;
			BinaryNode->OpToken = *Token;
			BinaryNode->RightNode = Second;

			node_pos *LeftPos = (node_pos *)(First + 1);
			node_pos *RightPos = (node_pos *)(Second + 1);

			BinaryNode->Pos.Start = LeftPos->Start;
			BinaryNode->Pos.End = RightPos->End;
		} break;

		case NodeType_compound_compare_node:
		{
			Assert(First && Second);
			Result = PushNode(ParserState, compound_compare_node);
			compound_compare_node *CompoundCompareNode = (compound_compare_node *)(Result + 1);

			CompoundCompareNode->LeftNode = First;
			CompoundCompareNode->OpToken = *Token;
			CompoundCompareNode->RightNode = Second;

			node_pos *LeftPos = (node_pos *)(First + 1);
			node_pos *RightPos = (node_pos *)(Second + 1);

			CompoundCompareNode->Pos.Start = LeftPos->Start;
			CompoundCompareNode->Pos.End = RightPos->End;
		} break;

		case NodeType_var_assign_node:
		{
			Assert(First);
			Result = PushNode(ParserState, var_assign_node);
			var_assign_node *VarAssignNode = (var_assign_node *)(Result + 1);

			VarAssignNode->Name = *Token;
			VarAssignNode->Value = First;
			if(Second) VarAssignNode->New = true;
			else VarAssignNode->New = false;

			node_pos *Pos = (node_pos *)(First + 1);

			VarAssignNode->Pos.Start = Token->StartPos;
			VarAssignNode->Pos.End = Pos->End;
		} break;

		case NodeType_var_access_node:
		{
			Result = PushNode(ParserState, var_access_node);
			var_access_node *VarAccessNode = (var_access_node *)(Result + 1);

			VarAccessNode->Name = *Token;

			VarAccessNode->Pos.Start = Token->StartPos;
			VarAccessNode->Pos.End = Token->EndPos;
		} break;

		case NodeType_comparison_node:
		{
			Assert(First && Second);
			Result = PushNode(ParserState, comparison_node);
			comparison_node *ComparisonNode = (comparison_node *)(Result + 1);

			ComparisonNode->LeftNode = First;
			ComparisonNode->OpToken = *Token;
			ComparisonNode->RightNode = Second;

			node_pos *LeftPos = (node_pos *)(First + 1);
			node_pos *RightPos = (node_pos *)(Second + 1);

			ComparisonNode->Pos.Start = LeftPos->Start;
			ComparisonNode->Pos.End = RightPos->End;
		} break;

		case NodeType_statements_node:
		{
			Result = PushNode(ParserState, statements_node);
			statements_node *StatementsNode = (statements_node *)(Result + 1);
			StatementsNode->MaxLength = 16;
			StatementsNode->Statement = (node **)PushDynamicSize_(&ParserState->AST,
																  StatementsNode->MaxLength*sizeof(node **));
			StatementsNode->Length = 0;
		} break;

		case NodeType_function_call_node:
		{
			Result = PushNode(ParserState, function_call_node);
			function_call_node *FuncCallNode = (function_call_node *)(Result + 1);
			FuncCallNode->MaxLength = 4;
			FuncCallNode->Args = (node **)PushDynamicSize_(&ParserState->AST,
														   FuncCallNode->MaxLength*sizeof(node **));
			FuncCallNode->ArgLength = 0;
			FuncCallNode->FuncName = *Token;

			FuncCallNode->Pos.Start = Token->StartPos;
			FuncCallNode->Pos.End = Token->EndPos;
		} break;

		case NodeType_function_def_node:
		{
			Result = PushNode(ParserState, function_def_node);
			function_def_node *FuncDefNode = (function_def_node *)(Result + 1);
			FuncDefNode ->MaxArgLength = 4;
			FuncDefNode ->Args = (node **)PushDynamicSize_(&ParserState->AST,
														   FuncDefNode->MaxArgLength*sizeof(node **));
			FuncDefNode ->ArgLength = 0;
			FuncDefNode ->FuncName = *Token;

			FuncDefNode ->Pos.Start = Token->StartPos;
			FuncDefNode ->Pos.End = Token->EndPos;
		} break;

		case NodeType_if_node:
		{
			Result = PushNode(ParserState, if_node);
			if_node *IfNode = (if_node *)(Result + 1);
			// IfNode->Args = (node **)PushDynamicSize_(&ParserState->AST, IfNode->OtherLength*sizeof(node **));
			IfNode->OtherLength = 0;

			IfNode->Pos.Start = Token->StartPos;
			IfNode->Pos.End = Token->EndPos;
		} break;
		case NodeType_other_node:
		{
			Result = PushNode(ParserState, other_node);
			other_node *OtherNode = (other_node *)(Result + 1);

			OtherNode->Pos.Start = Token->StartPos;
			OtherNode->Pos.End = Token->EndPos;
		} break;
		case NodeType_else_node:
		{
			Result = PushNode(ParserState, else_node);
			else_node *ElseNode = (else_node *)(Result + 1);

			ElseNode->Pos.Start = Token->StartPos;
			ElseNode->Pos.End = Token->EndPos;
		} break;
		case NodeType_string_node:
		{
			Result = PushNode(ParserState, string_node);
			string_node *StringNode = (string_node *)(Result + 1);

			StringNode->Value = Token->Value;

			StringNode->Pos.Start = Token->StartPos;
			StringNode->Pos.End = Token->EndPos;
		} break;

		// case NodeType_end_of_file_node:
		// {
		// 	Result = PushNode(ParserState, end_of_file_node);
		// 	end_of_file_node *EOFNode = (end_of_file_node *)(Result + 1);

		// 	EOFNode->Pos.Start = Token->StartPos;
		// 	EOFNode->Pos.End = Token->EndPos;

		// } break;

		default: InvalidCodePath;
	}

	return Result;
}

#define BinaryOperation(ParserState, DelegateFuncA, OpTypes, DelegateFuncB) BinaryOperation_(ParserState, DelegateFuncA,\
																			OpTypes, ArrayCount(OpTypes), DelegateFuncB)
internal parser_result
BinaryOperation_(parser_state *ParserState, bin_op_delegate *DelegateFuncA,
				 token_type *OpTypes, int32 OpLength, bin_op_delegate *DelegateFuncB = 0)
{
	if(DelegateFuncB == 0) DelegateFuncB = DelegateFuncA;

	parser_result Result = {};
	token *OpToken = {};
	node *Left = OnParseRegister(&Result, DelegateFuncA(ParserState));
	if (Result.Error.Type != NoError) return Result;

	while(IsTokenInList(ParserState->CurrentToken, OpTypes, OpLength))
	{
		OpToken = ParserState->CurrentToken;
		OnParseRegister(&Result, Advance(ParserState));
		node *Right = OnParseRegister(&Result, DelegateFuncB(ParserState));
		if (Result.Error.Type != NoError) return Result;
		Left = MakeNode(ParserState, OpToken, Left, Right, binary_node);
	}

	return OnParseSuccess(&Result, Left);
}

inline parser_result Statement(parser_state *ParserState);
inline parser_result CompareExpression(parser_state *ParserState);

inline parser_result
CompoundCompareExpression(parser_state *ParserState)
{
	parser_result Result = {};

	token *OpToken = {};
	node *Left = OnParseRegister(&Result, CompareExpression(ParserState));
	if (Result.Error.Type != NoError) return Result;

	token_type OpTypes[] = {TT_AND, TT_OR};
	if(IsTokenInList(ParserState->CurrentToken, OpTypes, ArrayCount(OpTypes)))
	{
		OpToken = ParserState->CurrentToken;
		OnParseRegister(&Result, Advance(ParserState));
		node *Right = OnParseRegister(&Result, CompoundCompareExpression(ParserState));
		if (Result.Error.Type != NoError) return Result;
		Left = MakeNode(ParserState, OpToken, Left, Right, compound_compare_node);
	}

	return OnParseSuccess(&Result, Left);
}

inline node *
PushFunctionArg(function_def_node *FuncDefNode, node *Memory)
{
	node **Node = 0;
	Assert(Memory);
	if(Memory)
	{
		if(FuncDefNode->ArgLength >= FuncDefNode->MaxArgLength)
		{
			FuncDefNode->MaxArgLength += 6;
			FuncDefNode->Args = (node **)PlatformReallocMem(FuncDefNode->Args,
															FuncDefNode->MaxArgLength*sizeof(node **));
		}

		Node = (FuncDefNode->Args + FuncDefNode->ArgLength++);
		if(Node) *Node = Memory;
	}

	return *Node;
}

inline node *
PushFunctionArg(function_call_node *FuncCallNode, node *Memory)
{
	node **Node = 0;
	Assert(Memory);
	if(Memory)
	{
		if(FuncCallNode->ArgLength >= FuncCallNode->MaxLength)
		{
			FuncCallNode->MaxLength += 6;
			FuncCallNode->Args = (node **)PlatformReallocMem(FuncCallNode->Args,
															 FuncCallNode->MaxLength*sizeof(node **));
		}

		Node = (FuncCallNode->Args + FuncCallNode->ArgLength++);
		if(Node) *Node = Memory;
	}

	return *Node;
}

internal parser_result
Primitive(parser_state *ParserState)
{
	parser_result Result = {};
	token *CurrentToken = ParserState->CurrentToken;

	if((CurrentToken->Type == TT_INT) || (CurrentToken->Type == TT_REAL))
	{
		OnParseRegister(&Result, Advance(ParserState));
		return OnParseSuccess(&Result, MakeNode(ParserState, CurrentToken, 0, 0, number_node));
	}
	else if(CurrentToken->Type == TT_STRING)
	{
		OnParseRegister(&Result, Advance(ParserState));
		return OnParseSuccess(&Result, MakeNode(ParserState, CurrentToken, 0, 0, string_node));
	}
	else if(CurrentToken->Type == TT_ID)
	{
		token *NextToken = LookAheadToken(ParserState);
		if(NextToken && NextToken->Type == TT_LPAREN)
		{
			Advance(ParserState);
			Advance(ParserState);

			node *FuncNode = MakeNode(ParserState, CurrentToken, 0, 0, function_call_node);
			function_call_node *FuncCallNode = (function_call_node *)(FuncNode + 1);

			while(ParserState->CurrentToken->Type == TT_NEWLINE) Advance(ParserState);
			if(ParserState->CurrentToken->Type == TT_RPAREN)
			{
				FuncCallNode->Pos.End = ParserState->CurrentToken->EndPos;
				Advance(ParserState);
				return OnParseSuccess(&Result, FuncNode);
			}

			do
			{
				while(ParserState->CurrentToken->Type == TT_NEWLINE) Advance(ParserState);
				node *Node = OnParseRegister(&Result, CompoundCompareExpression(ParserState));
				PushFunctionArg(FuncCallNode, Node);
				if(Result.Error.Type != NoError) return Result;
				while(ParserState->CurrentToken->Type == TT_NEWLINE) Advance(ParserState);
			} while(ParserState->CurrentToken->Type == TT_COMMA && Advance(ParserState));

			while(ParserState->CurrentToken->Type == TT_NEWLINE) Advance(ParserState);
			if(ParserState->CurrentToken->Type != TT_RPAREN)
			{
				error Error = {};
				Error.Type = InvalidSyntaxError;
				Concat(Error.Message, false, "InvalidSyntaxError: Expected ')' at the end of '", CurrentToken->Value, "' function call");
				return OnParseFailure(&Result, Error);
			}
			FuncCallNode->Pos.End = ParserState->CurrentToken->EndPos;
			Advance(ParserState);
			return OnParseSuccess(&Result, FuncNode);
		}
		OnParseRegister(&Result, Advance(ParserState));
		return OnParseSuccess(&Result, MakeNode(ParserState, CurrentToken, 0, 0, var_access_node));
	}
	else if(CurrentToken->Type == TT_NOT)
	{
		OnParseRegister(&Result, Advance(ParserState));
		node *Node = OnParseRegister(&Result, Primitive(ParserState));
		if(Result.Error.Type != NoError) return Result;
		return OnParseSuccess(&Result, MakeNode(ParserState, CurrentToken, Node, 0, unary_node));
	}
	else if(CurrentToken->Type == TT_LPAREN)
	{
		OnParseRegister(&Result, Advance(ParserState));
		node * Statement = OnParseRegister(&Result, CompoundCompareExpression(ParserState));
		if(Result.Error.Type != NoError) return Result;
		if(ParserState->CurrentToken->Type == TT_RPAREN)
		{
			OnParseRegister(&Result, Advance(ParserState));
			return OnParseSuccess(&Result, Statement);
		}
		else return OnParseFailure(&Result, MakeError(InvalidSyntaxError, "Expected ')'"));
	}

	return OnParseFailure(&Result, MakeError(InvalidSyntaxError, "Expected int or float '+', '-' or '('"));
}

internal parser_result Factor(parser_state *ParserState);

inline parser_result
Power(parser_state *ParserState)
{
	token_type OpTypes[] = {TT_POW};
	return BinaryOperation(ParserState, Primitive, OpTypes, Factor);
}

internal parser_result
Factor(parser_state *ParserState)
{
	parser_result Result = {};
	token *CurrentToken = ParserState->CurrentToken;

	if((CurrentToken->Type == TT_PLUS) || (CurrentToken->Type == TT_MINUS))
	{
		OnParseRegister(&Result, Advance(ParserState));
		parser_result ParResult = Factor(ParserState);
		node *Factor = OnParseRegister(&Result, ParResult);
		if(Result.Error.Type != NoError) return Result;
		return OnParseSuccess(&Result, MakeNode(ParserState, CurrentToken, Factor, 0, unary_node));
	}

	return Power(ParserState);
}

inline parser_result
Term(parser_state *ParserState)
{
	token_type OpTypes[] = {TT_MUL, TT_DIV};
	return BinaryOperation(ParserState, Factor, OpTypes, 0);
}

inline parser_result
Expression(parser_state *ParserState)
{
	parser_result Result = {};

	if(ParserState->CurrentToken->Type == TT_KEYWORD)
	{
		if(StringCompare(ParserState->CurrentToken->Value, "var"))
		{
			if(LookAheadToken(ParserState)->Type != TT_ID)
				return OnParseFailure(&Result, MakeError(InvalidSyntaxError, "Expected Identifier after 'var'"));
			if(LookAheadToken(ParserState, 2)->Type == TT_EQ)
			{
				Advance(ParserState);
				token *Name = ParserState->CurrentToken;
				Advance(ParserState);
				if(ParserState->CurrentToken->Type != TT_EQ)
					return OnParseFailure(&Result, MakeError(InvalidSyntaxError, "Expected '=' after identifier"));
				Advance(ParserState);
				parser_result ParResult = CompareExpression(ParserState);
				node *Value = OnParseRegister(&Result, ParResult);
				if(Result.Error.Type != NoError) return Result;
				return OnParseSuccess(&Result, MakeNode(ParserState, Name, Value, (node *)1, var_assign_node));
			}
		}
	}
	else if(ParserState->CurrentToken->Type == TT_ID)
	{
		token *NextToken = LookAheadToken(ParserState);
		if(NextToken && NextToken->Type == TT_EQ)
		{
			token *Name = ParserState->CurrentToken;
			Advance(ParserState);
			Advance(ParserState);
			parser_result ParResult = CompareExpression(ParserState);
			node *Value = OnParseRegister(&Result, ParResult);
			if(Result.Error.Type != NoError) return Result;
			return OnParseSuccess(&Result, MakeNode(ParserState, Name, Value, 0, var_assign_node));
		}
	}

	token_type OpTypes[] = {TT_PLUS, TT_MINUS};
	return BinaryOperation(ParserState, Term, OpTypes, 0);
}

inline parser_result
CompareExpression(parser_state *ParserState)
{
	parser_result Result = {};

	token *OpToken = {};
	node *Left = OnParseRegister(&Result, Expression(ParserState));
	if (Result.Error.Type != NoError) return Result;

	token_type OpTypes[] = {TT_EQEQ, TT_NEQ, TT_GT, TT_GTE, TT_LT, TT_LTE};
	if(IsTokenInList(ParserState->CurrentToken, OpTypes, ArrayCount(OpTypes)))
	{
		OpToken = ParserState->CurrentToken;
		OnParseRegister(&Result, Advance(ParserState));
		node *Right = OnParseRegister(&Result, Expression(ParserState));
		if (Result.Error.Type != NoError) return Result;
		Left = MakeNode(ParserState, OpToken, Left, Right, comparison_node);
	}

	return OnParseSuccess(&Result, Left);
}

inline parser_result Statements(parser_state *ParserState);

internal parser_result
MakeConditionalBlock(parser_state *ParserState, node *Node, bool32 Condition=true)
{
	parser_result Result = {};

	Result.Node = Node;
	if_node *IfConditionalNode = (if_node *)(Node + 1);
	if(Condition)
	{
		if(ParserState->CurrentToken->Type != TT_LPAREN)
			return OnParseFailure(&Result, MakeError(InvalidSyntaxError, "Expected '(' after conditional declaration"));
		Advance(ParserState);
		while(ParserState->CurrentToken->Type == TT_NEWLINE) Advance(ParserState);

		node *IfCondition = OnParseRegister(&Result, CompoundCompareExpression(ParserState));
		if (Result.Error.Type != NoError) return Result;

		while(ParserState->CurrentToken->Type == TT_NEWLINE) Advance(ParserState);
		if(ParserState->CurrentToken->Type != TT_RPAREN)
			return OnParseFailure(&Result, MakeError(InvalidSyntaxError, "Expected ')' succeeding the conditional"));
		Advance(ParserState);
		IfConditionalNode->Condition = IfCondition;
	}

	if(ParserState->CurrentToken->Type == TT_NEWLINE) Advance(ParserState);
	if(ParserState->CurrentToken->Type != TT_LCBRACKET)
		return OnParseFailure(&Result, MakeError(InvalidSyntaxError, "Expected '{' prior to conditional body statement"));
	Advance(ParserState);

	while(ParserState->CurrentToken->Type == TT_NEWLINE) Advance(ParserState);
	if(ParserState->CurrentToken->Type == TT_RCBRACKET) return Result;

	node *IfBody = OnParseRegister(&Result, Statements(ParserState));
	if (Result.Error.Type != NoError) return Result;

	while(ParserState->CurrentToken->Type == TT_NEWLINE) Advance(ParserState);
	if(ParserState->CurrentToken->Type != TT_RCBRACKET)
		return OnParseFailure(&Result, MakeError(InvalidSyntaxError, "Expected '}' after conditional statement block"));
	IfConditionalNode->Pos.End = ParserState->CurrentToken->EndPos;
	Advance(ParserState);
	IfConditionalNode->Body = IfBody;

	return OnParseSuccess(&Result, Node);
}

inline parser_result
Statement(parser_state *ParserState)
{
	parser_result Result = {};

	if(ParserState->CurrentToken->Type == TT_KEYWORD)
	{
		if(StringCompare(ParserState->CurrentToken->Value, "var"))
		{
			if(LookAheadToken(ParserState)->Type != TT_ID)
				return OnParseFailure(&Result, MakeError(InvalidSyntaxError, "Expected Identifier after 'var'"));
			if(LookAheadToken(ParserState, 2)->Type == TT_LPAREN)
			{
				Advance(ParserState);
				token *Name = ParserState->CurrentToken;
				Advance(ParserState);
				Advance(ParserState);

				node *FuncNode = MakeNode(ParserState, Name, 0, 0, function_def_node);
				function_def_node *FuncDefNode = (function_def_node *)(FuncNode + 1);
				Result.Node = FuncNode;

				while(ParserState->CurrentToken->Type == TT_NEWLINE) Advance(ParserState);
				if(ParserState->CurrentToken->Type != TT_RPAREN)
				{
					do
					{
						while(ParserState->CurrentToken->Type == TT_NEWLINE) Advance(ParserState);
						node *Node = OnParseRegister(&Result, CompoundCompareExpression(ParserState));
						PushFunctionArg(FuncDefNode, Node);
						if(Result.Error.Type != NoError) return Result;
						while(ParserState->CurrentToken->Type == TT_NEWLINE) Advance(ParserState);
					} while(ParserState->CurrentToken->Type == TT_COMMA && Advance(ParserState));
					while(ParserState->CurrentToken->Type == TT_NEWLINE) Advance(ParserState);
				}
				if(ParserState->CurrentToken->Type != TT_RPAREN)
					return OnParseFailure(&Result, MakeError(InvalidSyntaxError, "Expected ')' after function definition argument conclusion"));
				Advance(ParserState);

				if(ParserState->CurrentToken->Type == TT_NEWLINE) Advance(ParserState);
				if(ParserState->CurrentToken->Type != TT_LCBRACKET)
					return OnParseFailure(&Result, MakeError(InvalidSyntaxError, "Expected '{' prior to function statements"));
				Advance(ParserState);
				while(ParserState->CurrentToken->Type == TT_NEWLINE) Advance(ParserState);
				if(ParserState->CurrentToken->Type == TT_RCBRACKET) return Result;

				node *FuncBody = OnParseRegister(&Result, Statements(ParserState));
				if (Result.Error.Type != NoError) return Result;
				FuncDefNode->Body = FuncBody;
				while(ParserState->CurrentToken->Type == TT_NEWLINE) Advance(ParserState);
				if(ParserState->CurrentToken->Type != TT_RCBRACKET)
					return OnParseFailure(&Result, MakeError(InvalidSyntaxError, "Expected '{' after function definition argument conclusion"));

				FuncDefNode->Pos.End = ParserState->CurrentToken->EndPos;
				Advance(ParserState);
				return OnParseSuccess(&Result, FuncNode);
			}
		}
		if(StringCompare(ParserState->CurrentToken->Value, "if"))
		{
			node *Node = MakeNode(ParserState, ParserState->CurrentToken, 0, 0, if_node);
			Advance(ParserState);
			if_node *IfNode = (if_node *)(Node + 1);
			OnParseRegister(&Result, MakeConditionalBlock(ParserState, Node));
			if(Result.Error.Type != NoError) return Result;

			token *AheadToken = ParserState->CurrentToken;
			for(uint32 Increment = 1; AheadToken->Type == TT_NEWLINE; ++Increment)
			{
				AheadToken = LookAheadToken(ParserState, Increment);
				if(AheadToken->Type == TT_KEYWORD && StringCompare(AheadToken->Value, "other"))
				{
					while(ParserState->CurrentToken->Type == TT_NEWLINE) Advance(ParserState);
					break;
				}
			}

			if(ParserState->CurrentToken->Type == TT_KEYWORD &&
			   StringCompare(ParserState->CurrentToken->Value, "other"))
			{
				IfNode->MaxOtherLength += 8;
				IfNode->Others = (node **)PlatformAllocMem(IfNode->MaxOtherLength*sizeof(node **));
				while(ParserState->CurrentToken->Type == TT_KEYWORD &&
					  StringCompare(ParserState->CurrentToken->Value, "other"))
				{
					Advance(ParserState);
					node *OtherNode = MakeNode(ParserState, ParserState->CurrentToken, 0, 0, other_node);
					OnParseRegister(&Result, MakeConditionalBlock(ParserState, OtherNode));
					if(Result.Error.Type != NoError) return Result;

					if(IfNode->OtherLength >= IfNode->MaxOtherLength)
					{
						IfNode->MaxOtherLength += 8;
						IfNode->Others = (node **)PlatformReallocMem(IfNode->Others,
																	 IfNode->MaxOtherLength*sizeof(node **));
					}
					IfNode->Others[IfNode->OtherLength++] = OtherNode;
				}
			}
			AheadToken = ParserState->CurrentToken;
			for(uint32 Increment = 1; AheadToken->Type == TT_NEWLINE; ++Increment)
			{
				AheadToken = LookAheadToken(ParserState, Increment);
				if(AheadToken->Type == TT_KEYWORD && StringCompare(AheadToken->Value, "else"))
				{
					while(ParserState->CurrentToken->Type == TT_NEWLINE) Advance(ParserState);
					break;
				}
			}
			if(ParserState->CurrentToken->Type == TT_KEYWORD &&
			   StringCompare(ParserState->CurrentToken->Value, "else"))
			{
				Advance(ParserState);
				node *ElseNode = MakeNode(ParserState, ParserState->CurrentToken, 0, 0, else_node);
				OnParseRegister(&Result, MakeConditionalBlock(ParserState, ElseNode, false));
				if(Result.Error.Type != NoError) return Result;
				IfNode->Else = ElseNode;
			}

			return OnParseSuccess(&Result, Node);
		}
	}

	return CompoundCompareExpression(ParserState);
}

inline node *
PushStatement(statements_node *Statements, node *Memory)
{
	node **Node = 0;
	Assert(Memory);
	if(Memory)
	{
		if(Statements->Length >= Statements->MaxLength)
		{
			Statements->MaxLength += 32;
			Statements->Statement = (node **)PlatformReallocMem(Statements->Statement,
																Statements->MaxLength*sizeof(node **));
		}

		Node = (Statements->Statement + Statements->Length++);
		if(Node) *Node = Memory;
	}

	return *Node;
}

inline parser_result
Statements(parser_state *ParserState)
{
	parser_result Result = {};
	node *Node = MakeNode(ParserState, 0, 0, 0, statements_node);
	statements_node *StatementsNode = (statements_node *)(Node + 1);
	do
	{
		while(ParserState->CurrentToken->Type == TT_NEWLINE) Advance(ParserState);
		if((ParserState->CurrentToken->Type == TT_EOF) || (ParserState->CurrentToken->Type == TT_RCBRACKET)) break;
		parser_result ParResult = Statement(ParserState);
		if(ParResult.Error.Type != NoError) return ParResult;
		PushStatement(StatementsNode, ParResult.Node);
	} while(ParserState->CurrentToken->Type == TT_NEWLINE);

	return OnParseSuccess(&Result, Node);
}

inline parser_result
ParseTokens(parser_state *ParserState)
{
	parser_result Result = Statements(ParserState);
	if((Result.Error.Type == NoError) && (ParserState->CurrentToken->Type != TT_EOF))
	{
		return OnParseFailure(&Result, MakeError(InvalidSyntaxError, "Expected '+', '-', '*', '/' as the operator"));
	}

	return Result;
}
