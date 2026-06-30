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
			"Rogue10m/Core",
			"Rogue10m/Character",
			"Rogue10m/Components",
			"Rogue10m/Data",
			"Rogue10m/Enemy",
			"Rogue10m/UI",
			"Rogue10m/World",
			"Rogue10m/Variant_Horror",
			"Rogue10m/Variant_Horror/UI",
			"Rogue10m/Variant_Shooter",
			"Rogue10m/Variant_Shooter/AI",
			"Rogue10m/Variant_Shooter/UI",
			"Rogue10m/Variant_Shooter/Weapons"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
