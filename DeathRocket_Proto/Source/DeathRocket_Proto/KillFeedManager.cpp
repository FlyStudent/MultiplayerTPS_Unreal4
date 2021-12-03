#include "KillFeedManager.h"

#include "Kismet/GameplayStatics.h"
#include "DeathRocket_ProtoCharacter.h"

// Sets default values
AKillFeedManager::AKillFeedManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Get all player starts
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADeathRocket_ProtoCharacter::StaticClass(), actors);

	// Save them in memory
	for (AActor* actor : actors)
	{
		players.Add(actor);

		ADeathRocket_ProtoCharacter* player = Cast<ADeathRocket_ProtoCharacter>(actor);
		player->killfeedManager = this;
	}
}

void AKillFeedManager::KillHappened(const FString& killerName, const FColor& killerColor, const FString& victimName, const FColor& victimColor)
{
	for (AActor* actor : players)
	{
		ADeathRocket_ProtoCharacter* player = Cast<ADeathRocket_ProtoCharacter>(actor);
		player->OnDisplayFeed.Broadcast(killerName, killerColor, victimName, victimColor);
	}
}

void AKillFeedManager::CaptureHappened(const FString& name, const FColor& color)
{
	for (AActor* actor : players)
	{
		ADeathRocket_ProtoCharacter* player = Cast<ADeathRocket_ProtoCharacter>(actor);
		player->OnDisplayCapture.Broadcast(name, color);
	}
}