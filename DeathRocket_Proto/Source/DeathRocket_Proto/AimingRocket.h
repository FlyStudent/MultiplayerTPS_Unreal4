#pragma once

#include "CoreMinimal.h"
#include "Rocket.h"
#include "AimingRocket.generated.h"

UCLASS()
class DEATHROCKET_PROTO_API AAimingRocket : public ARocket
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float maxAngleDetection = 30.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float rotationSpeed = 10.f;

public:
	AAimingRocket();

	virtual void Tick(float DeltaTime) override;
};
