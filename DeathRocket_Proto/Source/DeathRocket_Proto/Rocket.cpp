#include "Rocket.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
ARocket::ARocket()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere component"));
    CollisionComp->InitSphereRadius(5.0f);
    CollisionComp->BodyInstance.SetCollisionProfileName("Rocket");
    //CollisionComp->OnComponentHit.AddDynamic(this, &AMyProjectProjectile::OnHit);        // set up a notification for when this component hits something blocking

    // Players can't walk on it
    CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
    CollisionComp->CanCharacterStepUpOn = ECB_No;

    // Set as root component
    RootComponent = CollisionComp;

    // Use a ProjectileMovementComponent to govern this projectile's movement
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile movement component"));
    ProjectileMovement->UpdatedComponent = CollisionComp;
    ProjectileMovement->InitialSpeed = 2000.f;
    ProjectileMovement->MaxSpeed = 3000.f;
    ProjectileMovement->ProjectileGravityScale = 0.f;
}

// Called when the game starts or when spawned
void ARocket::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}