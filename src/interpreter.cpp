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
	Assert(InterState);
	FreeDynamicArena(&InterState->RuntimeMem);
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
	else Result->Status = VisitResult.Status;
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

inline visit_result Visit(interpreter_state *InterState, node *Node, context* Context, bool32 Loop=false);

#define MakeVar(Arena, Struct) MakeVar_(Arena, sizeof(Struct), VarType_##Struct)
inline var *
MakeVar_(dynamic_memory_arena *Arena, memory_index Size, var_type Type)
{
	Size += sizeof(var);
	var *Result = (var *)PushDynamicSize_(Arena, Size);
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

internal visit_result
Visit_String(interpreter_state *InterState, node *Node, context *Context)
{
	visit_result Result = {};

	string_node *StringNode = GetNode(Node, string_node);
	var *Var = MakeVar(&InterState->RuntimeMem, string);
	string *String = GetVar(Var, string);
	String->Value = StringNode->Value;

	return OnVisitSuccess(&Result, Var);
}

#if 0
internal var *
PushVar(memory_arena *Arena, )
{
	PushStruct(Arena, var);
}
#endif

inline visit_result
NegateBoolean(dynamic_memory_arena *Arena, var *Var)
{
	visit_result Result = {};
	Result.Var = Var;

	boolean_ *Bool = GetVar(Var, boolean_);
	Bool->Value = !Bool->Value;

	return Result;
}

internal visit_result
Visit_UnaryOp(interpreter_state *InterState, node *Node, context *Context)
{
	visit_result Result = {};

	unary_node *Unary = GetNode(Node, unary_node);
	var *Var = OnVisitRegister(&Result, Visit(InterState, Unary->Node, Context));
	if(Result.Error.Type != NoError) return Result;

	if(Unary->OpToken.Type == TT_MINUS)
	{
		if(Var->Type != VarType_number)
		{
			FreeDynamicBlock(&InterState->RuntimeMem, Var);
			return OnVisitFailure(&Result, MakeError(VisitError, "Illegal use of '-' after non-number"));
		}
		var *NegativeOne = MakeVar(&InterState->RuntimeMem, number);
		number *Number = GetVar(NegativeOne, number);
		Number->Int = -1;
		Number->Type = NumberType_Int;
		Result = Multiply(Var, NegativeOne);
		FreeDynamicBlock(&InterState->RuntimeMem, NegativeOne);
	}
	else if(Unary->OpToken.Type == TT_NOT)
	{
		if(Var->Type != VarType_boolean_)
		{
			FreeDynamicBlock(&InterState->RuntimeMem, Var);
			return OnVisitFailure(&Result, MakeError(VisitError, "Illegal use of '!' after non-boolean"));
		}

		Result = NegateBoolean(&InterState->RuntimeMem, Var);
	}

	return OnVisitSuccess(&Result, Result.Var);
}

internal visit_result
Visit_CompoundCompare(interpreter_state *InterState, node *Node, context *Context)
{
	visit_result Result = {};
	compound_compare_node *ComCompare = GetNode(Node, compound_compare_node);
	var *Left = OnVisitRegister(&Result, Visit(InterState, ComCompare->LeftNode, Context));
	if(Result.Error.Type != NoError) return Result;

	if(Left->Type == VarType_boolean_)
	{
		Result.Var = Left;
		boolean_ *LeftBool = GetVar(Left, boolean_);
		switch(ComCompare->OpToken.Type)
		{
			case TT_AND:
			{
				if(LeftBool->Value == 0) return OnVisitSuccess(&Result, Result.Var);
				var *Right = OnVisitRegister(&Result, Visit(InterState, ComCompare->RightNode, Context));
				if(Result.Error.Type != NoError) return Result;

				if(Left->Type == Right->Type)
				{
					boolean_ *RightBool = GetVar(Right, boolean_);
					LeftBool->Value = LeftBool->Value && RightBool->Value;

					FreeDynamicBlock(&InterState->RuntimeMem, Right);
					if(Result.Error.Type != NoError) return OnVisitFailure(&Result, Result.Error);
					else return OnVisitSuccess(&Result, Result.Var);
				}
				else
				{
					FreeDynamicBlock(&InterState->RuntimeMem, Right);
					FreeDynamicBlock(&InterState->RuntimeMem, Left);
					return OnVisitFailure(&Result, MakeError(VisitError, "The two sides of the operation must be of the same type!"));
				}
			} break;

			case TT_OR:
			{
				if(LeftBool->Value == 1) return OnVisitSuccess(&Result, Result.Var);
				var *Right = OnVisitRegister(&Result, Visit(InterState, ComCompare->RightNode, Context));
				if(Result.Error.Type != NoError) return Result;

				if(Left->Type == Right->Type)
				{
					boolean_ *RightBool = GetVar(Right, boolean_);
					LeftBool->Value = LeftBool->Value || RightBool->Value;

					FreeDynamicBlock(&InterState->RuntimeMem, Right);
					if(Result.Error.Type != NoError) return OnVisitFailure(&Result, Result.Error);
					else return OnVisitSuccess(&Result, Result.Var);
				}
				else
				{
					FreeDynamicBlock(&InterState->RuntimeMem, Right);
					FreeDynamicBlock(&InterState->RuntimeMem, Left);
					return OnVisitFailure(&Result, MakeError(VisitError, "The two sides of the operation must be of the same type!"));
				}
			} break;

			default:
			{
				InvalidCodePath;
				return Result;
			} break;
		}
	}
	else
	{
		FreeDynamicBlock(&InterState->RuntimeMem, Left);
		return OnVisitFailure(&Result, MakeError(VisitError, "Non-boolean operands for the operation"));
	}
}

internal visit_result
Visit_BinaryOp(interpreter_state *InterState, node *Node, context *Context)
{
	visit_result Result = {};
	binary_node *Binary = GetNode(Node, binary_node);
	var *Left = OnVisitRegister(&Result, Visit(InterState, Binary->LeftNode, Context));
	if(Result.Error.Type != NoError) return Result;
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

			FreeDynamicBlock(&InterState->RuntimeMem, Right);
			if(Result.Error.Type != NoError) return OnVisitFailure(&Result, Result.Error);

			else return OnVisitSuccess(&Result, Result.Var);
		}
		else if(Left->Type == VarType_boolean_)
		{
			Result.Var = Left;
			boolean_ *LeftBool = GetVar(Left, boolean_);
			boolean_ *RightBool = GetVar(Right, boolean_);

			if(Binary->OpToken.Type == TT_AND) 
			{
				if(LeftBool->Value == 0) return OnVisitSuccess(&Result, Result.Var);
				LeftBool->Value = LeftBool->Value && RightBool->Value;
			}
			else if(Binary->OpToken.Type == TT_OR)
			{
				if(LeftBool->Value == 1) return OnVisitSuccess(&Result, Result.Var);
			   	LeftBool->Value = LeftBool->Value || RightBool->Value;
			}
			else
			{
				InvalidCodePath;
				return Result;
			}

			FreeDynamicBlock(&InterState->RuntimeMem, Right);
			if(Result.Error.Type != NoError) return OnVisitFailure(&Result, Result.Error);
			else return OnVisitSuccess(&Result, Result.Var);
		}
		else
		{
			FreeDynamicBlock(&InterState->RuntimeMem, Right);
			return OnVisitFailure(&Result, MakeError(VisitError, "Non-number, non-boolean operands for the operation"));
		}
	}
	else
	{
		FreeDynamicBlock(&InterState->RuntimeMem, Right);
		return OnVisitFailure(&Result, MakeError(VisitError, "The two sides of the operation must be of the same type!"));
	}
}

