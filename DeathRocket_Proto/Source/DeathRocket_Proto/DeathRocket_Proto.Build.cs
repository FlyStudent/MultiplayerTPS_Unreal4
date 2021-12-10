// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DeathRocket_Proto : ModuleRules
{
	public DeathRocket_Proto(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG" });
	}
}
