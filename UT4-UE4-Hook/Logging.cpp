#include "Logging.h"

#ifdef Logging

ofstream ofile;
char Log[] = { 'u', 't', '4', '.', 'l', 'o', 'g' };
char Appdata[] = { 'A', 'P', 'P', 'D', 'A', 'T', 'A' };
bool g_IsLoggingAlready = false; 

// Create path to file in executable directory (memory must be freed)
char *GetDirectoryFile(char *filename)
{
	char* path = new char[256];
	char dlldir[256] = { 0 };
	GetModuleFileName(g_hInst, dlldir, 256);
	for (int i = strlen(dlldir); i > 0; i--) { if (dlldir[i] == '\\') { dlldir[i + 1] = 0; break; } }
	strcpy_s(path, 256, dlldir);
	strcat_s(path, 256, filename);
	return path;
}

// Create path to file in appdata (memory must be freed)
char *GetAppdataFile(char *filename) 
{
	char* Temp = new char[512];
	char* buf = 0;
	size_t sz = 0;
	_dupenv_s(&buf, &sz, Appdata);
	strcpy_s(Temp, 512, buf);
	strcat_s(Temp, 512, "\\");
	strcat_s(Temp, 512, filename);
	free(buf);
	return Temp;
}

// Write log to file
void __cdecl add_log(const char *fmt, ...)
{
	if (g_IsLoggingAlready) return;

	g_IsLoggingAlready = true;
	char* Temp = GetAppdataFile(Log);
	ofile.open(Temp, ios::app);
	delete[] Temp;
	if (ofile.is_open())
	{
		if (!fmt) return;

		va_list va_alist;
		char logbuf[30000] = { 0 };

		va_start(va_alist, fmt);
		_vsnprintf_s(logbuf + strlen(logbuf), sizeof(logbuf) - strlen(logbuf), 15000, fmt, va_alist);
		va_end(va_alist);

		ofile << logbuf << endl;
	}
	ofile.close();
	g_IsLoggingAlready = false;
}

#endif

char *strstri(char *arg1, char *arg2)
{
	const char *a, *b;
	int size1 = strlen(arg1), size2 = strlen(arg2);
	if (size1 < size2) return (NULL);

	for (; *arg1; *arg1++) {

		a = arg1;
		b = arg2;

		while ((*a++ | 32) == (*b++ | 32))
			if (!*b)
				return (arg1);

	}

	return(NULL);
}