#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DataPacket.generated.h"

UCLASS()
class HAKONIWAPDU_API UDataPacket : public UObject
{
    GENERATED_BODY()

private:
    FString RobotName;
    int32 ChannelId;
    TArray<uint8> BodyData;

public:
    void SetRobotName(const FString& InName) { RobotName = InName; }
    void SetChannelId(int32 InId) { ChannelId = InId; }
    void SetPduData(const TArray<uint8>& InData) { BodyData = InData; }

    FString GetRobotName() const { return RobotName; }
    int32 GetChannelId() const { return ChannelId; }
    const TArray<uint8>& GetPduData() const { return BodyData; }

    TArray<uint8> Encode() const
    {
        FTCHARToUTF8 Utf8Name(*RobotName);
        int32 NameLen = Utf8Name.Length();
        int32 HeaderLength = 4 + NameLen + 4;
        int32 TotalLength = 4 + HeaderLength + BodyData.Num();

        TArray<uint8> Data;
        Data.SetNumUninitialized(TotalLength);
        int32 Index = 0;

        // header length
        FMemory::Memcpy(Data.GetData() + Index, &HeaderLength, 4);
        Index += 4;

        // robot name length
        FMemory::Memcpy(Data.GetData() + Index, &NameLen, 4);
        Index += 4;

        // robot name bytes
        FMemory::Memcpy(Data.GetData() + Index, Utf8Name.Get(), NameLen);
        Index += NameLen;

        // channel ID
        FMemory::Memcpy(Data.GetData() + Index, &ChannelId, 4);
        Index += 4;

        // body
        if (BodyData.Num() > 0)
        {
            FMemory::Memcpy(Data.GetData() + Index, BodyData.GetData(), BodyData.Num());
        }

        return Data;
    }

    static UDataPacket* Decode(const TArray<uint8>& Data)
    {
        if (Data.Num() < 12)
        {
            UE_LOG(LogTemp, Error, TEXT("Data too short"));
            return nullptr;
        }

        int32 Index = 0;
        int32 HeaderLength = 0;
        FMemory::Memcpy(&HeaderLength, Data.GetData() + Index, 4);
        Index += 4;

        int32 NameLen = 0;
        FMemory::Memcpy(&NameLen, Data.GetData() + Index, 4);
        Index += 4;

        if (Index + NameLen + 4 > Data.Num())
        {
            UE_LOG(LogTemp, Error, TEXT("Invalid robot name length"));
            return nullptr;
        }

        const ANSICHAR* Utf8Ptr = reinterpret_cast<const ANSICHAR*>(Data.GetData() + Index);
        FString RobotName = UTF8_TO_TCHAR(Utf8Ptr);
        Index += NameLen;

        int32 ChannelId = 0;
        FMemory::Memcpy(&ChannelId, Data.GetData() + Index, 4);
        Index += 4;

        TArray<uint8> Body;
        int32 BodyLen = Data.Num() - Index;
        if (BodyLen > 0)
        {
            Body.Append(Data.GetData() + Index, BodyLen);
        }

        UDataPacket* Packet = NewObject<UDataPacket>();
        Packet->SetRobotName(RobotName);
        Packet->SetChannelId(ChannelId);
        Packet->SetPduData(Body);
        return Packet;
    }
};

// グローバル定数定義
struct FPduMagicNumbers
{
    static constexpr uint32 DeclarePduForRead = 0x52455044; // "REPD"
    static constexpr uint32 DeclarePduForWrite = 0x57505044; // "WPPD"
};
