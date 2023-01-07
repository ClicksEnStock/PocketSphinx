#include "Common.h"

//static FILE* rawfd;
//
//static int32
//ad_file_read(ad_rec_t * ad, int16 * buf, int32 max)
//{
//    size_t nread;
//    
//    nread = fread(buf, sizeof(int16), max, rawfd);
//    
//    return (nread > 0 ? nread : -1);
//}
//
//static void
//print_word_times(int32 start)
//{
//	ps_seg_t *iter = ps_seg_iter(ps, NULL);
//	while (iter != NULL) {
//		int32 sf, ef, pprob;
//		float conf;
//		
//		ps_seg_frames (iter, &sf, &ef);
//		pprob = ps_seg_prob (iter, NULL, NULL, NULL);
//		conf = logmath_exp(ps_get_logmath(ps), pprob);
//		printf ("%s %f %f %f\n", ps_seg_word (iter), (sf + start) / 100.0, (ef + start) / 100.0, conf);
//		iter = ps_seg_next (iter);
//	}
//}
//
//static void
//recognize_from_file() {
//    cont_ad_t *cont;
//    ad_rec_t file_ad = {0};
//    int16 adbuf[4096];
//    const char* hyp;
//    const char* uttid;
//    int32 k, ts, start;
//
//    char waveheader[44];
//    if ((rawfd = fopen(cmd_ln_str_r(config, "-infile"), "rb")) == NULL) {
//	E_FATAL_SYSTEM("Failed to open file '%s' for reading", 
//			cmd_ln_str_r(config, "-infile"));
//    }
//    
//    fread(waveheader, 1, 44, rawfd);
//
//    file_ad.sps = (int32)cmd_ln_float32_r(config, "-samprate");
//    file_ad.bps = sizeof(int16);
//
//    if ((cont = cont_ad_init(&file_ad, ad_file_read)) == NULL) {
//        E_FATAL("Failed to initialize voice activity detection\n");
//    }
//
//    if (cont_ad_calib(cont) < 0)
//        E_INFO("Using default voice activity detection\n");
//    rewind (rawfd);
//
//    for (;;) {
//
//	while ((k = cont_ad_read(cont, adbuf, 4096)) == 0);
//	
//        if (k < 0) {
//    	    break;
//    	}
//
//        if (ps_start_utt(ps, NULL) < 0)
//            E_FATAL("ps_start_utt() failed\n");
//
//        ps_process_raw(ps, adbuf, k, FALSE, FALSE);
//        
//        ts = cont->read_ts;
//        start = ((ts - k) * 100.0) / file_ad.sps;
//        
//        for (;;) {
//            if ((k = cont_ad_read(cont, adbuf, 4096)) < 0)
//            	break;
//
//            if (k == 0) {
//                /*
//                 * No speech data available; check current timestamp with most recent
//                 * speech to see if more than 1 sec elapsed.  If so, end of utterance.
//                 */
//                if ((cont->read_ts - ts) > DEFAULT_SAMPLES_PER_SEC)
//                    break;
//            }
//            else {
//                /* New speech data received; note current timestamp */
//                ts = cont->read_ts;
//            }
//
//
//            ps_process_raw(ps, adbuf, k, FALSE, FALSE);
//        }
//
//        ps_end_utt(ps);
//        
//        if (cmd_ln_boolean_r(config, "-time")) {
//	    print_word_times(start);
//	} else {
//	    hyp = ps_get_hyp(ps, NULL, &uttid);
//            printf("%s: %s\n", uttid, hyp);
//        }
//        fflush(stdout);	
//    }
//
//    cont_ad_close(cont);
//    fclose(rawfd);
//}

void InitGrammarConfig(LPVOID lpParam)
{
	Extension* ext;
	ext = ((Extension*)lpParam);

	ext->config = cmd_ln_init(NULL, ps_args(), TRUE,
                             "-hmm",  ext->acModelFolder,
                             "-jsgf", ext->gramOrLmFile,
                             "-dict", ext->dicFile,
                             NULL);
	if(ext->config != NULL)
	{
		ext->ps = ps_init(ext->config);
		if(ext->ps != NULL)
		{
			ext->SetIsVoiceInitialized(true);
		}
		else
		{
			ext->SetLastError(ErrorS[voiceInitWrong]);
		}
	}
	else
	{
		ext->SetLastError(ErrorS[voiceInitWrong]);
	}
}

