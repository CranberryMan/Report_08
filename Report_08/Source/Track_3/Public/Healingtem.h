// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "Healingtem.generated.h"

/**
 * 
 */
UCLASS()
class TRACK_3_API AHealingtem : public ABaseItem
{
	GENERATED_BODY()
	
public:
	AHealingtem();
	virtual void ActivateItem(AActor* Activator) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 HealingAmount;
};
