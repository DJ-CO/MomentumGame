// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "SonyaPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "SonyaMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class SMC_API USonyaMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
	
public:
	USonyaMovementComponent();

//protected:
//	bool StepUp(const FVector& GravDir, const FVector& Delta, const FHitResult& InHit, FStepDownResult* OutStepDownResult);

protected:
	class ASonyaPlayerController* MyController;
	class UCameraComponent* MyCamera;


protected:
	FVector AngularVelocity;
	FVector UsableNormalForce;
	FVector GroundNormal;
	FVector RawInput;
	FVector BaseInput;
	FVector StoredNormalForce;
	FVector PlanarInput;
	FVector LedgeRightDirection;
	FVector LedgeWall;
	FVector LedgeFloor;
	FRotator ViewRotation;
	FRotator Rotation;
	float LastDeltaTime;
	bool bIsCrouching;
	bool bIsLedgeGrabbing;
	UPROPERTY(EditDefaultsOnly)
	float Gravity;
	UPROPERTY(EditDefaultsOnly)
	float NormalForceMultiplier;
	UPROPERTY(EditDefaultsOnly)
	float NormalForceUsageRate;
	int DoubleJumps;
	int RollJumps;

// For rotation
protected:
	FRotator FirstInputRotation;
	FRotator LastInputRotation;
	float SpinDistance;
	int IsSpinning;
	

protected:
	void ApplyGravity(float DeltaTime);
	void Move(float DeltaTime);

protected:

	UPROPERTY(EditDefaultsOnly)
	FRotator MaxRotationAcceleration;
	UPROPERTY(EditDefaultsOnly)
	FRotator MaxRotationDeceleration;
	UPROPERTY(EditDefaultsOnly)
	FVector MaxSpinSpeed;
	UPROPERTY(EditDefaultsOnly)
	FVector MaxSpinAcceleration;
	UPROPERTY(EditDefaultsOnly)
	FVector SpinDeceleration;
	UPROPERTY(EditDefaultsOnly)
	FVector RollJumpVector;
	UPROPERTY(EditDefaultsOnly)
	float MaxSpeed;
	UPROPERTY(EditDefaultsOnly)
	float MaxLedgeSpeed;
	UPROPERTY(EditDefaultsOnly)
	float MaxSpinDistance;
	UPROPERTY(EditDefaultsOnly)
	float ConstantNormalForce;
	UPROPERTY(EditDefaultsOnly)
	float MaxBaseWallCling;
	UPROPERTY(EditDefaultsOnly)
	float BaseBrake;
	UPROPERTY(EditDefaultsOnly)
	float BaseDeceleration;
	UPROPERTY(EditDefaultsOnly)
	float BaseAcceleration;
	UPROPERTY(EditDefaultsOnly)
	float BaseTurning;
	UPROPERTY(EditDefaultsOnly)
	float GroundBrake;
	UPROPERTY(EditDefaultsOnly)
	float GroundDeceleration;
	UPROPERTY(EditDefaultsOnly)
	float GroundAcceleration;
	UPROPERTY(EditDefaultsOnly)
	float LedgeBrake;
	UPROPERTY(EditDefaultsOnly)
	float LedgeDeceleration;
	UPROPERTY(EditDefaultsOnly)
	float LedgeAcceleration;
	UPROPERTY(EditDefaultsOnly)
	float WallAcceleration;
	UPROPERTY(EditDefaultsOnly)
	float GroundTurning;
	UPROPERTY(EditDefaultsOnly)
	float MaxJump;
	UPROPERTY(EditDefaultsOnly)
	float MaxDoubleJump;
	UPROPERTY(EditDefaultsOnly)
	float DoubleJumpDrift;
	UPROPERTY(EditDefaultsOnly)
	float MaxSpinJump;
	UPROPERTY(EditDefaultsOnly)
	float MaxSpinHover;
	UPROPERTY(EditDefaultsOnly)
	float MaxStoredNormalForce;
	UPROPERTY(EditDefaultsOnly)
	float MaxForceReleaseRate;
	UPROPERTY(EditDefaultsOnly)
	float SpinVerticalAcceleration;
	UPROPERTY(EditDefaultsOnly)
	float ReachHeightFromBottom;
	UPROPERTY(EditDefaultsOnly)
	float ReachBottomHeight;
	UPROPERTY(EditDefaultsOnly)
	float ReachDistance;


protected:
	void OutputVector(FString VectorName, FVector Vector);
	void OutputRotator(FString RotatorName, FRotator Rotator);
	FRotator VectorToRotator(FVector Vector);

protected:
	void ApplyBaseAcceleration(float DeltaTime);
	void ApplyGroundAcceleration(float DeltaTime);
	void ApplyLedgeAcceleration(float DeltaTime);
	void ApplyRotationAcceleration(float DeltaTime);
	void ApplyAcceleration(float DeltaTime, bool bIsBase);
	float FindAccelerationAmount();
	FVector FindInputPlanarVelocity(FVector InputNormal);
	FVector FindPlanarVelocity();
	FRotator OrientRotationToNormal(FVector UpVector, FVector Normal, FRotator CurrentRotation);
	FVector GetBaseForward();
	FVector GetGroundForward();
	FVector GetViewBaseForward();
	FVector GetViewGroundForward();
	FRotator GetBaseRotation();
	FRotator GetGroundRotation();
	FRotator GetViewBaseRotation();
	FRotator GetViewGroundRotation();
	float GetTargetRotationSpeed();
	float GetTargetRotationDistance();
	void CheckIsSpinning(float DeltaTime);
	void Spin(float DeltaTime);
	void UpdateUsableNormalForce(float DeltaTime);
	void TryRefreshResources();
	void TryGrabLedge(FHitResult *Hit);

public:
	void Jump();
	void DoubleJump();
	void RollJump();
	void Crouch();
	void UnCrouch();

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void CallableTick(float DeltaTime);
	void AddInput(FVector Input);
	
	void SetController(class ASonyaPlayerController* Controller);
	void SetCamera(class UCameraComponent* Camera);
};
