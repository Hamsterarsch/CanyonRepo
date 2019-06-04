// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class WwiseextTarget : TargetRules
{
	public WwiseextTarget(TargetInfo Target) : base(Target)
    {	
        //Type = TargetType.Game;
		
		ExtraModuleNames.Add("Wwiseext");
		
		
	}
	
	
}
