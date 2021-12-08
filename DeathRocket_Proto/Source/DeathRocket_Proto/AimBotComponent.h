#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AimBotComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEATHROCKET_PROTO_API UAimBotComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	TArray<AActor*> enemies;

	//pick every players of the map except self
	void PickEveryEnemies();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	//the enemies will be processed if they are in front of the player (at a certain angle)
	float maximumSight = PI * 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	//the closest enemy to the crosshair (processed with the maximumSight) will be targeted if
	//it is close to the crosshair enough (a the minumumAccuracy angle)
	float minimumAccuracy = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float aimBotStrengh = 4.f;

	AActor* target = nullptr;

	// Sets default values for this component's properties
	UAimBotComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//returns the closest target to the crosshair, or nullptr
	void SelectTarget(const FVector& cameraDir, const FVector& aimerPos);
	//resets target to nullptr
	void LoseTarget();
	//check if target is still in sight
	void CheckTarget(const FVector& cameraDir, const FVector& aimerPos);
};
