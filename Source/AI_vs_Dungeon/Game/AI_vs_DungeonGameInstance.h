// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "AI_vs_DungeonGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class AI_VS_DUNGEON_API UAI_vs_DungeonGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
 public:
     UAI_vs_DungeonGameInstance();

     UFUNCTION(BlueprintCallable, category = "GameInstance")
     float GetAverageError();
     void SetAverageError(float result);

  private:
      float AverageError;
	
};
