#include "HealthComponent.h"

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
    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Die!"));

    OnKill.Broadcast();
}

void UHealthComponent::SetLife(int value)
{
    life = value;

    if (OnLifeChanged.IsBound())
        OnLifeChanged.Broadcast(life);

    if (life <= 0)
        Kill();
}