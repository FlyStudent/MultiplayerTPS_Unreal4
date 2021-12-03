#include "HealthComponent.h"

void UHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    Reset();
}

void UHealthComponent::Reset()
{
    SetLife(maxLife);

    isInvicible = false;
    isAlive = true;
    GetWorld()->GetTimerManager().ClearTimer(invicibleTimer);
}

void UHealthComponent::Hurt(int damage)
{
    if (isInvicible)
        return;

    SetLife(life - damage);

    if (OnHurt.IsBound())
        OnHurt.Broadcast();

    if (invicibleCooldown <= 0.f)
        return;

    isInvicible = true;
    GetWorld()->GetTimerManager().SetTimer(invicibleTimer, this, &UHealthComponent::ResetInvicibility, invicibleCooldown, false);
}

void UHealthComponent::ResetInvicibility()
{
    isInvicible = false;
    GetWorld()->GetTimerManager().ClearTimer(invicibleTimer);

    if (OnInvicibilityStop.IsBound())
        OnInvicibilityStop.Broadcast();
}

void UHealthComponent::Kill()
{
    if (!isAlive)
        return;

    isAlive = false;
    SetLife(0);

    OnKill.Broadcast();
}

void UHealthComponent::SetLife(int value)
{
    life = value;

    if (OnLifeChanged.IsBound())
        OnLifeChanged.Broadcast(life);

    if (isAlive && life <= 0)
        Kill();
}