void InitLanguageConfig(LPVOID lpParam)
{
	Extension* ext;
	ext = ((Extension*)lpParam);

	ext->config = cmd_ln_init(NULL, ps_args(), TRUE,
                             "-hmm",  ext->acModelFolder,
                             "-lm",   ext->gramOrLmFile,
                             "-dict", ext->dicFile,
							 "-rawlogdir","./",
							 "-logfn","./ps.log",
							 //"-Dfile.encoding","UTF-8",
							 NULL);
	 if(ext->config != NULL)
	 {
		 ext->ps = ps_init(ext->config);
		 if(ext->ps != NULL)
		 {
			ext->SetIsVoiceInitialized(true);
		 }
		 else
		 {
			ext->SetLastError(ErrorS[voiceInitWrong]);
		 }
	 }
	 else
	 {
		 ext->SetLastError(ErrorS[voiceInitWrong]);
	 }
}

void Extension::InitWithGrammar(const TCHAR* acModelFolder,const TCHAR* gramFile,const TCHAR* dicFile)
{
	if(voiceInitThread!=nullptr && voiceInitThread->joinable())
	{
		voiceInitThread->join();
	}
	this->acModelFolder = Runtime.CopyString(acModelFolder);
	this->gramOrLmFile  = Runtime.CopyString(gramFile);
	this->dicFile		= Runtime.CopyString(dicFile);
	SetIsVoiceInitialized(false);
	voiceInitThread = new thread(InitGrammarConfig,this);
}

void Extension::InitWithLanguage(const TCHAR* acModelFolder,const TCHAR* lmFile,const TCHAR* dicFile)
{
	if(voiceInitThread!=nullptr && voiceInitThread->joinable())
	{
		voiceInitThread->join();
	}
	this->acModelFolder = Runtime.CopyString(acModelFolder);
	this->gramOrLmFile  = Runtime.CopyString(lmFile);
	this->dicFile		= Runtime.CopyString(dicFile);
	SetIsVoiceInitialized(false);
	voiceInitThread = new thread(InitLanguageConfig,this);
}

/*
 * Main utterance processing loop:
 *     for (;;) {
 * 	   wait for start of next utterance;
 * 	   decode utterance until silence of at least silenceTime in ms observed;
 *     }
 */
void RecognizeFromMicrophone(LPVOID lpParam)
{

	Extension* ext;
	ext = ((Extension*)lpParam);

    ad_rec_t *ad;
    int16 adbuf[4096];
    int32 k, ts, rem;
    cont_ad_t *cont;

	try
	{
		// -adcdev default or -adcdev sysdefault or -adcdev plughw:0,0
		// with default device
		if ((ad = ad_open_sps(ext->recSampleRate)) == NULL)
		{
			throw new exception("Failed to open audio device\n");
			//E_FATAL("Failed to open audio device\n");
		}
	
		/* Initialize continuous listening module */
		if ((cont = cont_ad_init(ad, ad_read)) == NULL)
		{
			E_FATAL("Failed to initialize voice activity detection\n");
		}
		if (ad_start_rec(ad) < 0)
		{
			E_FATAL("Failed to start recording\n");
		}
		if (cont_ad_calib(cont) < 0)
		{
			E_FATAL("Failed to calibrate voice activity detection\n");
		}

		ext->SetIsRecording(true);

		for (;;) 
		{
			// Exit if aborted
			if (!ext->GetIsVoiceRecognitionEnabled()) {
				break;
			}

			/* Wait data for next utterance */
			while ((k = cont_ad_read(cont, adbuf, 4096)) == 0)
			{
				// Exit if aborted
				if (!ext->GetIsVoiceRecognitionEnabled()) {
					break;
				}
				Sleep(100);
			}

			if (k < 0)
			{
				E_FATAL("Failed to read audio\n");
			}

			if(ext->ps == NULL)
			{
				throw new exception("Cannot process voice, decoder does not exist!\n");
			}

			/*
			 * Non-zero amount of data received; start recognition of new utterance.
			 * NULL argument to uttproc_begin_utt => automatic generation of utterance-id.
			 */
			if (ps_start_utt(ext->ps, NULL) < 0)
			{
				E_FATAL("Failed to start utterance\n");
			}
			ps_process_raw(ext->ps, adbuf, k, FALSE, FALSE);
       
			/* Note timestamp for this first block of data */
			// read_ts gives an absolute timestamp (total no. of raw samples consumed upto the most recent cont_ad_read call, starting from the very beginning) 
			ts = cont->read_ts;

			/* Decode utterance until end (marked by a "long" silence, >1sec) */
			for (;;) 
			{
				/* Read non-silence audio data, if any, from continuous listening module */
				if ((k = cont_ad_read(cont, adbuf, 4096)) < 0)
				{
					E_FATAL("Failed to read audio\n");
				}
				if (k == 0) {
					/*
					 * No speech data available; check current timestamp with most recent
					 * speech to see if more than 1 sec elapsed.  If so, end of utterance.
					 */
					if ((cont->read_ts - ts) > ((ext->recSampleRate/1000)*ext->GetSilenceTime()))
					{
						break;
					}
				}
				else {
					/* New speech data received; note current timestamp */
					ts = cont->read_ts;
				}
			
				ext->ClearRecognizedWords();
				// Decode whatever data was read above.
			   rem = ps_process_raw(ext->ps, adbuf, k, FALSE, FALSE);

				/* If no work to be done, sleep a bit */
				if ((rem == 0) && (k == 0))
					 Sleep(20);
			}

			// Utterance ended; flush any accumulated, unprocessed A/D data and stop
			// listening until current utterance completely decoded
			ext->SetIsRecording(false);
			ad_stop_rec(ad);
			while (ad_read(ad, adbuf, 4096) >= 0)
			{
				if (!ext->GetIsVoiceRecognitionEnabled()) {
					break;
				}
			}
			cont_ad_reset(cont);

			// Finish decoding, obtain result
			ps_end_utt(ext->ps);
			//ext->SetRecognizedWords(ps_get_hyp(ext->ps, NULL, NULL));
			ext->SetRecognizedWords(Edif::ConvertString(ps_get_hyp(ext->ps, NULL, NULL)));
			//hyp = ps_get_hyp(ext->ps, NULL, &uttid);
        
			/* Resume A/D recording for next utterance */
			if (ad_start_rec(ad) < 0)
			{
				E_FATAL("Failed to start recording\n");
			}
			ext->SetIsRecording(true);
		}

		cont_ad_close(cont);
		ad_close(ad);
	}
	catch(exception& ex)
	{
		const char* message = ex.what();
		int i=0;
	}
}

