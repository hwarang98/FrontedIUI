// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FrontedUI : ModuleRules
{
	public FrontedUI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"GameplayTags",
			"UMG",
			"CommonInput",
			"CommonUI",
			"PropertyPath",
			"PreLoadScreen",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new[] { "Slate", "SlateCore" });

		if (Target.bBuildEditor) PrivateDependencyModuleNames.Add("UnrealEd");
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}