// Fill out your copyright notice in the Description page of Project Settings.


#include "SpartaCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "SpartaPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SpartaGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"

// Sets default values
ASpartaCharacter::ASpartaCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.f;
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));	
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	SpringArmComp->AddLocalOffset(FVector(0.f, 0.f, 50.f));

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(GetMesh());
	OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);



	NormalSpeed = 600.f;
	SprintSpeedMultiplier = 1.7f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	// �ʱ� ü�� ����
	MaxHealth = 100.0f;
	Health = MaxHealth;
	// UpdateOverheadHP();
}

// Called when the game starts or when spawned
void ASpartaCharacter::BeginPlay()
{
	Super::BeginPlay();
	UpdateOverheadHP();
}

// Called every frame
void ASpartaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASpartaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	MyEIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!IsValid(MyEIC)) return;

	MySPC = Cast<ASpartaPlayerController>(GetController());
	if (!IsValid(MySPC)) return;
	if (MySPC->MyMove)
	{
		MyEIC->BindAction(MySPC->MyMove, ETriggerEvent::Triggered, this, &ASpartaCharacter::Move);
	}
	if (MySPC->MyLook)
	{
		MyEIC->BindAction(MySPC->MyLook, ETriggerEvent::Triggered, this, &ASpartaCharacter::Look);
	}
	if (MySPC->MyJump)
	{
		MyEIC->BindAction(MySPC->MyJump, ETriggerEvent::Triggered, this, &ASpartaCharacter::StartJump);
		MyEIC->BindAction(MySPC->MyJump, ETriggerEvent::Completed, this, &ASpartaCharacter::StopJump);
	}
	if (MySPC->MySprint)
	{
		MyEIC->BindAction(MySPC->MySprint, ETriggerEvent::Triggered, this, &ASpartaCharacter::StartSprint);
		MyEIC->BindAction(MySPC->MySprint, ETriggerEvent::Completed, this, &ASpartaCharacter::StopSprinting);

	}
	
}


void ASpartaCharacter::Move(const struct FInputActionValue& Value)
{
	if (!Controller) return;
	const FVector2D MoveAxis = Value.Get<FVector2D>();
	if(!FMath::IsNearlyZero(MoveAxis.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveAxis.X);
	}
	if (!FMath::IsNearlyZero(MoveAxis.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveAxis.Y);
	}
}

void ASpartaCharacter::Look(const struct FInputActionValue& Value)
{
	const FVector2D LookAxis = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxis.X);
	AddControllerPitchInput(LookAxis.Y);
	
}

void ASpartaCharacter::StartJump(const struct FInputActionValue& Value)
{
	if(Value.Get<bool>())
	{
		Jump();
	}
}

void ASpartaCharacter::StopJump(const FInputActionValue& Value)
{
	if (!Value.Get<bool>())
	{
		StopJumping();
	}
}

void ASpartaCharacter::StartSprint(const struct FInputActionValue& Value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void ASpartaCharacter::StopSprinting(const struct FInputActionValue& Value)
{
	
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
}

void ASpartaCharacter::OnDeath()
{
	// UE_LOG(LogTemp, Error, TEXT("Character is Dead!"));
	ASpartaGameStateBase* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameStateBase>() : nullptr;
	if (SpartaGameState)
	{
		SpartaGameState->OnGameOver();
	}
	
}

float ASpartaCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// �⺻ ������ ó�� ���� ȣ�� (�ʼ��� �ƴ�)
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// ü���� ��������ŭ ���ҽ�Ű��, 0 ���Ϸ� �������� �ʵ��� Clamp
	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
	UpdateOverheadHP();
	// UE_LOG(LogTemp, Warning, TEXT("Health decreased to: %f"), Health);

	// ü���� 0 ���ϰ� �Ǹ� ��� ó��
	if (Health <= 0.0f)
	{
		OnDeath();
	}

	// ���� ����� �������� ��ȯ
	return ActualDamage;
}

int32 ASpartaCharacter::GetHealth() const
{
	return Health;
}

// ü�� ȸ�� �Լ�
void ASpartaCharacter::AddHealth(float Amount)
{
	// ü���� ȸ����Ŵ. �ִ� ü���� �ʰ����� �ʵ��� ������
	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
	UpdateOverheadHP();
	// UE_LOG(LogTemp, Log, TEXT("Health increased to: %f"), Health);
}

void ASpartaCharacter::UpdateOverheadHP()
{
	if (!OverheadWidget) return;

	UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
	if (!OverheadWidgetInstance) return;

	if (UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName(TEXT("OverHeadHP"))))
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)));
	}
}