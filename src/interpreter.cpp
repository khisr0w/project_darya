/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  3/9/2021 6:23:02 AM                                           |
    |    Last Modified:                                                                |
    |                                                                                  |
    +=====================| Sayed Abid Hashimi, Copyright © All rights reserved |======+  */

#include "interpreter.h"

internal context
MakeContext(char *DisplayName, context *Parent = 0, position *ParentPos = 0)
{
	context Result = {};

	Result.DisplayName = DisplayName;
	if(Parent) Result.Parent = Parent;
	if(ParentPos) Result.ParentPos = ParentPos;

	return Result;
}

#define GetNode(Node, Type) (Type *)GetNode_(Node)
inline void *
GetNode_(node * Node)
{
	void *Result = (uint8 *)Node + sizeof(node *);

	return Result;
}

#define PushStruct(MemoryBase, MemorySize, MaxMemorySize, Struct) (Struct *)PushStruct_(MemoryBase, MemorySize, \
																			  			MaxMemorySize, sizeof(Struct))
inline void *
PushStruct_(void *MemoryBase, uint32 *MemorySize, uint32 MaxMemorySize, uint32 Size)
{
	void *Result = 0;

	if((*MemorySize + Size) < MaxMemorySize)
	{
		Result = (uint8 *)MemoryBase + *MemorySize;
		*MemorySize += Size;
	}
}

inline number *
OnVisitRegister(visit_result *Result, visit_result VisitResult)
{
	if(VisitResult.Error.Type != NoError) Result->Error = VisitResult.Error;
	return VisitResult.Number;
}

#if 0

inline visit_result
OnVisitSuccess(visit_result *Result, node *Node)
{
	Result->Value = Node;

	return *Result;
}
#endif

inline visit_result
OnVisitFailure(visit_result *Result, error Error)
{
	Result->Error = Error;

	return *Result;
}

inline bool32
IsTokenType(token Token, token_type Type) { return Token.Type == Type; }

inline bool32
IsNodeType(node *Node, node_type Type) { return Node->Header.Type == Type; }

internal visit_result Visit(node *Node, context* Context);

#if 0
internal visit_result
Visit_Number(node *Node, context *Context)
{
	visit_result Result = {};

	Result.Value = GetNode(Node, node);

	return OnVisitSuccess(&Result, Node);
}
#endif

internal number *
PushNumber(node_memory *NodeMemory, real32 Value)
{
	number *Number = PushStruct(NodeMemory->MemoryBase, &NodeMemory->Size,
								NodeMemory->MaxMemorySize, number);
	Number->Type = NUM_FLOAT;
	Number->Number = PushStruct(NodeMemory->MemoryBase, &NodeMemory->Size, NodeMemory->MaxMemorySize, real32);
	real32 *Num = (real32 *)Number->Number;
	*Num = Value;

	return Number;
}

internal number *
PushNumber(node_memory *NodeMemory, int32 Value)
{
	number *Number = PushStruct(NodeMemory->MemoryBase, &NodeMemory->Size,
								NodeMemory->MaxMemorySize, number);
	Number->Type = NUM_INT;
	Number->Number = PushStruct(NodeMemory->MemoryBase, &NodeMemory->Size, NodeMemory->MaxMemorySize, int32);
	int32 *Num = (int32 *)Number->Number;
	*Num = Value;

	return Number;
}

inline number *
Add(node_memory *NodeMemory, number *Left, number *Right)
{
	number_node *LeftNode = GetNode(Left, number_node);
	number_node *RightNode = GetNode(Right, number_node);

	token LeftToken = LeftNode->Token;
	token RightToken = RightNode->Token;

	if((LeftToken.Type == TT_FLOAT) || (RightToken.Type == TT_FLOAT))
	{
		real32 Result = ToReal(LeftToken.Value).Value + ToReal(RightToken.Value).Value;
		return PushNumber(NodeMemory, Result);
	}
	else
	{
		int32 Result = ToInt(LeftToken.Value).Value + ToInt(RightToken.Value).Value;
		return PushNumber(NodeMemory, Result);
	}
}

inline number *
Subtract(node_memory *NodeMemory, number *Left, number *Right)
{
	number_node *LeftNode = GetNode(Left, number_node);
	number_node *RightNode = GetNode(Right, number_node);

	token LeftToken = LeftNode->Token;
	token RightToken = RightNode->Token;

	if((LeftToken.Type == TT_FLOAT) || (RightToken.Type == TT_FLOAT))
	{
		real32 Result = ToReal(LeftToken.Value).Value - ToReal(RightToken.Value).Value;
		return PushNumber(NodeMemory, Result);
	}
	else
	{
		int32 Result = ToInt(LeftToken.Value).Value - ToInt(RightToken.Value).Value;
		return PushNumber(NodeMemory, Result);
	}
}

