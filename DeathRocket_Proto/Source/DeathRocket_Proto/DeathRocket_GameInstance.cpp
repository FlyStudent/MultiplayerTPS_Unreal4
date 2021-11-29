#include "DeathRocket_GameInstance.h"

#include "MenuGameMode.h"

UDeathRocket_GameInstance::UDeathRocket_GameInstance()
{

}

void UDeathRocket_GameInstance::SetPlayerParams()
{
	auto gameMode = GetWorld()->GetAuthGameMode<AMenuGameMode>();
	if (gameMode)
	{

	}
}