#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SprintComponent.generated.h"


UCLASS( ClassGroup=(Custom), Blueprintable, meta=(BlueprintSpawnableComponent) )
class DEATHROCKET_PROTO_API USprintComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USprintComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
