#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CaptureComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCaptureDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEATHROCKET_PROTO_API UCaptureComponent : public UActorComponent
{
	GENERATED_BODY()

private:	
	UFUNCTION()
	void AreaCapturedBySelf();
	UFUNCTION()
	void AreaDisconnect();

protected:
	virtual void BeginPlay() override;

	class ACaptureArea* capturingArea;
	bool isCapturing = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float captureTime = 6.f;

	UFUNCTION()
	void BeginAreaCapture();

public:	

	UCaptureComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FORCEINLINE void SetCapturingArea(class ACaptureArea* area) { capturingArea = area; }

	UFUNCTION()
	void BeginOverlap(ACaptureArea* area);
	UFUNCTION()
	void EndOverlap(ACaptureArea* area);

	UPROPERTY(BlueprintAssignable, Category = Event)
	FCaptureDelegate OnCaptureCompleted;
};
