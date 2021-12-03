// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHurtDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDeathDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInvicibilityDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLifeChangedDelegate, int, newLife);

UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class DEATHROCKET_PROTO_API UHealthComponent : public UActorComponent
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Life, meta = (AllowPrivateAccess = "true"))
    int life;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Life, meta = (AllowPrivateAccess = "true"))
    int maxLife = 5;

    FTimerHandle invicibleTimer;

    bool isInvicible = false;
    bool isAlive = false;

    UFUNCTION()
    void ResetInvicibility();

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Life, meta = (AllowPrivateAccess = "true"))
    float invicibleCooldown;

    UFUNCTION(BlueprintCallable)
    void Hurt(int damage);

    UFUNCTION(BlueprintCallable)
    void Kill();

    void SetLife(int value);

    void Reset();

    virtual void BeginPlay() override;

    int GetLife() { return life; }
    int GetMaxLife() { return maxLife; }
    bool GetIsAlive() { return isAlive; }

    UFUNCTION(BlueprintCallable)
    float GetLifePercent() { return (float)life / (float)maxLife; }

    UPROPERTY(BlueprintAssignable, Category = "Components|Life")
    FLifeChangedDelegate OnLifeChanged;

    UPROPERTY(BlueprintAssignable, Category = "Components|Life")
    FInvicibilityDelegate OnInvicibilityStop;

    UPROPERTY(BlueprintAssignable, Category = "Components|Life")
    FHurtDelegate OnHurt;

    UPROPERTY(BlueprintAssignable, Category = "Components|Life")
    FDeathDelegate OnKill;
};