/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  3/4/2021 6:40:34 AM                                           |
    |    Last Modified:                                                                |
    |                                                                                  |
    +=====================| Sayed Abid Hashimi, Copyright � All rights reserved |======+  */

#if !defined(COMMONS_H)

#define MAX_FILENAME 260


#define Concat(Dest, Bool, ...) Concat_(Dest, Bool, ArrayCount(Dest), __VA_ARGS__)
#define Concat_(Dest, Bool, Length, ...) {\
												 Assert(Dest);\
									   		     char *Dest1 = Dest;\
												 int32 StartLength = 0;\
												 if(Bool == true)\
												 {\
													 StartLength = StringLength(Dest1);\
												 }\
												 Dest1 += StartLength;\
									   		     int32 TotalLength = 0;\
												 int32 DestLength = Length;\
									   		     char *Temp[] = {__VA_ARGS__};\
									   		     int32 StrCount = ArrayCount(Temp);\
									   		     for(int32 StrIndex = 0; StrIndex < StrCount; ++StrIndex)\
									   		     {\
									   		  	   TotalLength += StringLength(Temp[StrIndex]);\
									   		     }\
									   		     Assert(TotalLength < (DestLength - StartLength));\
									   		     if(Dest1)\
									   		     {\
									   		  	   for(int32 StrIndex = 0; StrIndex < StrCount; ++StrIndex)\
									   		  	   {\
									   		  		   char *String = Temp[StrIndex];\
									   		  		   while(*String) *Dest1++ = *String++;\
									   		  	   }\
									   		  	   *Dest1 = '\0';\
									   		     }\
											}

#define PushStruct(Arena, Struct) (Struct *)PushSize_(Arena, sizeof(Struct))
#define PushSize(Arena, Struct, Size) (Struct *)PushSize_(Arena, sizeof(Struct)*(Size))
inline void *
PushSize_(memory_arena *MemoryArena, memory_index Size)
{
	void *Result = 0;

	Assert(Size > 0);
	Assert((MemoryArena->Used + Size) < MemoryArena->Size);
	if((MemoryArena->Used + Size) < MemoryArena->Size)
	{
		Result = (uint8 *)MemoryArena->Base + MemoryArena->Used;
		MemoryArena->Used += Size;
	}
	else InvalidCodePath;

	return Result;
}

#define GetBlockValue(Block, Type) (Type *)GetBlockValue_(Block)
inline void *
GetBlockValue_(dynamic_memory_block *Block)
{
	return (Block + 1);
}

#define PushDynamicStruct(Arena, Struct) (Struct *)PushDynamicSize_(Arena, sizeof(Struct))
#define PushDynamicSize(Arena, Struct, Size) (Struct *)PushDynamicSize_(Arena, sizeof(Struct)*(Size))
inline void *
PushDynamicSize_(dynamic_memory_arena *MemoryArena, memory_index Size)
{
	dynamic_memory_block *Memory = (dynamic_memory_block *)PlatformAllocMem(sizeof(dynamic_memory_block) + Size);
	Assert(Memory);
	if(Memory)
	{
		if(MemoryArena->LastBlock) MemoryArena->LastBlock->Next = Memory;
		else MemoryArena->Blocks = Memory;
		MemoryArena->LastBlock = Memory;

		MemoryArena->MemSize += PlatformMemSize(Memory);
		MemoryArena->Length++;
	}

	return (Memory + 1);
}

// TODO(Khisrow): WARNING This is broken, fix the LastBlock Bug
internal bool32
FreeDynamicBlock(dynamic_memory_arena *MemoryArena, void *MemoryToDelete)
{
	bool32 Result = false;

	int Length = 0;
	dynamic_memory_block *PriorBlock = 0;
	for(dynamic_memory_block *Block = MemoryArena->Blocks;
		Block;
		Block = Block->Next)
	{
		Length++;
		if(((Block + 1) == MemoryToDelete) || (Block == MemoryToDelete))
		{
			if(PriorBlock) PriorBlock->Next = Block->Next;
			else MemoryArena->Blocks = Block->Next;
			if(Length == MemoryArena->Length) MemoryArena->LastBlock = PriorBlock;
			MemoryArena->MemSize -= PlatformMemSize(Block);
			PlatformFreeMem(Block);
			MemoryArena->Length--;

			Result = true;
			break;
		}

		PriorBlock = Block;
	}

	return Result;
}

internal bool32
FreeDynamicArena(dynamic_memory_arena *MemoryArena)
{
	bool32 Result = false;

	dynamic_memory_block *NextBlock = 0;
	for(dynamic_memory_block *Block = MemoryArena->Blocks;
		Block;
		Block = NextBlock)
	{
		NextBlock = Block->Next;

		MemoryArena->MemSize -= PlatformMemSize(Block);
		bool32 Res = PlatformFreeMem(Block);
		MemoryArena->Length--;
	}

	Assert(MemoryArena->Length == 0);
	if(MemoryArena->Length == 0)
	{
		MemoryArena->Blocks = 0;
		MemoryArena->LastBlock = 0;
		Result = true;
	}

	return Result;
}

