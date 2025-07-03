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
    // �T�[�r�X�N���i�񓯊��Ȃ��Ń^�X�N�����j
    virtual bool StartService(UObject* CommBuffer, const FString& Uri = TEXT("")) PURE_VIRTUAL(UCommunicationService::StartService, return false;);

    // �T�[�r�X��~
    virtual bool StopService() PURE_VIRTUAL(UUCommunicationService::StopService, return false;);

    // �T�[�r�X�N�������H
    virtual bool IsServiceEnabled() const PURE_VIRTUAL(UUCommunicationService::IsServiceEnabled, return false;);

    // �f�[�^���M
    virtual bool SendData(const FString & RobotName, int32 ChannelId, const TArray<uint8>&PduData) PURE_VIRTUAL(UUCommunicationService::SendData, return false;);

    // �T�[�oURI�擾
    virtual FString GetServerUri() const PURE_VIRTUAL(UUCommunicationService::GetServerUri, return FString(TEXT("")););
};
