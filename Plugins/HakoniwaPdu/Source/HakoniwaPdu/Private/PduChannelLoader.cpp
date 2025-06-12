// Fill out your copyright notice in the Description page of Project Settings.


#include "PduChannelLoader.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "JsonObjectConverter.h"

FPduChannelConfigStruct UPduChannelLoader::Load(const FString& RelativePath)
{
    FString FullPath = FPaths::ProjectContentDir() / RelativePath;
    FString JsonContent;

    FPduChannelConfigStruct Result;

    if (!FFileHelper::LoadFileToString(JsonContent, *FullPath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load JSON: %s"), *FullPath);
        return Result;
    }

    if (!FJsonObjectConverter::JsonObjectStringToUStruct(JsonContent, &Result, 0, 0))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON from: %s"), *FullPath);
    }

    return Result;
}
