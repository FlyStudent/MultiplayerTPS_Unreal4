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

    // set projectil collider
    HeadComp = CreateDefaultSubobject<USphereComponent>(TEXT("HeadCollider"));
    HeadComp->BodyInstance.SetCollisionProfileName("BlockAll");
    HeadComp->OnComponentHit.AddDynamic(this, &ARocket::OnHit);		// set up a notification for when this component hits something blocking
    HeadComp->SetupAttachment(RootComponent);

    RootComponent = HeadComp;

    // Use a ProjectileMovementComponent to govern this projectile's movement
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile movement component"));
    ProjectileMovement->UpdatedComponent = RootComponent;
    ProjectileMovement->InitialSpeed = 2000.f;
    ProjectileMovement->MaxSpeed = 3000.f;
    ProjectileMovement->ProjectileGravityScale = 0.f;

    BoxColliderComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box collider"));
    BoxColliderComp->BodyInstance.SetCollisionProfileName("BlockAllDynamic");
    BoxColliderComp->SetupAttachment(RootComponent);
}

void ARocket::Initialize(const FVector& direction)
{
    if (ProjectileMovement)
        ProjectileMovement->Velocity = direction * ProjectileMovement->InitialSpeed;
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

void ARocket::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!HitComponent || !OtherActor || OtherActor == this || !OtherComp)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot find actors and components"));
        return;
    }

    OnExplosion.Broadcast();

    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, OtherActor->GetName());

    bool bIsImplemented = OtherActor->Implements<UDamageableInterface>(); // bIsImplemented will be true if OriginalObject implements UReactToTriggerInterfacce.

    if (bIsImplemented)
    {
        IDamageableInterface* Damageable = Cast<IDamageableInterface>(OtherActor); // ReactingObject will be non-null if OriginalObject implements UReactToTriggerInterface.
        Damageable->OnDamage(GetOwner(), 2);
    }

    Destroy();
}