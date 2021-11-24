#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UltimeLoaderComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUltimeLoaded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUltimeUsed);

UCLASS( ClassGroup=(Custom), Blueprintable, meta=(BlueprintSpawnableComponent) )
class DEATHROCKET_PROTO_API UUltimeLoaderComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	float ratio;
	float curPercent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess = "true"))
		float killIncreaePercent = 15;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess = "true"))
		float captureIncreasePercent = 50;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess = "true"))
		class Ultime* ulti = nullptr;
public:	
	UUltimeLoaderComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		
	UFUNCTION()
	void Increase(float value);
	UFUNCTION()
	void IncreaseByKill();
	UFUNCTION()
	void IncreaseByCapture();

	void SetUltime(class Ultime* ult);
	void Use();

	UPROPERTY(BlueprintAssignable, Category = "Components|Ultime")
	FUltimeLoaded OnUltimeLoaded;

	UPROPERTY(BlueprintAssignable, Category = "Components|Ultime")
	FUltimeUsed OnUltimeUsed;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetRatio() const { return ratio; }
};
