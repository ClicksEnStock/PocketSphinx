
class Extension
{
public:

    LPRDATA rdPtr;
    LPRH    rhPtr;

    Edif::Runtime Runtime;

    static const int MinimumBuild = 251;
    static const int Version = 1;

    static const int OEFLAGS = 0;
    static const int OEPREFS = 0;
    
    static const int WindowProcPriority = 100;

    Extension(LPRDATA rdPtr, LPEDATA edPtr, fpcob cobPtr);
    ~Extension();


    /*  Add any data you want to store in your extension to this class
        (eg. what you'd normally store in rdPtr)

        Unlike rdPtr, you can store real C++ objects with constructors
        and destructors, without having to call them manually or store
        a pointer.
    */
	static const int ERRORSIZE = 1000;
    char lastError[ERRORSIZE];

	ps_decoder_t *ps;
	TCHAR* acModelFolder;
	TCHAR* gramOrLmFile;
	TCHAR* dicFile;
	cmd_ln_t *config;
	int recSampleRate;
	int silenceTime;//in ms
	bool isVoiceInitialized;
	bool isRecording;
	bool isVoiceRecognitionEnabled;
	thread* voiceInitThread;
	thread* voiceRecThread;
	fast_mutex voiceRecMutex;
	TCHAR recognizedWords[4096]; 
	
    /*  Add your actions, conditions and expressions as real class member
        functions here. The arguments (and return type for expressions) must
        match EXACTLY what you defined in the JSON.

        Remember to link the actions, conditions and expressions to their
        numeric IDs in the class constructor (Extension.cpp)
    */
	/// Voice recognition Functions
	void InitWithGrammar(const TCHAR* acModelFolder, const TCHAR* gramFile,const TCHAR* dicFile);
	void InitWithLanguage(const TCHAR* acModelFolder, const TCHAR* lmFile, const TCHAR* dicFile);
	void StartVoiceRecThread();

	//Getter/Setter
	char*			GetLastError();
	int				GetRecSampleRate();
	int				GetSilenceTime();
	bool			GetIsVoiceInitialized();
	bool			GetIsRecording();
	bool			GetIsVoiceRecognitionEnabled();
	const TCHAR*	GetRecognizedWords();
	ps_decoder_t *	GetDecoder();

	void	SetLastError(char* error);
	void	SetRecSampleRate(int sampleRate);
	void	SetSilenceTime(int silenceT);
	void	SetIsVoiceInitialized(bool status);
	void	SetIsRecording(bool status);
	void	SetIsVoiceRecognitionEnabled(bool status);
	void	SetRecognizedWords(const TCHAR* word);
	void	Extension::ClearRecognizedWords();
	
    /// Actions
	void InitializeWithGrammar(const TCHAR* acModelFolder, const TCHAR* gramFile,const TCHAR* dicFile,int recSampleRate, int silenceTime);
	void InitializeWithStatLangModel(const TCHAR* acModelFolder, const TCHAR* lmFile, const TCHAR* dicFile,int recSampleRate, int silenceTime);
	void StartVoiceRecognition();
	void StopVoiceRecognition();
	
    /// Conditions

    bool WordsRecognized();
	bool IsInitialized();
	bool IsRecording();
	bool OnError();

    /// Expressions
    const TCHAR* RecognizedWord();
	const char* LastError();

	/* These are called if there's no function linked to an ID */

    void Action(int ID, LPRDATA rdPtr, long param1, long param2);
    long Condition(int ID, LPRDATA rdPtr, long param1, long param2);
    long Expression(int ID, LPRDATA rdPtr, long param);

    /*  These replace the functions like HandleRunObject that used to be
        implemented in Runtime.cpp. They work exactly the same, but they're
        inside the extension class.
    */

    short Handle();
    short Display();

    short Pause();
    short Continue();

    bool Save(HANDLE File);
    bool Load(HANDLE File);

};