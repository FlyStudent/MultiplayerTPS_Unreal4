#include "MenuGameMode.h"

#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerController.h"

AMenuGameMode::AMenuGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/MenuCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	for (int i = 0; i < 4; i++)
		playerConnected.Add(false);

}

void AMenuGameMode::StartPlay()
{
	Super::StartPlay();

	// Get all player starts
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), actors);
	UGameplayStatics::SetForceDisableSplitscreen(GetWorld(), true);

	// Save them in memory
	for (AActor* actor : actors)
		playerStarts.Add(Cast<APlayerStart>(actor));

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
	// Set the current transform with the player start transform
	APlayerStart* currentStart = playerStarts[UGameplayStatics::GetPlayerControllerID(controller) + 4];

	FVector initialScale = controller->AcknowledgedPawn->GetActorScale();

	FTransform spawnTransform = currentStart->GetActorTransform();
	controller->AcknowledgedPawn->SetActorTransform(spawnTransform);

	controller->AcknowledgedPawn->SetActorScale3D(initialScale);
}
