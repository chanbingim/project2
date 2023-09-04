// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class wqwe : ModuleRules
{
	public wqwe(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicIncludePaths.AddRange(
			new string[] {
				"Project",
				"test5",
				//"And so on..."
			}
		);
		
		PrivateDependencyModuleNames.AddRange(new string[] {  });

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"HeadMountedDisplay",
			"UMG",
			"EnhancedInput",
			"Slate",
			"SlateCore",
			"NavigationSystem",
			"Navmesh",
			"AIModule",
			"GameplayTasks",
			
			"AICoverSystem"
		});
	}
}
