// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeathRocket_ProtoGameMode.h"
#include "DeathRocket_ProtoCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADeathRocket_ProtoGameMode::ADeathRocket_ProtoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

}