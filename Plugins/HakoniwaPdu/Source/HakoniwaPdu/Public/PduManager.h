// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CommunicationBuffer.h"
#include "CommunicationService.h"
#include "PduChannelLoader.h"
#include "DataPacket.h"
#include "PduManager.generated.h"

/**
 * 
 */
UCLASS()
class HAKONIWAPDU_API UPduManager : public UObject
{
	GENERATED_BODY()


private:
	UPROPERTY()
	UCommunicationBuffer* CommBuffer;

	UPROPERTY()
	UCommunicationService* CommService;

	UPROPERTY()
	FPduChannelConfigStruct PduConfig;

public:
	void Initialize(const FString& RelativePath, UCommunicationService* InCommService)
	{
		PduConfig = UPduChannelLoader::Load(RelativePath);

		CommBuffer = NewObject<UCommunicationBuffer>();
		CommBuffer->SetChannelConfig(PduConfig);

		CommService = InCommService;
	}
	bool StartService(const FString& server_uri = TEXT(""))
	{
		if (CommService == nullptr) {
			UE_LOG(LogTemp, Error, TEXT("CommService is null"));
			return false;
		}
		if (CommService->IsServiceEnabled()) {
			UE_LOG(LogTemp, Error, TEXT("CommService is already enabled"));
			return false;
		}
		if (CommService->StartService(CommBuffer, server_uri) == false) {
			UE_LOG(LogTemp, Error, TEXT("Failed to start CommService"));

			return false;
		}
		return true;
	}
	bool StopService()
	{
		if (CommService && CommService->IsServiceEnabled()) {
			if (CommService->StopService()) {
				return true;
			}
			return false;
		}
		return false;
	}
	bool IsServiceEnabled()
	{
		if (CommService != nullptr) {
			return CommService->IsServiceEnabled();
		}
		return false;
	}

	bool DeclarePduForRead(const FString& RobotName, const FString& PduName)
	{
		return DeclarePdu(RobotName, PduName, true);
	}
	bool DeclarePduForWrite(const FString& RobotName, const FString& PduName)
	{
		return DeclarePdu(RobotName, PduName, false);
	}
	bool DeclarePduForReadWrite(const FString& RobotName, const FString& PduName)
	{
		if (DeclarePduForRead(RobotName, PduName)) {
			return DeclarePduForWrite(RobotName, PduName);
		}
		return false;
	}
	int32 GetPduChannelId(const FString& RobotName, const FString& PduName)
	{
		if (IsServiceEnabled() == false) {
			return false;
		}
		return CommBuffer->GetPduChannelId(RobotName, PduName);
	}
	int32 GetPduSize(const FString& RobotName, const FString& PduName)
	{
		if (IsServiceEnabled() == false) {
			return false;
		}
		return CommBuffer->GetPduSize(RobotName, PduName);
	}
	bool FlushPduRawData(const FString& RobotName, const FString& PduName, const void* top_ptr, int32 ptr_size)
	{
		if (!IsServiceEnabled()) {
			UE_LOG(LogTemp, Error, TEXT("Service is not enabled"));
			return false;
		}
		if (top_ptr == nullptr || ptr_size <= 0) {
			UE_LOG(LogTemp, Error, TEXT("Invalid memory pointer or size"));
			return false;
		}

		int32 ChannelId = CommBuffer->GetPduChannelId(RobotName, PduName);
		if (ChannelId < 0) {
			UE_LOG(LogTemp, Error, TEXT("Cannot find Channel ID for %s %s"), *RobotName, *PduName);
			return false;
		}
		TArray<uint8> PduRawData;
		PduRawData.SetNum(ptr_size);
		FMemory::Memcpy(PduRawData.GetData(), top_ptr, ptr_size);
		if (!CommService->SendData(RobotName, ChannelId, PduRawData))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to send magic number for %s:%s"), *RobotName, *PduName);
			return false;
		}
		return true;
	}
	bool ReadPduRawData(const FString& RobotName, const FString& PduName, void* top_ptr, int32 ptr_size)
	{
		if (!IsServiceEnabled()) {
			UE_LOG(LogTemp, Error, TEXT("Service is not enabled"));
			return false;
		}
		if (top_ptr == nullptr || ptr_size <= 0) {
			UE_LOG(LogTemp, Error, TEXT("Invalid destination pointer or size"));
			return false;
		}

		int32 ChannelId = CommBuffer->GetPduChannelId(RobotName, PduName);
		if (ChannelId < 0) {
			UE_LOG(LogTemp, Error, TEXT("Cannot find Channel ID for %s %s"), *RobotName, *PduName);
			return false;
		}

		FString Key = CommBuffer->GetKey(RobotName, PduName);
		TArray<uint8> RawData = CommBuffer->GetBuffer(Key);

		if (RawData.Num() <= 0) {
			//UE_LOG(LogTemp, Warning, TEXT("No data available for %s:%s"), *RobotName, *PduName);
			return false;
		}

		if (RawData.Num() > ptr_size) {
			UE_LOG(LogTemp, Warning, TEXT("Size mismatch: received %d bytes, destination size %d"), RawData.Num(), ptr_size);
			return false;
		}

		FMemory::Memcpy(top_ptr, RawData.GetData(), ptr_size);
		return true;
	}
private:

	bool DeclarePdu(const FString& RobotName, const FString& PduName, bool isRead)
	{
		if (IsServiceEnabled() == false) {
			return false;
		}
		int32 ChannelId = CommBuffer->GetPduChannelId(RobotName, PduName);
		if (ChannelId < 0) {
			UE_LOG(LogTemp, Error, TEXT("Cannot find Channel ID for %s %s"), *RobotName, *PduName);
			return false;
		}
		TArray<uint8> PduRawData;
		PduRawData.SetNum(sizeof(uint32));
		uint32 MagicNumber = FPduMagicNumbers::DeclarePduForWrite;
		if (isRead) {
			MagicNumber = FPduMagicNumbers::DeclarePduForRead;
		}
		FMemory::Memcpy(PduRawData.GetData(), &MagicNumber, sizeof(uint32));

		if (!CommService->SendData(RobotName, ChannelId, PduRawData))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to send magic number for %s:%s"), *RobotName, *PduName);
			return false;
		}
		return true;
	}
};
