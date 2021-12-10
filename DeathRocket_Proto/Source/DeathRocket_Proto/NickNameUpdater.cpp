#include "NickNameUpdater.h"

#include "Components/WidgetComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Camera/CameraComponent.h"

#include "DeathRocket_ProtoCharacter.h"

// Sets default values for this component's properties
UNickNameUpdater::UNickNameUpdater()
	: Super()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void UNickNameUpdater::AddWidgetForPlayer(UWidgetComponent* widget, AActor* player)
{
	//set values
	if (!self)
	{
		self = Cast<ADeathRocket_ProtoCharacter>(GetOwner());
		viewerCamera = self->GetFollowCamera();
		viewerTeam = self->GetTeam();

		ActorsToIgnore.Add(self);
	}

	widgets.Add(widget);

	ADeathRocket_ProtoCharacter* vp = Cast<ADeathRocket_ProtoCharacter>(player);
	viewedPlayers.Add(vp);

	//always display nickname if teammate
	if (viewerTeam == vp->GetTeam())
		widget->SetVisibility(true);
}

// Called when the game starts
void UNickNameUpdater::BeginPlay()
{
	Super::BeginPlay();

	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
}

// Called every frame
void UNickNameUpdater::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	int widgetIndex = 0;

	for (ADeathRocket_ProtoCharacter* vp : viewedPlayers)
	{
		if (viewerTeam == vp->GetTeam())
		{
			++widgetIndex;
			continue;
		}

		FHitResult hitObject;
		bool hit = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(),
			viewerCamera->GetComponentLocation(),
			vp->GetActorLocation(),
			ObjectTypes, false, ActorsToIgnore,
			EDrawDebugTrace::None, hitObject, true, FColor::White, FColor::Red, 0.3f);

		widgets[widgetIndex]->SetVisibility(hitObject.Actor.Get() == vp);
		++widgetIndex;
	}
}