inline number *
Multiply(node_memory *NodeMemory, number *Left, number *Right)
{
	number_node *LeftNode = GetNode(Left, number_node);
	number_node *RightNode = GetNode(Right, number_node);

	token LeftToken = LeftNode->Token;
	token RightToken = RightNode->Token;

	if((LeftToken.Type == TT_FLOAT) || (RightToken.Type == TT_FLOAT))
	{
		real32 Result = ToReal(LeftToken.Value).Value / ToReal(RightToken.Value).Value;
		return PushNumber(NodeMemory, Result);
	}
	else
	{
		int32 Result = ToInt(LeftToken.Value).Value + ToInt(RightToken.Value).Value;
		return PushNumber(NodeMemory, Result);
	}
}

inline number *
Division(node_memory *NodeMemory, number *Left, number *Right)
{
	number_node *LeftNode = GetNode(Left, number_node);
	number_node *RightNode = GetNode(Right, number_node);

	token LeftToken = LeftNode->Token;
	token RightToken = RightNode->Token;

	if((LeftToken.Type == TT_FLOAT) || (RightToken.Type == TT_FLOAT))
	{
		real32 Result = ToReal(LeftToken.Value).Value / ToReal(RightToken.Value).Value;
		return PushNumber(NodeMemory, Result);
	}
	else
	{
		real32 Result = ToInt(LeftToken.Value).Value / ToInt(RightToken.Value).Value;
		return PushNumber(NodeMemory, Result);
	}
}


internal visit_result
Visit_UnaryOp(node_memory *NodeMemory, node *Node, context *Context)
{
	visit_result Result = {};
	unary_node *Unary = GetNode(Node, unary_node);
	number *Number = OnVisitRegister(&Result, Visit(Node, Context));
	if(Result.Error.Type != NoError) return Result;

	op_result OpResult = {};
	if(Unary->OpToken.Type == TT_MINUS)
	{
		number_node One = {};
		Concat(One.Token.Value, false, "-1");
		One.Token.Type = TT_INT;
		OpResult = Multiply(NodeMemory, Number, One);
	}

	if(!OpResult.Error.Type != NoError) return OnVisitFailure(&Result, OpResult.Error);
	else return OnVisitSuccess(&Result, OpResult.Node);
}

internal visit_result
Visit_BinaryOp(node_memory *NodeMemory, node *Node, context *Context)
{
	visit_result Result = {};
	binary_node *Binary = GetNode(Node, binary_node);
	node *Left = OnVisitRegister(&Result, Visit(Binary->LeftNode, Context));
	if(Result.Error.Type != NoError) return Result;
	node *Right = OnVisitRegister(&Result, Visit(Binary->RightNode, Context));
	if(Result.Error.Type != NoError) return Result;

	op_result OpResult = {};

	if(Binary->OpToken.Type == TT_PLUS)
	{
		OpResult = Add(NodeMemory, Left, Right);
	}
	if(Binary->OpToken.Type == TT_MINUS)
	{
		OpResult = Subtract(NodeMemory, Left, Right);
	}
	if(Binary->OpToken.Type == TT_MUL)
	{
		OpResult = Multiply(NodeMemory, Left, Right);
	}
	if(Binary->OpToken.Type == TT_DIV)
	{
		OpResult = Division(NodeMemory, Left, Right);
	}

	if(!OpResult.Error.Type != NoError) return OnVisitFailure(&Result, OpResult.Error);
	else return OnVisitSuccess(&Result, OpResult.Node);
}

internal visit_result
InterpreterVisit(transient_memory *TranMemory, node *Node, context* Context)
{
	visit_result Result = {};

	if(IsNodeType(Node, NT_number_node)) Result = OnVisitSuccess(&Result, Node);
	else if(IsNodeType(Node, NT_unary_node)) Result = Visit_UnaryOp(TranMemory, Node, Context);
	else if(IsNodeType(Node, NT_binary_node)) Result = Visit_BinaryOp(TranMemory, Node, Context);

	else Result.Error = MakeError(VisitError, "Visit function not defined for this type!");

	return Result;
}
