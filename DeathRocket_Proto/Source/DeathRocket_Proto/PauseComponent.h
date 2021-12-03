#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PauseComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPauseEvent);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEATHROCKET_PROTO_API UPauseComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	TArray<AActor*> players;

	//pick every players of the map except self
	void PickEveryPlayers();

public:	
	// Sets default values for this component's properties
	UPauseComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable, Category = Event)
	FPauseEvent OnPause;

	void Pause();
	UFUNCTION(BlueprintCallable)
	bool IsGamePaused() const;
};