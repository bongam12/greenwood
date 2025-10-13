// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class greedwood : ModuleRules
{
	public greedwood(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		// Add the module's root directory to the include paths
		PublicIncludePaths.Add(ModuleDirectory);
		
        PublicDependencyModuleNames.AddRange(new string[] {
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput",
			"ProceduralMeshComponent", 
			"AIModule", 
			"GameplayTasks", 
			"NavigationSystem",
			"GameplayStateTreeModule",
			"StateTreeModule",
			"UMG",
			"Slate",
			"SlateCore"
		});

        PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
