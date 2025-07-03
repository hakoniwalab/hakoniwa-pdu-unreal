// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CommunicationService.generated.h"

class UCommunicationBuffer;

/**
 * 
 */
UCLASS()
class HAKONIWAPDU_API UCommunicationService : public UObject
{
	GENERATED_BODY()


public:
    // サービス起動（非同期なら後でタスク化も可）
    virtual bool StartService(UObject* CommBuffer, const FString& Uri = TEXT("")) PURE_VIRTUAL(UCommunicationService::StartService, return false;);

    // サービス停止
    virtual bool StopService() PURE_VIRTUAL(UUCommunicationService::StopService, return false;);

    // サービス起動中か？
    virtual bool IsServiceEnabled() const PURE_VIRTUAL(UUCommunicationService::IsServiceEnabled, return false;);

    // データ送信
    virtual bool SendData(const FString & RobotName, int32 ChannelId, const TArray<uint8>&PduData) PURE_VIRTUAL(UUCommunicationService::SendData, return false;);

    // サーバURI取得
    virtual FString GetServerUri() const PURE_VIRTUAL(UUCommunicationService::GetServerUri, return FString(TEXT("")););
};
