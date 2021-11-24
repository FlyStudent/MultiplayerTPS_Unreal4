#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CaptureArea.generated.h"

UCLASS()
class DEATHROCKET_PROTO_API ACaptureArea : public AActor
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* areaCollider;


protected:
	virtual void BeginPlay() override;

public:	
	float curPercent;

	ACaptureArea();

	virtual void Tick(float DeltaTime) override;
};
