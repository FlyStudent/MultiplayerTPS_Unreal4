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
	void AreaConnect();
	UFUNCTION()
	void AreaDisconnect();
	UFUNCTION()
	void AreaDestroyed();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	class ACaptureArea* currentArea;
	bool isEntered = false;
	bool isCapturing = false;

	int captureCount = 0;

	UFUNCTION()
	void BeginAreaCapture();
	UFUNCTION()
	void StopAreaCapture();
	UFUNCTION()
	void SearchArea();

public:	

	FColor teamColor = FColor::Blue;

	UCaptureComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	FVector GetAreaLocation() const;

	UFUNCTION()
	void BeginOverlap();
	UFUNCTION()
	void EndOverlap();

	UPROPERTY(BlueprintAssignable, Category = Event)
	FCaptureDelegate OnAreaDetected;
	UPROPERTY(BlueprintAssignable, Category = Event)
	FCaptureDelegate OnCaptureCompleted;
	UPROPERTY(BlueprintAssignable, Category = Event)
	FCaptureDelegate OnEnteringArea;
	UPROPERTY(BlueprintAssignable, Category = Event)
	FCaptureDelegate OnExitingArea;
	UPROPERTY(BlueprintAssignable, Category = Event)
	FCaptureDelegate OnAreaDestroyed;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool AreaDetected() const { return currentArea != nullptr; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int GetCaptureCount() const { return captureCount; }
};
