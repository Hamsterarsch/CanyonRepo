// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Canyon : ModuleRules
{
	public Canyon(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Wwiseext", "AssetRegistry", "StomtPlugin" });

		PrivateDependencyModuleNames.AddRange(new string[] { "SlateCore", "CoreUObject" });

		PrivateDependencyModuleNames.AddRange(new string[] { "GameAnalytics" });
		PrivateIncludePathModuleNames.AddRange(new string[] { "GameAnalytics"});

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