inline temporary_memory
BeginTemporaryMemory(memory_arena *Arena)
{
	temporary_memory Result;

	Result.Arena = Arena;
	Result.Used = Arena->Used;

	++Arena->TempCount;

	return Result;
}

inline void
EndTemporaryMemory(temporary_memory TempMem)
{
	memory_arena *Arena = TempMem.Arena;
	Assert(Arena->Used >= TempMem.Used);
	Arena->Used = TempMem.Used;
	Assert(Arena->TempCount > 0);
	--Arena->TempCount;
}

inline int32
StringLength(char *String)
{
	int Result = 0;

	if(String)
	{
		while(*String++)
		{
			++Result;
		}
	}
	
	return Result;
}

inline void
CopyToString(char Source, char *Dest, int32 DestListLength = -1)
{
	int32 DestLength = DestListLength;
	if(DestListLength == -1)
	{
		DestLength = StringLength(Dest);
	}
	Assert(DestLength > 0);

	*Dest++ = Source;
	*Dest = '\0';
}

inline void
CopyDelimitString(char *Source, int32 Index, int32 Length, char *Dest)
{
	Source = Source + Index;

	int32 LastIndex = Index + Length - 1;
	while(Index <= LastIndex)
	{
		*Dest++ = *Source++;
		++Index;
	}

	*Dest = '\0';
}

inline void
CopyToString(char *Source, char *Dest, int32 DestListLength)
{
	int32 SourceLength = StringLength(Source);
	int32 DestLength = DestListLength;
	if(DestListLength == -1)
	{
		DestLength = StringLength(Dest);
	}

	Assert(SourceLength <= DestLength);

	while(*Source)
	{
		*Dest++ = *Source++;
	}
	*Dest = '\0';
}

internal bool32
StringCompare(char * A, char *B)
{
	if(A && B)
	{
		int32 ALen = StringLength(A);
		int32 BLen = StringLength(B);
		if((ALen + BLen > 0) && (ALen != BLen)) { return false; }

		for(int32 Index = 0;
			Index < ALen;
			++Index)
		{
			if(*A++ != *B++) { return false; }
		}

		return true;
	}

	return false;
}

#define StringToArrayCompare(A, B) StringToArrayCompare_(A, B, ArrayCount(B))
internal bool32
StringToArrayCompare_(char * A, char **SArray, int32 SArrayLength)
{
	Assert(A);
	for(char **Temp = SArray;
		SArrayLength-- > 0;
		++Temp)
	{
		char *B = *Temp;
		if(StringCompare(A, B)) return true;
	}

	return false;
}

#if 0
inline bool32
CharToListCompare(char Char, char *List, int32 ListLength, int32 *FoundIndex = 0)
{
	for(int32 Index = 0;
		Index < ListLength;
		++Index)
	{
		if(Char == List[Index])
		{
			if(FoundIndex) *FoundIndex = Index;
			return true;
		}
	}

	return false;
}
#endif

inline char *
StringConcat(char A, char *B, char *Dest)
{
	if(B && Dest)
	{
		int Index = 0;
		*Dest++ = A;
		while(*B) *Dest++ = *B++;

		*Dest = '\0';
	}

	return Dest;
}

#if 0
inline char *
StringConcat(char *A, char *B, char *Dest)
{
	if(A && B && Dest)
	{
		int Index = 0;
		while(*A) *Dest++ = *A++;
		while(*B) *Dest++ = *B++;

		*Dest = '\0';
	}

	return Dest;
}
#endif

inline real32
Power(real32 Base, int32 Power)
{
	real32 Result = Base;

	if (Power == 0) return 1;
	if(Base == 0) return 0;

	while(--Power > 0)
	{
		Result *= Base;
	}

	return Result;
}

inline int32
Power(int32 Base, int32 Power)
{
	int32 Result = Base;

	if (Power == 0) return 1;
	if(Base == 0) return 0;

	while(--Power > 0)
	{
		Result *= Base;
	}

	return Result;
}

inline int32
IndexInString(char *String, char Delimiter)
{
	if(!String) return -1;

	int32 StrLength = StringLength(String);
	for(int32 Index = 0;
		Index < StrLength;
		++Index)
	{
		if(String[Index] == Delimiter) return Index;
	}

	return -1;
}

inline char *
SubString(char *Source, char *Dest,
		  int32 StartIndex, int32 EndIndex,
		  bool32 Termintor = true)
{
	if((!Source) || (!Dest)) return '\0';

	int32 Index = 0;
	while(StartIndex <= EndIndex)
	{
		Dest[Index++] = Source[StartIndex++];
	}

	if(Termintor) Dest[Index++] = '\0';

	return Dest;
}

inline bool32
StringContains(char *String, char Char, int32 Index = -1)
{
	bool32 Result = false;
	if(String)
	{
		int32 StrLength = StringLength(String);
		if(Index == -1)
		{
			for(int32 Index = 0;
				Index > StrLength;
				++Index)
			{
				if(String[Index] == Char)
				{
					Result = true;
					return Result;
				}
			}
		}
		else
		{
			Result = String[Index] == Char;
		}
	}

	return Result;
}


