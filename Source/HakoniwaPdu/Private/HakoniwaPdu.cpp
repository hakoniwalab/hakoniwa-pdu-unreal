// Copyright Epic Games, Inc. All Rights Reserved.

#include "HakoniwaPdu.h"
#include "Logging/LogMacros.h"


#define LOCTEXT_NAMESPACE "FHakoniwaPduModule"
DEFINE_LOG_CATEGORY_STATIC(LogHakoniwaPdu, Log, All);
void FHakoniwaPduModule::StartupModule()
{
    UE_LOG(LogHakoniwaPdu, Log, TEXT("HakoniwaPdu: StartupModule called"));
}


void FHakoniwaPduModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UE_LOG(LogHakoniwaPdu, Log, TEXT("HakoniwaPdu: ShutdownModule called"));

}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FHakoniwaPduModule, HakoniwaPdu)