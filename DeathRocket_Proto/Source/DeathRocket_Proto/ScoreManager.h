#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ScoreManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDisplayEvent, AController*, query);

UCLASS()
class DEATHROCKET_PROTO_API AScoreManager : public AActor
{
	GENERATED_BODY()

private:
	TMap<FString, int> globalScore;
	
public:	
	// Sets default values for this actor's properties
	AScoreManager();

	void Init();

	void DisplayScore(class ADeathRocket_ProtoCharacter* query);
	void HideScore(class ADeathRocket_ProtoCharacter* query);

	UPROPERTY(BlueprintAssignable, Category = Event)
	FDisplayEvent OnDisplay;

	UPROPERTY(BlueprintAssignable, Category = Event)
	FDisplayEvent OnHide;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};