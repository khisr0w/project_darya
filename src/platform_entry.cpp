/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  2/28/2021 1:58:15 AM                                          |
    |    Last Modified:                                                                |
    |                                                                                  |
    +=====================| Sayed Abid Hashimi, Copyright © All rights reserved |======+  */


/* NOTE(Khisrow): BUGS!!!

   TODO(Khisrow):
       - More Precise floating point conversion from string to float and vice versa
	   - Robust power operation; handle wrapping around 32 bit and scientific notation of big numbers
	   - MUST decide whether a fixed starting memory is a good idea for this or not?
	   - On the spot conversion of int to float and vice versa when the value precision allows it
	   - MUST Support operator overloading
	   - Make scope for the conditional, loop and function calls

   WARNING(Khisrow):
       - Power operation is unstable and possibly unusable at this point and must fixed!
       - Float-String conversion is extremely unstable and must be amended with a better algorithm

*/

typedef int int32;
typedef unsigned char uint8;
typedef unsigned int uint32;
typedef int bool32;
typedef float real32;
typedef unsigned long long memory_index;
typedef unsigned long long uint64;

#define internal static

#define MAX_STRING 1024

#include "windows.h"
// NOTE(Khisrow): Globals
HANDLE GLOBALConsoleOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
HANDLE GLOBALConsoleInputHandle = GetStdHandle(STD_INPUT_HANDLE);

#include "platform_entry.h"
#include "commons.h"
#include "lexer.cpp"
#include "parser.cpp"
#include "interpreter.cpp"

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
PlatformClearScreen()
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

inline uint32
SafeTruncateUInt64 (uint64 Value)
{
	Assert(Value <= 0xFFFFFFFF);
	return (uint32)Value;
}

