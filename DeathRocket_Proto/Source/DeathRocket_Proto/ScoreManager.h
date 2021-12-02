#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ScoreManager.generated.h"

USTRUCT(BlueprintType)
struct FTeamScoring
{
	GENERATED_BODY()

	FTeamScoring() {};
	FTeamScoring(const FColor& c)
		: color(c) {};

	TArray<class ADeathRocket_ProtoCharacter*> players;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FColor color = FColor::Blue;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int kills = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int areas = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int total = 0;
};

UCLASS()
class DEATHROCKET_PROTO_API AScoreManager : public AActor
{
	GENERATED_BODY()

private:
	TArray<AActor*> players;
	TArray<FTeamScoring> teams;
	TArray<FColor> teamColors;
	
public:	
	// Sets default values for this actor's properties
	AScoreManager();

	UFUNCTION(BlueprintCallable)
	const TArray<AActor*>& GetEveryPlayers() const;
	UFUNCTION(BlueprintCallable)
	const TArray<FTeamScoring>& GetEveryTeams();

	void UpdateEveryTeams();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};