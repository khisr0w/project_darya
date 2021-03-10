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
	void *Result = (uint8 *)Node + sizeof(node);

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

	return Result;
}

inline number *
OnVisitRegister(visit_result *Result, visit_result VisitResult)
{
	if(VisitResult.Error.Type != NoError) Result->Error = VisitResult.Error;
	return VisitResult.Number;
}

inline visit_result
OnVisitSuccess(visit_result *Result, number *Number)
{
	Result->Number = Number;

	return *Result;
}

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

inline visit_result Visit(node_memory *NodeMemory, node *Node, context* Context);

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

inline op_result
Add(node_memory *NodeMemory, number *Left, number *Right)
{
	op_result OpResult = {};

	if((Left->Type == NUM_FLOAT) && (Right->Type == NUM_FLOAT))
	{
		real32 Result = *((real32 *)Left->Number) + *((real32 *)Right->Number);
		OpResult.Number = PushNumber(NodeMemory, Result);
	}
	else if(Left->Type == NUM_FLOAT)
	{
		real32 Result = *((real32 *)Left->Number) + *((int32 *)Right->Number);
		OpResult.Number = PushNumber(NodeMemory, Result);
	}
	else if(Right->Type == NUM_FLOAT)
	{
		real32 Result = *((int32 *)Left->Number) + *((real32 *)Right->Number);
		OpResult.Number = PushNumber(NodeMemory, Result);
	}
	else
	{
		int32 Result = *((int32 *)Left->Number) + *((int32 *)Right->Number);
		OpResult.Number = PushNumber(NodeMemory, Result);
	}

	return OpResult;
}

inline op_result
Subtract(node_memory *NodeMemory, number *Left, number *Right)
{
	op_result OpResult = {};

	if((Left->Type == NUM_FLOAT) && (Right->Type == NUM_FLOAT))
	{
		real32 Result = *((real32 *)Left->Number) - *((real32 *)Right->Number);
		OpResult.Number = PushNumber(NodeMemory, Result);
	}
	else if(Left->Type == NUM_FLOAT)
	{
		real32 Result = *((real32 *)Left->Number) - *((int32 *)Right->Number);
		OpResult.Number = PushNumber(NodeMemory, Result);
	}
	else if(Right->Type == NUM_FLOAT)
	{
		real32 Result = *((int32 *)Left->Number) - *((real32 *)Right->Number);
		OpResult.Number = PushNumber(NodeMemory, Result);
	}
	else
	{
		int32 Result = *((int32 *)Left->Number) - *((int32 *)Right->Number);
		OpResult.Number = PushNumber(NodeMemory, Result);
	}

	return OpResult;
}

inline op_result
Multiply(node_memory *NodeMemory, number *Left, number *Right)
{
	op_result OpResult = {};

	if((Left->Type == NUM_FLOAT) && (Right->Type == NUM_FLOAT))
	{
		real32 Result = *((real32 *)Left->Number) * *((real32 *)Right->Number);
		OpResult.Number = PushNumber(NodeMemory, Result);
	}
	else if(Left->Type == NUM_FLOAT)
	{
		real32 Result = *((real32 *)Left->Number) * *((int32 *)Right->Number);
		OpResult.Number = PushNumber(NodeMemory, Result);
	}
	else if(Right->Type == NUM_FLOAT)
	{
		real32 Result = *((int32 *)Left->Number) * *((real32 *)Right->Number);
		OpResult.Number = PushNumber(NodeMemory, Result);
	}
	else
	{
		int32 Result = *((int32 *)Left->Number) * *((int32 *)Right->Number);
		OpResult.Number = PushNumber(NodeMemory, Result);
	}

	return OpResult;
}

