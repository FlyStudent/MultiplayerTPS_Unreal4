#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KillFeedManager.generated.h"

UCLASS()
class DEATHROCKET_PROTO_API AKillFeedManager : public AActor
{
	GENERATED_BODY()

private:
	TArray<AActor*> players;
	
public:	
	// Sets default values for this actor's properties
	AKillFeedManager();

	void KillHappened(const FString& killerName,
					  const FColor& killerColor,
					  const FString& victimName,
					  const FColor& victimColor);
	void CaptureHappened(const FString& name, const FColor& color);
};