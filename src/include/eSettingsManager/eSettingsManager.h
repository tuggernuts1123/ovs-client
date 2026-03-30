#pragma once
#include <string>
#include "IniReader/IniReader.h"

class eSettingsManager {
public:
    void Init();

public:
    // Settings

    bool bEnableKeyboardHotkeys;

    // Debug
    bool bEnableConsoleWindow;
    bool bPauseOnStart;
    int	iLogLevel;
    bool bDebug;
    bool bAllowNonMVS;


    // Toggles
    bool bSunsetDate;
    bool bDisableSignatureCheck;
    bool bHookUE;
    bool bDialog;
    bool bNotifs;

    // Addresses

    // Patterns
    std::string pSigCheck;
    std::string pEndpointLoader;
    std::string pProdEndpointLoader;
    std::string pSunsetDate;
    std::string pFText;
    std::string pCFName;
    std::string pWCFname;
    std::string pDialog;
    std::string pDialogParams;
    std::string pFighterInstance;
    std::string pDialogCallback;
    std::string pQuitGameCallback;
    std::string	pNotifs;


    // Menu Section
    std::string hkMenu;
    int iVKMenuToggle;

    //Other
    int iLogSize;
    bool FORCE_CHECK_VER = false;
    std::string szGameVer;
    std::string szModLoader;
    std::string szAntiCheatEngine;
    std::string szCurlSetOpt;
    std::string szCurlPerform;

    //Private Server
    std::wstring szServerUrl;
    bool bEnableServerProxy;
    // WB
    std::wstring szProdServerUrl;
    bool bEnableProdServerProxy;


    eSettingsManager()
    {
        // Set default values for settings
        bEnableKeyboardHotkeys = true;
        bEnableConsoleWindow = true;
        bPauseOnStart = false;
        iLogLevel = 0;
        bDebug = false;
        bAllowNonMVS = false;
        bSunsetDate = true;
        bDisableSignatureCheck = true;
        bHookUE = true;
        bDialog = true;
        bNotifs = true;
        hkMenu = "F1";
        iVKMenuToggle = 0x70; // VK_F1
        iLogSize = 50;
        bEnableServerProxy = true;
        bEnableProdServerProxy = true;

        pSigCheck = "";
        pEndpointLoader = "";
        pProdEndpointLoader = "";
        pSunsetDate = "";
        pFText = "";
        pCFName = "";
        pWCFname = "";
        pDialog = "";
        pDialogParams = "";
        pFighterInstance = "";
        pDialogCallback = "";
        pQuitGameCallback = "";
        pNotifs = "";

        szGameVer = "";
        szModLoader = "";
        szAntiCheatEngine = "";
        szCurlSetOpt = "";
        szCurlPerform = "";

        szServerUrl = L"";
        szProdServerUrl = L"";
    }

};

class eFirstRunManager
{
public:
    void Save();

public:
    bool bPaidModWarned = false;

private:
    CIniReader* ini;

public:
    eFirstRunManager() {
        ini = nullptr;
    }
    ~eFirstRunManager()
    {
        if (ini)
        {
            delete ini;
        }
    }

    void Init()
    {
        if (nullptr == ini)
        {
            ini = new CIniReader("OVSState.ini");
        }
        bPaidModWarned = ini->ReadBoolean("FirstRun", "PaidModWarned", false);
    }
};

class eCachedPatternsManager
{
private:
    char* Hash = nullptr;
    CIniReader* ini;
    static __int64 GameAddr;

public:
    void Init(uint64_t Hash, const char* version);
    void Save(char *key, uint64_t offset);
    uint64_t eCachedPatternsManager::Load(char* key);
    
    ~eCachedPatternsManager() { if (Hash) delete[] Hash; }
};

extern eSettingsManager*		SettingsMgr;
extern eFirstRunManager*		FirstRunMgr;
extern eCachedPatternsManager*	CachedPatternsMgr;