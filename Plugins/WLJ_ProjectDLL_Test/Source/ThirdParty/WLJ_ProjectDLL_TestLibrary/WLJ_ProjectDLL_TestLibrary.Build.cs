// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class WLJ_ProjectDLL_TestLibrary : ModuleRules
{
	public WLJ_ProjectDLL_TestLibrary(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));   //ͷ�ļ���·��
			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "x64", "Release", "ExampleLibrary.lib"));  //���ļ�������
			PublicDelayLoadDLLs.Add("ExampleLibrary.dll");  //�ӳټ���
			RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/WLJ_ProjectDLL_TestLibrary/Win64/ExampleLibrary.dll");//���صĿ��ļ���·��
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            PublicDelayLoadDLLs.Add(Path.Combine(ModuleDirectory, "Mac", "Release", "libExampleLibrary.dylib"));
            RuntimeDependencies.Add("$(PluginDir)/Source/ThirdParty/WLJ_ProjectDLL_TestLibrary/Mac/Release/libExampleLibrary.dylib");
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
		{
			string ExampleSoPath = Path.Combine("$(PluginDir)", "Binaries", "ThirdParty", "WLJ_ProjectDLL_TestLibrary", "Linux", "x86_64-unknown-linux-gnu", "libExampleLibrary.so");
			PublicAdditionalLibraries.Add(ExampleSoPath);
			PublicDelayLoadDLLs.Add(ExampleSoPath);
			RuntimeDependencies.Add(ExampleSoPath);
		}
	}
}
