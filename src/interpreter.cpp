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
GetNode_(node *Node)
{
	void *Result = (uint8 *)Node + sizeof(node);

	return Result;
}

inline var *
OnVisitRegister(visit_result *Result, visit_result VisitResult)
{
	if(VisitResult.Error.Type != NoError) Result->Error = VisitResult.Error;
	return VisitResult.Var;
}

inline visit_result
OnVisitSuccess(visit_result *Result, var *Var)
{
	Result->Var = Var;

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

inline visit_result Visit(interpreter_state *InterState, node *Node, context* Context);

#if 0
internal void *
PushNumber(memory_arena *RuntimeMem, real32 Value)
{
	number *Number = PushStruct(ParserState->AST, number);
	Number->Type = NUM_FLOAT;
	Number->Number = PushStruct(NodeMemory->MemoryBase, &NodeMemory->Size, NodeMemory->MaxMemorySize, real32);
	real32 *Num = (real32 *)Number->Number;
	*Num = Value;

	return Number;
}

internal void *
PushNumber(memory_arena *RuntimeMem, int32 Value)
{
	number *Number = PushStruct(ParserState->AST, number);

	Number->Type = NUM_INT;
	Number->Number = PushStruct(NodeMemory->MemoryBase, &NodeMemory->Size, NodeMemory->MaxMemorySize, int32);
	int32 *Num = (int32 *)Number->Number;
	*Num = Value;

	return Number;
}
#endif

inline visit_result
Add(var *Left, var *Right)
{
	visit_result Result = {};
	Result.Var = Left;

	number *LeftNum = (number *)Left->Value;
	number *RightNum = (number *)Right->Value;

	if((LeftNum->Type == NumberType_Real) && (RightNum->Type == NumberType_Real))
	{
		LeftNum->Real = LeftNum->Real + RightNum->Real;
		LeftNum->Type = NumberType_Real;
	}
	else if(LeftNum->Type == NumberType_Real)
	{
		LeftNum->Real = LeftNum->Real + RightNum->Int;
		LeftNum->Type = NumberType_Real;
	}
	else if(RightNum->Type == NumberType_Real)
	{
		LeftNum->Real = LeftNum->Int + RightNum->Real;
		LeftNum->Type = NumberType_Real;
	}
	else
	{
		LeftNum->Int = LeftNum->Int + RightNum->Int;
		LeftNum->Type = NumberType_Int;
	}

	return Result;
}

#if 0
inline op_result
Add(memory_arena *RuntimeMem, number *Left, number *Right)
{
	op_result OpResult = {};

	if((Left->Type == NumberType_Real) && (Right->Type == NumberType_Real))
	{
		real32 Result = *((real32 *)Left->Number) + *((real32 *)Right->Number);
		OpResult.Number = PushNumber(RuntimeMem, Result);
	}
	else if(Left->Type == NumberType_Real)
	{
		real32 Result = *((real32 *)Left->Number) + *((int32 *)Right->Number);
		OpResult.Number = PushNumber(RuntimeMem, Result);
	}
	else if(Right->Type == NumberType_Real)
	{
		real32 Result = *((int32 *)Left->Number) + *((real32 *)Right->Number);
		OpResult.Number = PushNumber(RuntimeMem, Result);
	}
	else
	{
		int32 Result = *((int32 *)Left->Number) + *((int32 *)Right->Number);
		OpResult.Number = PushNumber(RuntimeMem, Result);
	}

	return OpResult;
}

inline op_result
Subtract(memory_arena *RuntimeMem, number_node *Left, number_node *Right)
{
	op_result OpResult = {};

	if((Left->Type == NumberType_Real) && (Right->Type == NumberType_Real))
	{
		real32 Result = *((real32 *)Left->Number) - *((real32 *)Right->Number);
		OpResult.Number = PushNumber(RuntimeMem, Result);
	}
	else if(Left->Type == NumberType_Real)
	{
		real32 Result = *((real32 *)Left->Number) - *((int32 *)Right->Number);
		OpResult.Number = PushNumber(RuntimeMem, Result);
	}
	else if(Right->Type == NumberType_Real)
	{
		real32 Result = *((int32 *)Left->Number) - *((real32 *)Right->Number);
		OpResult.Number = PushNumber(RuntimeMem, Result);
	}
	else
	{
		int32 Result = *((int32 *)Left->Number) - *((int32 *)Right->Number);
		OpResult.Number = PushNumber(RuntimeMem, Result);
	}

	return OpResult;
}

inline op_result
Multiply(memory_arena *RuntimeMem, number_node *Left, number_node *Right)
{
	op_result OpResult = {};

	if((Left->Type == NumberType_Real) && (Right->Type == NumberType_Real))
	{
		real32 Result = *((real32 *)Left->Number) * *((real32 *)Right->Number);
		OpResult.Number = PushNumber(RuntimeMem, Result);
	}
	else if(Left->Type == NumberType_Real)
	{
		real32 Result = *((real32 *)Left->Number) * *((int32 *)Right->Number);
		OpResult.Number = PushNumber(RuntimeMem, Result);
	}
	else if(Right->Type == NumberType_Real)
	{
		real32 Result = *((int32 *)Left->Number) * *((real32 *)Right->Number);
		OpResult.Number = PushNumber(RuntimeMem, Result);
	}
	else
	{
		int32 Result = *((int32 *)Left->Number) * *((int32 *)Right->Number);
		OpResult.Number = PushNumber(RuntimeMem, Result);
	}

	return OpResult;
}

inline op_result
Division(memory_arena *RuntimeMem, number_node *Left, number_node *Right)
{
	op_result OpResult = {};

	real32 LeftNum = *((real32 *)Left->Number);
	real32 RightNum = *((real32 *)Right->Number);

	if((Left->Type == NumberType_Real) && (Right->Type == NumberType_Real)) {}
	else if(Left->Type == NumberType_Real)
	{
		RightNum = (real32)(*((int32 *)Right->Number));
	}
	else if(Right->Type == NumberType_Real)
	{
		LeftNum = (real32)(*((int32 *)Left->Number));
	}
	else
	{
		LeftNum = (real32)(*((int32 *)Left->Number));
		RightNum = (real32)(*((int32 *)Right->Number));
	}

	real32 Result = LeftNum / RightNum;
	OpResult.Number = PushNumber(RuntimeMem, Result);
	return OpResult;
}

inline op_result
Power(memory_arena *RuntimeMem, number_node *Left, number_node *Right)
{
	op_result OpResult = {};

	if(Right->Type == NumberType_Real)
		OpResult.Error = MakeError(VisitError, "Power of floating point not supported, for now");

	else if(Left->Type == NumberType_Int)
	{
		int32 Result = Power(*((int32 *)Left->Number), *((int32 *)Right->Number));
		OpResult.Number = PushNumber(RuntimeMem, Result);
	}
	else if(Left->Type == NumberType_Real)
	{
		real32 Result = Power(*((real32 *)Left->Number), *((int32 *)Right->Number));
		OpResult.Number = PushNumber(RuntimeMem, Result);
	}

	return OpResult;
}
#endif

internal visit_result
Visit_Number(interpreter_state *InterState, node *Node, context *Context)
{
	visit_result Result = {};

	number_node *NumberNode = GetNode(Node, number_node);

	number *Number = PushStruct(&InterState->RuntimeMem, number);
	Number->Int = NumberNode->Int;
	Number->Real = NumberNode->Real;
	Number->Type = NumberNode->Type;

	var *Var = PushStruct(&InterState->RuntimeMem, var);
	Var->Type = VarType_Number;
	Var->Value = Number;

	return OnVisitSuccess(&Result, Var);
}

#if 0
internal var *
PushVar(memory_arena *Arena, )
{
	PushStruct(Arena, var);
}
#endif

internal visit_result
Visit_UnaryOp(interpreter_state *InterState, node *Node, context *Context)
{
	visit_result Result = {};

	unary_node *Unary = GetNode(Node, unary_node);
	var *Var = OnVisitRegister(&Result, Visit(InterState, Unary->Node, Context));
	if(Result.Error.Type != NoError) return Result;

	visit_result OpResult = {};
	if(Unary->OpToken.Type == TT_MINUS)
	{
		number_node One = {};
		One.Type = NumberType_Int;
		int32 Temp = -1;
		//One.Number = &Temp;
		// OpResult = Multiply(InterState, Number, &One);
	}

	if(OpResult.Error.Type != NoError) return OnVisitFailure(&Result, OpResult.Error);
	else return OnVisitSuccess(&Result, OpResult.Var);
}

internal visit_result
Visit_BinaryOp(interpreter_state *InterState, node *Node, context *Context)
{
	visit_result Result = {};
	binary_node *Binary = GetNode(Node, binary_node);
	var *Left = OnVisitRegister(&Result, Visit(InterState, Binary->LeftNode, Context));
	if(Result.Error.Type != NoError) return Result;

	// NOTE(Khisrow): Temporary Memory to remove the Right var
	temporary_memory TempMem = BeginTemporaryMemory(&InterState->RuntimeMem);
	var *Right = OnVisitRegister(&Result, Visit(InterState, Binary->RightNode, Context));
	if(Result.Error.Type != NoError) return Result;

	if(Left->Type == Right->Type)
	{
		if(Left->Type == VarType_Number)
		{
			visit_result OpResult = {};

			switch(Binary->OpToken.Type)
			{
				case TT_PLUS:
				{
					OpResult = Add(Left, Right);
				} break;
#if 0
				case TT_MINUS:
				{
					OpResult = Subtract(Left, Right);
				} break;

				case TT_MUL:
				{
					OpResult = Multiply(Left, Right);
				} break;

				case TT_DIV:
				{
					OpResult = Division(InterState, Left, Right);
				} break;

				case TT_POW:
				{
					OpResult = Power(InterState, Left, Right);
				} break;
#endif
				default: InvalidCodePath;
			}

			EndTemporaryMemory(TempMem);
			if(OpResult.Error.Type != NoError) return OnVisitFailure(&Result, OpResult.Error);

			else return OnVisitSuccess(&Result, OpResult.Var);
		}
		else
		{
			EndTemporaryMemory(TempMem);
			InvalidCodePath;
			return OnVisitFailure(&Result, MakeError(VisitError, "Only numbers operations are supported"));
		}
	}
	else
	{
		EndTemporaryMemory(TempMem);
		InvalidCodePath;
		return OnVisitFailure(&Result, MakeError(VisitError, "the two sides of operation must be of the same type!"));
	}
}

inline visit_result
Visit(interpreter_state *InterState, node *Node, context* Context)
{
	visit_result Result = {};

	if(IsNodeType(Node, NT_number_node)) Result.Var = OnVisitRegister(&Result, Visit_Number(InterState, Node, Context));
	else if(IsNodeType(Node, NT_unary_node)) Result.Var = OnVisitRegister(&Result, Visit_UnaryOp(InterState, Node, Context));
	else if(IsNodeType(Node, NT_binary_node)) Result.Var = OnVisitRegister(&Result, Visit_BinaryOp(InterState, Node, Context));
	else if(IsNodeType(Node, NT_var_assign_node)) Result.Var = OnVisitRegister(&Result, Visit_BinaryOp(InterState, Node, Context));

	else Result.Error = MakeError(VisitError, "Visit function not defined for this type!");

	return Result;
}