// TODO(Khisrow): These needs to change to a more robust and efficient
// algorithms for conversion.
struct to_int_result
{
	bool32 Valid;
	int32 Value;
};
internal to_int_result
ToInt(char *String)
{
	to_int_result Result {};

	int32 Length = StringLength(String);
	if(Length > 0)
	{
		int32 Value = 0;
		int32 Sign = 1;
		if((int32)(*String - 45) == 0)
		{
			Sign = -1;
			--Length;
			++String;
		}

		for(int32 RaisedAmount = Length - 1;
			RaisedAmount >= 0;
			--RaisedAmount)
		{
			int32 Integer = (int32)((*String++) - 48);
			if((Integer >= 0) && (Integer <= 9))
			{
				Integer *= Power(10, RaisedAmount);
				Value += Integer;
			}

			else return Result;
		}

		Value *= Sign;
		Result.Value = Value;
		Result.Valid = true;
	}

	return Result;
}

struct to_real_result
{
	bool32 Valid;
	real32 Value;
};
internal to_real_result
ToReal(char *String)
{
	to_real_result Result = {};
	char Temp[MAX_STRING];

	int32 StrLength = StringLength(String);
	if(StrLength > 0)
	{
		int32 IndexToEnd = StrLength - 1;
		int32 IndexToFraction = IndexInString(String, '.');
		int32 StartIndex = 0;
		real32 Value = 1;
		if(StringContains(String, '-', 0))
		{
			Value = -1;
			StartIndex = 1;
		}

		SubString(String, Temp, StartIndex, IndexToFraction - 1);
		int32 Mantissa = 0;
		to_int_result IntResult = ToInt(Temp);
		if(IntResult.Valid) Mantissa = IntResult.Value;
		else return Result;

		SubString(String, Temp, IndexToFraction + 1, StrLength - 1);
		int32 Exponent = 0;
		if(StringLength(Temp))
		{
			IntResult = ToInt(Temp);
			if(IntResult.Valid) Exponent = IntResult.Value;
			else return Result;
		}
		real32 Divisor = (real32)(1.0f / (real32)Power(10.0f, StrLength - (IndexToFraction + 1)));
		Value *= (Mantissa + ((real32)Exponent)*Divisor);

		Result.Valid = true;
		Result.Value = Value;
	}

	return Result;
}

inline char *
ToString(char Char, char* Dest)
{
	Assert(Dest);
	char Result[2];
	Result[0] = Char;
	Result[1] = '\0';

	Dest = Result;

	return Result;
}

inline int32
IntLength(int32 Value)
{
	int32 Length = 0;
	while(Value)
	{
		int32 Remain = Value % 10;
		Value = (int32)((Value - Remain) * 0.1f);
		++Length;
	}

	return Length;
}

inline char *
ToString(int32 Value, char *String, int32 *StartIndex = 0, char EndChar = '\0')
{
	char Reverse[MAX_STRING];
	int32 *StrIndex = 0;
	int32 Temp = 0;
	if(StartIndex) StrIndex = StartIndex;
	else StrIndex = &Temp;

	if(Value == 0)
	{
		String[(*StrIndex)++] = '0';
		String[(*StrIndex)++] = EndChar;
		return String;
	}

	if(Value < 0)
	{
		String[(*StrIndex)++] = '-';
		Value *= -1;
	}

	int32 RevIndex = 0;
	while(Value)
	{
		int32 Remain = Value % 10;
		Reverse[RevIndex++] = Remain + 48;
		Value = (int32)((Value - Remain) * 0.1f);
	}

	for(; RevIndex > 0;) String[(*StrIndex)++] = Reverse[--RevIndex];

	String[(*StrIndex)++] = EndChar;

	return String;
}

inline int32
Real32Length(real32 Value)
{
	int32 Length = 0;
	int32 Mantissa = (int32)Value;

	Length = IntLength(Mantissa);
	Length++;

	real32 Subtract = (real32)(Value - Mantissa);
	if(Subtract < 0) Subtract *= -1;
	real32 Exponent = Subtract;
	while(true)
	{
		Exponent *= 10;
		Length++;
		if(Exponent == (int32)Exponent) break;
	}

	return Length;
}

inline char *
ToString(real32 Value, char *String)
{
	int32 Mantissa = (int32)Value;
	int32 Index = 0;

	if(Value == 0)
	{
		String[Index++] = '0';
		String[Index++] = '\0';
		return String;
	}
	ToString(Mantissa, String, &Index, '.');

	real32 Subtract = (real32)(Value - Mantissa);
	if(Subtract < 0) Subtract *= -1;
	real32 Exponent = Subtract;
	while(true)
	{
		Exponent *= 10;
		if(Exponent == (int32)Exponent) break;
	}

	ToString(Exponent, String, &Index);

	return String;
}


#define COMMONS_H
#endif
