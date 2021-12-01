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
	TArray<AActor*> ActorsToIgnore;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(VisibleDefaultsOnly)
	class UBoxComponent* BoxColliderComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* HeadComp;

	//the player that shoots this rocket
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class ADeathRocket_ProtoCharacter* shooter;

public:	
	UPROPERTY(EditAnywhere);
	float selfDamageMultiplier = 0.25f;

	UPROPERTY(EditAnywhere);
	float selfLaunchForceMultiplier = 2.f;

	UPROPERTY(EditAnywhere);
	int damage = 25.f;

	UPROPERTY(EditAnywhere);
	int impulseForce = 500.f;

	UPROPERTY(EditAnywhere);
	int launchForce = 5.f;

	UPROPERTY(EditAnywhere);
	float damageRadius = 100.f;

	UPROPERTY(EditAnywhere);
	float distanceMultiplier = 1.f;

	// Sets default values for this actor's properties
	ARocket();

	void Initialize(const FVector& direction);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(BlueprintAssignable, Category = "Components|Explosion")
	FRocketDelegate OnExplosion;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
