#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HAL/CriticalSection.h"
#include "DataPacket.h"
#include "PduChannelConfig.h"

#include "CommunicationBuffer.generated.h"


UCLASS()
class HAKONIWAPDU_API UCommunicationBuffer : public UObject
{
    GENERATED_BODY()

private:
    TMap<FString, TArray<uint8>> PduBuffer;
    FCriticalSection Mutex;

    static constexpr TCHAR Separator = 0x1F;

    UPROPERTY()
    FPduChannelConfigStruct ChannelConfig;


public:
    void SetChannelConfig(const FPduChannelConfigStruct& InConfig)
    {
        ChannelConfig = InConfig;
    }


    FString GetKey(const FString& RobotName, const FString& PduName) const
    {
        return RobotName + Separator + PduName;
    }

    void Key2RobotPdu(const FString& Key, FString& OutRobotName, FString& OutPduName) const
    {
        TArray<FString> Tokens;
        Key.ParseIntoArray(Tokens, &Separator, false);
        if (Tokens.Num() != 2)
        {
            UE_LOG(LogTemp, Error, TEXT("Invalid key format: %s"), *Key);
            OutRobotName = TEXT("");
            OutPduName = TEXT("");
            return;
        }
        OutRobotName = Tokens[0];
        OutPduName = Tokens[1];
    }

    void SetBuffer(const FString& Key, const TArray<uint8>& Data)
    {
        FScopeLock Lock(&Mutex);
        PduBuffer.Add(Key, Data);
    }

    TArray<uint8> GetBuffer(const FString& Key)
    {
        FScopeLock Lock(&Mutex);
        TArray<uint8> Data;
        if (PduBuffer.RemoveAndCopyValue(Key, Data))
        {
            return Data;
        }
        return {};
    }

    bool ContainsBuffer(const FString& Key)
    {
        FScopeLock Lock(&Mutex);
        return PduBuffer.Contains(Key);
    }

    void Clear()
    {
        FScopeLock Lock(&Mutex);
        PduBuffer.Empty();
    }

    void PutPacket(UDataPacket* Packet)
    {
        FString RobotName = Packet->GetRobotName();
        int32 ChannelId = Packet->GetChannelId();

        FString PduName = GetPduName(RobotName, ChannelId); // © new
        if (PduName.IsEmpty())
        {
            UE_LOG(LogTemp, Warning, TEXT("Unknown PDU for %s:%d"), *RobotName, ChannelId);
            return;
        }

        FString Key = GetKey(RobotName, PduName);
        SetBuffer(Key, Packet->GetPduData());
    }
    FString GetPduName(const FString& RobotName, int32 ChannelId) const
    {
        for (const FRobotConfig& Robot : ChannelConfig.robots)
        {
            if (Robot.name == RobotName)
            {
                for (const FPduChannel& Ch : Robot.shm_pdu_readers)
                    if (Ch.channel_id == ChannelId)
                        return Ch.org_name;

                for (const FPduChannel& Ch : Robot.shm_pdu_writers)
                    if (Ch.channel_id == ChannelId)
                        return Ch.org_name;
            }
        }
        return TEXT("");
    }
    int32 GetPduSize(const FString& RobotName, const FString& PduName) const
    {
        for (const FRobotConfig& Robot : ChannelConfig.robots)
        {
            if (Robot.name == RobotName)
            {
                for (const FPduChannel& Ch : Robot.shm_pdu_readers)
                    if (Ch.org_name == PduName)
                        return Ch.pdu_size;

                for (const FPduChannel& Ch : Robot.shm_pdu_writers)
                    if (Ch.org_name == PduName)
                        return Ch.pdu_size;
            }
        }
        return -1;
    }
    int32 GetPduChannelId(const FString& RobotName, const FString& PduName) const
    {
        for (const FRobotConfig& Robot : ChannelConfig.robots)
        {
            if (Robot.name == RobotName)
            {
                for (const FPduChannel& Ch : Robot.shm_pdu_readers)
                    if (Ch.org_name == PduName)
                        return Ch.channel_id;

                for (const FPduChannel& Ch : Robot.shm_pdu_writers)
                    if (Ch.org_name == PduName)
                        return Ch.channel_id;
            }
        }
        return -1;
    }

    void PutPacketDirect(const FString& RobotName, int32 ChannelId, const TArray<uint8>& PduData)
    {
        FString PduName = GetPduName(RobotName, ChannelId);
        if (PduName.IsEmpty())
        {
            UE_LOG(LogTemp, Warning, TEXT("Unknown PDU for %s:%d"), *RobotName, ChannelId);
            return;
        }
        FString Key = GetKey(RobotName, PduName);
        SetBuffer(Key, PduData);
    }
};