internal text_memory
PlatformOpenSourceFile(char *FileName)
{
	text_memory TextMemory = {};

	LARGE_INTEGER FileSize = {};
	DWORD BytesRead = 0;

	HANDLE FileHandle = CreateFileA(FileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if(FileHandle != INVALID_HANDLE_VALUE)
	{
		if(GetFileSizeEx(FileHandle, &FileSize))
		{
			uint32 FileSize32 = SafeTruncateUInt64(FileSize.QuadPart);
			TextMemory.Arena.Base = PlatformAllocMem(FileSize32);
			if(TextMemory.Arena.Base)
			{
				if(ReadFile(FileHandle, TextMemory.Arena.Base, FileSize32, &BytesRead, 0) &&
				   (BytesRead == FileSize32)) 
				{
					TextMemory.Arena.Size = FileSize32;
				}
				else PlatformFreeMem(TextMemory.Arena.Base);
			}
			else {}// TODO(Khisrow): LOG!!!
		}
		else {}// TODO(Khisrow): LOG!!!
	}
	else {}// TODO(Khisrow): LOG!!!

	CloseHandle(FileHandle);

	return TextMemory;
}

int main(int argc, char *argv[])
{
	// NOTE(Khisrow): Input Strings
	char String[MAX_STRING];
	char String2[MAX_STRING];

	if(GLOBALConsoleOutputHandle == INVALID_HANDLE_VALUE) return 0;
	if(GLOBALConsoleInputHandle == INVALID_HANDLE_VALUE) return 0;

	text_memory TextMemory = {};
	lexer_state LexerState = {};
	parser_state ParserState = {};
	interpreter_state InterState = {};

	// NOTE(Khisrow): Stack memory
	// WARNING(Khisrow): Make sure the size doesn't exceed SymbolSize
	symbol_table GlobalSymbolTable = {};
	GlobalSymbolTable.SymbolSize = 128;

	uint32 TotalMemorySize = (GlobalSymbolTable.SymbolSize*sizeof(symbol));

	GlobalSymbolTable.Symbols = (symbol *)VirtualAlloc(0, TotalMemorySize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

#if 0
	if(argc > 1)
#endif
	{
		if(StringCompare(argv[1], "shell"))
		{
			TextMemory.Arena.Size = Megabytes(1);
			TextMemory.Arena.Base = VirtualAlloc(0, TextMemory.Arena.Size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
			Assert(TextMemory.Arena.Base);

			PlatformClearScreen();
			SetConsoleTitle("Project Darya Shell");
			SetConsoleCtrlHandler(NULL, true);

			win32_console_stdin ReadData = {};
			ReadData.Input = (char *)TextMemory.Arena.Base;
			// PlatformStdOut("Project Darya Shell Module\n\n");
			while(true)
			{
				PlatformStdOut("Project Darya Shell >> ");
				ReadData.CharRead = PlatformStdIn(ReadData.Input, TextMemory.Arena.Size);

				if(StringCompare(ReadData.Input, "exit"))
				{
					PlatformStdOut("\nPress Enter to exit...");
					PlatformStdIn();
					break;
				}
				else if(StringCompare(ReadData.Input, "clear"))
				{
					PlatformClearScreen();
				}
				else
				{
					if(ReadData.CharRead == 0) PlatformStdOut("\nKeyboard Interrupt voided!\n\n");
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
							PlatformStdOut(String);
#endif
							// NOTE(Khisrow): Parser and AST
							InitializeParser(&ParserState, &LexerState.TokenMemory);
							parser_result AST = ParseTokens(&ParserState);
							if(AST.Error.Type != NoError)
							{
								Concat(String, false, AST.Error.Message, "\n");
								PlatformStdOut(String);
								continue;
							}

							context Stack = {"<root>", 0, 0, &GlobalSymbolTable};
							InitializeInterpreter(&InterState);
							visit_result VisResult = Visit(&InterState, AST.Node, &Stack);
							if(VisResult.Error.Type != NoError)
							{
								Concat(String, false, VisResult.Error.Message, "\n");
								PlatformStdOut(String);
								continue;
							}

							boolean_ *Number = GetVar(VisResult.Var, boolean_);
#if 0
							if(Number->Type == NumberType_Int) ToString(Number->Int, String);
							else if(Number->Type == NumberType_Real) ToString(Number->Real, String);
#else
							ToString(*((int32 *)(&Number->Value)), String);
#endif
							Concat(String, false, String, "\n")
								PlatformStdOut(String);
						}
						else 
						{
							Concat(String, false, LexerStatus.Error.Message, "\n");
							PlatformStdOut(String);
							continue;
						}
					}
				}
			}
		}
		else
		{
#if 1
			TextMemory = PlatformOpenSourceFile(argv[1]);
#else
			TextMemory = PlatformOpenSourceFile("./example.da");
#endif
			if(TextMemory.Arena.Size > 0)
			{
				char FileName[MAX_PATH] = "example.da";

				// NOTE(Khisrow): Lexer
				InitializeLexer(&LexerState, FileName, (char *)TextMemory.Arena.Base);
				op_status LexerStatus = PopulateTokens(&LexerState);
				if(!LexerStatus.Success)
				{
					Concat(String, false, LexerStatus.Error.Message, "\n");
					PlatformStdOut(String);
					return 0;
				}

				// NOTE(Khisrow): Parser and AST
				InitializeParser(&ParserState, &LexerState.TokenMemory);
				parser_result AST = ParseTokens(&ParserState);
				if(AST.Error.Type != NoError)
				{
					Concat(String, false, AST.Error.Message, "\n");
					PlatformStdOut(String);
					return 0;
				}

				// NOTE(Khisrow): Interpreter
				context Stack = {"<root>", 0, 0, &GlobalSymbolTable};
				InitializeInterpreter(&InterState);
				visit_result VisResult = Visit(&InterState, AST.Node, &Stack);
				if(VisResult.Error.Type != NoError)
				{
					Concat(String, false, VisResult.Error.Message, "\n");
					PlatformStdOut(String);
					return 0;
				}
#if 0
				boolean_ *Number = GetVar(VisResult.Var, boolean_);

				ToString(*((int32 *)(&Number->Value)), String);
				Concat(String, false, String, "\n");
				PlatformStdOut(String);
#endif
			}
			else PlatformStdOut("Cannot open the file specified");
		}
	}
}
