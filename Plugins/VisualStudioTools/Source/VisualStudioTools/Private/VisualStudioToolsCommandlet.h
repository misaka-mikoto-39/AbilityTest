// Copyright 2022 (c) Microsoft. All rights reserved.

#pragma once

#include "Commandlets/Commandlet.h"

#include "VisualStudioToolsCommandlet.generated.h"

UCLASS()
class UVisualStudioToolsCommandlet
	: public UCommandlet
{
	GENERATED_BODY()

public:
	UVisualStudioToolsCommandlet();

public:
	virtual int32 Main(const FString& Params) override;
};