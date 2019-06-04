// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class CanyonEditorTarget : TargetRules
{
	public CanyonEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		
		ExtraModuleNames.AddRange( new string[] { "Canyon", "CanyonEditor" } );
		
		
	}
	
	
}
