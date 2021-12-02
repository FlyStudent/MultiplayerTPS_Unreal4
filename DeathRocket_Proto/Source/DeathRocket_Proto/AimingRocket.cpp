#include "AimingRocket.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "DeathRocket_ProtoCharacter.h"

AAimingRocket::AAimingRocket()
{
}

void AAimingRocket::BeginPlay()
{
	Super::BeginPlay();
}

void AAimingRocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADeathRocket_ProtoCharacter::StaticClass(), actors);

	bool aim = false;
	float aimAngle = maxAngleDetection;
	int currentIndex = 0, playerIndex = 0;
	for (AActor* actor : actors)
	{
		auto player = Cast<ADeathRocket_ProtoCharacter>(actor);
		if (player && player != shooter)
		{
			FVector direction = (player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			float angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(GetActorForwardVector(), direction)));

			if (FMath::Abs(angle) < maxAngleDetection && FMath::Abs(angle) < aimAngle)
			{
				aimAngle = angle;
				playerIndex = currentIndex;
				aim = true;
			}
		}
		currentIndex++;
	}

	if (aim)
	{
		FRotator curRot = GetActorRotation();
		FRotator aimRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), actors[playerIndex]->GetActorLocation());

		SetActorRotation(FMath::Lerp<FRotator>(curRot, aimRot, rotationSpeed * DeltaTime));

		ProjectileMovement->Velocity = GetActorForwardVector() * ProjectileMovement->InitialSpeed;
	}
}