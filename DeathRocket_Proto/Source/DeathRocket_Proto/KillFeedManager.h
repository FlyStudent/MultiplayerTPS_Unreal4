#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KillFeedManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FKillFeedEvent, FString, killerName,
															  FColor, killerColor,
															  FString, victimName,
														      FColor, victimColor);

UCLASS()
class DEATHROCKET_PROTO_API AKillFeedManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKillFeedManager();

	UPROPERTY(BlueprintAssignable, Category = Event)
	FKillFeedEvent OnDisplayFeed;
};