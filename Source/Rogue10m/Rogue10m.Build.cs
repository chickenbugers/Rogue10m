// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Rogue10m : ModuleRules
{
	public Rogue10m(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"SlateCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Rogue10m",
			"Rogue10m/Ability",
			"Rogue10m/Core",
			"Rogue10m/Character",
			"Rogue10m/Components",
			"Rogue10m/Data",
			"Rogue10m/Enemy",
			"Rogue10m/UI",
			"Rogue10m/UI/Widgets",
			"Rogue10m/World"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