internal symbol *
GetHashFromSymbolTable(symbol_table *SymbolTable, char* SymName, bool32 New)
{
	// TODO(Khisrow): Better Hash Function!
	Assert(SymName);
	symbol *Result = 0;
	if(SymName)
	{
		int32 SymNameLength = StringLength(SymName);
		uint32 HashValue = 0;
		for(uint32 Index = 0;
			(Index <= 5) || (Index < SymNameLength);
			++Index)
		{
			int32 Arb = (int32)(137/(Index + 1));
			int32 CharNum = (int32)SymName[Index];
			HashValue += Arb*CharNum;
		}

		uint32 HashMask = (SymbolTable->SymbolSize - 1);
		uint32 HashIndex = HashValue  % HashMask;
		Result = SymbolTable->Symbols + HashIndex;

		do
		{
			if(StringCompare(Result->Name, SymName)) break;
			if(Result->Name == 0 & Result->Value == 0) break;

			if(New && !Result->Next)
			{
				Result->Next = (symbol *)PlatformAllocMem(sizeof(symbol));
				Result = Result->Next;
				Result->Value = 0;
				Result->Name = 0;
			}

			Result = Result->Next;
		} while(Result);
	}

	return Result;
}

internal var *
CopyVar(memory_arena *Arena, var *Source, var *Destination = 0)
{
	var *Result = 0;

	switch (Source->Type)
	{
		case VarType_number:
		{
			if(Destination) Result = Destination;
			else
			{
				memory_index Size = sizeof(var)+sizeof(number);
				Assert((Arena->Used + Size) < Arena->Size);
				Result = (var *)PushSize_(Arena, Size);
			}

			Result->Type = Source->Type;
			number *DestNum = GetVar(Result, number);
			number *SourceNum = GetVar(Source, number);
			DestNum->Int = SourceNum->Int;
			DestNum->Real = SourceNum->Real;
			DestNum->Type = SourceNum->Type;
		} break;

		default: InvalidCodePath;
	}

	return Result;
}

