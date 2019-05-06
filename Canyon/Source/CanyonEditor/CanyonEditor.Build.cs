// Fill out your copyright notice in the Description page of Project Settings.

using System;
using UnrealBuildTool;

public class CanyonEditor : ModuleRules
{
	public CanyonEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		System.Console.WriteLine("\tExecuting Build.cs for Canyon Editor");
		
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
		
		PrivateDependencyModuleNames.AddRange(new string[] { "Canyon", "Slate", "SlateCore", "UnrealEd", "LevelEditor", "EditorStyle", "UMG" });

        PrivateIncludePathModuleNames.AddRange(new String[] {"Slate", "Canyon"});


		PublicIncludePaths.AddRange(
            new string[]
            {
                "CanyonEditor/Public"
            }
		);

        PrivateIncludePaths.AddRange(
            new string[] 
            {
				"CanyonEditor/Private"
			}
		);

		
	}
	
	
}
