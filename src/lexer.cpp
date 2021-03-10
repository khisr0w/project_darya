/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  3/4/2021 6:37:02 AM                                           |
    |    Last Modified:                                                                |
    |                                                                                  |
    +=====================| Sayed Abid Hashimi, Copyright � All rights reserved |======+  */

#include "lexer.h"

inline position * 
Advance(lexer_state *LexerState)
{
	position *Pos = &LexerState->Pos;
	Assert(Pos->Index < StringLength(Pos->TextMemory));

	Pos->Index++;
	Pos->Col++;

	if(LexerState->CurrentChar == '\n')
	{
		Pos->Line++;
		Pos->Col = 0;
	}

	if(Pos->Index < StringLength(Pos->TextMemory))
	{
		LexerState->CurrentChar = Pos->TextMemory[Pos->Index];
	}
	else LexerState->CurrentChar = '\0';

	return Pos;
}

inline position
IncrementPosition(position Pos, char CurrentChar)
{
	Pos.Index++;
	Pos.Col++;

	if(CurrentChar == '\n')
	{
		Pos.Line++;
		Pos.Col = 0;
	}

	return Pos;
}

internal void
InitializeLexer(lexer_state *LexerState, char *FileName, char *TextMemory)
{
	LexerState->Tokens.TokenCount = 0;
	position *Pos = &LexerState->Pos;
	Pos->Index = -1;
	Pos->Line = 0;
	Pos->Col = -1;
	Pos->FileName = FileName;
	Pos->TextMemory = TextMemory;
	Advance(LexerState);
}

inline void
AppendToTokenList(token_list *Tokens, char *Value, token_type Type, position *StartPos, position *EndPos = 0)
{
	Assert(StringLength(Value));

	token *Token = Tokens->MemoryBase + Tokens->TokenCount++;
	CopyToString(Value, Token->Value, ArrayCount(Token->Value));
	Token->Type = Type;

	if(StartPos) 
	{
		Token->StartPos = *StartPos;
		Token->EndPos = IncrementPosition(*StartPos, Value[0]);
	}
	if(EndPos) Token->EndPos = *EndPos;
}

#if 0
internal token_type
CreateNumber(lexer_state *LexerState, char *Dest)
{
	token_type Type;
	int32 DestLength = StringLength(Dest);
	bool32 DotEncountered = false;
	Assert(DestLength);

	char CurrentChar = LexerState->CurrentChar;
	while((CurrentChar != '\0') && ((CurrentChar == '.') ||
		   CharToListCompare(CurrentChar, DIGITS, ArrayCount(DIGITS))))
	{
		if(CurrentChar == '.')
		{
			if(DotEncountered) break;
			DotEncountered = true;
			*Dest++ = '.';
		}
		else *Dest++ = CurrentChar;

		Advance(LexerState);
		CurrentChar = LexerState->CurrentChar;
	}
	*Dest = '\0';

	if(DotEncountered) Type = TT_FLOAT;
	else Type = TT_INT;

	return Type;
}
#endif

#define MakeError(ErrorType, Message) MakeError_(#ErrorType ": ", Message, ErrorType)
inline error
MakeError_(char * MessageHeader, char *Message, error_type ErrorType)
{
	error Error = {};
	Error.Type = ErrorType;
	Concat(Error.Message, false, MessageHeader, Message);

	return Error;
}

#define AppendNumberToList(LexerState, Dest) AppendNumberToList_(LexerState, Dest, ArrayCount(Dest))
internal token *
AppendNumberToList_(lexer_state *LexerState, char *Dest, int32 DestLength)
{
	Assert(DestLength);

	token_list *Tokens = &LexerState->Tokens;
	token *Token = Tokens->MemoryBase + Tokens->TokenCount++;
	Token->StartPos = LexerState->Pos;

	bool32 DotEncountered = false;

	char CurrentChar = LexerState->CurrentChar;
	int32 Index = 0;
	while((CurrentChar != '\0') && ((CurrentChar == '.') ||
		   CharToListCompare(CurrentChar, DIGITS, ArrayCount(DIGITS))))
	{
		if(CurrentChar == '.')
		{
			if(DotEncountered) break;
			DotEncountered = true;
			Dest[Index++] = '.';
		}
		else
		{
			Dest[Index++] = CurrentChar;
		}

		Advance(LexerState);
		CurrentChar = LexerState->CurrentChar;
	}
	Dest[Index] = '\0';
	Token->EndPos = LexerState->Pos;

	CopyToString(Dest, Token->Value, ArrayCount(Token->Value));
	if(DotEncountered) Token->Type = TT_FLOAT;
	else Token->Type = TT_INT;

	return Token;
}

internal op_status
PopulateTokens(lexer_state *LexerState)
{
	op_status Status = {};
	token_list *Tokens = &LexerState->Tokens;
	char CurrentChar[2];
	CurrentChar[1] = '\0';
	CurrentChar[0] = LexerState->CurrentChar;
	char Temp[100];
	Temp[99] = '\0';

	while(CurrentChar[0] != '\0')
	{
		if(*CurrentChar == ' ' || *CurrentChar == '\t')
		{
			Advance(LexerState);
		}
		else if(CharToListCompare(*CurrentChar, DIGITS, ArrayCount(DIGITS)))
		{
			AppendNumberToList(LexerState, Temp);
		}
		else if(*CurrentChar == '+')
		{
			AppendToTokenList(Tokens, CurrentChar, TT_PLUS, &LexerState->Pos);
			Advance(LexerState);
		}
		else if(*CurrentChar == '-')
		{
			AppendToTokenList(Tokens, CurrentChar, TT_MINUS, &LexerState->Pos);
			Advance(LexerState);
		}
		else if(*CurrentChar == '*')
		{
			AppendToTokenList(Tokens, CurrentChar, TT_MUL, &LexerState->Pos);
			Advance(LexerState);
		}
		else if(*CurrentChar == '/')
		{
			AppendToTokenList(Tokens, CurrentChar, TT_DIV, &LexerState->Pos);
			Advance(LexerState);
		}
		else if(*CurrentChar == '(')
		{
			AppendToTokenList(Tokens, CurrentChar, TT_LPAREN, &LexerState->Pos);
			Advance(LexerState);
		}
		else if(*CurrentChar == ')')
		{
			AppendToTokenList(Tokens, CurrentChar, TT_RPAREN, &LexerState->Pos);
			Advance(LexerState);
		}
		else
		{
			Advance(LexerState);
			Concat(Temp, false, "'", CurrentChar, "'", " is not a valid character!");
			Status.Error = MakeError(CharacterError, Temp);
			return Status;
		}

		CurrentChar[0] = LexerState->CurrentChar;
	}

	AppendToTokenList(Tokens, TokenTypeString[TT_EOF], TT_EOF, &LexerState->Pos);

	Status.Success = true;
	return Status;
}