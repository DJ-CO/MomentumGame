// Fill out your copyright notice in the Description page of Project Settings.


#include "SonyaMovementComponent.h"
#include "Public/CollisionQueryParams.h"
#include "Engine/World.h"
#define INV_SQRT_TWO 0.70710678118

USonyaMovementComponent::USonyaMovementComponent()
{
	Gravity = 2940.0f;
	MaxJump = 1300.0f;
	MaxSpeed = 1000;
	MaxBaseWallCling = 200.0f;
	NormalForceUsageRate = Gravity;
	RollJumpVector = FVector(1400.0f, 0.0f, 300.0f);
	MaxSpinSpeed = FVector(100.0f, 100.0f, 2880.0f);
	SpinDeceleration = FVector(100.0f, 100.0f, 500.0f);
	MaxSpinAcceleration = FVector(100.0f, 100.0f, 500.0f);
	NormalForceMultiplier = 1.0f;
	MaxSpinDistance = 2160.0;
	MaxDoubleJump = 1150;
	DoubleJumpDrift = 150;
	MaxLedgeSpeed = 300.0f;
	LedgeDeceleration = 2000.0f;
	LedgeAcceleration = 1000.0f;
	LedgeBrake = 5000.0f;


	MaxSpinJump = 800.0;
	MaxSpinHover = 2940.0;
	ConstantNormalForce = 1500.0;

	BaseBrake = 100;
	BaseDeceleration = 0;
	BaseAcceleration = 100;
	BaseTurning = 100;

	GroundBrake = 400;
	GroundDeceleration = 100;
	GroundAcceleration = 200;
	WallAcceleration = 5000;
	GroundTurning = 100;

	MaxRotationAcceleration = FRotator(100.0f, 100.0f, 100.0f);
	MaxRotationDeceleration = MaxRotationAcceleration;
	MaxStoredNormalForce = 6000.0;
	MaxForceReleaseRate = 3000.0;
	SpinVerticalAcceleration = 2500.0;
	ReachHeightFromBottom = 100.0f;
	ReachBottomHeight = 80.0f;
	ReachDistance = 10.0;

	DoubleJumps = 1;
	RollJumps = 1;
}

void USonyaMovementComponent::SetController(class ASonyaPlayerController* Controller)
{
	MyController = Controller;
}

void USonyaMovementComponent::SetCamera(class UCameraComponent* NewCamera)
{
	MyCamera = NewCamera;
}

FRotator USonyaMovementComponent::GetBaseRotation()
{
	return FRotator(0.0f, UpdatedComponent->GetComponentRotation().Yaw, 0.0f);
}

FRotator USonyaMovementComponent::GetViewBaseRotation()
{
	if (MyCamera)
	{
		return FRotator(0.0f, MyCamera->GetComponentRotation().Yaw, 0.0f);
	}
	else
	{
		return GetBaseRotation();
	}
}

FRotator USonyaMovementComponent::GetGroundRotation()
{
	FVector Normal;
	if (UsableNormalForce.IsNearlyZero())
	{
		Normal = FVector(0.0, 0.0, 1.0);
	}
	else
	{
		Normal = UsableNormalForce.GetSafeNormal();
	}

	return OrientRotationToNormal(FVector(0.0f, 0.0f, 1.0f), Normal, GetBaseRotation());
}

void USonyaMovementComponent::CheckIsSpinning(float DeltaTime)
{
	FRotator CurrentInputRotation = BaseInput.Rotation();
	float NegativeDistance = (LastInputRotation- CurrentInputRotation).Clamp().Yaw;
	float PositiveDistance = (CurrentInputRotation - LastInputRotation).Clamp().Yaw;
	float CurrentSpinDistance;

	if (PositiveDistance < NegativeDistance)
	{
		CurrentSpinDistance = PositiveDistance;
	}
	else
	{
		CurrentSpinDistance = -NegativeDistance;
	}

	if ((BaseInput.Size() > 0.9f))
	{
		SpinDistance += CurrentSpinDistance;
	}
	
	SpinDistance = SpinDistance - FMath::Sign(SpinDistance) * SpinDeceleration.Z * DeltaTime;

	if (FMath::Abs(SpinDistance) > MaxSpinDistance)
	{
		SpinDistance = FMath::Sign(SpinDistance) * MaxSpinDistance;
	}

	//UE_LOG(LogTemp, Warning, TEXT("Spin Distance: %f"), SpinDistance);
	//UE_LOG(LogTemp, Warning, TEXT("TEST"));
	if (FMath::Abs(SpinDistance) > 360.0f)
	{
		IsSpinning = FMath::Sign(SpinDistance);
	}
	else
	{
		IsSpinning = 0;
	}

	if (IsSpinning != 0)
	{
		Spin(DeltaTime);
	}

	LastInputRotation = CurrentInputRotation;
}

void USonyaMovementComponent::Spin(float DeltaTime)
{
	Velocity += FVector(0.0f, 0.0f, SpinVerticalAcceleration * DeltaTime);
}

FRotator USonyaMovementComponent::GetViewGroundRotation()
{
	FVector Normal;
	if (UsableNormalForce.IsNearlyZero())
	{
		Normal = FVector(0.0, 0.0, 1.0);
	}
	else
	{
		Normal = UsableNormalForce.GetSafeNormal();
	}

	return OrientRotationToNormal(FVector(0.0f, 0.0f, 1.0f), Normal, GetViewBaseRotation());
}

