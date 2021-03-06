#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Rocket.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRocketDelegate);

UCLASS()
class DEATHROCKET_PROTO_API ARocket : public AActor
{
	GENERATED_BODY()
	
protected:
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
	float selfDamageMultiplier = 0.7f;

	UPROPERTY(EditAnywhere);
	float selfLaunchForceMultiplier = 5.f;

	UPROPERTY(EditAnywhere);
	int damage = 50;

	UPROPERTY(EditAnywhere);
	int impulseForce = 250000;

	UPROPERTY(EditAnywhere);
	int launchForce = 5000;

	UPROPERTY(EditAnywhere);
	float damageRadius = 250.f;

	UPROPERTY(EditAnywhere);
	float distanceMultiplier = 100.f;

	// Sets default values for this actor's properties
	ARocket();

	void Initialize(const FVector& direction);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void Explode(AActor* self);

public:	
	UPROPERTY(BlueprintAssignable, Category = "Components|Explosion")
	FRocketDelegate OnExplosion;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
