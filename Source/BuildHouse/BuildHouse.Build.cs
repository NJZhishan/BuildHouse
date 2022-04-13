// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class BuildHouse : ModuleRules
{
	private string ModulePath
	{
		get
		{
			return ModuleDirectory;
		}
	}

	private string ThirdPartyPath
	{
		get
		{
			return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty"));
		}
	}
	public BuildHouse(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine", "InputCore", "ProceduralMeshComponent", "UMG","HTTP","WLJ_ProjectDLL_TestLibrary","Json" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
		//fbx是用静态库的方式导入的
		PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "fbxSDK/include"));
		PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "fbxSDK/lib", "libfbxsdk-md.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "fbxSDK/lib", "libxml2-md.lib")); 
		PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "fbxSDK/lib", "zlib-md.lib"));
	}
}
