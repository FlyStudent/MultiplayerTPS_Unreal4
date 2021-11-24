#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CaptureArea.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCaptureAreaDelegate);


UCLASS()
class DEATHROCKET_PROTO_API ACaptureArea : public AActor
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* areaCollider;

	UFUNCTION()
	void AreaCaptured();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float curPercent;
	int tickFactor = 1;
	class UCaptureComponent* capturingActor = nullptr;
	class UCaptureComponent* previousCapturingActor = nullptr;

public:	


	ACaptureArea();

	virtual void Tick(float DeltaTime) override;

	float TickCapturePercent(UCaptureComponent* actor, float deltaPercent);

	bool TryCaptureArea(class UCaptureComponent* actor);
	void ExitCaptureArea(class UCaptureComponent* actor);

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(BlueprintAssignable, Category = Event)
	FCaptureAreaDelegate OnCaptureCompleted;
	UPROPERTY(BlueprintAssignable, Category = Event)
	FCaptureAreaDelegate OnCaptureProcess;
};
