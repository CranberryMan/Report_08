// Fill out your copyright notice in the Description page of Project Settings.


#include "Healingtem.h"
#include "SpartaCharacter.h"


AHealingtem::AHealingtem()
{
	HealingAmount = 20;
	ItemType = "HealingItem";
}

void AHealingtem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("Heal +%d"), HealingAmount));

		if (ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(Activator))
		{
			// 캐릭터의 체력을 회복
			PlayerCharacter->AddHealth(HealingAmount);
		}

		DestroyItem();
	}
}
