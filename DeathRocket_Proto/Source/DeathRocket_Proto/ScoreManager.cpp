#include "ScoreManager.h"

#include "Kismet/GameplayStatics.h"
#include "DeathRocket_ProtoCharacter.h"

// Sets default values
AScoreManager::AScoreManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AScoreManager::Init()
{
	// Get all player starts
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADeathRocket_ProtoCharacter::StaticClass(), actors);

	// Save them in memory
	for (AActor* actor : actors)
	{
		ADeathRocket_ProtoCharacter* player = Cast<ADeathRocket_ProtoCharacter>(actor);
		globalScore.Add(player->GetName(), player->GetKillsCount());

		player->sm = this;
	}
}

// Called when the game starts or when spawned
void AScoreManager::BeginPlay()
{
	Super::BeginPlay();
}

void AScoreManager::DisplayScore(ADeathRocket_ProtoCharacter* query)
{
	OnDisplay.Broadcast(query->GetController());
}

void AScoreManager::HideScore(ADeathRocket_ProtoCharacter* query)
{
	OnHide.Broadcast(query->GetController());
}

// Called every frame
void AScoreManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}