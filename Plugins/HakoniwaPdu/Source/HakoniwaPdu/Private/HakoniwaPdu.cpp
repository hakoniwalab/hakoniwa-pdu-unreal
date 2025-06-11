// Copyright Epic Games, Inc. All Rights Reserved.

#include "HakoniwaPdu.h"
#include "Logging/LogMacros.h"

#if 1
#include "IWebSocket.h"
#include "WebSocketsModule.h"

TSharedPtr<IWebSocket> WebSocket;


void ConnectWebSocket()
{
    if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
    {
        FModuleManager::Get().LoadModule("WebSockets");
    }

    FString Url = TEXT("ws://172.31.9.252:8765");  // © “K‹X•ÏX
    WebSocket = FWebSocketsModule::Get().CreateWebSocket(Url);

    WebSocket->OnConnected().AddLambda([]()
        {
            UE_LOG(LogTemp, Log, TEXT("WebSocket Connected!"));
        });

    WebSocket->OnConnectionError().AddLambda([](const FString& Error)
        {
            UE_LOG(LogTemp, Error, TEXT("WebSocket Connection Error: %s"), *Error);
        });

    WebSocket->Connect();
}
#endif

#define LOCTEXT_NAMESPACE "FHakoniwaPduModule"
DEFINE_LOG_CATEGORY_STATIC(LogHakoniwaPdu, Log, All);

void FHakoniwaPduModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(LogHakoniwaPdu, Log, TEXT("HakoniwaPdu: StartupModule called"));
    ConnectWebSocket();
}

void FHakoniwaPduModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UE_LOG(LogHakoniwaPdu, Log, TEXT("HakoniwaPdu: ShutdownModule called"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FHakoniwaPduModule, HakoniwaPdu)