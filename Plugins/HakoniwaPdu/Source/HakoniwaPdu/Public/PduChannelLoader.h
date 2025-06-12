// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PduChannelConfig.h"
#include "PduChannelLoader.generated.h"

/**
 * 
 */
UCLASS()
class HAKONIWAPDU_API UPduChannelLoader : public UObject
{
	GENERATED_BODY()

public:
	static FPduChannelConfigStruct Load(const FString& RelativePath);

};
