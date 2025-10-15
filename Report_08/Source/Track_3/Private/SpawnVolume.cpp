// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"
#include "Components/BoxComponent.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawningBox->SetupAttachment(Scene);

}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	
}

AActor* ASpawnVolume::SpawnItem(TSubclassOf<AActor> _Item)
{
	if (!_Item) return nullptr;

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(_Item, GetRandomPointInVloume(), FRotator::ZeroRotator);

	WaveActor.Add(SpawnedActor);
	return SpawnedActor;
}

void ASpawnVolume::ClearWaveActor()
{
	if (WaveActor.Num() == 0) return;
	for (AActor* Actor : WaveActor)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
	}
}

AActor* ASpawnVolume::SpawnRandomItem()
{
	itemCount++;
	// UE_LOG(LogTemp, Warning, TEXT("Item Count: %d"), itemCount);
	if (FItemSpawnRow* SelectedRow = GetRandomItem())
	{
		if (UClass* ActualClass = SelectedRow->ItemClass.Get())
		{
			return SpawnItem(ActualClass);
		}
	}
	
	return nullptr;
}

FVector ASpawnVolume::GetRandomPointInVloume() const
{

	FVector BoxExtent = SpawningBox->GetScaledBoxExtent();
	FVector BoxOrigin = SpawningBox->GetComponentLocation();
	return BoxOrigin + FVector(
		FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
		FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
		FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z)
	);
}

FItemSpawnRow* ASpawnVolume::GetRandomItem() const
{
	if (!ItemDataTable) return nullptr;
	TArray<FItemSpawnRow*> AllRows;

	static const FString ContextString(TEXT("Item Data Table Context"));

	ItemDataTable->GetAllRows(ContextString, AllRows);
	
	if (AllRows.Num() == 0) return nullptr;

	float TotalChance = 0.0f;
	for (const FItemSpawnRow* Row : AllRows)
	{
		TotalChance += Row->SpawnChance;
	}
	const float RandomValue = FMath::FRandRange(0.0f, TotalChance);
	float AccumlateChance = 0.f;

	for (FItemSpawnRow* Row : AllRows)
	{
		AccumlateChance += Row->SpawnChance;
		if(RandomValue <= AccumlateChance)
			return Row; // 실질적 찾은 Item 전달
	}

	return nullptr;
}