FVector USonyaMovementComponent::GetBaseForward()
{
	return GetBaseRotation().RotateVector(FVector(1.0f, 0.0f, 0.0f));
}

FVector USonyaMovementComponent::GetViewBaseForward()
{
	return GetViewBaseRotation().RotateVector(FVector(1.0f, 0.0f, 0.0f));
}


FVector USonyaMovementComponent::GetGroundForward()
{
	return GetGroundRotation().RotateVector(FVector(1.0f, 0.0f, 0.0f));
}

FVector USonyaMovementComponent::GetViewGroundForward()
{
	return GetViewGroundRotation().RotateVector(FVector(1.0f, 0.0f, 0.0f));
}

void USonyaMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// InterpretInput();
	// UpdateRotationBank();
	// Move(DeltaTime);
}

void USonyaMovementComponent::CallableTick(float DeltaTime)
{
	AddInput(MyController->GetXYDirection());
	ApplyGravity(DeltaTime);
	if (bIsLedgeGrabbing)
	{
		ApplyLedgeAcceleration(DeltaTime);
		UE_LOG(LogTemp, Warning, TEXT("Was hanging ledge"));
		OutputVector("Ledge Right Direction", LedgeRightDirection);
	}
	else
	{
		ApplyBaseAcceleration(DeltaTime);
		ApplyGroundAcceleration(DeltaTime);
		ApplyRotationAcceleration(DeltaTime);
	}
	Move(DeltaTime);
	UpdateUsableNormalForce(DeltaTime);
	TryRefreshResources();
	LastDeltaTime = DeltaTime;
}

void USonyaMovementComponent::Jump()
{
	if (!UsableNormalForce.IsNearlyZero() || bIsLedgeGrabbing)
	{
		float JumpHeight;
		FVector JumpNormal;
		if (IsSpinning != 0)
		{
			JumpHeight = MaxSpinJump;
			JumpNormal = GroundNormal;
		}
		else if (!bIsCrouching)
		{
			JumpHeight = MaxJump;
			JumpNormal = FVector(GroundNormal.X, GroundNormal.Y, 1.0f);
		}
		else
		{
			RollJump();
			return;
		}

		Velocity += JumpNormal * JumpHeight;
	}
	else
	{
		if (IsSpinning != 0)
		{
			Velocity += FVector(0.0f, 0.0f, 1.0f * LastDeltaTime * MaxSpinHover);
		}
		else if (bIsCrouching)
		{
			if (RollJumps > 0)
			{
				RollJump();
			}
		}
		else if (DoubleJumps > 0)
		{
			DoubleJump();
		}
	}

	bIsLedgeGrabbing = false;
	LedgeRightDirection = FVector(0.0f);
}


void USonyaMovementComponent::UpdateUsableNormalForce(float DeltaTime)
{
	if (bIsLedgeGrabbing)
	{
		UsableNormalForce = FVector(0.0f, 0.0f, 1.0f) * FVector::DotProduct(LedgeFloor, FVector(0.0f, 0.0f, Gravity)) * DeltaTime;
		return;
	}

	if (StoredNormalForce.Size() > MaxStoredNormalForce)
	{
		StoredNormalForce = StoredNormalForce.GetSafeNormal() * MaxStoredNormalForce;
	}

	UsableNormalForce = StoredNormalForce;
	if (UsableNormalForce.Size() > MaxForceReleaseRate * DeltaTime)
	{
		UsableNormalForce = UsableNormalForce.GetSafeNormal() * MaxForceReleaseRate * DeltaTime;
	}

	StoredNormalForce = StoredNormalForce - UsableNormalForce;

	if (GroundNormal.IsNearlyZero())
	{
		UsableNormalForce = FVector(0.0f);
	}

	UsableNormalForce *= NormalForceMultiplier;
	
}

float USonyaMovementComponent::FindAccelerationAmount()
{

	//float AccelerationAmount = FVector::DotProduct(BaseInput, GetViewBaseForward());
	float AccelerationAmount = BaseInput.Size();
	//UE_LOG(LogTemp, Warning, TEXT("Acceleration Amount = %f"), AccelerationAmount);
	return AccelerationAmount;
}

void USonyaMovementComponent::ApplyGravity(float DeltaTime)
{
	/*FVector GravitationalForce = FVector(0.0f, 0.0f, -1.0f) * Gravity;
	FVector NormalGravity = FVector::DotProduct(FVector(0.0f, 0.0f, -1.0f), GroundNormal) * GroundNormal * Gravity;
	FVector Friction = NormalGravity - GravitationalForce;

	if (Friction.Size() > UsableNormalForce.Size())
	{
		Friction = Friction.GetSafeNormal() * UsableNormalForce.Size();
	}

	GravitationalForce += Friction;
	Velocity += GravitationalForce * DeltaTime;*/
	//OutputVector("Gravity", FVector(0.0f, 0.0f, 1.0f) * Gravity * (-1.0f) * DeltaTime);
	Velocity += FVector(0.0f, 0.0f, 1.0f) * Gravity * (-1.0f) * DeltaTime;
}

FVector USonyaMovementComponent::FindInputPlanarVelocity(FVector InputNormal)
{
	FVector InputPlanarVelocity;
	InputPlanarVelocity = Velocity - FVector::DotProduct(Velocity, InputNormal.GetSafeNormal()) * InputNormal.GetSafeNormal();
	//OutputVector("Input planar velocity", InputPlanarVelocity);
	return InputPlanarVelocity;
}

