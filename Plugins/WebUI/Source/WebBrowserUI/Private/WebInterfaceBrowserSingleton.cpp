// Engine/Source/Runtime/WebBrowser/Private/WebBrowserSingleton.cpp

#include "WebInterfaceBrowserSingleton.h"
#include "Misc/Paths.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "Misc/CommandLine.h"
#include "Misc/ConfigCacheIni.h"
#include "Internationalization/Culture.h"
#include "Misc/App.h"
#include "WebInterfaceBrowserModule.h"
#include "Misc/EngineVersion.h"
#include "Framework/Application/SlateApplication.h"
#include "IWebInterfaceBrowserCookieManager.h"
#include "WebInterfaceBrowserLog.h"

#if PLATFORM_WINDOWS
#include "Windows/WindowsHWrapper.h"
#endif

#if WITH_CEF3
#include "Misc/ScopeLock.h"
#include "CEF/CEFInterfaceBrowserApp.h"
#include "CEF/CEFInterfaceBrowserHandler.h"
#include "CEF/CEFWebInterfaceBrowserWindow.h"
#include "CEF/CEFInterfaceSchemeHandler.h"
#	if PLATFORM_WINDOWS
#		include "Windows/AllowWindowsPlatformTypes.h"
#	endif
#	pragma push_macro("OVERRIDE")
#		undef OVERRIDE // cef headers provide their own OVERRIDE macro
THIRD_PARTY_INCLUDES_START
#if PLATFORM_APPLE
PRAGMA_DISABLE_DEPRECATION_WARNINGS
#endif
#		include "include/cef_app.h"
#if PLATFORM_APPLE
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#endif
THIRD_PARTY_INCLUDES_END
#	pragma pop_macro("OVERRIDE")
#	if PLATFORM_WINDOWS
#		include "Windows/HideWindowsPlatformTypes.h"
#	endif
#endif

#if BUILD_EMBEDDED_APP
#	include "Native/NativeWebInterfaceBrowserProxy.h"
#endif

// Define some platform-dependent file locations
#if WITH_CEF3
#	define CEF3_BIN_DIR TEXT("Binaries/ThirdParty/CEF3")
#	if PLATFORM_WINDOWS && PLATFORM_64BITS
#		define CEF3_RESOURCES_DIR CEF3_BIN_DIR TEXT("/Win64/Resources")
#		define CEF3_SUBPROCES_EXE TEXT("Binaries/Win64/UnrealCEFSubProcess.exe")
#	elif PLATFORM_WINDOWS && PLATFORM_32BITS
#		define CEF3_RESOURCES_DIR CEF3_BIN_DIR TEXT("/Win32/Resources")
#		define CEF3_SUBPROCES_EXE TEXT("Binaries/Win32/UnrealCEFSubProcess.exe")
#	elif PLATFORM_MAC
#		define CEF3_FRAMEWORK_DIR CEF3_BIN_DIR TEXT("/Mac/Chromium Embedded Framework.framework")
#		define CEF3_RESOURCES_DIR CEF3_FRAMEWORK_DIR TEXT("/Resources")
#		define CEF3_SUBPROCES_EXE TEXT("Binaries/Mac/UnrealCEFSubProcess.app/Contents/MacOS/UnrealCEFSubProcess")
#	elif PLATFORM_LINUX // @todo Linux
#		define CEF3_RESOURCES_DIR CEF3_BIN_DIR TEXT("/Linux/Resources")
#		define CEF3_SUBPROCES_EXE TEXT("Binaries/Linux/UnrealCEFSubProcess")
#	endif
	// Caching is enabled by default.
#	ifndef CEF3_DEFAULT_CACHE
#		define CEF3_DEFAULT_CACHE 1
#	endif
#endif

namespace {

