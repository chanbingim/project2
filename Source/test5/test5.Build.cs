// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class test5 : ModuleRules
{
	public test5(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(new string[] {
			"Project",
			"Project/Character",
			"Project/AI",
			"wqwe"
		});
		
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"HeadMountedDisplay",
			"EnhancedInput",
			"NavigationSystem",
			"AIModule",
			"GameplayTasks",
			"UMG",
            "SlateCore",
            "PhysicsCore",
			"Niagara",
        });
	}
}