FVector USonyaMovementComponent::FindPlanarVelocity()
{
	FVector PlanarVelocity;
	FVector Normal;

	/*if (UsableNormalForce.IsNearlyZero())
	{
		Normal = FVector(0.0f, 0.0f, 1.0f);
	}
	else
	{
		Normal = UsableNormalForce.GetSafeNormal();
	}*/

	Normal = UpdatedComponent->GetUpVector();

	PlanarVelocity = Velocity - FVector::DotProduct(Velocity, Normal) * Normal;

	return PlanarVelocity;
}

void USonyaMovementComponent::ApplyLedgeAcceleration(float DeltaTime)
{
	//float AccelAmount = FindAccelerationAmount();
	float MaxDecel;
	float MaxAccel;
	float MaxBrake;
	float MaxTurn;
	float MaxMoveSpeed;
	FVector InputNormal;
	FVector Input;
	FVector Forward;
	FVector InputPlanarVelocity;
	FVector ForwardVelocity;
	FVector SideVelocity;
	FVector DeltaPlanarVelocity;
	FRotator InputRotation = OrientRotationToNormal(FVector(0.0f,0.0f,1.0f), LedgeWall, GetViewBaseRotation());
	// Slides up hills for some reason
	//UpdatedComponent->SetWorldRotation(GetGroundRotation());


	//UE_LOG(LogTemp, Warning, TEXT("Ground Move"));
	MaxMoveSpeed = MaxLedgeSpeed;
	MaxDecel = LedgeDeceleration;
	MaxAccel = LedgeAcceleration;
	MaxBrake = LedgeBrake;
	InputNormal = InputRotation.RotateVector(FVector(0.0f, 0.0f, 1.0f));
	//OutputVector("Usable normal force", UsableNormalForce);
	//OutputVector("Input normal", InputNormal);
	Input = InputRotation.RotateVector(RawInput);
	//Forward = GetViewGroundForward();
	Forward = PlanarInput.GetSafeNormal();
	MaxTurn = GroundTurning;

	float LedgeOption = FVector::DotProduct(Input.GetSafeNormal(), LedgeRightDirection);

	InputPlanarVelocity = FindInputPlanarVelocity(LedgeFloor);

	FVector DesiredVelocity = LedgeRightDirection * LedgeOption * MaxMoveSpeed;
	FVector DeltaVelocity = DesiredVelocity - InputPlanarVelocity;
	FVector AccelVelocity = InputPlanarVelocity.GetSafeNormal() * FVector::DotProduct(DeltaVelocity, InputPlanarVelocity.GetSafeNormal());
	FVector TurningVelocity = DeltaVelocity - AccelVelocity;
	float AddedNormalForce = 0.0f;
	float ConstantForce = FVector::DotProduct(InputNormal, FVector(0.0f, 0.0f, 1.0f)) * ConstantNormalForce * DeltaTime;
	float AccelAmount = FVector::DotProduct(DeltaVelocity.GetSafeNormal(), InputPlanarVelocity.GetSafeNormal());

	if (ConstantForce < 0.0f)
	{
		ConstantForce = 0.0f;
	}

	if (AccelAmount >= 0.0)
	{

		if (DeltaVelocity.Size() > MaxAccel * DeltaTime)
		{
			DeltaVelocity = DeltaVelocity.GetSafeNormal() * MaxAccel * DeltaTime;
		}

	}
	else
	{
		if (FVector::DotProduct(Input, DeltaVelocity.GetSafeNormal()) <= 0.0f)
		{
			if (AccelVelocity.Size() > MaxDecel * DeltaTime)
			{
				AccelVelocity = AccelVelocity.GetSafeNormal() * MaxDecel * DeltaTime;
			}
		}
		else
		{
			if (AccelVelocity.Size() > MaxBrake * DeltaTime)
			{
				AccelVelocity = AccelVelocity.GetSafeNormal() * MaxBrake * DeltaTime;
			}
		}


		if (TurningVelocity.Size() > MaxAccel * DeltaTime)
		{
			TurningVelocity = TurningVelocity.GetSafeNormal() * MaxAccel * DeltaTime;
		}


		if (UsableNormalForce.IsNearlyZero())
		{
			AddedNormalForce = INV_SQRT_TWO * AccelVelocity.Size();
		}
		else
		{
			AddedNormalForce = INV_SQRT_TWO * AccelVelocity.Size();
		}

		if (AddedNormalForce > UsableNormalForce.Size() + ConstantForce)
		{
			AddedNormalForce = UsableNormalForce.Size() + ConstantForce;
		}

		DeltaVelocity = AccelVelocity + TurningVelocity;
	}


	if (DeltaVelocity.Size() > UsableNormalForce.Size() + AddedNormalForce + ConstantForce)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not enough normal force"));
		DeltaVelocity = DeltaVelocity.GetSafeNormal() * (UsableNormalForce.Size() + AddedNormalForce + ConstantForce);
	}

	Velocity = Velocity + DeltaVelocity;

	Velocity += FVector(0.0f,0.0f,Gravity) * DeltaTime;
}