	/**
	 * Helper function to set the current thread name, visible by the debugger.
	 * @param ThreadName	Name to set
	 */
	void SetCurrentThreadName(char* ThreadName)
	{
#if PLATFORM_MAC
		pthread_setname_np(ThreadName);
#elif PLATFORM_LINUX
		pthread_setname_np(pthread_self(), ThreadName);
#elif PLATFORM_WINDOWS && !PLATFORM_SEH_EXCEPTIONS_DISABLED
		/**
		 * Code setting the thread name for use in the debugger.
		 * Copied implementation from WindowsRunnableThread as it is private.
		 *
		 * http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
		 */
		const uint32 MS_VC_EXCEPTION=0x406D1388;

		struct THREADNAME_INFO
		{
			uint32 dwType;		// Must be 0x1000.
			LPCSTR szName;		// Pointer to name (in user addr space).
			uint32 dwThreadID;	// Thread ID (-1=caller thread).
			uint32 dwFlags;		// Reserved for future use, must be zero.
		};

		THREADNAME_INFO ThreadNameInfo = {0x1000, ThreadName, (uint32)-1, 0};

		__try
		{
			RaiseException( MS_VC_EXCEPTION, 0, sizeof(ThreadNameInfo)/sizeof(ULONG_PTR), (ULONG_PTR*)&ThreadNameInfo );
		}
		__except( EXCEPTION_EXECUTE_HANDLER )
		CA_SUPPRESS(6322)
		{
		}
#endif
	}
}

FString FWebInterfaceBrowserSingleton::ApplicationCacheDir() const
{
#if PLATFORM_MAC
	// OSX wants caches in a separate location from other app data
	static TCHAR Result[MAC_MAX_PATH] = TEXT("");
	if (!Result[0])
	{
		SCOPED_AUTORELEASE_POOL;
		NSString *CacheBaseDir = [NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES) objectAtIndex: 0];
		NSString* BundleID = [[NSBundle mainBundle] bundleIdentifier];
		if(!BundleID)
		{
			BundleID = [[NSProcessInfo processInfo] processName];
		}
		check(BundleID);

		NSString* AppCacheDir = [CacheBaseDir stringByAppendingPathComponent: BundleID];
		FPlatformString::CFStringToTCHAR((CFStringRef)AppCacheDir, Result);
	}
	return FString(Result);
#else
	// Other platforms use the application data directory
	return FPaths::ProjectSavedDir();
#endif
}


PRAGMA_DISABLE_DEPRECATION_WARNINGS
class FWebInterfaceBrowserWindowFactory
	: public IWebInterfaceBrowserWindowFactory
{
public:

	virtual ~FWebInterfaceBrowserWindowFactory()
	{ }

	virtual TSharedPtr<IWebInterfaceBrowserWindow> Create(
		TSharedPtr<FCEFWebInterfaceBrowserWindow>& BrowserWindowParent,
		TSharedPtr<FWebInterfaceBrowserWindowInfo>& BrowserWindowInfo) override
	{
		return IWebInterfaceBrowserModule::Get().GetSingleton()->CreateBrowserWindow(
			BrowserWindowParent,
			BrowserWindowInfo);
	}

	virtual TSharedPtr<IWebInterfaceBrowserWindow> Create(
		void* OSWindowHandle,
		FString InitialURL,
		bool bUseTransparency,
		bool bThumbMouseButtonNavigation,
		TOptional<FString> ContentsToLoad = TOptional<FString>(),
		bool ShowErrorMessage = true,
		FColor BackgroundColor = FColor(255, 255, 255, 255)) override
	{
		return IWebInterfaceBrowserModule::Get().GetSingleton()->CreateBrowserWindow(
			OSWindowHandle,
			InitialURL,
			bUseTransparency,
			bThumbMouseButtonNavigation,
			ContentsToLoad,
			ShowErrorMessage,
			BackgroundColor);
	}
};
PRAGMA_ENABLE_DEPRECATION_WARNINGS

class FNoWebInterfaceBrowserWindowFactory
	: public IWebInterfaceBrowserWindowFactory
{
public:

	virtual ~FNoWebInterfaceBrowserWindowFactory()
	{ }

	virtual TSharedPtr<IWebInterfaceBrowserWindow> Create(
		TSharedPtr<FCEFWebInterfaceBrowserWindow>& BrowserWindowParent,
		TSharedPtr<FWebInterfaceBrowserWindowInfo>& BrowserWindowInfo) override
	{
		return nullptr;
	}

	virtual TSharedPtr<IWebInterfaceBrowserWindow> Create(
		void* OSWindowHandle,
		FString InitialURL,
		bool bUseTransparency,
		bool bThumbMouseButtonNavigation,
		TOptional<FString> ContentsToLoad = TOptional<FString>(),
		bool ShowErrorMessage = true,
		FColor BackgroundColor = FColor(255, 255, 255, 255)) override
	{
		return nullptr;
	}
};

