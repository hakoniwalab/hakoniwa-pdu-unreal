// PluginTester.cpp

#include "PluginTester.h"
#include "Modules/ModuleManager.h"

UPluginTester::UPluginTester()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UPluginTester::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Log, TEXT("PluginTester BeginPlay"));

    // 例：プラグインの手動ロード
    const FName ModuleName = "HakoniwaPdu";
    if (!FModuleManager::Get().IsModuleLoaded(ModuleName))
    {
        FModuleManager::Get().LoadModule(ModuleName);
    }

    service = NewObject<UWebSocketCommunicationService>();
    pduManager = NewObject<UPduManager>();

    pduManager->Initialize("Config/webavatar.json", service);
    pduManager->StartService("ws://172.31.9.252:8765");

}

void UPluginTester::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (isDeclared == false) {
        if (pduManager->IsServiceEnabled()) {
            if (pduManager->DeclarePduForRead("Drone", "motor")) {
                UE_LOG(LogTemp, Log, TEXT("Successfully declared Drone:pos"));
            }
            else {
                UE_LOG(LogTemp, Warning, TEXT("Failed to declare Drone:pos"));
            }
            if (pduManager->DeclarePduForRead("Drone", "pos")) {
                UE_LOG(LogTemp, Log, TEXT("Successfully declared Drone:pos"));
            }
            else {
                UE_LOG(LogTemp, Warning, TEXT("Failed to declare Drone:pos"));
            }
            isDeclared = true;
        }
        else {
            UE_LOG(LogTemp, Warning, TEXT("Can not be enabled pduManager service"));
        }
    }

}


void UPluginTester::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    if (pduManager) {
        pduManager->StopService();
    }
#if 0
    const FName ModuleName = "HakoniwaPdu";
    if (FModuleManager::Get().IsModuleLoaded(ModuleName))
    {
        bool bSuccess = FModuleManager::Get().UnloadModule(ModuleName);
        if (bSuccess)
        {
            UE_LOG(LogTemp, Log, TEXT("HakoniwaPdu unloaded"));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to unload HakoniwaPdu"));
        }
    }
#endif
}