void USonyaMovementComponent::ApplyAcceleration(float DeltaTime, bool bIsBase)
{
	//float AccelAmount = FindAccelerationAmount();
	float MaxDecel;
	float MaxAccel;
	float MaxBrake;
	float MaxTurn;
	float MaxMoveSpeed;
	FVector InputNormal;
	FVector Input;
	FVector Forward;
	FVector InputPlanarVelocity;
	FVector ForwardVelocity;
	FVector SideVelocity;
	FVector DeltaPlanarVelocity;

	// Slides up hills for some reason
	//UpdatedComponent->SetWorldRotation(GetGroundRotation());

	if (bIsBase)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Base Move"));
		MaxMoveSpeed = MaxSpeed;
		MaxDecel = BaseDeceleration;
		MaxAccel = BaseAcceleration;
		MaxBrake = BaseBrake;
		InputNormal = GetViewGroundRotation().RotateVector(FVector(0.0f, 0.0f, 1.0f));
		Input = PlanarInput;
		//Forward = GetViewBaseForward();
		Forward = PlanarInput.GetSafeNormal();
		MaxTurn = BaseTurning;

		if (UsableNormalForce.IsNearlyZero() || true)
		{
			InputNormal = FVector(0.0f, 0.0f, 1.0f);
			Input = BaseInput;
			Forward = BaseInput.GetSafeNormal();
		}
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("Ground Move"));
		MaxMoveSpeed = MaxSpeed;
		MaxDecel = GroundDeceleration;

		float GroundAccelAmount = FVector::DotProduct(FVector(0.0f, 0.0f, 1.0f), GroundNormal);
		if (GroundNormal.IsNearlyZero())
		{
			MaxAccel = GroundAcceleration;
		}
		else if (GroundAccelAmount < 0.0f)
		{
			MaxAccel = WallAcceleration;
		}
		else
		{
			MaxAccel = GroundAcceleration * GroundAccelAmount + WallAcceleration * (1- GroundAccelAmount);
		}
		
		MaxBrake = GroundBrake;
		InputNormal = GetViewGroundRotation().RotateVector(FVector(0.0f,0.0f,1.0f));
		//OutputVector("Usable normal force", UsableNormalForce);
		//OutputVector("Input normal", InputNormal);
		Input = PlanarInput;
		//Forward = GetViewGroundForward();
		Forward = PlanarInput.GetSafeNormal();
		MaxTurn = GroundTurning;

		if (IsSpinning != 0)
		{
			InputNormal = FVector(0.0f, 0.0f, 1.0f);
			Input = BaseInput;
			Forward = BaseInput.GetSafeNormal();
		}

		if (GroundNormal.IsNearlyZero())
		{
			MaxDecel = BaseDeceleration;
		}
	}

	if (bIsCrouching)
	{
		MaxMoveSpeed = 0.0f;
		MaxAccel = 0.0f;
	}

	InputPlanarVelocity = FindInputPlanarVelocity(InputNormal);

	FVector DesiredVelocity = Input * MaxMoveSpeed;
	FVector DeltaVelocity = DesiredVelocity - InputPlanarVelocity;
	FVector AccelVelocity = InputPlanarVelocity.GetSafeNormal() * FVector::DotProduct(DeltaVelocity, InputPlanarVelocity.GetSafeNormal());
	FVector TurningVelocity = DeltaVelocity - AccelVelocity;
	float AddedNormalForce = 0.0f;
	float ConstantForce = FVector::DotProduct(InputNormal, FVector(0.0f, 0.0f, 1.0f)) * ConstantNormalForce * DeltaTime;
	float AccelAmount = FVector::DotProduct(DeltaVelocity.GetSafeNormal(), InputPlanarVelocity.GetSafeNormal());

	if (ConstantForce < 0.0f)
	{
		ConstantForce = 0.0f;
	}

	if (AccelAmount >= 0.0)
	{

		if (DeltaVelocity.Size() > MaxAccel * DeltaTime)
		{
			DeltaVelocity = DeltaVelocity.GetSafeNormal() * MaxAccel * DeltaTime;
		}

	}
	else
	{
		if (FVector::DotProduct(Input, DeltaVelocity.GetSafeNormal()) <= 0.0f)
		{
			if (AccelVelocity.Size() > MaxDecel* DeltaTime)
			{
				AccelVelocity = AccelVelocity.GetSafeNormal() * MaxDecel * DeltaTime;
			}
		}
		else
		{
			if (AccelVelocity.Size() > MaxBrake * DeltaTime)
			{
				AccelVelocity = AccelVelocity.GetSafeNormal() * MaxBrake * DeltaTime;
			}
		}
	

		if (TurningVelocity.Size() > MaxAccel * DeltaTime)
		{
			TurningVelocity = TurningVelocity.GetSafeNormal() * MaxAccel * DeltaTime;
		}

		if (!bIsBase)
		{
			if (UsableNormalForce.IsNearlyZero())
			{
				AddedNormalForce =  INV_SQRT_TWO * AccelVelocity.Size();
			}
			else
			{
				AddedNormalForce = INV_SQRT_TWO * AccelVelocity.Size();
			}

			if (AddedNormalForce > UsableNormalForce.Size() + ConstantForce)
			{
				AddedNormalForce = UsableNormalForce.Size() + ConstantForce;
			}

		}

		DeltaVelocity = AccelVelocity + TurningVelocity;
	}

	if (bIsBase)
	{
		//DeltaVelocity += BaseInput * MaxBaseWallCling * DeltaTime;
	}
	
	if (!bIsBase && DeltaVelocity.Size() > UsableNormalForce.Size() + AddedNormalForce + ConstantForce)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not enough normal force"));
		DeltaVelocity = DeltaVelocity.GetSafeNormal() * (UsableNormalForce.Size() + AddedNormalForce + ConstantForce);
	}

	Velocity = Velocity + DeltaVelocity;
}

