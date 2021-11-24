#include "Rocket.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "DamageableInterface.h"

// Sets default values
ARocket::ARocket()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    ProjectileColliderComp = CreateDefaultSubobject<USphereComponent>(TEXT("Projectile comp"));
    ProjectileColliderComp->BodyInstance.SetCollisionProfileName("Projectile");
    //CollisionComp->OnComponentHit.AddDynamic(this, &AMyProjectProjectile::OnHit);        // set up a notification for when this component hits something blocking

    RootComponent = ProjectileColliderComp;

    // Use a ProjectileMovementComponent to govern this projectile's movement
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile movement component"));
    ProjectileMovement->UpdatedComponent = ProjectileColliderComp;
    ProjectileMovement->InitialSpeed = 2000.f;
    ProjectileMovement->MaxSpeed = 3000.f;
    ProjectileMovement->ProjectileGravityScale = 0.f;

    BoxColliderComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box collider"));
    BoxColliderComp->BodyInstance.SetCollisionProfileName("BlockAllDynamic");
    BoxColliderComp->SetupAttachment(RootComponent);

    // set projectil collider
    HeadColliderComp = CreateDefaultSubobject<USphereComponent>(TEXT("Head collider"));
    HeadColliderComp->BodyInstance.SetCollisionProfileName("OverlapAll");
    HeadColliderComp->OnComponentBeginOverlap.AddDynamic(this, &ARocket::OnOverlap);		// set up a notification for when this component hits something blocking
    HeadColliderComp->SetupAttachment(RootComponent);
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

void ARocket::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this || !OtherComp)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot find actors and components"));
        return;
    }

    OnExplosion.Broadcast();

    bool bIsImplemented = OtherActor->Implements<UDamageableInterface>(); // bIsImplemented will be true if OriginalObject implements UReactToTriggerInterfacce.

    if (bIsImplemented)
    {
        IDamageableInterface* Damageable = Cast<IDamageableInterface>(OtherActor); // ReactingObject will be non-null if OriginalObject implements UReactToTriggerInterface.
        Damageable->OnDamage(1);
    }

    Destroy();
}