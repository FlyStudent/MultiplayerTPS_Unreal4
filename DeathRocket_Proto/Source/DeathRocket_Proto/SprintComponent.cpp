#include "SprintComponent.h"

// Sets default values for this component's properties
USprintComponent::USprintComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void USprintComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void USprintComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

