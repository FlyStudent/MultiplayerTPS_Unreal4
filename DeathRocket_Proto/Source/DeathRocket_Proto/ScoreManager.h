#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ScoreManager.generated.h"

UCLASS()
class DEATHROCKET_PROTO_API AScoreManager : public AActor
{
	GENERATED_BODY()

private:
	TArray<AActor*> players;
	
public:	
	// Sets default values for this actor's properties
	AScoreManager();

	void Init();

	UFUNCTION(BlueprintCallable)
	const TArray<AActor*>& GetEveryPlayers() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};