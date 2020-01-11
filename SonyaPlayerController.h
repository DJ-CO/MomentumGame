// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SonyaPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SMC_API ASonyaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASonyaPlayerController();

protected:
	// APawn interface
	virtual void SetupInputComponent() override;

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** Called for forwards/backward input */
	void AddPitch(float Value);

	/** Called for side to side input */
	void AddRoll(float Value);

	void OnJumpPressed();
	void OnJumpReleased();
	void OnCrouchPressed();
	void OnCrouchReleased();
	void OnImpulsePressed();
	void OnImpulseReleased();
	void OnMoveCameraPressed();
	void OnMoveCameraReleased();

	bool bIsJumpPressed;
	bool bIsCrouchPressed;
	bool bIsImpulsePressed;
	bool bIsMoveCameraPressed;

	float YValue;
	float XValue;

	float PitchValue;
	float RollValue;
public:
	float GetXValue();
	float GetYValue();
	float GetPitchValue();
	float GetRollValue();
	FVector GetXYDirection();
	FVector GetRollPitchDirection();
	bool GetIsJumpPressed();
	bool GetIsCrouchPressed();
	bool GetIsImpulsePressed();
	bool GetIsMoveCameraPressed();
	
};
