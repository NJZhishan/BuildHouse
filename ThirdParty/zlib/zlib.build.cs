// Copyright 2021-2022, DearBing. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class zlib : ModuleRules
{
	public Vlc(ReadOnlyTargetRules Target) : base(Target)
	{
        bEnableUndefinedIdentifierWarnings = false;
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        Type = ModuleType.External;

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));   //头文件的路径
			PublicLibraryPaths.Add(Path.Combine(ModuleDirectory, "lib"));       //库文件的路径
            PublicAdditionalLibraries.Add("zlib.lib");                          //库文件的名字
            PublicDelayLoadDLLs.Add("zlib.dll");                                //库文件延迟加载
            RuntimeDependencies.Add(Path.Combine(ModuleDirectory, "lib", "zlib.dll"));         
        }
	}
}
