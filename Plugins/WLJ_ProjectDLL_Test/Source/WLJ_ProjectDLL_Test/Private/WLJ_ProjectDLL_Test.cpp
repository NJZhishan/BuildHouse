// Copyright Epic Games, Inc. All Rights Reserved.

#include "WLJ_ProjectDLL_Test.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "WLJ_ProjectDLL_TestLibrary/ExampleLibrary.h"

#define LOCTEXT_NAMESPACE "FWLJ_ProjectDLL_TestModule"

void FWLJ_ProjectDLL_TestModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("WLJ_ProjectDLL_Test")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath_ExampleLibrary;
	FString LibraryPath_zlib;  //因为ExampleLibrary.dll库引用了zlib库，所以这里要输出，否则的话会造成无法导入ExampleLibrary.dll
#if PLATFORM_WINDOWS
	LibraryPath_ExampleLibrary = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/WLJ_ProjectDLL_TestLibrary/Win64/ExampleLibrary.dll"));
	LibraryPath_zlib = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/WLJ_ProjectDLL_TestLibrary/Win64/zlib.dll"));
#elif PLATFORM_MAC
    LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/WLJ_ProjectDLL_TestLibrary/Mac/Release/libExampleLibrary.dylib"));
#elif PLATFORM_LINUX
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/WLJ_ProjectDLL_TestLibrary/Linux/x86_64-unknown-linux-gnu/libExampleLibrary.so"));
#endif // PLATFORM_WINDOWS
	ExampleLibraryHandle_zlib = !LibraryPath_zlib.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath_zlib) : nullptr;
	ExampleLibraryHandle_ExampleLibrary = !LibraryPath_ExampleLibrary.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath_ExampleLibrary) : nullptr;
	if (!ExampleLibraryHandle_zlib)
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("zlibError", "Failed to load zlib"));
	}
	if (!ExampleLibraryHandle_ExampleLibrary)
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ProjectDllLibraryError", "Failed to load ProjectDllLibrary"));
	}
}

void FWLJ_ProjectDLL_TestModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Free the dll handle
	FPlatformProcess::FreeDllHandle(ExampleLibraryHandle_ExampleLibrary);
	ExampleLibraryHandle_ExampleLibrary = nullptr;
	FPlatformProcess::FreeDllHandle(ExampleLibraryHandle_zlib);
	ExampleLibraryHandle_zlib = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FWLJ_ProjectDLL_TestModule, WLJ_ProjectDLL_Test)
