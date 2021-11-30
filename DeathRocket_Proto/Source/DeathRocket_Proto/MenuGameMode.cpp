#include "MenuGameMode.h"

#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

AMenuGameMode::AMenuGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/MainMenu/MenuCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
		DefaultPawnClass = PlayerPawnBPClass.Class;

	for (int i = 0; i < 4; i++)
		playerConnected.Add(false);
}

void AMenuGameMode::StartPlay()
{
	Super::StartPlay();

	UGameplayStatics::SetForceDisableSplitscreen(GetWorld(), true);

	// Save them in memory
	for (float f = 0; f < 4 * 270.f; f += 270.f)
	{
		FVector newLoc = FVector(firstPlayerLocation.X, firstPlayerLocation.Y + f, firstPlayerLocation.Z);
		playerFlags.Add(newLoc);
	}
}

void AMenuGameMode::SetPlayer(APlayerController* controller)
{
	int id = UGameplayStatics::GetPlayerControllerID(controller);
	playerConnected[id] = true;
	SpawnControllerAtPlayerStart(controller);
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString("Join"));
	OnPlayerJoin.Broadcast(id);
}

void AMenuGameMode::SpawnControllerAtPlayerStart(APlayerController* controller)
{
	if (playerFlags.Num() > 0)
	{
		// Set the current transform with the player start transform
		FVector currentStart = playerFlags[UGameplayStatics::GetPlayerControllerID(controller)];
		controller->AcknowledgedPawn->SetActorLocation(currentStart);
	}
}
