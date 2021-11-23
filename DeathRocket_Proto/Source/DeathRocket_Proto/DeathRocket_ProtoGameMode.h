// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DeathRocket_ProtoGameMode.generated.h"

UCLASS(minimalapi)
class ADeathRocket_ProtoGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADeathRocket_ProtoGameMode();

	UFUNCTION()
	void OnSpawnPlayer(AGameModeBase* gamemode, APlayerController* controller);

	void SpawnControllerAtPlayerStart(APlayerController* controller);

private:
	int lastPlayerStart = 0;

	TArray<class APlayerStart*> playerStarts;
	TArray<APlayerController*> controllers;

protected:
	virtual void StartPlay() override;
};



