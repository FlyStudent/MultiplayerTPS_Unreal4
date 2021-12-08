#include "AimBotComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "DeathRocket_ProtoCharacter.h"
#include "HealthComponent.h"

// Sets default values for this component's properties
UAimBotComponent::UAimBotComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

void UAimBotComponent::PickEveryEnemies()
{
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADeathRocket_ProtoCharacter::StaticClass(), actors);

	// Save them in memory
	for (AActor* actor : actors)
	{
		ADeathRocket_ProtoCharacter* enemy = Cast<ADeathRocket_ProtoCharacter>(actor);

		//don't take self
		if (enemy->aimBotComp == this)
			continue;

		enemies.Add(actor);
	}
}

// Called when the game starts
void UAimBotComponent::BeginPlay()
{
	Super::BeginPlay();

	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
}

// Called every frame
void UAimBotComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAimBotComponent::SelectTarget(const FVector& cameraDir, const FVector& aimerPos)
{
	if (enemies.Num() == 0)
		PickEveryEnemies();

	float smallestAngle = maximumSight;
	int iterationIndex = 0;
	int targetIndex = 0;

	for (AActor* actor : enemies)
	{
		ADeathRocket_ProtoCharacter* enemy = Cast<ADeathRocket_ProtoCharacter>(actor);

		if (!enemy->healthComp->GetIsAlive())
		{
			++iterationIndex;
			continue;
		}

		FHitResult hitObject;
		FVector offsetFromCaster = (enemy->GetActorLocation() - aimerPos).GetSafeNormal() * 50.f;
		bool hit = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(),
																   aimerPos + offsetFromCaster,
																   enemy->GetActorLocation(),
																   ObjectTypes, false, ActorsToIgnore,
																   EDrawDebugTrace::None, hitObject, true, FColor::White, FColor::Red, 0.3f);
		if (hitObject.Actor.Get() != actor)
		{
			++iterationIndex;
			continue;
		}

		FVector dirToEnemy = enemy->GetActorLocation() - aimerPos;
		dirToEnemy.Normalize();

		float cos = FVector::DotProduct(cameraDir.GetSafeNormal(), dirToEnemy);
		float angle = acosf(cos);	//angle in rad

		if (angle < smallestAngle)
		{
			smallestAngle = angle;
			targetIndex = iterationIndex;
		}

		++iterationIndex;
	}

	if (smallestAngle <= minimumAccuracy)
	{
		target = enemies[targetIndex];
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, enemies[targetIndex]->GetName());
	}
	else
		target = nullptr;
}

void UAimBotComponent::LoseTarget()
{
	target = nullptr;
}

void UAimBotComponent::CheckTarget(const FVector& cameraDir, const FVector& aimerPos)
{
	if (!target)
		return;

	ADeathRocket_ProtoCharacter* enemy = Cast<ADeathRocket_ProtoCharacter>(target);

	if (!enemy->healthComp->GetIsAlive())
	{
		LoseTarget();
		return;
	}

	FHitResult hitObject;
	FVector offsetFromCaster = (enemy->GetActorLocation() - aimerPos).GetSafeNormal() * 50.f;
	bool hit = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(),
															   aimerPos + offsetFromCaster,
															   enemy->GetActorLocation(),
															   ObjectTypes, false, ActorsToIgnore,
															   EDrawDebugTrace::None, hitObject, true, FColor::White, FColor::Red, 0.3f);
	if (hitObject.Actor.Get() != enemy)
	{
		LoseTarget();
		return;
	}

	FVector dirToEnemy = enemy->GetActorLocation() - aimerPos;
	dirToEnemy.Normalize();

	float cos = FVector::DotProduct(cameraDir.GetSafeNormal(), dirToEnemy);
	float angle = acosf(cos);	//angle in rad

	if (angle > minimumAccuracy)
	{
		LoseTarget();
	}
}