// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PduManager.h"
#include "WebSocketCommunicationService.h"

#include "PluginTester.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HAKONIWAPDUDEV_API UPluginTester : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UWebSocketCommunicationService* service = nullptr;
	UPROPERTY()
	UPduManager* pduManager = nullptr;
	bool isDeclared = false;
	TArray<uint8> ReadTest(const FString& RobotName, const FString& PduName);

public:	
	// Sets default values for this component's properties
	UPluginTester();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
