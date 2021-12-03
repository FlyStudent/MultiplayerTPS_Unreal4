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

	teamList.Add(FColor::Blue);
	teamList.Add(FColor::Red);
	teamList.Add(FColor::Orange);
	teamList.Add(FColor::Green);
	teamList.Add(FColor::Purple);
	teamList.Add(FColor::MakeRandomColor());

	AddNickname("Player 1");
	AddNickname("Player 2");
	AddNickname("Ironman");
	AddNickname("Chocolate");
	AddNickname("Winner");
	AddNickname("Loser");
	AddNickname("Fly64");
	AddNickname("Dr3auxis");
	AddNickname("Damien");
	AddNickname("Rémi");

	OnGoToSelection.AddDynamic(this, &AMenuGameMode::ResetSelectionMenu);
}

void AMenuGameMode::AddNickname(const FString& nickname)
{
	nicknameList.Add(nickname);
	nicknameAccess.Add(true);
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
	connectedCount++;

	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString("Player number ") + FString::FromInt(id));
	bool prev = true;
	goodConnectionOrder = true;
	for (bool& connected : playerConnected)
	{
		if (goodConnectionOrder && !prev && connected)
		{
			goodConnectionOrder = false;
			break;
		}
		prev = connected;
	}

	SpawnControllerAtPlayerStart(controller);
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

void AMenuGameMode::ResetSelectionMenu()
{
	for (bool& connect : playerConnected)
		connect = false;

	connectedCount = 0;
	goodConnectionOrder = false;

	// Remove player spawn
}

void AMenuGameMode::PlayGame()
{
	// Reset player count
	for (int i = 3; i >= 0; i--)
		UGameplayStatics::RemovePlayer(UGameplayStatics::GetPlayerController(GetWorld(), i), true);

	UGameplayStatics::SetForceDisableSplitscreen(GetWorld(), false);
	UGameplayStatics::OpenLevel(GetWorld(), "ThirdPersonExampleMap");

}

void AMenuGameMode::FreeNickname(int index)
{
	if (index >= 0 && index < nicknameAccess.Num())
		nicknameAccess[index] = true;
}


bool AMenuGameMode::TryGetNickname(int index, FString& nickname)
{
	if (index < 0 && index >= nicknameAccess.Num())
		return false;

	if (nicknameAccess[index])
	{
		nickname = nicknameList[index];
		nicknameAccess[index] = false;
		return true;
	}

	return false;
}