internal var *
PushVar(dynamic_memory_arena *Arena, var *Source, var *Destination = 0)
{
	var *Result = 0;

	switch (Source->Type)
	{
		case VarType_number:
		{
			if(Destination) Result = Destination;
			else
			{
				if(Arena) Result = MakeVar(Arena, number);
				else Result = (var *)PlatformAllocMem(sizeof(var) + sizeof(number));
				Assert(Result);
			}

			Result->Type = Source->Type;
			number *DestNum = GetVar(Result, number);
			number *SourceNum = GetVar(Source, number);
			DestNum->Int = SourceNum->Int;
			DestNum->Real = SourceNum->Real;
			DestNum->Type = SourceNum->Type;
		} break;
		case VarType_string:
		{
			string *SourceString = GetVar(Source, string);

			if(Destination) Result = Destination;
			else
			{
				if(Arena)
				{
					Result = (var *)PushDynamicSize_(Arena, sizeof(var) + sizeof(string) +
														  StringLength(SourceString->Value) + 1);
				}
				else Result = (var *)PlatformAllocMem(sizeof(var) + sizeof(string) +
													  StringLength(SourceString->Value) + 1);
				Assert(Result);
			}

			Result->Type = Source->Type;
			string *DestString = GetVar(Result, string);
			DestString->Value = (char *)(DestString + 1);
			CopyToString(SourceString->Value, DestString->Value, StringLength(SourceString->Value));
		} break;
		case VarType_boolean_:
		{
			if(Destination) Result = Destination;
			else
			{
				if(Arena) Result = MakeVar(Arena, boolean_);
				else Result = (var *)PlatformAllocMem(sizeof(var) + sizeof(boolean_));
				Assert(Result);
			}

			Result->Type = Source->Type;
			boolean_ *DestBool = GetVar(Result, boolean_);
			boolean_ *SourceBool = GetVar(Source, boolean_);
			DestBool->Value = SourceBool->Value;
		} break;

		default: InvalidCodePath;
	}

	return Result;
}