FWebInterfaceBrowserSingleton::FWebInterfaceBrowserSingleton(const FWebInterfaceBrowserInitSettings& WebBrowserInitSettings)
#if WITH_CEF3
	: WebBrowserWindowFactory(MakeShareable(new FWebInterfaceBrowserWindowFactory()))
#else
	: WebBrowserWindowFactory(MakeShareable(new FNoWebInterfaceBrowserWindowFactory()))
#endif
	, bDevToolsShortcutEnabled(UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG)
	, bJSBindingsToLoweringEnabled(true)
	, DefaultMaterial(nullptr)
	, DefaultTranslucentMaterial(nullptr)
{
#if WITH_CEF3

	// Only enable CEF if we have CEF3, we are not running a commandlet without rendering (e.g. cooking assets) and it has not been explicitly disabled
	bAllowCEF = (!IsRunningCommandlet() || IsAllowCommandletRendering()) && !FParse::Param(FCommandLine::Get(), TEXT("nocef"));
	if (bAllowCEF)
	{
		// The FWebBrowserSingleton must be initialized on the game thread
		check(IsInGameThread());

		// Provide CEF with command-line arguments.
#if PLATFORM_WINDOWS
		CefMainArgs MainArgs(hInstance);
#else
		CefMainArgs MainArgs;
#endif

		bool bWebGL = FParse::Param(FCommandLine::Get(), TEXT("webgl"));
		bool bVerboseLogging = FParse::Param(FCommandLine::Get(), TEXT("cefverbose")) || FParse::Param(FCommandLine::Get(), TEXT("debuglog"));
		// CEFBrowserApp implements application-level callbacks.
		CEFBrowserApp = new FCEFInterfaceBrowserApp(bWebGL);
		CEFBrowserApp->OnRenderProcessThreadCreated().BindRaw(this, &FWebInterfaceBrowserSingleton::HandleRenderProcessCreated);

		// Specify CEF global settings here.
		CefSettings Settings;
		Settings.no_sandbox = true;
		Settings.command_line_args_disabled = true;
#if !PLATFORM_LINUX
		Settings.enable_net_security_expiration = true;
		Settings.external_message_pump = true;
#endif
		//@todo change to threaded version instead of using external_message_pump & OnScheduleMessagePumpWork
		Settings.multi_threaded_message_loop = false;
	

		FString CefLogFile(FPaths::Combine(*FPaths::ProjectLogDir(), TEXT("cef3.log")));
		CefLogFile = FPaths::ConvertRelativePathToFull(CefLogFile);
		CefString(&Settings.log_file) = TCHAR_TO_WCHAR(*CefLogFile);
		Settings.log_severity = bVerboseLogging ? LOGSEVERITY_VERBOSE : LOGSEVERITY_WARNING;

		uint16 DebugPort;
		if(FParse::Value(FCommandLine::Get(), TEXT("cefdebug="), DebugPort))
		{
			Settings.remote_debugging_port = DebugPort;
		}

		// Specify locale from our settings
		FString LocaleCode = GetCurrentLocaleCode();
		CefString(&Settings.locale) = TCHAR_TO_WCHAR(*LocaleCode);

		// Append engine version to the user agent string.
		CefString(&Settings.product_version) = TCHAR_TO_WCHAR(*WebBrowserInitSettings.ProductVersion);

#if CEF3_DEFAULT_CACHE
		// Enable on disk cache
		FString CachePath(FPaths::Combine(ApplicationCacheDir(), TEXT("webcache")));
		CachePath = FPaths::ConvertRelativePathToFull(CachePath);
		CefString(&Settings.cache_path) = TCHAR_TO_WCHAR(*CachePath);
#endif

		// Specify path to resources
		FString ResourcesPath(FPaths::Combine(*FPaths::EngineDir(), CEF3_RESOURCES_DIR));
		ResourcesPath = FPaths::ConvertRelativePathToFull(ResourcesPath);
		if (!FPaths::DirectoryExists(ResourcesPath))
		{
			UE_LOG(LogWebInterfaceBrowser, Error, TEXT("Chromium Resources information not found at: %s."), *ResourcesPath);
		}
		CefString(&Settings.resources_dir_path) = TCHAR_TO_WCHAR(*ResourcesPath);

#if !PLATFORM_MAC
		// On Mac Chromium ignores custom locales dir. Files need to be stored in Resources folder in the app bundle
		FString LocalesPath(FPaths::Combine(*ResourcesPath, TEXT("locales")));
		LocalesPath = FPaths::ConvertRelativePathToFull(LocalesPath);
		if (!FPaths::DirectoryExists(LocalesPath))
		{
			UE_LOG(LogWebInterfaceBrowser, Error, TEXT("Chromium Locales information not found at: %s."), *LocalesPath);
		}
		CefString(&Settings.locales_dir_path) = TCHAR_TO_WCHAR(*LocalesPath);
#else
		// LocaleCode may contain region, which for some languages may make CEF unable to find the locale pak files
		// In that case use the language name for CEF locale
		FString LocalePakPath = ResourcesPath + TEXT("/") + LocaleCode.Replace(TEXT("-"), TEXT("_")) + TEXT(".lproj/locale.pak");
		if (!FPaths::FileExists(LocalePakPath))
		{
			FCultureRef Culture = FInternationalization::Get().GetCurrentCulture();
			LocaleCode = Culture->GetTwoLetterISOLanguageName();
			LocalePakPath = ResourcesPath + TEXT("/") + LocaleCode + TEXT(".lproj/locale.pak");
			if (FPaths::FileExists(LocalePakPath))
			{
				CefString(&Settings.locale) = TCHAR_TO_WCHAR(*LocaleCode);
			}
		}
#endif

		// Specify path to sub process exe
		FString SubProcessPath(FPaths::Combine(*FPaths::EngineDir(), CEF3_SUBPROCES_EXE));
		SubProcessPath = FPaths::ConvertRelativePathToFull(SubProcessPath);

		if (!IPlatformFile::GetPlatformPhysical().FileExists(*SubProcessPath))
		{
			UE_LOG(LogWebInterfaceBrowser, Error, TEXT("UnrealCEFSubProcess.exe not found, check that this program has been built and is placed in: %s."), *SubProcessPath);
		}
		CefString(&Settings.browser_subprocess_path) = TCHAR_TO_WCHAR(*SubProcessPath);

		// Initialize CEF.
		bool bSuccess = CefInitialize(MainArgs, Settings, CEFBrowserApp.get(), nullptr);
		check(bSuccess);

		// Set the thread name back to GameThread.
		SetCurrentThreadName(TCHAR_TO_ANSI( *(FName( NAME_GameThread ).GetPlainNameString()) ));

		DefaultCookieManager = FCefWebInterfaceCookieManagerFactory::Create(CefCookieManager::GetGlobalManager(nullptr));
	}
#endif
}

