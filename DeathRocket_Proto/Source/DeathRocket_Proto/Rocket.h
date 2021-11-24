#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Rocket.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRocketDelegate);

UCLASS()
class DEATHROCKET_PROTO_API ARocket : public AActor
{
	GENERATED_BODY()
	
private:

	UPROPERTY(VisibleDefaultsOnly)
	class USphereComponent* ProjectileColliderComp;

	UPROPERTY(VisibleDefaultsOnly)
	class UBoxComponent* BoxColliderComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* HeadColliderComp;

public:	
	// Sets default values for this actor's properties
	ARocket();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(BlueprintAssignable, Category = "Components|Explosion")
	FRocketDelegate OnExplosion;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
