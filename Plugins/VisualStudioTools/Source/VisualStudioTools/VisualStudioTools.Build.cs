// Copyright 2022 (c) Microsoft. All rights reserved.

using UnrealBuildTool;

public class VisualStudioTools : ModuleRules
{
	public VisualStudioTools(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"AssetRegistry",
				"CoreUObject",
				"Engine",
				"Json",
				"JsonUtilities",
			}
		);
	}
}
