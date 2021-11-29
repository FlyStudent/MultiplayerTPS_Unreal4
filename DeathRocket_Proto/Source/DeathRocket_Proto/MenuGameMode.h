#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MenuGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameModeDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerJoinDelegate, int, ID);

UCLASS()
class DEATHROCKET_PROTO_API AMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
private:
	TArray<class APlayerStart*> playerStarts;

	void StartPlay() override;
	void SpawnControllerAtPlayerStart(APlayerController* controller);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool waiting = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<bool> playerConnected;

	AMenuGameMode();

	UFUNCTION(BlueprintCallable)
	void SetPlayer(APlayerController* controller);

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FGameModeDelegate OnGoToSelection;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FGameModeDelegate OnGoToMainMenu;
	UPROPERTY(BlueprintAssignable)
	FPlayerJoinDelegate OnPlayerJoin;
};
