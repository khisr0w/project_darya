/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  3/9/2021 6:56:38 AM                                           |
    |    Last Modified:                                                                |
    |                                                                                  |
    +=====================| Sayed Abid Hashimi, Copyright © All rights reserved |======+  */

#if !defined(WIN32ENTRY_H)

struct transient_memory
{
	void *MemoryBase;
	uint32 MemorySize;
};

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


#define WIN32ENTRY_H
#endif
