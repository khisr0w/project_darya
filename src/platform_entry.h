/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  3/9/2021 6:56:38 AM                                           |
    |    Last Modified:                                                                |
    |                                                                                  |
    +=====================| Sayed Abid Hashimi, Copyright © All rights reserved |======+  */

#if !defined(WIN32ENTRY_H)

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

struct transient_memory
{
	memory_arena Arena;
};

#define WIN32ENTRY_H
#endif
