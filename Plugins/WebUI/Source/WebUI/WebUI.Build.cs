// Copyright 2021 Tracer Interactive, LLC. All Rights Reserved.
namespace UnrealBuildTool.Rules
{
	public class WebUI : ModuleRules
	{
		public WebUI(ReadOnlyTargetRules Target) : base(Target)
		{
			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"Engine",
                    "InputCore",
                    "RenderCore",
                    "RHI",
                    "Slate",
					"SlateCore",
					"UMG",
					"JsonLibrary"
				}
			);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"Http"
				}
			);

			if (Target.Type != TargetType.Server)
			{
				if (Target.Platform == UnrealTargetPlatform.Win64
				||  Target.Platform == UnrealTargetPlatform.Win32
				||  Target.Platform == UnrealTargetPlatform.Mac
				||  Target.Platform == UnrealTargetPlatform.Linux)
				{
					PublicDependencyModuleNames.AddRange(
						new string[]
						{
							"WebBrowserUI"
						}
					);
				}
				else
				{
					PublicDependencyModuleNames.AddRange(
						new string[]
						{
							"WebBrowser"
						}
					);
				}

				if (Target.bBuildEditor || Target.Platform == UnrealTargetPlatform.Android)
				{
					PrivateIncludePathModuleNames.AddRange(
						new string[]
						{
							"WebBrowserTexture"
						}
					);

					PrivateDependencyModuleNames.AddRange(
						new string[]
						{
							"WebBrowserTexture"
						}
					);
				}
			}
		}
	}
}
