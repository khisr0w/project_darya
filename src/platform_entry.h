/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  3/9/2021 6:56:38 AM                                           |
    |    Last Modified:                                                                |
    |                                                                                  |
    +=====================| Sayed Abid Hashimi, Copyright © All rights reserved |======+  */

#if !defined(WIN32ENTRY_H)

#if DEBUG
#define Assert(Expression) if(!(Expression)) {*(int*)0 = 0;}
#define InvalidCodePath Assert(!"InvalidCodePath")
#else
#define Assert(Expression)
#endif

#define ArrayCount(Array) (sizeof((Array)) / sizeof((Array)[0]))

#define Kilobytes(Value) ((Value) * 1024LL)
#define Megabytes(Value) (Kilobytes(Value) * 1024LL)
#define Gigabytes(Value) (Megabytes(Value) * 1024LL)
#define Terabytes(Value) (Gigabytes(Value) * 1024LL)

struct win32_console_stdin
{
	char *Input;
	DWORD CharRead;
};

struct memory_arena
{
	memory_index Size;
	void *Base;
	memory_index Used;

	int32 TempCount;
};

struct temporary_memory
{
	memory_arena *Arena;
	memory_index Used;
};

struct dynamic_memory_block
{
	dynamic_memory_block *Next;
};

struct dynamic_memory_arena
{
	memory_index MemSize;
	int32 Length;
	dynamic_memory_block *Blocks;
	dynamic_memory_block *LastBlock;

	int32 TempCount;
};

struct dynamic_temporary_memory
{
	dynamic_memory_arena Arena;
	memory_index Size;
};

inline void *
PlatformAllocMem(memory_index Size, HANDLE *HeapHandle = 0)
{
	return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, Size);
}

inline void *
PlatformReallocMem(void *Memory, memory_index Size, HANDLE *HeapHandle = 0)
{
	return HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, Memory, Size);
}

inline bool32
PlatformFreeMem(void *Memory, HANDLE *HeapHandle = 0)
{
	return HeapFree(GetProcessHeap(), 0, Memory);
}

inline memory_index
PlatformMemSize(void *Memory, HANDLE *HeapHandle = 0)
{
	return HeapSize(GetProcessHeap(), 0, Memory);
}

inline int32 StringLength(char *String);
inline int32 IndexInString(char *String, char Delimiter);

internal void
PlatformStdOut(char *Text)
{
	DWORD CharRead;
	WriteConsole(GLOBALConsoleOutputHandle, Text, StringLength(Text), NULL, NULL);
}

// TODO(Khisrow): The Input takes only 1024 characters, should it be increased?
internal DWORD
PlatformStdIn(char *String = 0, uint32 StringSize = 0)
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

#define WIN32ENTRY_H
#endif
