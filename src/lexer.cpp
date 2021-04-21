/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  3/4/2021 6:37:02 AM                                           |
    |    Last Modified:                                                                |
    |                                                                                  |
    +=====================| Sayed Abid Hashimi, Copyright © All rights reserved |======+  */

#include "lexer.h"

inline position *
Advance(lexer_state *LexerState)
{
	position *Pos = &LexerState->Pos;
	Assert(Pos->Index < StringLength(LexerState->TextMemory));

	Pos->Index++;
	Pos->Col++;

	if(LexerState->CurrentChar == '\n')
	{
		Pos->Line++;
		Pos->Col = 0;
	}

	if(Pos->Index < StringLength(LexerState->TextMemory))
	{
		LexerState->CurrentChar = LexerState->TextMemory[Pos->Index];
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
	FreeDynamicArena(&LexerState->TokenMemory);
	position *Pos = &LexerState->Pos;
	LexerState->TextMemory = TextMemory;
	Pos->Index = -1;
	Pos->Line = 0;
	Pos->Col = -1;
	Pos->FileName = FileName;
	Advance(LexerState);
}

inline void
AppendCharToken(lexer_state *LexerState, char *Value, token_type Type)
{
	Assert(StringLength(Value));

	token *Token = (token *)PushDynamicSize_(&LexerState->TokenMemory, sizeof(token) + StringLength(Value) + 1);
	Token->Value = (char *)(Token + 1);
	CopyToString(Value, Token->Value, ArrayCount(Token->Value));
	Token->Type = Type;

	Token->StartPos = LexerState->Pos;
	int32 EndPos = StringLength(Value);
	Token->EndPos.Index += EndPos;
	Token->EndPos.Col += EndPos;
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

internal token *
AppendNumberToken(lexer_state *LexerState)
{
	position StartPos = LexerState->Pos;
	bool32 DotEncountered = false;
	char CurrentChar = LexerState->CurrentChar;
	int32 StartIndex = LexerState->Pos.Index;
	int32 Length = 0;
	while((CurrentChar != '\0') && ((CurrentChar == '.') || ((CurrentChar >= '0') && (CurrentChar <= '9'))))
	{
		if(CurrentChar == '.')
		{
			if(DotEncountered) break;
			DotEncountered = true;
		}

		Length++;
		Advance(LexerState);
		CurrentChar = LexerState->CurrentChar;
	}

	token *Token = (token *)PushDynamicSize_(&LexerState->TokenMemory, sizeof(token) + Length + 1);
	Token->Value = (char *)(Token + 1);
	Token->StartPos = StartPos;
	Token->EndPos = LexerState->Pos;
	CopyDelimitString(LexerState->TextMemory, StartIndex, Length, Token->Value);
	if(DotEncountered) Token->Type = TT_REAL;
	else Token->Type = TT_INT;

	return Token;
}

internal token *
AppendWordToken(lexer_state *LexerState)
{
	position StartPos = LexerState->Pos;
	char CurrentChar = LexerState->CurrentChar;
	int32 StartIndex = LexerState->Pos.Index;
	int32 Length = 0;
	while((CurrentChar >= 'a' && CurrentChar <= 'z') ||
		  (CurrentChar >= 'A' && CurrentChar <= 'Z') ||
		  (CurrentChar == '_'))
	{
		Length++;
		Advance(LexerState);
		CurrentChar = LexerState->CurrentChar;
	}

	token *Token = (token *)PushDynamicSize_(&LexerState->TokenMemory, sizeof(token) + Length + 1);
	Token->Value = (char *)(Token + 1);
	Token->StartPos = StartPos;
	Token->EndPos = LexerState->Pos;
	CopyDelimitString(LexerState->TextMemory, StartIndex, Length, Token->Value);
	if(StringToArrayCompare(Token->Value, KEYWORDS)) Token->Type = TT_KEYWORD;
	else Token->Type = TT_ID;

	return Token;
}

inline char
LookAheadChar(lexer_state *LexerState)
{
	return LexerState->TextMemory[LexerState->Pos.Index + 1];
}

internal token *
AppendStringToken(lexer_state *LexerState, char Delimit)
{
	Advance(LexerState);
	position StartPos = LexerState->Pos;
	int32 StartIndex = LexerState->Pos.Index;
	int32 Length = 0;
	while(LexerState->CurrentChar != Delimit)
	{
		Length++;
		Advance(LexerState);
	}

	token *Token = (token *)PushDynamicSize_(&LexerState->TokenMemory, sizeof(token) + Length + 1);
	Token->Value = (char *)(Token + 1);
	Token->StartPos = StartPos;
	Token->EndPos = LexerState->Pos;
	CopyDelimitString(LexerState->TextMemory, StartIndex, Length, Token->Value);
	Token->Type = TT_STRING;

	return Token;
}

internal op_status
PopulateTokens(lexer_state *LexerState)
{
	op_status Status = {};
	char CurrentChar[2];
	CurrentChar[0] = LexerState->CurrentChar;
	CurrentChar[1] = '\0';
	char Temp[100];

	while(CurrentChar[0] != '\0')
	{
		if((*CurrentChar >= '0') && (*CurrentChar <= '9')) AppendNumberToken(LexerState);
		else if((*CurrentChar >= 'a' && *CurrentChar <= 'z') || (*CurrentChar >= 'A' && *CurrentChar <= 'Z')) AppendWordToken(LexerState);
		else
		{
			switch(*CurrentChar)
			{
				case ' ': break;
				case '\t': break;
				case '\r': break;
				case '\n': AppendCharToken(LexerState, "NEWLINE", TT_NEWLINE); break;
				case '+': AppendCharToken(LexerState, CurrentChar, TT_PLUS); break;
				case '-': AppendCharToken(LexerState, CurrentChar, TT_MINUS); break;
				case '^': AppendCharToken(LexerState, CurrentChar, TT_POW); break;
				case '(': AppendCharToken(LexerState, CurrentChar, TT_LPAREN); break;
				case ')': AppendCharToken(LexerState, CurrentChar, TT_RPAREN); break;
				case '{': AppendCharToken(LexerState, CurrentChar, TT_LCBRACKET); break;
				case '}': AppendCharToken(LexerState, CurrentChar, TT_RCBRACKET); break;
				case ',': AppendCharToken(LexerState, CurrentChar, TT_COMMA); break;
				case '*': AppendCharToken(LexerState, CurrentChar, TT_MUL); break;
				case '\'': AppendStringToken(LexerState, *CurrentChar); break;
				case '"': AppendStringToken(LexerState, *CurrentChar); break;
				case '/': 
				{
					if(LookAheadChar(LexerState) == '/')
					{
						while(LexerState->CurrentChar != '\n') Advance(LexerState);
					}
					else if(LookAheadChar(LexerState) == '*')
					{
						while(!(LexerState->CurrentChar == '*' && LookAheadChar(LexerState) == '/'))
						{
							if(LexerState->CurrentChar == '\0')
							{
								Status.Error = MakeError(InvalidSyntaxError,  "Multi-line comment must match with closing '*/' symbols");
								return Status;
							}
							Advance(LexerState);
						}
						Advance(LexerState);
					}
					else AppendCharToken(LexerState, CurrentChar, TT_DIV);
				} break;
				case '&':
			  	{
					if(LookAheadChar(LexerState) == '&')
					{
						AppendCharToken(LexerState, "&&", TT_AND);
						Advance(LexerState);
					}
					else 
					{
						Advance(LexerState);
						Status.Error = MakeError(CharacterError, "'&' is not a valid character, did you mean '&&'?");
						return Status;
					}
				} break;
				case '|':
			  	{
					if(LookAheadChar(LexerState) == '|')
					{
						AppendCharToken(LexerState, "||", TT_OR);
						Advance(LexerState);
					}
					else 
					{
						Advance(LexerState);
						Status.Error = MakeError(CharacterError, "'|' is not a valid character, did you mean '||'?");
						return Status;
					}
				} break;
				case '=':
			  	{
					if(LookAheadChar(LexerState) == '=')
					{
						AppendCharToken(LexerState, "==", TT_EQEQ);
						Advance(LexerState);
					}
					else AppendCharToken(LexerState, CurrentChar, TT_EQ);
				} break;

				case '!':
			  	{
					if(LookAheadChar(LexerState) == '=')
					{
						AppendCharToken(LexerState, "!=", TT_NEQ);
						Advance(LexerState);
					}
					else AppendCharToken(LexerState, CurrentChar, TT_NOT);
				} break;

				case '<':
				{
					if(LookAheadChar(LexerState) == '=')
					{
						AppendCharToken(LexerState, "<=", TT_LTE);
						Advance(LexerState);
					}
					else AppendCharToken(LexerState, CurrentChar, TT_LT);
				} break;

				case '>': 
				{
					if(LookAheadChar(LexerState) == '=')
					{
						AppendCharToken(LexerState, ">=", TT_GTE);
						Advance(LexerState);
					}
					else AppendCharToken(LexerState, CurrentChar, TT_GT);
				} break;

				default:
				{
					Advance(LexerState);
					Concat(Temp, false, "'", CurrentChar, "'", " is not a valid character!");
					Status.Error = MakeError(CharacterError, Temp);
					return Status;
				} break;
			}
			Advance(LexerState);
		}
		CurrentChar[0] = LexerState->CurrentChar;
	}

	AppendCharToken(LexerState, "EOF", TT_EOF);

	Status.Success = true;
	return Status;
}
