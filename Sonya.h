// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "OrientationStateComponent.h"
#include "SonyaPlayerController.h"
#include "Curves/CurveFloat.h"
#include "GameFramework/PawnMovementComponent.h"
#include "SonyaMovementComponent.h"
#include "Sonya.generated.h"

UCLASS()
class SMC_API ASonya : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASonya();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

// Components
protected:
	UPROPERTY(VisibleAnywhere)
	class UCapsuleComponent * RootCapsule;

	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent * SonyaMesh;

	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere)
	class USonyaMovementComponent * SonyaMovement;

	class ASonyaPlayerController* MyController;

// Action variables
protected:
	UPROPERTY(EditDefaultsOnly)
	float MovementAcceleration;
	float JumpAcceleration;
	UPROPERTY(EditDefaultsOnly)
	float CameraSpeed;

// Physics variables
protected:
	FVector Velocity;
	FRotator AngularVelocity;
	FVector UsableNormalForce;

	float LastDeltaTime;

	UPROPERTY(EditDefaultsOnly)
	float Gravity;

	UPROPERTY(EditDefaultsOnly)
	float GroundFriction;

	UPROPERTY(EditDefaultsOnly)
	class UCurveFloat* AccelerationCurve;

	UPROPERTY(EditDefaultsOnly)
	class UCurveFloat* DecelerationCurve;

	UPROPERTY(EditDefaultsOnly)
	class UCurveFloat* RotationCurve;

// Physics functions
protected:
	void Move(float DeltaTime);
	void Rotate(float DeltaTime);
	void ApplyGravity(float DeltaTime);
	void ApplyInput(float DeltaTime);
	void MoveCamera(float DeltaTime);
	void Decelerate(float DeltaTime);
	void Accelerate(float DeltaTime, FVector Direction);
	void RotateToMatchYaw(float DeltaTime, FVector Direction);
	void ApplyRotationInput(float DeltaTime);
	void UpdateNormalForce(float DeltaTime);
	FVector GetVelocityAlongPlane();

// Actions
protected:
	void OnForwardInput(float Value);
	void OnYawInput(float Value);
	void OnJumpInput();
	void OnCrouchPressed();
	void OnCrouchReleased();

// Orientation States
protected:
	UPROPERTY(VisibleAnywhere)
	class UOrientationStateComponent* CurrentState;

	UPROPERTY(EditDefaultsOnly)
	class UOrientationStateComponent* FrontState;

	UPROPERTY(EditDefaultsOnly)
	class UOrientationStateComponent* SideState;

	UPROPERTY(EditDefaultsOnly)
	class UOrientationStateComponent* BackState;

	UPROPERTY(EditDefaultsOnly)
	class UOrientationStateComponent* BottomState;

	UPROPERTY(EditDefaultsOnly)
	class UOrientationStateComponent* TopState;

// Utility
protected:
	FVector GetViewForwardVector();
	FVector GetViewUpVector();
	FRotator OrientRotationToNormal(FVector UpVector, FVector Normal, FRotator CurrentRotation);
	FRotator GetInputRotation();

public:	

	void OutputVector(FString VectorName, FVector Vector);
	void OutputRotator(FString RotatorName, FRotator Rotator);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};