// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpartaCharacter.generated.h"

class UWidgetComponent;


UCLASS()
class TRACK_3_API ASpartaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASpartaCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyCharacter|Camera")
	TObjectPtr<class USpringArmComponent> SpringArmComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyCharacter|Camera")
	TObjectPtr<class UCameraComponent> CameraComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* OverheadWidget;

	// ���� ü���� �������� �Լ�
	UFUNCTION(BlueprintPure, Category = "Health")
	int32 GetHealth() const;
	// ü���� ȸ����Ű�� �Լ�
	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float Amount);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;



protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyCharacter|Camera")
	TObjectPtr<class UEnhancedInputComponent> MyEIC;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyCharacter|Camera")
	TObjectPtr<class ASpartaPlayerController> MySPC;

	UFUNCTION()
	void Move(const struct FInputActionValue& Value);
	UFUNCTION()
	void Look(const struct FInputActionValue& Value);
	UFUNCTION()
	void StartJump(const struct FInputActionValue& Value);
	UFUNCTION()
	void StopJump(const struct FInputActionValue& Value);
	UFUNCTION()
	void StartSprint(const struct FInputActionValue& Value);
	UFUNCTION()
	void StopSprinting(const struct FInputActionValue& Value);

	// �ִ� ü��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;
	// ���� ü��
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;
	// ��� ó�� �Լ� (ü���� 0 ���ϰ� �Ǿ��� �� ȣ��)
	UFUNCTION(BlueprintCallable, Category = "Health")
	virtual void OnDeath();

	// ������ ó�� �Լ� - �ܺηκ��� �������� ���� �� ȣ���
	// �Ǵ� AActor�� TakeDamage()�� �������̵�
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	void UpdateOverheadHP();

private:
	float NormalSpeed;
	float SprintSpeedMultiplier;
	float SprintSpeed;

};
