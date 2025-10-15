// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SpartaPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TRACK_3_API ASpartaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASpartaPlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerController")
	TObjectPtr<class UInputMappingContext> MyIMC;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerController")
	TObjectPtr<class UInputAction> MyMove;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerController")
	TObjectPtr<class UInputAction> MyLook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerController")
	TObjectPtr<class UInputAction> MyJump;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerController")
	TObjectPtr<class UInputAction> MySprint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<class UUserWidget> HUDWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD")
	class UUserWidget* HUDWidgetInstance;

	UFUNCTION(BlueprintCallable, Category = "HUD")
	UUserWidget* GetHUDWiget() const;



	// �޴� UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu")
	UUserWidget* MainMenuWidgetInstance;
	// HUD ǥ��
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowGameHUD();
	// ���� �޴� ǥ��
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void ShowMainMenu(bool bIsRestart);
	// ���� ����
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void StartGame();


protected:
	virtual void BeginPlay() override;



	
};