#if WITH_CEF3
void FWebInterfaceBrowserSingleton::HandleRenderProcessCreated(CefRefPtr<CefListValue> ExtraInfo)
{
	FScopeLock Lock(&WindowInterfacesCS);
	for (int32 Index = WindowInterfaces.Num() - 1; Index >= 0; --Index)
	{
		TSharedPtr<FCEFWebInterfaceBrowserWindow> BrowserWindow = WindowInterfaces[Index].Pin();
		if (BrowserWindow.IsValid())
		{
			CefRefPtr<CefDictionaryValue> Bindings = BrowserWindow->GetProcessInfo();
			if (Bindings.get())
			{
				ExtraInfo->SetDictionary(ExtraInfo->GetSize(), Bindings);
			}
		}
	}
}
#endif

FWebInterfaceBrowserSingleton::~FWebInterfaceBrowserSingleton()
{
#if WITH_CEF3
	if (bAllowCEF)
	{
		{
			FScopeLock Lock(&WindowInterfacesCS);
			// Force all existing browsers to close in case any haven't been deleted
			for (int32 Index = 0; Index < WindowInterfaces.Num(); ++Index)
			{
				auto BrowserWindow = WindowInterfaces[Index].Pin();
				if (BrowserWindow.IsValid() && BrowserWindow->IsValid())
				{
					// Call CloseBrowser directly on the Host object as FWebBrowserWindow::CloseBrowser is delayed
					BrowserWindow->InternalCefBrowser->GetHost()->CloseBrowser(true);
				}
			}
			// Clear this before CefShutdown() below
			WindowInterfaces.Reset();
		}

		// Remove references to the scheme handler factories
		CefClearSchemeHandlerFactories();
		for (const TPair<FString, CefRefPtr<CefRequestContext>>& RequestContextPair : RequestContexts)
		{
			RequestContextPair.Value->ClearSchemeHandlerFactories();
		}
		// Clear this before CefShutdown() below
		RequestContexts.Reset();
		// Just in case, although we deallocate CEFBrowserApp right after this.
		CEFBrowserApp->OnRenderProcessThreadCreated().Unbind();
		// CefRefPtr takes care of delete
		CEFBrowserApp = nullptr;
		// Shut down CEF.
		CefShutdown();
	}
#endif
}