internal visit_result
UpdateSymbolTable(interpreter_state *InterState, context *Context, char *Name, var *Value, bool32 New)
{
	visit_result Result = {};

	symbol_table *SymbolTable = Context->SymbolTable;
	symbol *Symbol = 0;
	if(New)
	{
		Symbol = GetHashFromSymbolTable(SymbolTable, Name, true);
		if(StringCompare(Symbol->Name, Name))
		{
			error Error = {};
			Error.Type = VisitError;
			Concat(Error.Message, false, "VisitError: Variable '", Name, "' redefinition");
			return OnVisitFailure(&Result, Error);
		}

		int32 NameLength = StringLength(Name);
		char *VarName = (char *)PlatformAllocMem(sizeof(char)*(NameLength+1));
		Assert(VarName);
		CopyToString(Name, VarName, NameLength);
		var *VarValue = PushVar(0, Value, 0);
		Assert(VarValue);
		Symbol->Name = VarName;
		Symbol->Value = VarValue;
		return OnVisitSuccess(&Result, Symbol->Value);
	}
	else
	{
		Symbol = GetHashFromSymbolTable(SymbolTable, Name, false);
		if(!StringCompare(Symbol->Name, Name))
		{
			error Error = {};
			Error.Type = VisitError;
			Concat(Error.Message, false, "VisitError: Undefined variable '", Name, "'");
			return OnVisitFailure(&Result, Error);
		}

		Symbol->Value = PushVar(&InterState->RuntimeMem, Value, Symbol->Value);
		return OnVisitSuccess(&Result, Symbol->Value);
	}

	return OnVisitFailure(&Result, MakeError(VisitError, "VisitError: Variable cannot be created!"));
}

internal visit_result
Visit_VarAssign(interpreter_state *InterState, node *Node, context *Context)
{
	visit_result Result = {};
	var_assign_node *VarAssignNode = GetNode(Node, var_assign_node);
	var *Value = OnVisitRegister(&Result, Visit(InterState, VarAssignNode->Value, Context));
	if(Result.Error.Type != NoError)
	{
		FreeDynamicBlock(&InterState->RuntimeMem, Value);
		return Result;
	}

	var *SymVar = OnVisitRegister(&Result, UpdateSymbolTable(InterState, Context,
															 VarAssignNode->Name.Value,
															 Value, VarAssignNode->New));
	if(Result.Error.Type != NoError) 
	{
		FreeDynamicBlock(&InterState->RuntimeMem, Value);
		return Result;
	}

	FreeDynamicBlock(&InterState->RuntimeMem, Value);
	return OnVisitSuccess(&Result, SymVar);
}

