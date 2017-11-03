#include <Windows.h>
#include <fstream>
#ifndef _LoggingHeader_
#define _LoggingHeader_

char *GetDirectoryFile(char *filename);
char *GetAppdataFile(char *filename);
void __cdecl add_log(const char * fmt, ...);
char *strstri(char *arg1, char *arg2);
extern HMODULE g_hInst;
using namespace std;


#endif