TSharedRef<IWebInterfaceBrowserWindowFactory> FWebInterfaceBrowserSingleton::GetWebBrowserWindowFactory() const
{
	return WebBrowserWindowFactory;
}

TSharedPtr<IWebInterfaceBrowserWindow> FWebInterfaceBrowserSingleton::CreateBrowserWindow(
	TSharedPtr<FCEFWebInterfaceBrowserWindow>& BrowserWindowParent,
	TSharedPtr<FWebInterfaceBrowserWindowInfo>& BrowserWindowInfo
	)
{
#if WITH_CEF3
	if (bAllowCEF)
	{
		TOptional<FString> ContentsToLoad;

		bool bShowErrorMessage = BrowserWindowParent->IsShowingErrorMessages();
		bool bThumbMouseButtonNavigation = BrowserWindowParent->IsThumbMouseButtonNavigationEnabled();
		bool bUseTransparency = BrowserWindowParent->UseTransparency();
		bool bUseNativeCursors = BrowserWindowParent->UseNativeCursors();
		FString InitialURL = WCHAR_TO_TCHAR(BrowserWindowInfo->Browser->GetMainFrame()->GetURL().ToWString().c_str());
		TSharedPtr<FCEFWebInterfaceBrowserWindow> NewBrowserWindow(new FCEFWebInterfaceBrowserWindow(BrowserWindowInfo->Browser, BrowserWindowInfo->Handler, InitialURL, ContentsToLoad, bShowErrorMessage, bThumbMouseButtonNavigation, bUseTransparency, bUseNativeCursors, bJSBindingsToLoweringEnabled));
		BrowserWindowInfo->Handler->SetBrowserWindow(NewBrowserWindow);
		{
			FScopeLock Lock(&WindowInterfacesCS);
			WindowInterfaces.Add(NewBrowserWindow);
		}
		NewBrowserWindow->GetCefBrowser()->GetHost()->SetWindowlessFrameRate(BrowserWindowParent->GetCefBrowser()->GetHost()->GetWindowlessFrameRate());
		return NewBrowserWindow;
	}
#endif
	return nullptr;
}

TSharedPtr<IWebInterfaceBrowserWindow> FWebInterfaceBrowserSingleton::CreateBrowserWindow(
	void* OSWindowHandle,
	FString InitialURL,
	bool bUseTransparency,
	bool bThumbMouseButtonNavigation,
	TOptional<FString> ContentsToLoad,
	bool ShowErrorMessage,
	FColor BackgroundColor,
	int BrowserFrameRate,
	const TArray<FString>& AltRetryDomains)
{
	FCreateInterfaceBrowserWindowSettings Settings;
	Settings.OSWindowHandle = OSWindowHandle;
	Settings.InitialURL = InitialURL;
	Settings.bUseTransparency = false;// bUseTransparency;
	Settings.bThumbMouseButtonNavigation = bThumbMouseButtonNavigation;
	Settings.ContentsToLoad = ContentsToLoad;
	Settings.bShowErrorMessage = ShowErrorMessage;
	Settings.BackgroundColor = FColor::Black;// BackgroundColor;
	Settings.BrowserFrameRate = BrowserFrameRate;
	Settings.AltRetryDomains = AltRetryDomains;

	return CreateBrowserWindow(Settings);
}

