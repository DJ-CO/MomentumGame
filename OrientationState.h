// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OrientationState.generated.h"

/**
 * 
 */
UCLASS()
class SMC_API UOrientationState : public UObject
{
	GENERATED_BODY()

public:
	UOrientationState();

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

};
