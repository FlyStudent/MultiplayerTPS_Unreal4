#include "NickNameManager.h"

#include "Kismet/GameplayStatics.h"
#include "DeathRocket_ProtoCharacter.h"

// Sets default values
ANickNameManager::ANickNameManager()
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
	}
}

// Called when the game starts or when spawned
void ANickNameManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ANickNameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const TArray<AActor*>& ANickNameManager::GetEveryPlayers() const
{
	return players;
}