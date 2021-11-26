// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeathRocket_ProtoGameMode.h"
#include "DeathRocket_ProtoCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerController.h"

#include "ScoreManager.h"

ADeathRocket_ProtoGameMode::ADeathRocket_ProtoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	FGameModeEvents::GameModePostLoginEvent.AddUObject(this, &ADeathRocket_ProtoGameMode::OnSpawnPlayer);
}

void ADeathRocket_ProtoGameMode::StartPlay()
{
	Super::StartPlay();

	// Get all player starts
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), actors);

	// Save them in memory
	for (AActor* actor : actors)
		playerStarts.Add(Cast<APlayerStart>(actor));
}

void ADeathRocket_ProtoGameMode::SpawnControllerAtPlayerStart(APlayerController* controller)
{
	// Get the last used player start index 
	int playerStartIndex = lastPlayerStart % playerStarts.Num();

	// Set the current transform with the player start transform
	APlayerStart* currentStart = playerStarts[playerStartIndex];

	controller->AcknowledgedPawn->SetActorTransform(currentStart->GetActorTransform());
}

void ADeathRocket_ProtoGameMode::OnSpawnPlayer(AGameModeBase* gamemode, APlayerController* controller)
{
	// Check if the current controller is not already registered
	if (controllers.Find(controller))
		return;

	controllers.Add(controller);

	SpawnControllerAtPlayerStart(controller);
}