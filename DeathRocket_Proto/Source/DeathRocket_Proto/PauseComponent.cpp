#include "PauseComponent.h"

#include "Kismet/GameplayStatics.h"
#include "DeathRocket_ProtoCharacter.h"

// Sets default values for this component's properties
UPauseComponent::UPauseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UPauseComponent::PickEveryPlayers()
{
	// Get all player starts
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADeathRocket_ProtoCharacter::StaticClass(), actors);

	// Save them in memory
	for (AActor* actor : actors)
	{
		ADeathRocket_ProtoCharacter* player = Cast<ADeathRocket_ProtoCharacter>(actor);

		//don't take self
		if (player->pauseComp == this)
			continue;

		players.Add(actor);
	}
}

// Called when the game starts
void UPauseComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UPauseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPauseComponent::Pause()
{
	if (players.Num() == 0)
		PickEveryPlayers();

	for (AActor* actor : players)
	{
		ADeathRocket_ProtoCharacter* player = Cast<ADeathRocket_ProtoCharacter>(actor);

		player->pauseComp->OnPause.Broadcast(false);
	}

	//self has a different pause UI
	OnPause.Broadcast(true);

	UGameplayStatics::SetGamePaused(GetWorld(), !GetWorld()->IsPaused());
}

bool UPauseComponent::IsGamePaused() const
{
	return GetWorld()->IsPaused();
}