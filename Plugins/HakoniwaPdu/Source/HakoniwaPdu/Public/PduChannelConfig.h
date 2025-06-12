// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PduChannelConfig.generated.h"

USTRUCT()
struct FPduChannel
{
    GENERATED_BODY()

    UPROPERTY() FString type;
    UPROPERTY() FString org_name;
    UPROPERTY() FString name;
    UPROPERTY() FString class_name;
    UPROPERTY() FString conv_class_name;
    UPROPERTY() int32 channel_id = -1;
    UPROPERTY() int32 pdu_size = 0;
    UPROPERTY() int32 write_cycle = 0;
    UPROPERTY() FString method_type;
};

USTRUCT()
struct FRobotConfig
{
    GENERATED_BODY()

    UPROPERTY() FString name;
    UPROPERTY() TArray<FPduChannel> rpc_pdu_readers;
    UPROPERTY() TArray<FPduChannel> rpc_pdu_writers;
    UPROPERTY() TArray<FPduChannel> shm_pdu_readers;
    UPROPERTY() TArray<FPduChannel> shm_pdu_writers;
};

USTRUCT()
struct FPduChannelConfigStruct
{
    GENERATED_BODY()

    UPROPERTY() TArray<FRobotConfig> robots;
};


/**
 * 
 */
UCLASS()
class HAKONIWAPDU_API UPduChannelConfig : public UObject
{
	GENERATED_BODY()
	
};
