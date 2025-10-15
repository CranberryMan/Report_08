// Fill out your copyright notice in the Description page of Project Settings.


#include "CoinItem.h"
#include "SpartaGameStateBase.h"


ACoinItem::ACoinItem()
{
	PointValue = 0;
	ItemType = "DefaultCoin";
}

void ACoinItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
	if(Activator && Activator->ActorHasTag("Player"))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("COIN +%d"), PointValue));

		if (UWorld* World = GetWorld())
		{
			if (ASpartaGameStateBase* GameState = World->GetGameState<ASpartaGameStateBase>())
			{
				GameState->AddScore(PointValue);
				GameState->OnCoinCollected();
			}
		}


		DestroyItem();
	}
}
