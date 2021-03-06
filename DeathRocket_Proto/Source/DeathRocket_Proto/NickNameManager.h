#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NickNameManager.generated.h"

UCLASS()
class DEATHROCKET_PROTO_API ANickNameManager : public AActor
{
	GENERATED_BODY()
	
private:
	TArray<AActor*> players;

public:
	// Sets default values for this actor's properties
	ANickNameManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	const TArray<AActor*>& GetEveryPlayers() const;
};
