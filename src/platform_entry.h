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

struct node_memory
{
	void *MemoryBase;
	uint32 MaxMemorySize;
	uint32 Size;
};

struct memory_arena
{
	uint32 MaxSize;
	uint32 Size;
	void *Base;
};

struct transient_memory
{
	memory_arena Arena;
};

#define WIN32ENTRY_H
#endif
