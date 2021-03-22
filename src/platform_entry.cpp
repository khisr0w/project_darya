/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  2/28/2021 1:58:15 AM                                          |
    |    Last Modified:                                                                |
    |                                                                                  |
    +=====================| Sayed Abid Hashimi, Copyright © All rights reserved |======+  */


/* NOTE(Khisrow): BUGS!!!

   TODO(Khisrow):
       1. More Precise floating point conversion from string to float and vice versa
	   2. Robust power operation; handle wrapping around 32 bit and scientific notation of big numbers
	   3. MUST decide whether a fixed starting memory is a good idea for this or not?
	   4. Change to dynamic memory allocation! Even for stack, since we will allow things like variable strings

   WARNING(Khisrow):
       1. Power operation is unstable and possibly unusable at this point and must fixed!
       2. Float-String conversion is extremely unstable and must be amended with a better algorithm

*/

typedef int int32;
typedef unsigned char uint8;
typedef unsigned int uint32;
typedef int bool32;
typedef float real32;
typedef unsigned long long memory_index;

#define internal static

#define MAX_STRING 1024

#include "windows.h"
#include "platform_entry.h"
#include "commons.h"
#include "lexer.cpp"
#include "parser.cpp"
#include "interpreter.cpp"

// NOTE(Khisrow): Globals
HANDLE GLOBALConsoleOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
HANDLE GLOBALConsoleInputHandle = GetStdHandle(STD_INPUT_HANDLE);

internal void
Win32StdOut(char *Text)
{
	DWORD CharRead;
	WriteConsole(GLOBALConsoleOutputHandle, Text, StringLength(Text), NULL, NULL);
}

// TODO(Khisrow): The Input takes only 1024 characters, should it be increased?
internal DWORD
Win32StdIn(char *String = 0, uint32 StringSize = 0)
{
	char Temp[4];
	if(!String)
	{
		String = Temp;
		StringSize = 4;
	}
	else Assert(StringSize != 0);

	String[0] = '\0';
	DWORD CharRead = 0;
	ReadConsole(GLOBALConsoleInputHandle, String,
				StringSize, &CharRead, NULL);
	if(IndexInString(String, '\r'))
	{
		String[IndexInString(String, '\r')] = '\0';
	}
	else
	{
		String[IndexInString(String, '\n') + 1] = '\0';
	}

	return CharRead;
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

	text_memory TextMemory = {};
	TextMemory.Arena.Size = Megabytes(1);

	lexer_state LexerState = {};
	LexerState.TokenMemory.Size = Megabytes(10);

	parser_state ParserState = {};
	ParserState.AST.Size = Megabytes(10);

	interpreter_state InterState = {};
	InterState.RuntimeMem.Size = Megabytes(4);
	InterState.Stack.Size = Megabytes(10);

	// NOTE(Khisrow): Stack memory
	// WARNING(Khisrow): Make sure the size doesn't exceed SymbolSize
	symbol_table SymbolTable = {};
	SymbolTable.SymbolSize = 1000;
	SymbolTable.Arena.Size = SymbolTable.SymbolSize * sizeof(symbol);

	uint32 TotalMemorySize = TextMemory.Arena.Size +
							 LexerState.TokenMemory.Size +
							 ParserState.AST.Size +
							 InterState.RuntimeMem.Size +
							 InterState.Stack.Size +
							 SymbolTable.Arena.Size;

	TextMemory.Arena.Base = VirtualAlloc(0, TotalMemorySize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
	LexerState.TokenMemory.Base = (uint8 *)TextMemory.Arena.Base + TextMemory.Arena.Size;
	ParserState.AST.Base = (uint8 *)LexerState.TokenMemory.Base + LexerState.TokenMemory.Size;
	InterState.RuntimeMem.Base = (uint8 *)ParserState.AST.Base + ParserState.AST.Size;
	SymbolTable.Arena.Base = (uint8 *)InterState.RuntimeMem.Base + InterState.RuntimeMem.Size;
	InterState.Stack.Base = (uint8 *)SymbolTable.Arena.Base + SymbolTable.Arena.Size;

	Assert(TextMemory.Arena.Base);
	Assert(LexerState.TokenMemory.Base);
	Assert(ParserState.AST.Base);
	Assert(InterState.RuntimeMem.Base);

	//if(StringCompare(argv[1], "shell"))
	{
		ClearScreen();
		SetConsoleTitle("Project Darya Shell");
		SetConsoleCtrlHandler(NULL, true);

		win32_console_stdin ReadData = {};
		ReadData.Input = (char *)TextMemory.Arena.Base;
		// Win32StdOut("Project Darya Shell Module\n\n");
		while(true)
		{
			Win32StdOut("Project Darya Shell >> ");
			ReadData.CharRead = Win32StdIn(ReadData.Input, TextMemory.Arena.Size);

			if(StringCompare(ReadData.Input, "exit"))
			{
				Win32StdOut("\nPress Enter to exit...");
				Win32StdIn();
				break;
			}
			else if(StringCompare(ReadData.Input, "clear"))
			{
				ClearScreen();
			}
			else
			{
				if(ReadData.CharRead == 0) Win32StdOut("\nKeyboard Interrupt voided!\n\n");
				else if(StringCompare(ReadData.Input, "\r\n")) {}
				else
				{
					// NOTE(Khisrow): Lexer and Tokenizer
					char FileName[MAX_PATH] = "<stdin>";
					InitializeLexer(&LexerState, FileName, (char *)TextMemory.Arena.Base);
					op_status LexerStatus = PopulateTokens(&LexerState);
					if(LexerStatus.Success)
					{
#if 0
						String[0] = '\0';
						for(token *Token = LexerState.Tokens.MemoryBase;
							Token->Type != TT_EOF;
							++Token)
						{
							Concat(String, true, " [", Token->Value, " : ", TokenTypeString[Token->Type], "] ");
						}
						Concat(String, true, "\n");
						Win32StdOut(String);
#endif
						//NOTE(Khisrow): Parser and AST
						InitializeParser(&ParserState, &LexerState.TokenMemory);
						parser_result AST = ParseTokens(&ParserState);
						if(AST.Error.Type != NoError)
						{
							Concat(String, false, AST.Error.Message, "\n");
							Win32StdOut(String);
							continue;
						}

						context Stack = {"<root>", 0, 0, &SymbolTable};
						InitializeInterpreter(&InterState);
						visit_result VisResult = Visit(&InterState, AST.Node, &Stack);
						if(VisResult.Error.Type != NoError)
						{
							Concat(String, false, VisResult.Error.Message, "\n");
							Win32StdOut(String);
							continue;
						}

						number *Number = (number *)(VisResult.Var + 1);
						if(Number->Type == NumberType_Int) ToString(Number->Int, String);
						else if(Number->Type == NumberType_Real) ToString(Number->Real, String);
						Concat(String, false, String, "\n")
						Win32StdOut(String);
					}
					else 
					{
						Concat(String, false, LexerStatus.Error.Message, "\n");
						Win32StdOut(String);
						continue;
					}
				}
			}
		}
	}
}
