// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OrientationStateComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SMC_API UOrientationStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOrientationStateComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere)
		int ID;

	UPROPERTY(EditAnywhere)
		float MaxAcceleration;

	UPROPERTY(EditAnywhere)
		float BaseImpact;

	UPROPERTY(EditAnywhere)
		float MaxImpact;

	UPROPERTY(EditAnywhere)
		float MaxJump;

	UPROPERTY(EditAnywhere)
		float MaxSpeed;

	UPROPERTY(EditAnywhere)
		float MaxBrakingAcceleration;

	UPROPERTY(EditAnywhere)
		float DriftingDeceleration;

	UPROPERTY(EditAnywhere)
		FRotator MaxRotationAceleration;

	UPROPERTY(EditAnywhere)
		FRotator MaxRotationBraking;

	UPROPERTY(EditAnywhere)
		FRotator RotationDeceleration;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
