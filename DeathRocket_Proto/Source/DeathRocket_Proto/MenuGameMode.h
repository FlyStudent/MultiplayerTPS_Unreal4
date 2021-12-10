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
	TArray<FVector> playerFlags;
	FVector firstPlayerLocation = FVector(600.f, -400.f, 400.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FColor> teamList;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FString> nicknameList;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<bool> nicknameAccess;

	UFUNCTION(BlueprintCallable)
	void AddNickname(const FString& nickname);
	UFUNCTION(BlueprintCallable)
	void FreeNickname(int index);
	UFUNCTION(BlueprintCallable)
	bool TryGetNickname(int index, FString& nickname);

	void StartPlay() override;
	void SpawnControllerAtPlayerStart(APlayerController* controller);
	UFUNCTION()
	void ResetSelectionMenu();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool waiting = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<bool> playerConnected;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool goodConnectionOrder = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int connectedCount = 0;

	AMenuGameMode();

	UFUNCTION(BlueprintCallable)
	void SetPlayer(APlayerController* controller);
	UFUNCTION(BlueprintCallable)
	void PlayGame();

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FGameModeDelegate OnGoToSelection;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FGameModeDelegate OnGoToMainMenu;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FGameModeDelegate OnGoToModeSelection;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FGameModeDelegate OnGoToSettings;

	UPROPERTY(BlueprintAssignable)
	FPlayerJoinDelegate OnPlayerJoin;
};
