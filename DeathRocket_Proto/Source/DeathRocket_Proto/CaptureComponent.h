#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CaptureComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCaptureDelegate)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEATHROCKET_PROTO_API UCaptureComponent : public UActorComponent
{
	GENERATED_BODY()

private:	

protected:
	virtual void BeginPlay() override;

	class ACaptureArea* capturingArea;
	bool isCapturing = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float captureTime = 10.f;

public:	
	UCaptureComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(BlueprintAssignable, Category = Event)
	FCaptureDelegate OnCaptureCompleted;
};
