// PluginTester.cpp

#include "PluginTester.h"
#include "Modules/ModuleManager.h"
#include "geometry_msgs/pdu_cpptype_conv_Twist.hpp"
#include "hako_mavlink_msgs/pdu_cpptype_conv_HakoHilActuatorControls.hpp"
#include "pdu_convertor.hpp"

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
    else {
        TArray<char> buffer = ReadTest("Drone", "pos");
        if (buffer.Num() > 0) {
            HakoCpp_Twist pos;
            hako::pdu::PduConvertor<HakoCpp_Twist, hako::pdu::msgs::geometry_msgs::Twist> conv;
            conv.pdu2cpp(buffer.GetData(), pos);
            UE_LOG(LogTemp, Log, TEXT("Twist.linear = (%lf, %lf, %lf), angular = (%lf, %lf, %lf)"),
                pos.linear.x, pos.linear.y, pos.linear.z,
                pos.angular.x, pos.angular.y, pos.angular.z);
        }
        buffer = ReadTest("Drone", "motor");
        if (buffer.Num() > 0) {
            HakoCpp_HakoHilActuatorControls motor;
            hako::pdu::PduConvertor<HakoCpp_HakoHilActuatorControls, hako::pdu::msgs::hako_mavlink_msgs::HakoHilActuatorControls> conv;
            conv.pdu2cpp(buffer.GetData(), motor);
            UE_LOG(LogTemp, Log, TEXT("motor.time_usec = %llu"), motor.time_usec);

            FString controlStr;
            for (int i = 0; i < motor.controls.size(); i++) {
                controlStr += FString::Printf(TEXT("%.3f "), motor.controls[i]);
            }
            UE_LOG(LogTemp, Log, TEXT("motor.controls = [%s]"), *controlStr);
            UE_LOG(LogTemp, Log, TEXT("motor.mode = %d"), motor.mode);
            UE_LOG(LogTemp, Log, TEXT("motor.flags = %llu"), motor.flags);
        }
    }

}
TArray<char> UPluginTester::ReadTest(const FString& RobotName, const FString& PduName)
{
    TArray<char> buffer;

    int32 pdu_size = pduManager->GetPduSize(RobotName, PduName);
    if (pdu_size <= 0) {
        UE_LOG(LogTemp, Error, TEXT("Can not get pdu size..."));
        return buffer;  // 空の配列を返す
    }

    buffer.SetNum(pdu_size);
    char* rawPtr = buffer.GetData();

    if (pduManager->ReadPduRawData(RobotName, PduName, rawPtr, pdu_size)) {
        UE_LOG(LogTemp, Log, TEXT("Read PDU succeeded: robot=%s, pdu=%s, size=%d"),
            *RobotName, *PduName, pdu_size);
        return buffer;
    }
    else {
        //UE_LOG(LogTemp, Error, TEXT("Failed to read PDU: robot=%s, pdu=%s"), *RobotName, *PduName);
        return TArray<char>();  // 読み取り失敗時は空の配列を返す
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
