
#include "Common.h"


 void Extension::InitializeWithGrammar(const TCHAR* acModelFolder, const TCHAR* gramFile, const TCHAR* dicFile,int recSampleRate, int silenceTime)
 {
	 if(voiceRecThread==nullptr)
	 {
		InitWithGrammar(acModelFolder, gramFile, dicFile);
		SetRecSampleRate(recSampleRate);
		SetSilenceTime(silenceTime);
	 }
	 else
	 {
		 SetLastError(ErrorS[voiceRecInitBlocked]);
	 }
 }

void  Extension::InitializeWithStatLangModel(const TCHAR* acModelFolder, const TCHAR* lmFile, const TCHAR* dicFile,int recSampleRate, int silenceTime)
{
	if(voiceRecThread==nullptr)
	{
		InitWithLanguage(acModelFolder, lmFile, dicFile);
		SetRecSampleRate(recSampleRate);
		SetSilenceTime(silenceTime);
	}
	else
	{
		SetLastError(ErrorS[voiceRecInitBlocked]);
	}
}

void  Extension::StartVoiceRecognition()
{
	StartVoiceRecThread();
}

void  Extension::StopVoiceRecognition()
{
	SetIsVoiceRecognitionEnabled(false);
}
