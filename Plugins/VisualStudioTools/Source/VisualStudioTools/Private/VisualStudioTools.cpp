// Copyright 2022 (c) Microsoft. All rights reserved.

#include "VisualStudioTools.h"

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY(LogVisualStudioTools);

class FVisualStudioToolsModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override {}
	virtual void ShutdownModule() override {}
};

IMPLEMENT_MODULE(FVisualStudioToolsModule, VisualStudioTools)