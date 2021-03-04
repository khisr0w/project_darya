/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /WinConsole                                                   |
    |    Creation date:  2/28/2021 1:58:15 AM                                          |
    |    Last Modified:                                                                |
    |                                                                                  |
    +=====================| Sayed Abid Hashimi, Copyright © All rights reserved |======+  */

#include "windows.h"

typedef int int32;
typedef int bool32;
typedef float real32;

#define internal static
#define ArrayCount(Array) sizeof((Array)) / sizeof((Array)[0])

#define MAX_STRING 1024

// NOTE(Khisrow): Globals
HANDLE GLOBALConsoleOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
HANDLE GLOBALConsoleInputHandle = GetStdHandle(STD_INPUT_HANDLE);

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

inline bool32
StringCompare(char * A, char *B)
{
	if(A && B)
	{
		int32 ALen = StringLength(A);
		int32 BLen = StringLength(B);
		if((ALen + BLen > 0) && (ALen != BLen))
		{
			return false;
		}

		for(int32 Index = 0;
			Index < ALen;
			++Index)
		{
			if(*A++ != *B++)
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

// TODO(Khisrow): The Result is only 1024 characters, should it be increased?
inline void
StringConcat(char *A, char *B, char *Dest)
{
	if(A && B)
	{
		int Index = 0;
		while(*A) Dest[Index++] = *A++;
		while(*B) Dest[Index++] = *B++;

		Dest[Index] = '\0';
	}
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

struct to_int_result
{
	bool32 Valid;
	int32 Value;
};
inline to_int_result
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

struct to_real_result
{
	bool32 Valid;
	real32 Value;
};
inline to_real_result
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
ToString(int32 Value, char *String, int32 *StartIndex = 0, char EndChar = '\0')
{
	char Reverse[MAX_STRING];
	int32 *StrIndex = 0;
	int32 Temp = 0;
	if(StartIndex) 
	{
		StrIndex = StartIndex;
	}
	else StrIndex = &Temp;

	if(Value < 0)
	{
		String[*StrIndex++] = '-';
		Value *= -1;
	}

	int32 RevIndex = 0;
	while(Value)
	{
		int32 Remain = Value % 10;
		Reverse[RevIndex++] = Remain + 48;
		Value = (int32)((Value - Remain) * 0.1f);
	}

	for(; RevIndex > 0;)
	{
		String[(*StrIndex)++] = Reverse[--RevIndex];
	}

	String[(*StrIndex)++] = EndChar;

	return String;
}

inline char *
ToString(real32 Value, char *String)
{
	int32 Mantissa = (int32)Value;
	int32 Index = 0;
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

internal void
Win32StdOut(char *Text)
{
	DWORD CharRead;
	WriteConsole(GLOBALConsoleOutputHandle, Text, StringLength(Text), NULL, NULL);
}

// TODO(Khisrow): The Input takes only 1024 characters, should it be increased?
struct win32_console_stdin
{
	char Input[1024];
	DWORD CharRead;
};

internal win32_console_stdin
Win32StdIn()
{
	win32_console_stdin Result = {};
	ReadConsole(GLOBALConsoleInputHandle, Result.Input,
				ArrayCount(Result.Input) + sizeof(char),
				&Result.CharRead, NULL);

	return Result;
}

#if 0
BOOL WINAPI
HandlerRoutine(DWORD CtrlType)
{
	switch (CtrlType)
	{
		case CTRL_C_EVENT:
		{
			Beep(750, 300);
			return true;
		}

		default:
		{
			return false;
		}
	}
}
#endif

internal void
ClearScreen()
{
	CONSOLE_SCREEN_BUFFER_INFO ConBufferInfo = {};
	GetConsoleScreenBufferInfo(GLOBALConsoleOutputHandle, &ConBufferInfo);

	SMALL_RECT RectToMove = {};
	RectToMove.Left = 0;
	RectToMove.Top = 0;
	RectToMove.Right = ConBufferInfo.dwSize.X;
	RectToMove.Bottom = ConBufferInfo.dwSize.X;

	COORD BufferOrigin = {};
	BufferOrigin.X = -ConBufferInfo.dwSize.X;
	BufferOrigin.Y = -ConBufferInfo.dwSize.Y;

	CHAR_INFO CharFill = {};
	CharFill.Attributes = 0;
	CharFill.Char.AsciiChar = (char)' ';

	ScrollConsoleScreenBuffer(GLOBALConsoleOutputHandle, &RectToMove, NULL, BufferOrigin, &CharFill);
	COORD Cursor = {};
	SetConsoleCursorPosition(GLOBALConsoleOutputHandle, Cursor);
}

int main(int argc, char *argv[])
{
	// NOTE(Khisrow): Input Strings
	char String[MAX_STRING];
	char String2[MAX_STRING];

	if(GLOBALConsoleOutputHandle == INVALID_HANDLE_VALUE) return 0;
	if(GLOBALConsoleInputHandle == INVALID_HANDLE_VALUE) return 0;

	if(StringCompare(argv[1], "shell"))
	{
		ClearScreen();
		SetConsoleTitle("Project Darya Shell");
		SetConsoleCtrlHandler(NULL, true);

		win32_console_stdin ReadData;
		// Win32StdOut("Project Darya Shell Module\n\n");
		while(true)
		{
			Win32StdOut("Project Darya Shell >> ");
			ReadData = Win32StdIn();
			if(StringCompare(ReadData.Input, "exit\r\n"))
			{
				Win32StdOut("\nPress Enter to exit...");
				Win32StdIn();
				break;
			}
			else if(StringCompare(ReadData.Input, "clear\r\n"))
			{
				ClearScreen();
			}
			else
			{
				if(StringLength(ReadData.Input) == 0)
				{
					Win32StdOut("\nKeyboard Interrupt voided!\n\n");
				}
				else if(StringCompare(ReadData.Input, "\r\n"))
				{
					Win32StdOut("\n");
				}
				else
				{
					Win32StdOut("ERROR: Command not found!\n\n");
				}
			}
		}
	}
}
