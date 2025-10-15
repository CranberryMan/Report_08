// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemSpawnRow.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

UCLASS()
class TRACK_3_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnVolume();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TObjectPtr<USceneComponent> Scene;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TObjectPtr<class UBoxComponent> SpawningBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	UDataTable* ItemDataTable;
	
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	AActor* SpawnRandomItem();

	FVector GetRandomPointInVloume() const;
	FItemSpawnRow* GetRandomItem() const;
	AActor* SpawnItem(TSubclassOf<AActor> ItemClass);

	int32 itemCount = 0;

	TArray<AActor*> WaveActor;

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void ClearWaveActor();

};
