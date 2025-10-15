// Fill out your copyright notice in the Description page of Project Settings.


#include "MineItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AMineItem::AMineItem()
{
	ExplosionDelay = 2.0f;
	ExplosionRadius = 300.0f;
	ExplosionDamage = 55;
	ItemType = "MineItem";
	bHasExploded = false;

	ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
	ExplosionCollision->SetupAttachment(Scene);
	ExplosionCollision->InitSphereRadius(ExplosionRadius);
	ExplosionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

void AMineItem::ActivateItem(AActor* Activator)
{
	if (bHasExploded) return;

	Super::ActivateItem(Activator);

	// Ÿ�̸� �ڵ鷯
	GetWorld()->GetTimerManager().SetTimer(
		ExplosionTimerHandle,					// � Ÿ�̸�
		this,									// �� ��ü�� Ÿ�̸� �ް�
		&AMineItem::Explode,					// �� �Լ��� ����
		ExplosionDelay,							// �ɸ��� �ð� float
		false);									// �ݺ�����

	bHasExploded = true;
}

void AMineItem::Explode()
{
	UParticleSystemComponent* Particle = nullptr;

	if (ExplosionParticle)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ExplosionParticle,
			GetActorLocation(),
			GetActorRotation(),
			false
		);
	}
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			ExplosionSound,
			GetActorLocation()
		);
	}

	TArray<AActor*> OverlappedActors;
	ExplosionCollision->GetOverlappingActors(OverlappedActors);
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("BOOM!")));

	for (AActor* Actor : OverlappedActors)
	{
		if (Actor && Actor->ActorHasTag("Player"))
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("DAMAGED!")));
			/// �������� ����Ʈ �߰�

			UGameplayStatics::ApplyDamage(
				Actor,                      // �������� ���� ����
				ExplosionDamage,            // ������ ��
				nullptr,                    // �������� ������ ��ü (���ڸ� ��ġ�� ĳ���Ͱ� �����Ƿ� nullptr)
				this,                       // �������� ������ ������Ʈ(����)
				UDamageType::StaticClass()  // �⺻ ������ ����
			);

		}
	}

	this->Destroy();

	if (Particle)
	{
		FTimerHandle DestroyParticleTimerHandle;
		TWeakObjectPtr<UParticleSystemComponent> WeakParticle = Particle;

		GetWorld()->GetTimerManager().SetTimer(
			DestroyParticleTimerHandle,
			[WeakParticle]()
			{
				if (WeakParticle.IsValid())
				{
					WeakParticle->DestroyComponent();
				}
			},
			2.0f,
			false
		);
	}
}