inline op_result
Division(node_memory *NodeMemory, number *Left, number *Right)
{
	op_result OpResult = {};

	real32 LeftNum = *((real32 *)Left->Number);
	real32 RightNum = *((real32 *)Right->Number);

	if((Left->Type == NUM_FLOAT) && (Right->Type == NUM_FLOAT)) {}
	else if(Left->Type == NUM_FLOAT)
	{
		RightNum = (real32)(*((int32 *)Right->Number));
	}
	else if(Right->Type == NUM_FLOAT)
	{
		LeftNum = (real32)(*((int32 *)Left->Number));
	}
	else
	{
		LeftNum = (real32)(*((int32 *)Left->Number));
		RightNum = (real32)(*((int32 *)Right->Number));
	}

	real32 Result = LeftNum / RightNum;
	OpResult.Number = PushNumber(NodeMemory, Result);
	return OpResult;
}

internal visit_result
Visit_Number(node_memory *NodeMemory, node *Node, context *Context)
{
	visit_result Result = {};

	number_node *NumberNode = GetNode(Node, number_node);
	token Token = NumberNode->Token;
	number *Number = 0;

	if(Token.Type == TT_FLOAT)
	{
		real32 Num = ToReal(Token.Value).Value;
		Number = PushNumber(NodeMemory, Num);
	}
	else if(Token.Type == TT_INT)
	{
		int32 Num = ToInt(Token.Value).Value;
		Number = PushNumber(NodeMemory, Num);
	}
	else InvalidCodePath;

	return OnVisitSuccess(&Result, Number);
}

internal visit_result
Visit_UnaryOp(node_memory *NodeMemory, node *Node, context *Context)
{
	visit_result Result = {};

	unary_node *Unary = GetNode(Node, unary_node);
	number *Number = OnVisitRegister(&Result, Visit(NodeMemory, Unary->Node, Context));
	if(Result.Error.Type != NoError) return Result;

	op_result OpResult = {};
	if(Unary->OpToken.Type == TT_MINUS)
	{
		number One = {};
		One.Type = NUM_INT;
		int32 Temp = -1;
		One.Number = &Temp;
		OpResult = Multiply(NodeMemory, Number, &One);
	}

	if(OpResult.Error.Type != NoError) return OnVisitFailure(&Result, OpResult.Error);
	else return OnVisitSuccess(&Result, OpResult.Number);
}

internal visit_result
Visit_BinaryOp(node_memory *NodeMemory, node *Node, context *Context)
{
	visit_result Result = {};
	binary_node *Binary = GetNode(Node, binary_node);
	number *Left = OnVisitRegister(&Result, Visit(NodeMemory, Binary->LeftNode, Context));
	if(Result.Error.Type != NoError) return Result;
	number *Right = OnVisitRegister(&Result, Visit(NodeMemory, Binary->RightNode, Context));
	if(Result.Error.Type != NoError) return Result;

	op_result OpResult = {};

	if(Binary->OpToken.Type == TT_PLUS)
	{
		OpResult = Add(NodeMemory, Left, Right);
	}
	else if(Binary->OpToken.Type == TT_MINUS)
	{
		OpResult = Subtract(NodeMemory, Left, Right);
	}
	else if(Binary->OpToken.Type == TT_MUL)
	{
		OpResult = Multiply(NodeMemory, Left, Right);
	}
	else if(Binary->OpToken.Type == TT_DIV)
	{
		OpResult = Division(NodeMemory, Left, Right);
	}
	else InvalidCodePath;

	if(OpResult.Error.Type != NoError) return OnVisitFailure(&Result, OpResult.Error);
	else return OnVisitSuccess(&Result, OpResult.Number);
}

inline visit_result
Visit(node_memory *NodeMemory, node *Node, context* Context)
{
	visit_result Result = {};

	if(IsNodeType(Node, NT_number_node)) Result.Number = OnVisitRegister(&Result, Visit_Number(NodeMemory, Node, Context));
	else if(IsNodeType(Node, NT_unary_node)) Result.Number = OnVisitRegister(&Result, Visit_UnaryOp(NodeMemory, Node, Context));
	else if(IsNodeType(Node, NT_binary_node)) Result.Number = OnVisitRegister(&Result, Visit_BinaryOp(NodeMemory, Node, Context));

	else Result.Error = MakeError(VisitError, "Visit function not defined for this type!");

	return Result;
}
