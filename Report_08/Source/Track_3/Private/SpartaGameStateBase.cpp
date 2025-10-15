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
	LevelDuration = 10.0f; // 한 레벨당 30초
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
	// 시간이 다 되면 레벨을 종료
	// nowWave가 2보다 크면 레벨 종료
	// UE_LOG(LogTemp, Warning, TEXT("Wave %d Time Up!"), nowWave);
	if (nowWave > 2)
	{
		EndLevel();
		return;
	}
	
	if (SpawnVolume)
	{
		// 레벨 시작 시, 코인 개수 초기화
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

		// 현재 레벨에서 스폰된 코인을 전부 주웠다면 즉시 레벨 종료
		if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
		{
			// EndLevel();
			// 웨이브 클리어로 변경
			OnLevelTimeUp();
		}
}

void ASpartaGameStateBase::EndLevel()
{
	// 타이머 해제
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);

	// 웨이브 초기화
	nowWave = 0;

	// 다음 레벨 인덱스로
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
	// 모든 레벨을 다 돌았다면 게임 오버 처리
	if (CurrentLevelIndex >= MaxLevels)
	{
		OnGameOver();
		return;
	}

	// 레벨 맵 이름이 있다면 해당 맵 불러오기
	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		// 맵 이름이 없으면 게임오버
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

	// 레벨 시작 시, 코인 개수 초기화
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	nowWave++;
	UpdateHUD();
	// 현재 맵에 배치된 모든 SpawnVolume을 찾아 아이템 40개를 스폰
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
				// 만약 스폰된 액터가 코인 타입이라면 SpawnedCoinCount 증가
				if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					SpawnedCoinCount++;
				}
			}
		}
	}

	// 30초 후에 OnLevelTimeUp()가 호출되도록 타이머 설정
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

	/// WAVE알림
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
	// 여기서 UI를 띄운다거나, 재시작 기능을 넣을 수도 있음

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowMainMenu(true);
		}
	}
}