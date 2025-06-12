// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CommunicationService.h"
#include "CommunicationBuffer.h"
#include "IWebSocket.h"
#include "WebSocketsModule.h"
#include "Async/Async.h"
#include "HAL/RunnableThread.h"
#include "WebSocketCommunicationService.generated.h"

/**
 * 
 */
UCLASS()
class HAKONIWAPDU_API UWebSocketCommunicationService : public UCommunicationService
{
	GENERATED_BODY()


private:
    TSharedPtr<IWebSocket> WebSocket;
    UPROPERTY()
    UCommunicationBuffer* CommBuffer;

    FString ServerUri;
    FThreadSafeBool bServiceEnabled = false;

public:
    virtual bool StartService(UObject* CommBufferObj, const FString& Uri = TEXT("")) override
    {
        CommBuffer = Cast<UCommunicationBuffer>(CommBufferObj);
        ServerUri = Uri;

        WebSocket = FWebSocketsModule::Get().CreateWebSocket(Uri);
        WebSocket->OnConnected().AddLambda([this]()
            {
                UE_LOG(LogTemp, Log, TEXT("WebSocket connected."));
                bServiceEnabled = true;
            });

        WebSocket->OnClosed().AddLambda([this](int32 StatusCode, const FString& Reason, bool bWasClean)
            {
                UE_LOG(LogTemp, Warning, TEXT("WebSocket closed: %s"), *Reason);
                bServiceEnabled = false;
            });

        WebSocket->OnRawMessage().AddLambda([this](const void* Data, SIZE_T Size, SIZE_T TotalSize)
            {
                TArray<uint8> Payload;
                Payload.Append(reinterpret_cast<const uint8*>(Data), Size);

                AsyncTask(ENamedThreads::GameThread, [this, Payload]()
                    {
                        UDataPacket* Packet = UDataPacket::Decode(Payload);
                        if (Packet && CommBuffer)
                        {
                            CommBuffer->PutPacket(Packet);
                        }
                    });
            });

        WebSocket->Connect();
        return true;
    }

    virtual bool StopService() override
    {
        if (!bServiceEnabled || !WebSocket.IsValid())
        {
            return false;
        }

        bServiceEnabled = false;

        // イベントハンドラの解除
        WebSocket->OnConnected().Clear();
        WebSocket->OnConnectionError().Clear();
        WebSocket->OnClosed().Clear();
        WebSocket->OnMessage().Clear();
        WebSocket->OnRawMessage().Clear();
        WebSocket->OnMessageSent().Clear();

        // 安全に切断
        if (WebSocket->IsConnected())
        {
            WebSocket->Close();
        }

        // 遅延しても完全解放
        WebSocket.Reset();
        return true;
    }

    virtual bool IsServiceEnabled() const override
    {
        return bServiceEnabled;
    }

    virtual bool SendData(const FString& RobotName, int32 ChannelId, const TArray<uint8>& PduData) override
    {
        if (!bServiceEnabled || !WebSocket.IsValid() || WebSocket->IsConnected() == false)
        {
            UE_LOG(LogTemp, Warning, TEXT("WebSocket not connected"));
            return false;
        }

        UDataPacket* Packet = NewObject<UDataPacket>();
        Packet->SetRobotName(RobotName);
        Packet->SetChannelId(ChannelId);
        Packet->SetPduData(PduData);

        TArray<uint8> Encoded = Packet->Encode();
        WebSocket->Send(Encoded.GetData(), Encoded.Num(), true);
        return true;
    }

    virtual FString GetServerUri() const override
    {
        return ServerUri;
    }

};
