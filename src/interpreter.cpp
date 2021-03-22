/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  3/9/2021 6:23:02 AM                                           |
    |    Last Modified:                                                                |
    |                                                                                  |
    +=====================| Sayed Abid Hashimi, Copyright © All rights reserved |======+  */

#include "interpreter.h"

#if 0
internal context
MakeContext(char *DisplayName, context *Parent = 0, position *ParentPos = 0)
{
	context Result = {};

	Result.DisplayName = DisplayName;
	if(Parent) Result.Parent = Parent;
	if(ParentPos) Result.ParentPos = ParentPos;

	return Result;
}
#endif

inline void
InitializeInterpreter(interpreter_state *InterState)
{
	InterState->RuntimeMem.Used = 0;
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

#define MakeVar(Arena, Struct) MakeVar_(Arena, sizeof(Struct), VarType_##Struct)
inline var *
MakeVar_(memory_arena *Arena, memory_index Size, var_type Type)
{
	Size += sizeof(var);
	var *Result = (var *)PushSize_(Arena, Size);
	Result->Type = Type;

	return Result;
}

#define GetVar(Var, Struct) (Struct *)GetVar_(Var)
inline void *
GetVar_(var *Var)
{
	return Var + 1;
}

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

	number *LeftNum = GetVar(Left, number);
	number *RightNum = GetVar(Right, number);

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

inline visit_result
Subtract(var *Left, var *Right)
{
	visit_result Result = {};
	Result.Var = Left;

	number *LeftNum = GetVar(Left, number);
	number *RightNum = GetVar(Right, number);

	if((LeftNum->Type == NumberType_Real) && (RightNum->Type == NumberType_Real))
	{
		LeftNum->Real = LeftNum->Real - RightNum->Real;
		LeftNum->Type = NumberType_Real;
	}
	else if(LeftNum->Type == NumberType_Real)
	{
		LeftNum->Real = LeftNum->Real - RightNum->Int;
		LeftNum->Type = NumberType_Real;
	}
	else if(RightNum->Type == NumberType_Real)
	{
		LeftNum->Real = LeftNum->Int - RightNum->Real;
		LeftNum->Type = NumberType_Real;
	}
	else
	{
		LeftNum->Int = LeftNum->Int - RightNum->Int;
		LeftNum->Type = NumberType_Int;
	}

	return Result;
}

inline visit_result
Multiply(var *Left, var *Right)
{
	visit_result Result = {};
	Result.Var = Left;

	number *LeftNum = GetVar(Left, number);
	number *RightNum = GetVar(Right, number);

	if((LeftNum->Type == NumberType_Real) && (RightNum->Type == NumberType_Real))
	{
		LeftNum->Real = LeftNum->Real * RightNum->Real;
		LeftNum->Type = NumberType_Real;
	}
	else if(LeftNum->Type == NumberType_Real)
	{
		LeftNum->Real = LeftNum->Real * RightNum->Int;
		LeftNum->Type = NumberType_Real;
	}
	else if(RightNum->Type == NumberType_Real)
	{
		LeftNum->Real = LeftNum->Int * RightNum->Real;
		LeftNum->Type = NumberType_Real;
	}
	else
	{
		LeftNum->Int = LeftNum->Int * RightNum->Int;
		LeftNum->Type = NumberType_Int;
	}

	return Result;
}

inline visit_result
Division(var *Left, var *Right)
{
	visit_result Result = {};
	Result.Var = Left;

	number *LeftNum = GetVar(Left, number);
	number *RightNum = GetVar(Right, number);

	if((LeftNum->Type == NumberType_Real) && (RightNum->Type == NumberType_Real))
	{
		LeftNum->Real = LeftNum->Real / RightNum->Real;
		LeftNum->Type = NumberType_Real;
	}
	else if(LeftNum->Type == NumberType_Real)
	{
		LeftNum->Real = LeftNum->Real / RightNum->Int;
		LeftNum->Type = NumberType_Real;
	}
	else if(RightNum->Type == NumberType_Real)
	{
		LeftNum->Real = LeftNum->Int / RightNum->Real;
		LeftNum->Type = NumberType_Real;
	}
	else
	{
		LeftNum->Real = (real32)LeftNum->Int / (real32)RightNum->Int;
		LeftNum->Type = NumberType_Real;
	}

	return Result;
}

inline visit_result
Power(var *Left, var *Right)
{
	visit_result Result = {};
	Result.Var = Left;

	number *LeftNum = GetVar(Left, number);
	number *RightNum = GetVar(Right, number);

	if(RightNum->Type == NumberType_Real)
	{
		Result.Error = MakeError(VisitError, "Power of floating point not supported, for now");
	}

	else if(LeftNum->Type == NumberType_Int)
	{
		LeftNum->Int = Power(LeftNum->Int, RightNum->Int);
		LeftNum->Type = NumberType_Int;
	}
	else if(LeftNum->Type == NumberType_Real)
	{
		LeftNum->Real = Power(LeftNum->Real, RightNum->Int);
		LeftNum->Type = NumberType_Real;
	}

	return Result;
}

internal visit_result
Visit_Number(interpreter_state *InterState, node *Node, context *Context)
{
	visit_result Result = {};

	number_node *NumberNode = GetNode(Node, number_node);
	var *Var = MakeVar(&InterState->RuntimeMem, number);
	number *Number = GetVar(Var, number);
	Number->Int = NumberNode->Int;
	Number->Real = NumberNode->Real;
	Number->Type = NumberNode->Type;

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

	if(Unary->OpToken.Type == TT_MINUS)
	{
		temporary_memory TempMem = BeginTemporaryMemory(&InterState->RuntimeMem);
		var *NegativeOne = MakeVar(&InterState->RuntimeMem, number);
		number *Number = GetVar(NegativeOne, number);
		Number->Int = -1;
		Number->Type = NumberType_Int;

		Result = Multiply(Var, NegativeOne);
		EndTemporaryMemory(TempMem);
		if(Result.Error.Type != NoError) return OnVisitFailure(&Result, Result.Error);
	}

	return OnVisitSuccess(&Result, Result.Var);
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
		if(Left->Type == VarType_number)
		{
			switch(Binary->OpToken.Type)
			{
				case TT_PLUS:
				{
					Result = Add(Left, Right);
				} break;

				case TT_MINUS:
				{
					Result = Subtract(Left, Right);
				} break;

				case TT_MUL:
				{
					Result = Multiply(Left, Right);
				} break;

				case TT_DIV:
				{
					Result = Division(Left, Right);
				} break;

				case TT_POW:
				{
					Result = Power(Left, Right);
				} break;

				default: InvalidCodePath;
			}

			EndTemporaryMemory(TempMem);
			if(Result.Error.Type != NoError) return OnVisitFailure(&Result, Result.Error);

			else return OnVisitSuccess(&Result, Result.Var);
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

internal symbol *
GetHashFromSymbolTable(symbol_table *SymbolTable, char* SymName)
{
	Assert(SymName);
	symbol *Result = 0;
	if(SymName)
	{
		symbol *Symbols = (symbol *)SymbolTable->Arena.Base;
		int32 SymNameLength = StringLength(SymName);
		uint32 HashValue = 0;
		for(uint32 Index = 0;
			(Index <= 2) && (Index < SymNameLength);
			++Index)
		{
			int32 Arb = (int32)(137/(Index + 1));
			int32 CharNum = (int32)SymName[Index];

			HashValue += Arb*CharNum;
		}

		for(uint32 Offset = 0;
			Offset < SymbolTable->SymbolSize;
			++Offset)
		{
			uint32 HashMask = (SymbolTable->SymbolSize - 1);
			uint32 HashIndex = ((HashValue + Offset) % HashMask);
			symbol *Entry = Symbols + HashIndex;
									 
			if((Entry->Value == 0) || StringCompare(Entry->Name, SymName))
			{
				Result = Entry;
				break;
			}
		}
	}

	return Result;
}

internal var *
PushVar(memory_arena *Arena, var *Source, var *Destination)
{
	var *Dest = 0;

	switch (Source->Type)
	{
		case VarType_number:
		{
			if(Arena) Dest = MakeVar(Arena, number);
			else 
			{
				Assert(Destination);
				Dest = Destination;
			}
			number *DestNumber = GetVar(Dest, number);
			number *SourceNum = GetVar(Source, number);

			DestNumber->Int = SourceNum->Int;
			DestNumber->Real = SourceNum->Real;
			DestNumber->Type = SourceNum->Type;
		} break;

		default: InvalidCodePath;
	}

	return Dest;
}

internal visit_result
UpdateSymbolTable(interpreter_state *InterState, context *Context, char *Name, var *Value, bool32 New)
{
	visit_result Result = {};

	symbol_table *SymbolTable = Context->SymbolTable;
	symbol *Symbol = GetHashFromSymbolTable(SymbolTable, Name);
	if(New)
	{
		Assert(SymbolTable->SymbolSize > (SymbolTable->Arena.Used / sizeof(symbol)));
		if(SymbolTable->SymbolSize > (SymbolTable->Arena.Used/sizeof(symbol) + 1))
		{
			if(StringCompare(Symbol->Name, Name))
			{
				error Error = {};
				Error.Type = VisitError;
				Concat(Error.Message, false, "VisitError: Variable '", Name, "' redefinition");
				return OnVisitFailure(&Result, Error);
			}

			int32 NameLength = StringLength(Name);
			char *VarName = PushSize(&InterState->Stack, char, NameLength + 1);
			CopyToString(Name, VarName, NameLength);
			var *VarValue = PushVar(&InterState->Stack, Value, 0);
			Assert(VarValue);
			Symbol->Name = VarName;
			Symbol->Value = VarValue;
			return OnVisitSuccess(&Result, Symbol->Value);
		}
	}
	else
	{
		if(!StringCompare(Symbol->Name, Name))
		{
			error Error = {};
			Error.Type = VisitError;
			Concat(Error.Message, false, "VisitError: Undefined variable '", Name, "'");
			return OnVisitFailure(&Result, Error);
		}

		Symbol->Value = PushVar(0, Value, Symbol->Value);
		return OnVisitSuccess(&Result, Symbol->Value);
	}

	return OnVisitFailure(&Result, MakeError(VisitError, "VisitError: Variable cannot be created!"));
}

internal visit_result
Visit_VarAssign(interpreter_state *InterState, node *Node, context *Context)
{
	temporary_memory TempMem = BeginTemporaryMemory(&InterState->RuntimeMem);

	visit_result Result = {};
	var_assign_node *VarAssignNode = GetNode(Node, var_assign_node);
	var *Value = OnVisitRegister(&Result, Visit(InterState, VarAssignNode->Value, Context));
	if(Result.Error.Type != NoError)
	{
		EndTemporaryMemory(TempMem);
		return Result;
	}

	var *SymVar = OnVisitRegister(&Result, UpdateSymbolTable(InterState, Context,
															 VarAssignNode->Name.Value,
															 Value, VarAssignNode->New));
	if(Result.Error.Type != NoError) 
	{
		EndTemporaryMemory(TempMem);
		return Result;
	}

	EndTemporaryMemory(TempMem);
	return OnVisitSuccess(&Result, SymVar);
}

internal visit_result
Visit_VarAccess(interpreter_state *InterState, node *Node, context *Context)
{
	visit_result Result = {};
	var_access_node *VarAccessNode = GetNode(Node, var_access_node);
	symbol_table *SymbolTable = Context->SymbolTable;
	symbol *Symbol = GetHashFromSymbolTable(SymbolTable, VarAccessNode->Name.Value);
	if(StringCompare(VarAccessNode->Name.Value, Symbol->Name))
	{
		var *Var = PushVar(&InterState->RuntimeMem, Symbol->Value, 0);
		return OnVisitSuccess(&Result, Var);
	}

	error Error = {};
	Error.Type = VisitError;
	Concat(Error.Message, false, "VisitError: Undefined variable '", VarAccessNode->Name.Value, "'");
	return OnVisitFailure(&Result, Error);
}

inline visit_result
Visit(interpreter_state *InterState, node *Node, context* Context)
{
	// TODO(Khisrow): Must apply firstvisit to avoid memory waste on the RuntimeMem

	visit_result Result = {};

	if(IsNodeType(Node, NodeType_number_node)) Result.Var = OnVisitRegister(&Result, Visit_Number(InterState, Node, Context));
	else if(IsNodeType(Node, NodeType_unary_node)) Result.Var = OnVisitRegister(&Result, Visit_UnaryOp(InterState, Node, Context));
	else if(IsNodeType(Node, NodeType_binary_node)) Result.Var = OnVisitRegister(&Result, Visit_BinaryOp(InterState, Node, Context));
	else if(IsNodeType(Node, NodeType_var_assign_node)) Result.Var = OnVisitRegister(&Result, Visit_VarAssign(InterState, Node, Context));
	else if(IsNodeType(Node, NodeType_var_access_node)) Result.Var = OnVisitRegister(&Result, Visit_VarAccess(InterState, Node, Context));

	else Result.Error = MakeError(VisitError, "Visit function not defined for this type!");

	return Result;
}
