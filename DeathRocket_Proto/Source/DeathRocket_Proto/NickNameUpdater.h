#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NickNameUpdater.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEATHROCKET_PROTO_API UNickNameUpdater : public UActorComponent
{
	GENERATED_BODY()

private:
	//viewed nicknames
	TArray<class UWidgetComponent*>				widgets;

	class ADeathRocket_ProtoCharacter*			self;
	class UCameraComponent*						viewerCamera;
	FColor										viewerTeam;
	TArray<class ADeathRocket_ProtoCharacter*>	viewedPlayers;

	//used for raycast
	TArray<AActor*> ActorsToIgnore;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

public:	
	// Sets default values for this component's properties
	UNickNameUpdater();

	UFUNCTION(BlueprintCallable)
	void AddWidgetForPlayer(class UWidgetComponent* widget, AActor* player);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};