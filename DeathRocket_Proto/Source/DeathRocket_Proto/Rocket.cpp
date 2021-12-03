#include "Rocket.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/BoxComponent.h"
#include "DamageableInterface.h"
#include "DamageableInterface.h"
#include "GameFramework/Character.h"
#include "DeathRocket_ProtoCharacter.h"

// Sets default values
ARocket::ARocket()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    OnDestroyed.AddDynamic(this, &ARocket::Explode);

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

    shooter = Cast<ADeathRocket_ProtoCharacter>(GetOwner());
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

void ARocket::Explode(AActor* self)
{
    FVector position = GetActorLocation();

    OnExplosion.Broadcast();

    TArray<AActor*> overlappedActors;
    UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(), damageRadius, ObjectTypes, AActor::StaticClass(), ActorsToIgnore, overlappedActors);

    for (AActor* overlappedActor : overlappedActors)
    {
        if (overlappedActor == this)
            continue;

        FVector overlappedActorLocation = overlappedActor->GetActorLocation();
        FVector difference = overlappedActorLocation - position;
        float distance = difference.Size();
        float oneOverDistance = 1.f / distance;

        float power = distanceMultiplier * oneOverDistance;

        if (overlappedActor->Implements<UDamageableInterface>())
        {
            IDamageableInterface* Damageable = Cast<IDamageableInterface>(overlappedActor); // ReactingObject will be non-null if OriginalObject implements UReactToTriggerInterface.

            int distanceDamage = power * (float)damage;

            if (overlappedActor == GetOwner())
                distanceDamage *= selfDamageMultiplier;

            Damageable->OnDamage(GetOwner(), distanceDamage);
        }

        FVector direction = difference * oneOverDistance;

        if (auto* physicComp = Cast<UPrimitiveComponent>(overlappedActor->GetComponentByClass(UPrimitiveComponent::StaticClass())))
        {
            FVector distanceImpulse = direction * impulseForce;

            if (physicComp->IsSimulatingPhysics())
                physicComp->AddImpulseAtLocation(distanceImpulse, overlappedActorLocation);
        }

        if (auto* character = Cast<ACharacter>(overlappedActor))
        {
            FVector distanceLaunch = direction * launchForce;

            if (overlappedActor == GetOwner())
                distanceLaunch *= selfLaunchForceMultiplier;

            character->LaunchCharacter(distanceLaunch, true, true);
        }
    }
}

void ARocket::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!HitComponent || !OtherActor || OtherActor == this || !OtherComp)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot find actors and components"));
        return;
    }

    Destroy();
}