void USonyaMovementComponent::Move(float DeltaTime)
{
	FVector Delta = Velocity * DeltaTime;

	if (!(Velocity * DeltaTime).IsNearlyZero())
	{
		FHitResult Hit;

		SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentRotation() + VectorToRotator(AngularVelocity * DeltaTime), true, Hit);

		FVector NormalForce = -FVector::DotProduct(Hit.Normal, Velocity) * Hit.Normal;

		GroundNormal = Hit.Normal;
		
		Velocity = Velocity + NormalForce;

		StoredNormalForce += NormalForce;

		// If we bumped into something, try to slide along it
		if (Hit.IsValidBlockingHit())
		{
			SlideAlongSurface(Delta, 1.0f - Hit.Time, Hit.Normal, Hit);
		}

		TryGrabLedge(&Hit);
	}

}

void USonyaMovementComponent::ApplyBaseAcceleration(float DeltaTime)
{
	ApplyAcceleration(DeltaTime, true);
}

void USonyaMovementComponent::ApplyGroundAcceleration(float DeltaTime)
{
	ApplyAcceleration(DeltaTime, false);
}

float USonyaMovementComponent::GetTargetRotationSpeed()
{
	float Distance = GetTargetRotationDistance();
	float Sign = FMath::Sign(Distance);

	Distance = FMath::Abs(Distance);

	return Sign * FMath::Sqrt(2 * MaxRotationDeceleration.Yaw * Distance);
}

float USonyaMovementComponent::GetTargetRotationDistance()
{
	FVector AimDirection = FindPlanarVelocity();
	if (AimDirection.IsNearlyZero())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Returned Zero Distance: %f"), 0.0f);
		return 0.0f;
	}

	float InevitableDistance = FMath::Sign(-AngularVelocity.Z)* (AngularVelocity.Z * AngularVelocity.Z) / (2 * MaxRotationDeceleration.Yaw);

	float NegativeDistance = (UpdatedComponent->GetComponentRotation() - AimDirection.Rotation()).Clamp().Yaw - InevitableDistance;
	float PositiveDistance = (AimDirection.Rotation() - UpdatedComponent->GetComponentRotation()).Clamp().Yaw + InevitableDistance;

	//OutputRotator("ComponentRotation", UpdatedComponent->GetComponentRotation());
	//OutputRotator("AimDirectionRotation", AimDirection.Rotation());
	//UE_LOG(LogTemp, Warning, TEXT("Negative Distance: %f"), NegativeDistance);
	//UE_LOG(LogTemp, Warning, TEXT("Positive Distance: %f"), PositiveDistance);

	if (NegativeDistance < PositiveDistance)
	{
			
		//UE_LOG(LogTemp, Warning, TEXT("Returned Negative Distance: %f"), NegativeDistance);
		return -NegativeDistance;
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("Returned Positive Distance: %f"), PositiveDistance);
		return PositiveDistance;
	}
}

void USonyaMovementComponent::ApplyRotationAcceleration(float DeltaTime)
{
	float TargetSpeed = GetTargetRotationSpeed();
	float Speed = AngularVelocity.Z;
	CheckIsSpinning(DeltaTime);

	//UE_LOG(LogTemp, Warning, TEXT("Is Spinning: %d"), IsSpinning);

	if (IsSpinning != 0)
	{
		Speed = Speed + IsSpinning * MaxSpinAcceleration.Z * DeltaTime;

		if (FMath::Abs(Speed) > MaxSpinSpeed.Z)
		{
			Speed = IsSpinning * MaxSpinSpeed.Z;
		}
	}
	else
	{
		if (Speed >= 0.0)
		{
			if (Speed > TargetSpeed)
			{
				Speed = Speed - MaxRotationDeceleration.Yaw * DeltaTime;

				if (Speed < TargetSpeed)
				{
					Speed = TargetSpeed;
				}
			}
			else if (Speed < TargetSpeed)
			{
				Speed = Speed + MaxRotationAcceleration.Yaw * DeltaTime;

				if (Speed > TargetSpeed)
				{
					Speed = TargetSpeed;
				}
			}
		}
		else
		{
			if (Speed > TargetSpeed)
			{
				Speed = Speed - MaxRotationAcceleration.Yaw * DeltaTime;

				if (Speed < TargetSpeed)
				{
					Speed = TargetSpeed;
				}
			}
			else if (Speed < TargetSpeed)
			{
				Speed = Speed + MaxRotationDeceleration.Yaw * DeltaTime;

				if (Speed > TargetSpeed)
				{
					Speed = TargetSpeed;
				}
			}
		}
	}
	
	AngularVelocity.Z = Speed;
	//OutputVector("Angular Velocity", AngularVelocity);
}

void USonyaMovementComponent::AddInput(FVector GivenInput)
{
	BaseInput = GetViewBaseRotation().RotateVector(GivenInput);
	//OutputVector("BaseInput", BaseInput);

	PlanarInput = GetViewGroundRotation().RotateVector(GivenInput);
	//OutputVector("PlanarInput", PlanarInput);
	RawInput = GivenInput;
}