TSharedPtr<IWebInterfaceBrowserWindow> FWebInterfaceBrowserSingleton::CreateBrowserWindow(const FCreateInterfaceBrowserWindowSettings& WindowSettings)
{
	bool bBrowserEnabled = true;
	GConfig->GetBool(TEXT("Browser"), TEXT("bEnabled"), bBrowserEnabled, GEngineIni);
	if (!bBrowserEnabled || !FApp::CanEverRender())
	{
		return nullptr;
	}

#if WITH_CEF3
	if (bAllowCEF)
	{
		// Information used when creating the native window.
		CefWindowInfo WindowInfo;

		// Specify CEF browser settings here.
		CefBrowserSettings BrowserSettings;

		// Set max framerate to maximum supported.
		BrowserSettings.background_color = CefColorSetARGB(WindowSettings.bUseTransparency ? 0 : WindowSettings.BackgroundColor.A, WindowSettings.BackgroundColor.R, WindowSettings.BackgroundColor.G, WindowSettings.BackgroundColor.B);

		// Disable plugins
		BrowserSettings.plugins = STATE_DISABLED;


#if PLATFORM_WINDOWS
		// Create the widget as a child window on windows when passing in a parent window
		if (WindowSettings.OSWindowHandle != nullptr)
		{
			RECT ClientRect = { 0, 0, 0, 0 };
			WindowInfo.SetAsChild((CefWindowHandle)WindowSettings.OSWindowHandle, ClientRect);
		}
		else
#endif
		{
			// Use off screen rendering so we can integrate with our windows
#if PLATFORM_LINUX
			WindowInfo.SetAsWindowless(kNullWindowHandle, WindowSettings.bUseTransparency);
#else
			WindowInfo.SetAsWindowless(kNullWindowHandle);
#endif
			BrowserSettings.windowless_frame_rate = WindowSettings.BrowserFrameRate;
		}


		// WebBrowserHandler implements browser-level callbacks.
		CefRefPtr<FCEFInterfaceBrowserHandler> NewHandler(new FCEFInterfaceBrowserHandler(WindowSettings.bUseTransparency, WindowSettings.AltRetryDomains));

		CefRefPtr<CefRequestContext> RequestContext = nullptr;
		if (WindowSettings.Context.IsSet())
		{
			const FInterfaceBrowserContextSettings Context = WindowSettings.Context.GetValue();
			const CefRefPtr<CefRequestContext>* ExistingRequestContext = RequestContexts.Find(Context.Id);

			if (ExistingRequestContext == nullptr)
			{
				CefRequestContextSettings RequestContextSettings;
				CefString(&RequestContextSettings.accept_language_list) = TCHAR_TO_WCHAR(*Context.AcceptLanguageList);
				CefString(&RequestContextSettings.cache_path) = TCHAR_TO_WCHAR(*Context.CookieStorageLocation);
				RequestContextSettings.persist_session_cookies = Context.bPersistSessionCookies;
				RequestContextSettings.ignore_certificate_errors = Context.bIgnoreCertificateErrors;
#if !PLATFORM_LINUX
				RequestContextSettings.enable_net_security_expiration = Context.bEnableNetSecurityExpiration;
#endif

				//Create a new one
				RequestContext = CefRequestContext::CreateContext(RequestContextSettings, nullptr);
				RequestContexts.Add(Context.Id, RequestContext);
			}
			else
			{
				RequestContext = *ExistingRequestContext;
			}
			SchemeHandlerFactories.RegisterFactoriesWith(RequestContext);
		}

		// Create the CEF browser window.
		CefRefPtr<CefBrowser> Browser = CefBrowserHost::CreateBrowserSync(WindowInfo, NewHandler.get(), TCHAR_TO_WCHAR(*WindowSettings.InitialURL), BrowserSettings, RequestContext);
		if (Browser.get())
		{
			// Create new window
			TSharedPtr<FCEFWebInterfaceBrowserWindow> NewBrowserWindow = MakeShareable(new FCEFWebInterfaceBrowserWindow(
				Browser,
				NewHandler,
				WindowSettings.InitialURL,
				WindowSettings.ContentsToLoad,
				WindowSettings.bShowErrorMessage,
				WindowSettings.bThumbMouseButtonNavigation,
				WindowSettings.bUseTransparency,
				WindowSettings.bUseNativeCursors,
				bJSBindingsToLoweringEnabled));
			NewHandler->SetBrowserWindow(NewBrowserWindow);
			{
				FScopeLock Lock(&WindowInterfacesCS);
				WindowInterfaces.Add(NewBrowserWindow);
			}

			return NewBrowserWindow;
		}
	}
#endif
	return nullptr;
}

