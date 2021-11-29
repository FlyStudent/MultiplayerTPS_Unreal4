#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DeathRocket_GameInstance.generated.h"

USTRUCT(BlueprintType)
struct FPlayerParams
{
	GENERATED_BODY()

	bool instance;
	FColor team;
};

UCLASS()
class DEATHROCKET_PROTO_API UDeathRocket_GameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	UPROPERTY(BlueprintReadOnly)
	TArray<FPlayerParams> playerParams;

public:
	UDeathRocket_GameInstance();

	void SetPlayerParams();
};
