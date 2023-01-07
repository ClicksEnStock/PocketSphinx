
#include "Common.h"

bool Extension:: WordsRecognized()
{
	return (strcmp(GetRecognizedWords(),"")>0);
}

bool Extension:: IsInitialized()
{
	return GetIsVoiceInitialized();
}

bool Extension:: IsRecording()
{
	return GetIsRecording();
}

bool Extension:: OnError()
{
	return (strcmp(GetLastError(),ErrorS[noError])>0);
}

