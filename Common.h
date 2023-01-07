#pragma once

// #define TGFEXT       // TGF2, MMF2, MMF2 Dev
   #define MMFEXT       // MMF2, MMF2 Dev
// #define PROEXT       // MMF2 Dev only

#include    "Edif.h"
#include	"Resource.h"
#include <sphinxbase/err.h>
#include <sphinxbase/ad.h>
#include <sphinxbase/cont_ad.h>
#include "pocketsphinx.h"
#include <tinythread.h>
#include <fast_mutex.h>
using namespace tthread;

// edPtr : Used at edittime and saved in the MFA/CCN/EXE files

enum ErrorN
{
	noError,
	voiceNotInitialized,
	voiceRecAlreadyStarted,
	voiceRecInitBlocked,
	voiceInitWrong	
};

static char* ErrorS[100] = {
	"[00] No Error",
	"[01] Voice is not initialized.",
	"[02] Voice Recognition is already started.",
	"[03] Cannot initialize, voice Recognition is working.",
	"[04] Cannot initialize, one language file is wrong or missing."
};

struct EDITDATA
{
	// Header - required
	extHeader		eHeader;

	// Object's data
//	short			swidth;
//	short			sheight;

};

#include "Extension.h"


