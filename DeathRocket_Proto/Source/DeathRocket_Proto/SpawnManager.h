// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnManager.generated.h"

UCLASS()
class DEATHROCKET_PROTO_API ASpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnManager();

	UFUNCTION()
	void OnSpawnPlayer(class AGameModeBase* gamemode, class APlayerController* controller);

	void SpawnControllerAtPlayerStart(class APlayerController* controller);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetPlayerStarts();

	int lastPlayerStart = 0;

	TArray<class APlayerStart*> playerStarts;
	TArray<APlayerController*> controllers;
};