FRotator USonyaMovementComponent::OrientRotationToNormal(FVector UpVector, FVector Normal, FRotator CurrentRotation)
{
	FQuat RootQuat = CurrentRotation.Quaternion();
	FVector RotationAxis = FVector::CrossProduct(UpVector, Normal);
	RotationAxis.Normalize();

	float DotProduct = FVector::DotProduct(UpVector, Normal);
	float RotationAngle = acosf(DotProduct);

	FQuat Quat = FQuat(RotationAxis, RotationAngle);

	FQuat NewQuat = Quat * RootQuat;

	return NewQuat.Rotator();
}

void USonyaMovementComponent::DoubleJump()
{	
	Velocity.Z = MaxDoubleJump;
	if (FVector::DotProduct(Velocity, BaseInput.GetSafeNormal()) <= MaxSpeed)
	{
		Velocity += BaseInput * DoubleJumpDrift;
	}
	
	DoubleJumps = 0;
	RollJumps = 1;
}

void USonyaMovementComponent::RollJump()
{
	FRotator GroundInputRotation;
	if (BaseInput.IsNearlyZero())
	{
		GroundInputRotation = FRotator(0.0f,UpdatedComponent->GetComponentRotation().Yaw, 0.0f);
	}
	else
	{
		GroundInputRotation = BaseInput.Rotation();
	}
	Velocity = GroundInputRotation.RotateVector(RollJumpVector);
	RollJumps = 0;
}

void USonyaMovementComponent::TryRefreshResources()
{
	if (FVector::DotProduct(GroundNormal, FVector(0.0f, 0.0f, 1.0f)) > 0.1f)
	{
		DoubleJumps = 1;
		RollJumps = 1;
	}
}

void USonyaMovementComponent::Crouch()
{
	bIsCrouching = true;
	IsSpinning = 0;
	bIsLedgeGrabbing = false;
	LedgeRightDirection = FVector(0.0f, 0.0f, 0.0f);
	AngularVelocity = FVector(0.0f);
	SpinDistance = 0.0f;
	UpdatedComponent->SetWorldScale3D(FVector(1.0f, 1.0f, 0.5f));
}

void USonyaMovementComponent::UnCrouch()
{
	bIsCrouching = false;
	UpdatedComponent->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
}

void USonyaMovementComponent::TryGrabLedge(FHitResult * Hit)
{
	if (IsSpinning != 0 || bIsCrouching || (Velocity.Z > 0.0f && !bIsLedgeGrabbing))
	{
		bIsLedgeGrabbing = false;
		LedgeRightDirection = FVector(0.0f, 0.0f, 0.0f);
		return;
	}
	FHitResult LedgeHit;
	FVector Reach;
	if (BaseInput.IsNearlyZero() || bIsLedgeGrabbing)
	{
		Reach = UpdatedComponent->GetComponentRotation().RotateVector(FVector(ReachDistance, 0.0f, 0.0f));
	}
	else
	{
		Reach = BaseInput.GetSafeNormal() * ReachDistance;
	}
	FVector Start = Reach + UpdatedComponent->GetComponentLocation() + FVector(0.0f, 0.0f, ReachBottomHeight + ReachHeightFromBottom);
	FVector End = Reach + UpdatedComponent->GetComponentLocation() + FVector(0.0f, 0.0f, ReachBottomHeight);;
	FCollisionQueryParams TraceParams;
	FCollisionShape CollisionSphere;
	CollisionSphere.ShapeType = ECollisionShape::Sphere;
	CollisionSphere.SetSphere(1.0f);
	TraceParams.bTraceComplex = true;
	TraceParams.bFindInitialOverlaps = true;
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.bIgnoreTouches = false;
	TraceParams.bIgnoreBlocks = false;
	TraceParams.AddIgnoredActor(UpdatedComponent->GetOwner());

	bool DidTrace = false;

	DidTrace = GetWorld()->SweepSingleByChannel(LedgeHit, Start, End, FQuat(0.0f, 0.0f, 0.0f, 0.0f), ECC_Pawn, CollisionSphere, TraceParams);
	
	if (LedgeHit.bBlockingHit)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Did Trace"));
		if (FVector::DotProduct(LedgeHit.Normal, FVector(0.0f, 0.0f, 1.0f)) > 0.8 && !LedgeHit.bStartPenetrating && LedgeHit.bBlockingHit)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Grabbed Ledge"));
			//OutputVector("Floor Normal", LedgeHit.Normal);
			//OutputVector("Impact Floor Normal", LedgeHit.ImpactNormal);
			
			FVector Floor = LedgeHit.Normal;
			Start = FVector(UpdatedComponent->GetComponentLocation().X , UpdatedComponent->GetComponentLocation().Y, LedgeHit.ImpactPoint.Z -2.0);
			End = LedgeHit.ImpactPoint - FVector(0.0f,0.0f,2.0f);
			DidTrace = GetWorld()->SweepSingleByChannel(LedgeHit, Start, End, FQuat(0.0f, 0.0f, 0.0f, 0.0f), ECC_Pawn, CollisionSphere, TraceParams);
			FVector Wall;
			if (LedgeHit.bBlockingHit)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Found Wall"));
				//OutputVector("Wall Normal", LedgeHit.Normal);
				//OutputVector("Impact Wall Normal", LedgeHit.ImpactNormal);
				Wall = LedgeHit.Normal;
				LedgeRightDirection = FVector::CrossProduct(Floor, Wall).GetSafeNormal();
				LedgeWall = Wall;

				FRotator FacingWall = FVector(-LedgeWall.X, -LedgeWall.Y, 0.0f).Rotation();
				UpdatedComponent->SetWorldRotation(FacingWall);
				AngularVelocity = FVector(0.0f);
			}

			if (!bIsLedgeGrabbing)
			{
				Velocity = FVector(0.0f);
			}
			Velocity.Z = 0.0f;
			
			//OutputVector("Ledge Right Direction", LedgeRightDirection);

			LedgeFloor = Floor;
			bIsLedgeGrabbing = true;

			DoubleJumps = 1;
			RollJumps = 1;
		}
		else
		{
			bIsLedgeGrabbing = false;
			LedgeRightDirection = FVector(0.0f,0.0f,0.0f);
		}
	}
	else
	{
		bIsLedgeGrabbing = false;
		LedgeRightDirection = FVector(0.0f, 0.0f, 0.0f);
	}
	

}

