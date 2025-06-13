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
    UPROPERTY()
    bool bIsDestroying = false;

public:
    virtual bool StartService(UObject* CommBufferObj, const FString& Uri = TEXT("")) override
    {
        CommBuffer = Cast<UCommunicationBuffer>(CommBufferObj);
        ServerUri = Uri;
        TWeakObjectPtr<UWebSocketCommunicationService> WeakThis(this);

        WebSocket = FWebSocketsModule::Get().CreateWebSocket(Uri);
        WebSocket->OnConnected().AddLambda([this]()
            {
                UE_LOG(LogTemp, Log, TEXT("WebSocket connected."));
                bServiceEnabled = true;
            });


        WebSocket->OnRawMessage().AddLambda([WeakThis](const void* Data, SIZE_T Size, SIZE_T TotalSize)
            {
                TArray<uint8> Payload;
                Payload.Append(reinterpret_cast<const uint8*>(Data), Size);

                if (!WeakThis.IsValid()) return;
                UWebSocketCommunicationService* Service = WeakThis.Get();
                if (Service->bIsDestroying) return;
                //UE_LOG(LogTemp, Log, TEXT("DataIn: len=%d"), Payload.Num());

                TOptional<FDataPacket> OptionalPacket = FDataPacket::Decode(Payload);
                if (OptionalPacket.IsSet() && Service->CommBuffer)
                {
                    const FDataPacket& Packet = OptionalPacket.GetValue();
                    // UE_LOG(LogTemp, Log, TEXT("DataIn: Robot(%s) ChannelId(%d)"), *Packet.GetRobotName(), Packet.GetChannelId());
                    Service->CommBuffer->PutPacket(Packet);
                }
            });

        WebSocket->Connect();
        return true;
    }
    bool bClosedByEvent = false;
    virtual bool StopService() override
    {
        bIsDestroying = true;

        if (!bServiceEnabled || !WebSocket.IsValid())
        {
            return true;
        }

        bServiceEnabled = false;

        // Promise/Futureで完了を待つ
        TSharedPtr<TPromise<void>> ClosePromise = MakeShared<TPromise<void>>();
        TFuture<void> CloseFuture = ClosePromise->GetFuture();

        WebSocket->OnClosed().AddLambda([this, ClosePromise](int32 StatusCode, const FString& Reason, bool bWasClean)
            {
                UE_LOG(LogTemp, Log, TEXT("WebSocket closed: %s"), *Reason);
                this->bClosedByEvent = true;
                this->bServiceEnabled = false;
                ClosePromise->SetValue();
            });

        if (WebSocket->IsConnected())
        {
            WebSocket->Close();

            if (CloseFuture.WaitFor(FTimespan::FromSeconds(1.0)))
            {
                UE_LOG(LogTemp, Log, TEXT("WebSocket closed successfully"));
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("WebSocket close timeout"));
                // 強制SetValueでASSERT回避
                if (!bClosedByEvent)
                {
                    ClosePromise->SetValue();
                }
            }
        }
        else {
            ClosePromise->SetValue();
        }

        // クリーンアップ
        WebSocket->OnConnected().Clear();
        WebSocket->OnConnectionError().Clear();
        WebSocket->OnClosed().Clear();
        WebSocket->OnMessage().Clear();
        WebSocket->OnRawMessage().Clear();
        WebSocket->OnMessageSent().Clear();

        WebSocket.Reset();
        return true;
    }

    virtual bool IsServiceEnabled() const override
    {
        return bServiceEnabled;
    }

    virtual bool SendData(const FString& RobotName, int32 ChannelId, const TArray<uint8>& PduData) override
    {
        if (!bServiceEnabled || !WebSocket.IsValid() || !WebSocket->IsConnected())
        {
            UE_LOG(LogTemp, Warning, TEXT("WebSocket not connected"));
            return false;
        }

        FDataPacket Packet;
        Packet.SetRobotName(RobotName);
        Packet.SetChannelId(ChannelId);
        Packet.SetPduData(PduData);

        TArray<uint8> Encoded = Packet.Encode();
        WebSocket->Send(Encoded.GetData(), Encoded.Num(), true);
        return true;
    }


    virtual FString GetServerUri() const override
    {
        return ServerUri;
    }

};
