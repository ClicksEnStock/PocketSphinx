
#include "Common.h"

const TCHAR* Extension::RecognizedWord()
{
	return GetRecognizedWords();
}

const char* Extension::LastError()
{
	char error[ERRORSIZE];
	strcpy (error,GetLastError());
	SetLastError("[00] No Error");
	return  error;
}