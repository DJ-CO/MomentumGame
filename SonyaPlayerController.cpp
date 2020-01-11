// Fill out your copyright notice in the Description page of Project Settings.


#include "SonyaPlayerController.h"
#include "Components/InputComponent.h"


ASonyaPlayerController::ASonyaPlayerController()
{
	XValue = 0;
	YValue = 0;
}

void ASonyaPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	check(InputComponent);

	FInputActionBinding* ActionBinding;
	FInputAxisBinding* AxisBinding;

	ActionBinding = &InputComponent->BindAction("JumpInput", IE_Pressed, this, &ASonyaPlayerController::OnJumpPressed);
	ActionBinding->bConsumeInput = false;
	ActionBinding = &InputComponent->BindAction("JumpInput", IE_Released, this, &ASonyaPlayerController::OnJumpReleased);
	ActionBinding->bConsumeInput = false;

	ActionBinding = &InputComponent->BindAction("CrouchInput", IE_Pressed, this, &ASonyaPlayerController::OnCrouchPressed);
	ActionBinding->bConsumeInput = false;
	ActionBinding = &InputComponent->BindAction("CrouchInput", IE_Released, this, &ASonyaPlayerController::OnCrouchReleased);
	ActionBinding->bConsumeInput = false;

	ActionBinding = &InputComponent->BindAction("ImpulseInput", IE_Pressed, this, &ASonyaPlayerController::OnImpulsePressed);
	ActionBinding->bConsumeInput = false;
	ActionBinding = &InputComponent->BindAction("ImpulseInput", IE_Released, this, &ASonyaPlayerController::OnImpulseReleased);
	ActionBinding->bConsumeInput = false;

	ActionBinding = &InputComponent->BindAction("MoveCameraInput", IE_Pressed, this, &ASonyaPlayerController::OnMoveCameraPressed);
	ActionBinding->bConsumeInput = false;
	ActionBinding = &InputComponent->BindAction("MoveCameraInput", IE_Released, this, &ASonyaPlayerController::OnMoveCameraReleased);
	ActionBinding->bConsumeInput = false;

	AxisBinding = &InputComponent->BindAxis("ForwardInput", this, &ASonyaPlayerController::MoveForward);
	AxisBinding->bConsumeInput = false;
	AxisBinding = &InputComponent->BindAxis("RightInput", this, &ASonyaPlayerController::MoveRight);
	AxisBinding->bConsumeInput = false;
	AxisBinding = &InputComponent->BindAxis("PitchInput", this, &ASonyaPlayerController::AddPitch);
	AxisBinding->bConsumeInput = false;
	AxisBinding = &InputComponent->BindAxis("RollInput", this, &ASonyaPlayerController::AddRoll);
	AxisBinding->bConsumeInput = false;
}

void ASonyaPlayerController::OnJumpPressed()
{
	bIsJumpPressed = true;
}

void ASonyaPlayerController::OnJumpReleased()
{
	bIsJumpPressed = false;
}

void ASonyaPlayerController::OnCrouchPressed()
{
	bIsCrouchPressed = true;
}

void ASonyaPlayerController::OnCrouchReleased()
{
	bIsCrouchPressed = false;
}

void ASonyaPlayerController::OnImpulsePressed()
{
	bIsImpulsePressed = true;
}

void ASonyaPlayerController::OnImpulseReleased()
{
	bIsImpulsePressed = false;
}

void ASonyaPlayerController::OnMoveCameraPressed()
{
	bIsMoveCameraPressed = true;
}

void ASonyaPlayerController::OnMoveCameraReleased()
{
	bIsMoveCameraPressed = false;
}

/** Called for forwards/backward input */
void ASonyaPlayerController::MoveForward(float Value)
{
	XValue = Value;
	//UE_LOG(LogTemp, Warning, TEXT("X is %f"), XValue);
}

/** Called for side to side input */
void ASonyaPlayerController::MoveRight(float Value)
{
	YValue = Value;
	//UE_LOG(LogTemp, Warning, TEXT("Y is %f"), YValue);
}

/** Called for forwards/backward input */
void ASonyaPlayerController::AddPitch(float Value)
{
	PitchValue = Value;
}

/** Called for side to side input */
void ASonyaPlayerController::AddRoll(float Value)
{
	RollValue = Value;
}

bool ASonyaPlayerController::GetIsJumpPressed()
{
	return bIsJumpPressed;
}

bool ASonyaPlayerController::GetIsCrouchPressed()
{
	return bIsCrouchPressed;
}

bool ASonyaPlayerController::GetIsImpulsePressed()
{
	return bIsImpulsePressed;
}

bool ASonyaPlayerController::GetIsMoveCameraPressed()
{
	return bIsMoveCameraPressed;
}

float ASonyaPlayerController::GetPitchValue()
{
	return PitchValue;
}

float ASonyaPlayerController::GetRollValue()
{
	return RollValue;
}

FVector ASonyaPlayerController::GetRollPitchDirection()
{
	return FVector(RollValue, PitchValue, 0.0f);
}

float ASonyaPlayerController::GetXValue()
{
	return XValue;
}

float ASonyaPlayerController::GetYValue()
{
	return YValue;
}

FVector ASonyaPlayerController::GetXYDirection()
{
	//UE_LOG(LogTemp, Warning, TEXT("X is %f , Y is %f"), XValue, YValue);
	return FVector(XValue, YValue, 0.0f);
}