#if BUILD_EMBEDDED_APP
TSharedPtr<IWebInterfaceBrowserWindow> FWebInterfaceBrowserSingleton::CreateNativeBrowserProxy()
{
	TSharedPtr<FNativeWebInterfaceBrowserProxy> NewBrowserWindow = MakeShareable(new FNativeWebInterfaceBrowserProxy(
		bJSBindingsToLoweringEnabled
	));
	NewBrowserWindow->Initialize();
	return NewBrowserWindow;
}
#endif //BUILD_EMBEDDED_APP

bool FWebInterfaceBrowserSingleton::Tick(float DeltaTime)
{
    QUICK_SCOPE_CYCLE_COUNTER(STAT_FWebInterfaceBrowserSingleton_Tick);

#if WITH_CEF3
	if (bAllowCEF)
	{
		{
			FScopeLock Lock(&WindowInterfacesCS);
			bool bIsSlateAwake = FSlateApplication::IsInitialized() && !FSlateApplication::Get().IsSlateAsleep();
			// Remove any windows that have been deleted and check whether it's currently visible
			for (int32 Index = WindowInterfaces.Num() - 1; Index >= 0; --Index)
			{
				if (!WindowInterfaces[Index].IsValid())
				{
					WindowInterfaces.RemoveAt(Index);
				}
				else if (bIsSlateAwake) // only check for Tick activity if Slate is currently ticking
				{
					TSharedPtr<FCEFWebInterfaceBrowserWindow> BrowserWindow = WindowInterfaces[Index].Pin();
					if(BrowserWindow.IsValid())
					{
						// Test if we've ticked recently. If not assume the browser window has become hidden.
						BrowserWindow->CheckTickActivity();
					}
				}
			}
		}

		bool bForceMessageLoop = false;
		GConfig->GetBool(TEXT("Browser"), TEXT("bForceMessageLoop"), bForceMessageLoop, GEngineIni);
		if (CEFBrowserApp != nullptr)
		{
			// force via config override or if there are active browser windows
			const bool bForce = bForceMessageLoop || WindowInterfaces.Num() > 0;
			// tick the CEF app to determine when to run CefDoMessageLoopWork
			CEFBrowserApp->TickMessagePump(DeltaTime, bForce);
		}

		// Update video buffering for any windows that need it
		for (int32 Index = 0; Index < WindowInterfaces.Num(); Index++)
		{
			if (WindowInterfaces[Index].IsValid())
			{
				TSharedPtr<FCEFWebInterfaceBrowserWindow> BrowserWindow = WindowInterfaces[Index].Pin();
				if (BrowserWindow.IsValid())
				{
					BrowserWindow->UpdateVideoBuffering();
				}
			}
		}
	}
#endif
	return true;
}

FString FWebInterfaceBrowserSingleton::GetCurrentLocaleCode()
{
	FCultureRef Culture = FInternationalization::Get().GetCurrentCulture();
	FString LocaleCode = Culture->GetTwoLetterISOLanguageName();
	FString Country = Culture->GetRegion();
	if (!Country.IsEmpty())
	{
		LocaleCode = LocaleCode + TEXT("-") + Country;
	}
	return LocaleCode;
}

