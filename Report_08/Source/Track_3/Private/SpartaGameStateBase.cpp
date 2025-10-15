#include "SpartaGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "Engine/World.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

ASpartaGameStateBase::ASpartaGameStateBase()
{
    Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	LevelDuration = 10.0f; // �� ������ 30��
	// CurrentLevelIndex = 0;
	MaxLevels = 3;
}

void ASpartaGameStateBase::BeginPlay()
{
	Super::BeginPlay();
	StartLevel();
	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&ASpartaGameStateBase::UpdateHUD,
		0.1f,
		true
	);
}

int32 ASpartaGameStateBase::GetScore() const
{
    return Score;
}

void ASpartaGameStateBase::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance))
		{
			if (SpartaGameInstance)
			{
				SpartaGameInstance->AddToScore(Amount);
			}			
		}
	}

	//Score += Amount;
	//UE_LOG(LogTemp, Log, TEXT("Now Score: %d"), Score);
}


void ASpartaGameStateBase::StartLevel()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowGameHUD();
		}
	}

	ItemSpawan(nowWave * 10 + 40);
}

void ASpartaGameStateBase::OnLevelTimeUp()
{
	// �ð��� �� �Ǹ� ������ ����
	// nowWave�� 2���� ũ�� ���� ����
	// UE_LOG(LogTemp, Warning, TEXT("Wave %d Time Up!"), nowWave);
	if (nowWave > 2)
	{
		EndLevel();
		return;
	}
	
	if (SpawnVolume)
	{
		// ���� ���� ��, ���� ���� �ʱ�ȭ
		SpawnedCoinCount = 0;
		CollectedCoinCount = 0;
		SpawnVolume->ClearWaveActor();
		ItemSpawan(nowWave * 10 + 40);
	}
	
	// EndLevel();
}

void ASpartaGameStateBase::OnCoinCollected()
{
	CollectedCoinCount++;

	//UE_LOG(LogTemp, Warning, TEXT("Coin Collected: %d / %d"),
	//	CollectedCoinCount,
	//	SpawnedCoinCount)

		// ���� �������� ������ ������ ���� �ֿ��ٸ� ��� ���� ����
		if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
		{
			// EndLevel();
			// ���̺� Ŭ����� ����
			OnLevelTimeUp();
		}
}

void ASpartaGameStateBase::EndLevel()
{
	// Ÿ�̸� ����
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);

	// ���̺� �ʱ�ȭ
	nowWave = 0;

	// ���� ���� �ε�����
	int CurrentLevelIndex = 0;
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance))
		{
			if (SpartaGameInstance)
			{
				SpartaGameInstance->CurrentLevelIndex += 1;
				CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("CurrentLevelIndex %d!"), CurrentLevelIndex);
	// ��� ������ �� ���Ҵٸ� ���� ���� ó��
	if (CurrentLevelIndex >= MaxLevels)
	{
		OnGameOver();
		return;
	}

	// ���� �� �̸��� �ִٸ� �ش� �� �ҷ�����
	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		// �� �̸��� ������ ���ӿ���
		OnGameOver();
	}
}

void ASpartaGameStateBase::UpdateHUD()
{
	if(APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if(ASpartaPlayerController* SpartaPC = Cast<ASpartaPlayerController>(PC))
		{
			if(UUserWidget* HUDWidget = SpartaPC->GetHUDWiget())
			{
				int nowScore = 0;
				int nowLevel = 0;
				if (UGameInstance* GameInstance = GetGameInstance())
				{
					if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance))
					{
						if (SpartaGameInstance)
						{
							nowScore = SpartaGameInstance->TotalScore;
							nowLevel = SpartaGameInstance->CurrentLevelIndex + 1;
						}
					}
				}



				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("TIME : %.1f"), RemainingTime)));

				}

				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score : %d"), nowScore)));
				}

				if (UTextBlock* StageText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Stage"))))
				{
					StageText->SetText(FText::FromString(FString::Printf(TEXT("Stage %d"), nowLevel)));
				}


				if (UTextBlock* StageText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Coin"))))
				{
					StageText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), CollectedCoinCount, SpawnedCoinCount)));
				}

				//UE_LOG(LogTemp, Warning, TEXT("Coin Collected: %d / %d"),
				//	CollectedCoinCount,
				//	SpawnedCoinCount)


			}
		}
	}


}

void ASpartaGameStateBase::ItemSpawan(int32 TotalItemCount)
{

	// ���� ���� ��, ���� ���� �ʱ�ȭ
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	nowWave++;
	UpdateHUD();
	// ���� �ʿ� ��ġ�� ��� SpawnVolume�� ã�� ������ 40���� ����
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	const int32 ItemToSpawn = TotalItemCount;

	for (int32 i = 0; i < ItemToSpawn; i++)
	{
		if (FoundVolumes.Num() > 0)
		{
			SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				// ���� ������ ���Ͱ� ���� Ÿ���̶�� SpawnedCoinCount ����
				if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					SpawnedCoinCount++;
				}
			}
		}
	}

	// 30�� �Ŀ� OnLevelTimeUp()�� ȣ��ǵ��� Ÿ�̸� ����
	GetWorldTimerManager().SetTimer(
		LevelTimerHandle,
		this,
		&ASpartaGameStateBase::OnLevelTimeUp,
		LevelDuration,
		false
	);

	int CurrentLevelIndex = 0;
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance))
		{
			if (SpartaGameInstance)
			{
				CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Level %d Start!, Spawned %d coin"),
		CurrentLevelIndex + 1,
		SpawnedCoinCount);

	/// WAVE�˸�
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			UTextBlock* ButtonText = Cast<UTextBlock>(SpartaPlayerController->GetHUDWiget()->GetWidgetFromName(TEXT("Wave")));
			ButtonText->SetText(FText::FromString(FString::Printf(TEXT("WAVE %d"), nowWave)));
			UFunction* PlayAnimFunc = SpartaPlayerController->GetHUDWiget()->FindFunction(FName("WaveAnimStart"));
			if (PlayAnimFunc)
			{
				SpartaPlayerController->GetHUDWiget()->ProcessEvent(PlayAnimFunc, nullptr);
			}
		}
	}
}

void ASpartaGameStateBase::OnGameOver()
{
	UpdateHUD();
	UE_LOG(LogTemp, Warning, TEXT("Game Over!!"));
	// ���⼭ UI�� ���ٰų�, ����� ����� ���� ���� ����

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowMainMenu(true);
		}
	}
}