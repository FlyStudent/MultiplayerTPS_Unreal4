// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnManager.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerController.h"

// Sets default values
ASpawnManager::ASpawnManager()
{
	PrimaryActorTick.bCanEverTick = false;

	FGameModeEvents::GameModePostLoginEvent.AddUObject(this, &ASpawnManager::OnSpawnPlayer);
}

// Called when the game starts or when spawned
void ASpawnManager::BeginPlay()
{
	Super::BeginPlay();

	if (playerStarts.Num() == 0)
		SetPlayerStarts();
}

void ASpawnManager::SetPlayerStarts()
{
	// Get all player starts
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), actors);

	// Save them in memory
	for (AActor* actor : actors)
	{
		auto* playerStart = Cast<APlayerStart>(actor);
		if (playerStarts.Find(playerStart) == -1)
			playerStarts.Add(playerStart);
	}
}

void ASpawnManager::OnSpawnPlayer(AGameModeBase* gamemode, APlayerController* controller)
{
	// Check if the current controller is not already registered
	if (controllers.Find(controller) != -1)
		return;

	controllers.Add(controller);

	SpawnControllerAtPlayerStart(controller);
}

void ASpawnManager::SpawnControllerAtPlayerStart(APlayerController* controller)
{
	if (playerStarts.Num() == 0)
		return;

	// Get the last used player start index 
	int playerStartIndex = lastPlayerStart++ % playerStarts.Num();

	// Set the current transform with the player start transform
	APlayerStart* currentStart = playerStarts[playerStartIndex];

	FTransform spawnTransform = currentStart->GetActorTransform();

	if (controller->AcknowledgedPawn)
		controller->AcknowledgedPawn->SetActorLocationAndRotation(spawnTransform.GetLocation(), spawnTransform.GetRotation());
}