#include "ScoreManager.h"

#include "Kismet/GameplayStatics.h"
#include "DeathRocket_ProtoCharacter.h"

// Sets default values
AScoreManager::AScoreManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


}

void AScoreManager::UpdateEveryTeams()
{
	for (FTeamScoring& team : teams)
	{
		team.kills = team.areas = team.total = 0;
		for (auto player : team.players)
		{
			team.kills += player->GetKillsCount();
			team.areas += player->GetCaptureCount();
		}

		team.total = team.kills + team.areas;
	}
}

const TArray<AActor*>& AScoreManager::GetEveryPlayers() const
{
	return players;
}

const TArray<FTeamScoring>& AScoreManager::GetEveryTeams()
{
	UpdateEveryTeams();
	return teams;
}

// Called when the game starts or when spawned
void AScoreManager::BeginPlay()
{
	Super::BeginPlay();

	// Get all player starts
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADeathRocket_ProtoCharacter::StaticClass(), actors);

	// Save them in memory
	for (AActor* actor : actors)
	{
		players.Add(actor);

		ADeathRocket_ProtoCharacter* player = Cast<ADeathRocket_ProtoCharacter>(actor);
		player->scoreManager = this;

		int32 id = teamColors.Find(player->GetTeam());
		if (id != INDEX_NONE)
			teams[id].players.Add(player);
		else
		{
			id = teams.Add(FTeamScoring(player->GetTeam()));
			teams[id].players.Add(player);
			teamColors.Add(player->GetTeam());
		}
	}
}