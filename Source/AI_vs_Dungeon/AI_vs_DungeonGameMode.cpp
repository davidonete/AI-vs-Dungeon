// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "AI_vs_Dungeon.h"
#include "AI_vs_DungeonGameMode.h"
#include "AI_vs_DungeonCharacter.h"

AAI_vs_DungeonGameMode::AAI_vs_DungeonGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Characters/Main/Main"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