internal visit_result
Visit_VarAccess(interpreter_state *InterState, node *Node, context *Context)
{
	visit_result Result = {};
	var_access_node *VarAccessNode = GetNode(Node, var_access_node);
	symbol_table *SymbolTable = Context->SymbolTable;
	symbol *Symbol = GetHashFromSymbolTable(SymbolTable, VarAccessNode->Name.Value, false);
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

inline bool32
IsEqual(number *LeftNum, number *RightNum)
{
	if((LeftNum->Type == NumberType_Real) && (RightNum->Type == NumberType_Real))
	{
		return LeftNum->Real == RightNum->Real;
	}
	else if(LeftNum->Type == NumberType_Real)
	{
		return LeftNum->Real == RightNum->Int;
	}
	else if(RightNum->Type == NumberType_Real)
	{
		return LeftNum->Int == RightNum->Real;
	}
	else return LeftNum->Int == RightNum->Int;
}

inline bool32
IsGreater(number *LeftNum, number *RightNum)
{
	if((LeftNum->Type == NumberType_Real) && (RightNum->Type == NumberType_Real))
	{
		return LeftNum->Real > RightNum->Real;
	}
	else if(LeftNum->Type == NumberType_Real)
	{
		return LeftNum->Real > RightNum->Int;
	}
	else if(RightNum->Type == NumberType_Real)
	{
		return LeftNum->Int > RightNum->Real;
	}
	else return LeftNum->Int > RightNum->Int;
}
	
inline bool32
IsGreaterAndEqual(number *LeftNum, number *RightNum)
{
	if((LeftNum->Type == NumberType_Real) && (RightNum->Type == NumberType_Real))
	{
		return LeftNum->Real >= RightNum->Real;
	}
	else if(LeftNum->Type == NumberType_Real)
	{
		return LeftNum->Real >= RightNum->Int;
	}
	else if(RightNum->Type == NumberType_Real)
	{
		return LeftNum->Int >= RightNum->Real;
	}
	else return LeftNum->Int >= RightNum->Int;
}

inline bool32
IsLesser(number *LeftNum, number *RightNum)
{
	if((LeftNum->Type == NumberType_Real) && (RightNum->Type == NumberType_Real))
	{
		return LeftNum->Real < RightNum->Real;
	}
	else if(LeftNum->Type == NumberType_Real)
	{
		return LeftNum->Real < RightNum->Int;
	}
	else if(RightNum->Type == NumberType_Real)
	{
		return LeftNum->Int < RightNum->Real;
	}
	else return LeftNum->Int < RightNum->Int;
}

inline bool32
IsLesserAndEqual(number *LeftNum, number *RightNum)
{
	if((LeftNum->Type == NumberType_Real) && (RightNum->Type == NumberType_Real))
	{
		return LeftNum->Real <= RightNum->Real;
	}
	else if(LeftNum->Type == NumberType_Real)
	{
		return LeftNum->Real <= RightNum->Int;
	}
	else if(RightNum->Type == NumberType_Real)
	{
		return LeftNum->Int <= RightNum->Real;
	}
	else return LeftNum->Int <= RightNum->Int;
}

internal visit_result
Visit_Comparison(interpreter_state *InterState, node *Node, context *Context)
{
	visit_result Result = {};

	comparison_node *Comparison = GetNode(Node, comparison_node);
	var *Left = OnVisitRegister(&Result, Visit(InterState, Comparison->LeftNode, Context));
	if(Result.Error.Type != NoError) return Result;

	var *Right = OnVisitRegister(&Result, Visit(InterState, Comparison->RightNode, Context));
	if(Result.Error.Type != NoError) return Result;

	if(Left->Type == Right->Type)
	{
		if(Left->Type == VarType_number)
		{
			var *Var = MakeVar(&InterState->RuntimeMem, boolean_);
			Result.Var = Var;

			number *LeftNum = GetVar(Left, number);
			number *RightNum = GetVar(Right, number);
			boolean_ *ResBool = GetVar(Var, boolean_);

			switch(Comparison->OpToken.Type)
			{
				case TT_EQEQ: ResBool->Value = IsEqual(LeftNum, RightNum); break;
				case TT_NEQ: ResBool->Value = !IsEqual(LeftNum, RightNum); break;
				case TT_GT: ResBool->Value = IsGreater(LeftNum, RightNum); break;
				case TT_GTE: ResBool->Value = IsGreaterAndEqual(LeftNum, RightNum); break;
				case TT_LT: ResBool->Value = IsLesser(LeftNum, RightNum); break;
				case TT_LTE: ResBool->Value = IsLesserAndEqual(LeftNum, RightNum); break;
				default: InvalidCodePath;
			}
		}
		else if(Left->Type == VarType_boolean_)
		{
			var *Var = MakeVar(&InterState->RuntimeMem, boolean_);
			Result.Var = Var;

			boolean_ *LeftBool = GetVar(Left, boolean_);
			boolean_ *RightBool = GetVar(Right, boolean_);
			boolean_ *ResBool = GetVar(Var, boolean_);

			if(Comparison->OpToken.Type == TT_EQEQ) ResBool->Value = (LeftBool->Value == RightBool->Value);
			if(Comparison->OpToken.Type == TT_NEQ) ResBool->Value = (LeftBool->Value != RightBool->Value);
			else 
			{
				FreeDynamicBlock(&InterState->RuntimeMem, Left);
				FreeDynamicBlock(&InterState->RuntimeMem, Right);
				return OnVisitFailure(&Result, MakeError(VisitError, "Invalid boolean operation, only equality of booleans are validated"));
			}
		}
		else
		{
			FreeDynamicBlock(&InterState->RuntimeMem, Left);
			FreeDynamicBlock(&InterState->RuntimeMem, Right);
			return OnVisitFailure(&Result, MakeError(VisitError, "Invalid type as comparison operands"));
		}

		FreeDynamicBlock(&InterState->RuntimeMem, Left);
		FreeDynamicBlock(&InterState->RuntimeMem, Right);
		if(Result.Error.Type != NoError) return OnVisitFailure(&Result, Result.Error);

		else return OnVisitSuccess(&Result, Result.Var);
	}
	else
	{
		FreeDynamicBlock(&InterState->RuntimeMem, Left);
		FreeDynamicBlock(&InterState->RuntimeMem, Right);
		return OnVisitFailure(&Result, MakeError(VisitError, "the two sides of operation must be of the same type!"));
		InvalidCodePath;
	}
	return Result;
}

// TODO(Khisrow): Clean up after the statement is done, free blocks after each 5 statements
// TODO(Khisrow): Fix the VarAccess routine, this could memory bleeding
inline visit_result
Visit_Statements(interpreter_state *InterState, node *Node, context *Context, bool32 Loop=false)
{
	visit_result Result = {};

	statements_node *StatementsNode = GetNode(Node, statements_node);
	for(int32 Index = 0;
		Index < StatementsNode->Length;
		++Index)
	{
		node *Node = *(StatementsNode->Statement + Index);
		if(Node->Header.Type == NodeType_out_node)
		{
			Result.Status = Status_OUT;
		}
		else if(Node->Header.Type == NodeType_ignore_node)
		{
			Result.Status = Status_IGNORE;
		}
		if(Result.Status == Status_OUT || Result.Status == Status_IGNORE) break;
		OnVisitRegister(&Result, Visit(InterState, Node, Context, Loop));
		if(Result.Error.Type != NoError) return Result;
	}

	return Result;
}

internal visit_result
Visit_Func_Abs(interpreter_state *InterState, function_call_node *FuncCallNode, context *Context)
{
	visit_result Result = {};
	if(FuncCallNode->ArgLength == 1)
	{
		var *Var = OnVisitRegister(&Result, Visit(InterState, *FuncCallNode->Args, Context));
		if(Var->Type == VarType_number)
		{
			number *Number = GetVar(Var, number);
			if(Number->Type == NumberType_Int)
			{
				if(Number->Int < 0) Number->Int *= -1;
			}
			else if(Number->Type == NumberType_Real)
			{
				if(Number->Real < 0.0f) Number->Real *= -1.0f;
			}

			return OnVisitSuccess(&Result, Var);
		}
		else return OnVisitFailure(&Result, MakeError(VisitError, "Expected number as an argument"));
	}
	else return OnVisitFailure(&Result, MakeError(VisitError, "Function abs() requires only one argument"));
}

internal visit_result
Visit_Func_Print(interpreter_state *InterState, function_call_node *FuncCallNode, context *Context)
{
	visit_result Result = {};

	int32 Index = 0;
	for(node *Arg = *FuncCallNode->Args;
	    Index < FuncCallNode->ArgLength;
		Arg = *(FuncCallNode->Args + ++Index))
	{
		var *Var = OnVisitRegister(&Result, Visit(InterState, Arg, Context));

		if(Var->Type == VarType_number)
		{
			number *Number = GetVar(Var, number);
			if(Number->Type == NumberType_Int)
			{
				char * String = (char *)PushDynamicSize_(&InterState->RuntimeMem,
														 (IntLength(Number->Int) + 1)*sizeof(char));
				ToString(Number->Int, String);
				PlatformStdOut(String);
				PlatformStdOut("\n");
				FreeDynamicBlock(&InterState->RuntimeMem, String);
			}
			else if(Number->Type == NumberType_Real)
			{
				char * String = (char *)PushDynamicSize_(&InterState->RuntimeMem,
														 (Real32Length(Number->Real) + 1)*sizeof(char));
				ToString(Number->Real, String);
				PlatformStdOut(String);
				PlatformStdOut("\n");
				FreeDynamicBlock(&InterState->RuntimeMem, String);
			}
		}
		else if(Var->Type == VarType_string)
		{
			string *String = GetVar(Var, string);
			PlatformStdOut(String->Value);
			PlatformStdOut("\n");
		}
	}

	return Result;
}

inline visit_result
Visit_Func_Input(interpreter_state *InterState, function_call_node *FuncCallNode, context *Context)
{
	visit_result Result = {};

	PlatformStdIn();

	return Result;
}

inline visit_result
Visit_FunctionCall(interpreter_state *InterState, node *Node, context *Context)
{
	visit_result Result = {};
	function_call_node *FuncCallNode = GetNode(Node, function_call_node);

	if(StringCompare(FuncCallNode->FuncName.Value, "print"))
	{
		return Visit_Func_Print(InterState, FuncCallNode, Context);
	}
	else if(StringCompare(FuncCallNode->FuncName.Value, "input"))
	{
		return Visit_Func_Input(InterState, FuncCallNode, Context);
	}
	else if(StringCompare(FuncCallNode->FuncName.Value, "abs"))
	{
		return Visit_Func_Abs(InterState, FuncCallNode, Context);
	}
	else
	{
		error Error = {};
		Error.Type = VisitError;
		Concat(Error.Message, false, "VisitError: Undefined function '", FuncCallNode->FuncName.Value, "' called");
		return OnVisitFailure(&Result, Error);
	}
}

// TODO(Khisrow): Make statements for scoping the variable definitions and whatnot
internal visit_result
Visit_IfCondition(interpreter_state *InterState, node *Node, context *Context, bool32 Loop=false)
{
	visit_result Result = {};
	if_node *IfNode = GetNode(Node, if_node);
	var *Var = OnVisitRegister(&Result, Visit(InterState, IfNode->Condition, Context));
	if(Var->Type == VarType_boolean_)
	{
		boolean_ *Bool = GetVar(Var, boolean_);
		if(Bool->Value == 1)
		{
			OnVisitRegister(&Result, Visit(InterState, IfNode->Body, Context, Loop));
			if(Result.Error.Type != NoError) return Result;
		   	return OnVisitSuccess(&Result, Result.Var);
		}
		else if(IfNode->Others)
		{
			for(int32 Index = 0; Index < IfNode->OtherLength; ++Index)
			{
				other_node *OtherNode = GetNode(*(IfNode->Others + Index), other_node);
				Var = OnVisitRegister(&Result, Visit(InterState, OtherNode->Condition, Context));
				if(Var->Type != VarType_boolean_) return OnVisitFailure(&Result, MakeError(VisitError, "Unexpected expression in conditional statement predicate!, boolean expected"));
				Bool = GetVar(Var, boolean_);
				if(Bool->Value == 1)
				{
					OnVisitRegister(&Result, Visit(InterState, OtherNode->Body, Context, Loop));
					if(Result.Error.Type != NoError) return Result;
					return OnVisitSuccess(&Result, Result.Var);
				}
			}
		}
		if(IfNode->Else)
		{
			else_node *ElseNode = GetNode(IfNode->Else, else_node);
			OnVisitRegister(&Result, Visit(InterState, ElseNode->Body, Context, Loop));
			if(Result.Error.Type != NoError) return Result;
			return OnVisitSuccess(&Result, Result.Var);
		}

		return OnVisitSuccess(&Result, Result.Var);
	}
	else
	{
		return OnVisitFailure(&Result, MakeError(VisitError, "Unexpected expression in conditional statement predicate!, boolean expected"));
	}
}

internal visit_result
Visit_WhileLoop(interpreter_state *InterState, node *Node, context *Context)
{
	visit_result Result = {};
	if_node *IfNode = GetNode(Node, if_node);
	var *Var = OnVisitRegister(&Result, Visit(InterState, IfNode->Condition, Context));
	if(Var->Type == VarType_boolean_)
	{
		while(((boolean_ *)(Var + 1))->Value == 1)
		{
			OnVisitRegister(&Result, Visit(InterState, IfNode->Body, Context, true));
			if(Result.Error.Type != NoError) return Result;
			if(Result.Status == Status_OUT) break;
			if(Result.Status == Status_IGNORE) continue;
			Var = OnVisitRegister(&Result, Visit(InterState, IfNode->Condition, Context));
		}
		Result.Status = Status_NONE;
		return OnVisitSuccess(&Result, Result.Var);
	}

	return OnVisitFailure(&Result, MakeError(VisitError, "Unexpected expression in while statement predicate!, boolean expected"));
}

inline visit_result
Visit(interpreter_state *InterState, node *Node, context* Context, bool32 Loop)
{
	// TODO(Khisrow): Must apply firstvisit to avoid memory waste on the RuntimeMem
	visit_result Result = {};

	switch(Node->Header.Type)
	{
		case NodeType_number_node:
		{
			Result.Var = OnVisitRegister(&Result, Visit_Number(InterState, Node, Context));
		} break;
		case NodeType_string_node:
		{
			Result.Var = OnVisitRegister(&Result, Visit_String(InterState, Node, Context));
		} break;
		case NodeType_unary_node:
		{
			Result.Var = OnVisitRegister(&Result, Visit_UnaryOp(InterState, Node, Context));
		} break;
		case NodeType_binary_node:
		{
			Result.Var = OnVisitRegister(&Result, Visit_BinaryOp(InterState, Node, Context));
		} break;
		case NodeType_var_assign_node:
		{
			Result.Var = OnVisitRegister(&Result, Visit_VarAssign(InterState, Node, Context));
		} break;
		case NodeType_var_access_node:
		{
			Result.Var = OnVisitRegister(&Result, Visit_VarAccess(InterState, Node, Context));
		} break;
		case NodeType_comparison_node:
		{
			Result.Var = OnVisitRegister(&Result, Visit_Comparison(InterState, Node, Context));
		} break;
		case NodeType_compound_compare_node:
		{
			Result.Var = OnVisitRegister(&Result, Visit_CompoundCompare(InterState, Node, Context));
		} break;
		case NodeType_statements_node:
		{
			Result.Var = OnVisitRegister(&Result, Visit_Statements(InterState, Node, Context, Loop));
		} break;
		case NodeType_function_call_node:
		{
			Result.Var = OnVisitRegister(&Result, Visit_FunctionCall(InterState, Node, Context));
		} break;
		case NodeType_if_node:
		{
			Result.Var = OnVisitRegister(&Result, Visit_IfCondition(InterState, Node, Context, Loop));
		} break;
		case NodeType_while_node:
		{
			Result.Var = OnVisitRegister(&Result, Visit_WhileLoop(InterState, Node, Context));
		} break;
		default: return OnVisitFailure(&Result, MakeError(VisitError, "Visit function not defined for this type!"));
	}

	return Result;
}
