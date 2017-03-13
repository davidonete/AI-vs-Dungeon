// Fill out your copyright notice in the Description page of Project Settings.

#include "AI_vs_Dungeon.h"
#include "AI_vs_DungeonGameInstance.h"

UAI_vs_DungeonGameInstance::UAI_vs_DungeonGameInstance()
{

}

float UAI_vs_DungeonGameInstance::GetAverageError()
{
    return AverageError;
}

void UAI_vs_DungeonGameInstance::SetAverageError(float result)
{
    AverageError = result;
}