void Extension::StartVoiceRecThread()
{
	if(!GetIsVoiceInitialized())
	{
		SetLastError(ErrorS[voiceNotInitialized]);
	}
	else
	{
		if(voiceRecThread==nullptr || !voiceRecThread->joinable())
		{
			SetIsVoiceRecognitionEnabled(true);
			voiceRecThread = new thread(RecognizeFromMicrophone,this);
		}
		else
		{
			SetLastError(ErrorS[voiceRecAlreadyStarted]);
		}
	}
}

char*	Extension::GetLastError()
{
	lock_guard<fast_mutex> lock(voiceRecMutex);
	return lastError;
}
int		Extension::GetRecSampleRate()
{
	lock_guard<fast_mutex> lock(voiceRecMutex);
	return recSampleRate;
}
int		Extension::GetSilenceTime()
{
	lock_guard<fast_mutex> lock(voiceRecMutex);
	return silenceTime;
}
bool	Extension::GetIsVoiceInitialized()
{
	lock_guard<fast_mutex> lock(voiceRecMutex);
	return isVoiceInitialized;
}
bool	Extension::GetIsRecording()
{
	lock_guard<fast_mutex> lock(voiceRecMutex);
	return isRecording;
}
bool	Extension::GetIsVoiceRecognitionEnabled()
{
	lock_guard<fast_mutex> lock(voiceRecMutex);
	return isVoiceRecognitionEnabled;
}
const TCHAR* Extension::GetRecognizedWords()
{
	lock_guard<fast_mutex> lock(voiceRecMutex);
	return recognizedWords;
}


void	Extension::SetLastError(char* error)
{
	lock_guard<fast_mutex> lock(voiceRecMutex);
	_snprintf_s(lastError, _countof(lastError),_countof(lastError),error);
}
void	Extension::SetRecSampleRate(int sampleRate)
{
	lock_guard<fast_mutex> lock(voiceRecMutex);
	recSampleRate=sampleRate;
}
void	Extension::SetSilenceTime(int silenceT)
{
	lock_guard<fast_mutex> lock(voiceRecMutex);
	silenceTime = silenceT;
}
void	Extension::SetIsVoiceInitialized(bool status)
{
	lock_guard<fast_mutex> lock(voiceRecMutex);
	isVoiceInitialized=status;
}
void	Extension::SetIsRecording(bool status)
{
	lock_guard<fast_mutex> lock(voiceRecMutex);
	isRecording=status;
}
void	Extension::SetIsVoiceRecognitionEnabled(bool status)
{
	lock_guard<fast_mutex> lock(voiceRecMutex);
	isVoiceRecognitionEnabled=status;
}
void	Extension::SetRecognizedWords(const TCHAR* word)
{
	lock_guard<fast_mutex> lock(voiceRecMutex);
	if(word!=nullptr)
	{
		_snprintf_s(recognizedWords, _countof(recognizedWords),4096,word);
	}
}
void	Extension::ClearRecognizedWords()
{
	lock_guard<fast_mutex> lock(voiceRecMutex);
	_snprintf_s(recognizedWords, _countof(recognizedWords),4096,"");
}