PRAGMA_DISABLE_DEPRECATION_WARNINGS
void FWebInterfaceBrowserSingleton::DeleteBrowserCookies(FString URL, FString CookieName, TFunction<void(int)> Completed)
{
	if (DefaultCookieManager.IsValid())
	{
		DefaultCookieManager->DeleteCookies(URL, CookieName, Completed);
	}
}
PRAGMA_ENABLE_DEPRECATION_WARNINGS

TSharedPtr<IWebInterfaceBrowserCookieManager> FWebInterfaceBrowserSingleton::GetCookieManager(TOptional<FString> ContextId) const
{
	if (ContextId.IsSet())
	{
#if WITH_CEF3
		if (bAllowCEF)
		{
			const CefRefPtr<CefRequestContext>* ExistingContext = RequestContexts.Find(ContextId.GetValue());

			if (ExistingContext)
			{
				// Cache these cookie managers?
				return FCefWebInterfaceCookieManagerFactory::Create((*ExistingContext)->GetDefaultCookieManager(nullptr));
			}
			else
			{
				UE_LOG(LogWebInterfaceBrowser, Log, TEXT("No cookie manager for ContextId=%s.  Using default cookie manager"), *ContextId.GetValue());
			}
		}
#endif
	}
	// No ContextId or cookie manager instance associated with it.  Use default
	return DefaultCookieManager;
}

bool FWebInterfaceBrowserSingleton::RegisterContext(const FInterfaceBrowserContextSettings& Settings)
{
#if WITH_CEF3
	if (bAllowCEF)
	{
		const CefRefPtr<CefRequestContext>* ExistingContext = RequestContexts.Find(Settings.Id);

		if (ExistingContext != nullptr)
		{
			// You can't register the same context twice and
			// you can't update the settings for a context that already exists
			return false;
		}

		CefRequestContextSettings RequestContextSettings;
		CefString(&RequestContextSettings.accept_language_list) = TCHAR_TO_WCHAR(*Settings.AcceptLanguageList);
		CefString(&RequestContextSettings.cache_path) = TCHAR_TO_WCHAR(*Settings.CookieStorageLocation);
		RequestContextSettings.persist_session_cookies = Settings.bPersistSessionCookies;
		RequestContextSettings.ignore_certificate_errors = Settings.bIgnoreCertificateErrors;
#if !PLATFORM_LINUX
		RequestContextSettings.enable_net_security_expiration = Settings.bEnableNetSecurityExpiration;
#endif

		//Create a new one
		CefRefPtr<CefRequestContext> RequestContext = CefRequestContext::CreateContext(RequestContextSettings, nullptr);
		RequestContexts.Add(Settings.Id, RequestContext);
		return true;
	}
#endif
	return false;
}

bool FWebInterfaceBrowserSingleton::UnregisterContext(const FString& ContextId)
{
#if WITH_CEF3
	if (bAllowCEF)
	{
		CefRefPtr<CefRequestContext> Context;
		if (RequestContexts.RemoveAndCopyValue(ContextId, Context))
		{
			Context->ClearSchemeHandlerFactories();
			return true;
		}
	}
#endif
	return false;
}

bool FWebInterfaceBrowserSingleton::RegisterSchemeHandlerFactory(FString Scheme, FString Domain, IWebInterfaceBrowserSchemeHandlerFactory* WebBrowserSchemeHandlerFactory)
{
#if WITH_CEF3
	if (bAllowCEF)
	{
		SchemeHandlerFactories.AddSchemeHandlerFactory(MoveTemp(Scheme), MoveTemp(Domain), WebBrowserSchemeHandlerFactory);
		return true;
	}
#endif
	return false;
}

bool FWebInterfaceBrowserSingleton::UnregisterSchemeHandlerFactory(IWebInterfaceBrowserSchemeHandlerFactory* WebBrowserSchemeHandlerFactory)
{
#if WITH_CEF3
	if (bAllowCEF)
	{
		SchemeHandlerFactories.RemoveSchemeHandlerFactory(WebBrowserSchemeHandlerFactory);
		return true;
	}
#endif
	return false;
}

// Cleanup macros to avoid having them leak outside this source file
#undef CEF3_BIN_DIR
#undef CEF3_FRAMEWORK_DIR
#undef CEF3_RESOURCES_DIR
#undef CEF3_SUBPROCES_EXE
