// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HelaPlayFabIntegration : ModuleRules
{
	public HelaPlayFabIntegration(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "PlayFabCpp", "UMG", "Json", "JsonUtilities" });

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"PlayFab", "PlayFabCpp", "PlayFabCommon"
		});
		PrivateDependencyModuleNames.AddRange(new string[] { "CoreUObject", "Engine", "Slate", "SlateCore", "Http" });

		DynamicallyLoadedModuleNames.AddRange(new string[] { });
	}
}