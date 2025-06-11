// PluginTester.cpp

#include "PluginTester.h"
#include "Modules/ModuleManager.h"

UPluginTester::UPluginTester()
{
    PrimaryComponentTick.bCanEverTick = false;  // 必要なら true に
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
}

void UPluginTester::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // 必要ならTick処理を書く
}


void UPluginTester::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

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
}