void USonyaMovementComponent::OutputVector(FString VectorName, FVector Vector)
{
	UE_LOG(LogTemp, Warning, TEXT(" %s = ( %f , %f , %f )"), *VectorName, Vector.X, Vector.Y, Vector.Z);
}

void USonyaMovementComponent::OutputRotator(FString RotatorName, FRotator Rotator)
{
	UE_LOG(LogTemp, Warning, TEXT(" %s = ( P %f , Y %f , R %f )"), *RotatorName, Rotator.Pitch, Rotator.Yaw, Rotator.Roll);
}

FRotator USonyaMovementComponent::VectorToRotator(FVector Vector)
{
	return FRotator(Vector.Y, Vector.Z, Vector.X);
}

//bool USonyaMovementComponent::StepUp(const FVector& GravDir, const FVector& Delta, const FHitResult& InHit, FStepDownResult* OutStepDownResult)
//{
//	SCOPE_CYCLE_COUNTER(STAT_CharStepUp);
//
//	if (!CanStepUp(InHit) || MaxStepHeight <= 0.f)
//	{
//		return false;
//	}
//
//	const FVector OldLocation = UpdatedComponent->GetComponentLocation();
//	float PawnRadius, PawnHalfHeight;
//	CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);
//
//	// Don't bother stepping up if top of capsule is hitting something.
//	const float InitialImpactZ = InHit.ImpactPoint.Z;
//	if (InitialImpactZ > OldLocation.Z + (PawnHalfHeight - PawnRadius))
//	{
//		return false;
//	}
//
//	if (GravDir.IsZero())
//	{
//		return false;
//	}
//
//	// Gravity should be a normalized direction
//	ensure(GravDir.IsNormalized());
//
//	float StepTravelUpHeight = MaxStepHeight;
//	float StepTravelDownHeight = StepTravelUpHeight;
//	const float StepSideZ = -1.f * FVector::DotProduct(InHit.ImpactNormal, GravDir);
//	float PawnInitialFloorBaseZ = OldLocation.Z - PawnHalfHeight;
//	float PawnFloorPointZ = PawnInitialFloorBaseZ;
//
//	if (IsMovingOnGround() && CurrentFloor.IsWalkableFloor())
//	{
//		// Since we float a variable amount off the floor, we need to enforce max step height off the actual point of impact with the floor.
//		const float FloorDist = FMath::Max(0.f, CurrentFloor.GetDistanceToFloor());
//		PawnInitialFloorBaseZ -= FloorDist;
//		StepTravelUpHeight = FMath::Max(StepTravelUpHeight - FloorDist, 0.f);
//		StepTravelDownHeight = (MaxStepHeight + MAX_FLOOR_DIST * 2.f);
//
//		const bool bHitVerticalFace = !IsWithinEdgeTolerance(InHit.Location, InHit.ImpactPoint, PawnRadius);
//		if (!CurrentFloor.bLineTrace && !bHitVerticalFace)
//		{
//			PawnFloorPointZ = CurrentFloor.HitResult.ImpactPoint.Z;
//		}
//		else
//		{
//			// Base floor point is the base of the capsule moved down by how far we are hovering over the surface we are hitting.
//			PawnFloorPointZ -= CurrentFloor.FloorDist;
//		}
//	}
//
//	// Don't step up if the impact is below us, accounting for distance from floor.
//	if (InitialImpactZ <= PawnInitialFloorBaseZ)
//	{
//		return false;
//	}
//
//	// Scope our movement updates, and do not apply them until all intermediate moves are completed.
//	FScopedMovementUpdate ScopedStepUpMovement(UpdatedComponent, EScopedUpdate::DeferredUpdates);
//
//	// step up - treat as vertical wall
//	FHitResult SweepUpHit(1.f);
//	const FQuat PawnRotation = UpdatedComponent->GetComponentQuat();
//	MoveUpdatedComponent(-GravDir * StepTravelUpHeight, PawnRotation, true, &SweepUpHit);
//
//	if (SweepUpHit.bStartPenetrating)
//	{
//		// Undo movement
//		ScopedStepUpMovement.RevertMove();
//		return false;
//	}
//
//	// step fwd
//	FHitResult Hit(1.f);
//	MoveUpdatedComponent(Delta, PawnRotation, true, &Hit);
//
//	// Check result of forward movement
//	if (Hit.bBlockingHit)
//	{
//		if (Hit.bStartPenetrating)
//		{
//			// Undo movement
//			ScopedStepUpMovement.RevertMove();
//			return false;
//		}
//
//		// If we hit something above us and also something ahead of us, we should notify about the upward hit as well.
//		// The forward hit will be handled later (in the bSteppedOver case below).
//		// In the case of hitting something above but not forward, we are not blocked from moving so we don't need the notification.
//		if (SweepUpHit.bBlockingHit && Hit.bBlockingHit)
//		{
//			HandleImpact(SweepUpHit);
//		}
//
//		// pawn ran into a wall
//		HandleImpact(Hit);
//		if (IsFalling())
//		{
//			return true;
//		}
//
//		// adjust and try again
//		const float ForwardHitTime = Hit.Time;
//		const float ForwardSlideAmount = SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit, true);
//
//		if (IsFalling())
//		{
//			ScopedStepUpMovement.RevertMove();
//			return false;
//		}
//
//		// If both the forward hit and the deflection got us nowhere, there is no point in this step up.
//		if (ForwardHitTime == 0.f && ForwardSlideAmount == 0.f)
//		{
//			ScopedStepUpMovement.RevertMove();
//			return false;
//		}
//	}
//
//	// Step down
//	MoveUpdatedComponent(GravDir * StepTravelDownHeight, UpdatedComponent->GetComponentQuat(), true, &Hit);
//
//	// If step down was initially penetrating abort the step up
//	if (Hit.bStartPenetrating)
//	{
//		ScopedStepUpMovement.RevertMove();
//		return false;
//	}
//
//	FStepDownResult StepDownResult;
//	if (Hit.IsValidBlockingHit())
//	{
//		// See if this step sequence would have allowed us to travel higher than our max step height allows.
//		const float DeltaZ = Hit.ImpactPoint.Z - PawnFloorPointZ;
//		if (DeltaZ > MaxStepHeight)
//		{
//			//UE_LOG(LogCharacterMovement, VeryVerbose, TEXT("- Reject StepUp (too high Height %.3f) up from floor base %f to %f"), DeltaZ, PawnInitialFloorBaseZ, NewLocation.Z);
//			ScopedStepUpMovement.RevertMove();
//			return false;
//		}
//
//		// Reject unwalkable surface normals here.
//		if (!IsWalkable(Hit))
//		{
//			// Reject if normal opposes movement direction
//			const bool bNormalTowardsMe = (Delta | Hit.ImpactNormal) < 0.f;
//			if (bNormalTowardsMe)
//			{
//				//UE_LOG(LogCharacterMovement, VeryVerbose, TEXT("- Reject StepUp (unwalkable normal %s opposed to movement)"), *Hit.ImpactNormal.ToString());
//				ScopedStepUpMovement.RevertMove();
//				return false;
//			}
//
//			// Also reject if we would end up being higher than our starting location by stepping down.
//			// It's fine to step down onto an unwalkable normal below us, we will just slide off. Rejecting those moves would prevent us from being able to walk off the edge.
//			if (Hit.Location.Z > OldLocation.Z)
//			{
//				//UE_LOG(LogCharacterMovement, VeryVerbose, TEXT("- Reject StepUp (unwalkable normal %s above old position)"), *Hit.ImpactNormal.ToString());
//				ScopedStepUpMovement.RevertMove();
//				return false;
//			}
//		}
//
//		// Reject moves where the downward sweep hit something very close to the edge of the capsule. This maintains consistency with FindFloor as well.
//		if (!IsWithinEdgeTolerance(Hit.Location, Hit.ImpactPoint, PawnRadius))
//		{
//			//UE_LOG(LogCharacterMovement, VeryVerbose, TEXT("- Reject StepUp (outside edge tolerance)"));
//			ScopedStepUpMovement.RevertMove();
//			return false;
//		}
//
//		// Don't step up onto invalid surfaces if traveling higher.
//		if (DeltaZ > 0.f && !CanStepUp(Hit))
//		{
//			//UE_LOG(LogCharacterMovement, VeryVerbose, TEXT("- Reject StepUp (up onto surface with !CanStepUp())"));
//			ScopedStepUpMovement.RevertMove();
//			return false;
//		}
//
//		// See if we can validate the floor as a result of this step down. In almost all cases this should succeed, and we can avoid computing the floor outside this method.
//		if (OutStepDownResult != NULL)
//		{
//			FindFloor(UpdatedComponent->GetComponentLocation(), StepDownResult.FloorResult, false, &Hit);
//
//			// Reject unwalkable normals if we end up higher than our initial height.
//			// It's fine to walk down onto an unwalkable surface, don't reject those moves.
//			if (Hit.Location.Z > OldLocation.Z)
//			{
//				// We should reject the floor result if we are trying to step up an actual step where we are not able to perch (this is rare).
//				// In those cases we should instead abort the step up and try to slide along the stair.
//				if (!StepDownResult.FloorResult.bBlockingHit && StepSideZ < MAX_STEP_SIDE_Z)
//				{
//					ScopedStepUpMovement.RevertMove();
//					return false;
//				}
//			}
//
//			StepDownResult.bComputedFloor = true;
//		}
//	}
//
//	// Copy step down result.
//	if (OutStepDownResult != NULL)
//	{
//		*OutStepDownResult = StepDownResult;
//	}
//
//	// Don't recalculate velocity based on this height adjustment, if considering vertical adjustments.
//	bJustTeleported |= !bMaintainHorizontalGroundVelocity;
//
//	return true;
//}