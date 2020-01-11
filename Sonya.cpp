// Fill out your copyright notice in the Description page of Project Settings.


#include "Sonya.h"
#include "Components/InputComponent.h"
#include "Kismet/KismetMathLibrary.h"
#define RAW 0

// Sets default values
ASonya::ASonya()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Gravity = 980;
	MovementAcceleration = 100.0f;
	JumpAcceleration = 500.0f;
	GroundFriction = 0.9f;
	CameraSpeed = 100.0f;

	FrontState = CreateDefaultSubobject<UOrientationStateComponent>("FrontState");
	AddOwnedComponent(FrontState);

	SideState = CreateDefaultSubobject<UOrientationStateComponent>("SideState");
	AddOwnedComponent(SideState);

	BackState = CreateDefaultSubobject<UOrientationStateComponent>("BackState");
	AddOwnedComponent(BackState);

	BottomState = CreateDefaultSubobject<UOrientationStateComponent>("BottomState");
	AddOwnedComponent(BottomState);

	TopState = CreateDefaultSubobject<UOrientationStateComponent>("TopState");
	AddOwnedComponent(TopState);

	CurrentState = BottomState;

	RootCapsule = CreateDefaultSubobject<UCapsuleComponent>("RootCapsule");
	SetRootComponent(RootCapsule);
	RootCapsule->SetCollisionResponseToAllChannels(ECR_Block);

	SonyaMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SonyaMesh");
	SonyaMesh->SetupAttachment(RootComponent);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 50.0f), FRotator(0.0f, 0.0f, 0.0f));
	SpringArm->TargetArmLength = 400.f;
	SpringArm->bEnableCameraLag = false;
	SpringArm->CameraLagSpeed = 0.0f;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;
	SpringArm->bInheritYaw = false;

	SonyaMovement = CreateDefaultSubobject<USonyaMovementComponent>("SonyaMovement");
	SonyaMovement->SetUpdatedComponent(RootComponent);
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
}

// Called when the game starts or when spawned
void ASonya::BeginPlay()
{
	Super::BeginPlay();

	if (Controller != NULL)
	{
		MyController = Cast<ASonyaPlayerController>(Controller);
		SonyaMovement->SetController(MyController);
	}
	if (Camera != NULL)
	{
		SonyaMovement->SetCamera(Camera);
	}
}

// Called every frame
void ASonya::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	MoveCamera(DeltaTime);

	//SonyaMovement->AddInput();
	SonyaMovement->CallableTick(DeltaTime);
	//ApplyRotationInput(DeltaTime);
	//ApplyInput(DeltaTime);
	//ApplyGravity(DeltaTime);
	//Move(DeltaTime);
	//Rotate(DeltaTime);
	////UpdateNormalForce(DeltaTime);
	UE_LOG(LogTemp, Warning, TEXT("%f fps"), 1 / DeltaTime);
	LastDeltaTime = DeltaTime;
}

// Called to bind functionality to input
void ASonya::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	FInputActionBinding* ActionBinding;
	FInputAxisBinding* AxisBinding;

	ActionBinding = &PlayerInputComponent->BindAction("JumpInput", IE_Pressed, this, &ASonya::OnJumpInput);
	ActionBinding->bConsumeInput = false;

	ActionBinding = &PlayerInputComponent->BindAction("CrouchInput", IE_Pressed, this, &ASonya::OnCrouchPressed);
	ActionBinding->bConsumeInput = false;

	ActionBinding = &PlayerInputComponent->BindAction("CrouchInput", IE_Released, this, &ASonya::OnCrouchReleased);
	ActionBinding->bConsumeInput = false;

	AxisBinding = &PlayerInputComponent->BindAxis("ForwardInput", this, &ASonya::OnForwardInput);
	AxisBinding->bConsumeInput = false;

	AxisBinding = &PlayerInputComponent->BindAxis("RightInput", this, &ASonya::OnYawInput);
	AxisBinding->bConsumeInput = false;

}

void ASonya::OnForwardInput(float Value)
{
	if (!RAW)
	{
		return;
	}

	if (Value != 0.0)
	{
		Velocity += GetActorForwardVector() * Value * CurrentState->MaxAcceleration * LastDeltaTime;
	}
}

void ASonya::OnYawInput(float Value)
{
	if (!RAW)
	{
		return;
	}

	if (Value != 0.0)
	{
		AngularVelocity.Yaw += Value * CurrentState->MaxRotationAceleration.Yaw * LastDeltaTime;
	}
	else
	{
		float NewYaw = FMath::Abs(AngularVelocity.Yaw) - CurrentState->RotationDeceleration.Yaw;

		if (NewYaw < 0)
		{
			NewYaw = 0;
		}

		if (AngularVelocity.Yaw >= 0.0)
		{
			AngularVelocity.Yaw = NewYaw;
		}
		else
		{
			AngularVelocity.Yaw = -NewYaw;
		}
		
	}
}

void ASonya::OnJumpInput()
{
	/*if (!RAW)
	{
		return;
	}*/
	SonyaMovement->Jump();
}

void ASonya::Move(float DeltaTime)
{
	FVector Delta = Velocity * DeltaTime;
	

	if (!(Velocity * DeltaTime).IsNearlyZero())
	{
		FHitResult Hit;

		SonyaMovement->SafeMoveUpdatedComponent(Delta, GetActorRotation().Quaternion(), true, Hit);

		UsableNormalForce = -FVector::DotProduct(Hit.Normal, Velocity) * Hit.Normal;

		Velocity = Velocity + UsableNormalForce;

		UsableNormalForce = UsableNormalForce * DeltaTime;

		// If we bumped into something, try to slide along it
		if (Hit.IsValidBlockingHit())
		{
			SonyaMovement->SlideAlongSurface(Delta, 1.0f - Hit.Time, Hit.Normal, Hit);
		}

		OutputVector("Velocity Long After", Velocity);
	}
	
}

void ASonya::Rotate(float DeltaTime)
{
	/*if (!RAW)
	{
		return;
	}*/

	SetActorRotation(GetActorRotation() + AngularVelocity * DeltaTime);
	SpringArm->SetRelativeRotation(SpringArm->RelativeRotation + AngularVelocity*(-DeltaTime));

	AngularVelocity = FRotator(0.0f,0.0f,0.0f);
}

void ASonya::ApplyGravity(float DeltaTime)
{
	Velocity += FVector(0.0f, 0.0f, -1.0f * Gravity) * DeltaTime;
}

void ASonya::ApplyInput(float DeltaTime)
{
	// Rotate input
	FVector Direction = GetInputRotation().RotateVector(MyController->GetXYDirection());
	if (Direction.IsNearlyZero())
	{
		//Decelerate
		Decelerate(DeltaTime);
	}
	else
	{
		//Accelerate
		Accelerate(DeltaTime, Direction);
	}
	// Interpret input

	//Direction = ViewRotation.RotateVector(Direction);
	Velocity += DeltaTime * CurrentState->MaxAcceleration * Direction;
	//OutputVector(TEXT("Velocity"), Velocity);
	
	//OrientRotationToNormal(FVector(0.0f, 0.0f, 1.0f), FVector(0.0f, 0.0f, 1.0f).Rotation());
}

void ASonya::OutputVector(FString VectorName, FVector Vector)
{
	UE_LOG(LogTemp, Warning, TEXT(" %s = ( %f , %f , %f )"), *VectorName, Vector.X, Vector.Y, Vector.Z);
}

void ASonya::OutputRotator(FString RotatorName, FRotator Rotator)
{
	UE_LOG(LogTemp, Warning, TEXT(" %s = ( P %f , Y %f , R %f )"), *RotatorName, Rotator.Pitch, Rotator.Yaw, Rotator.Roll);
}

FVector ASonya::GetViewForwardVector()
{
	return FRotator(0.0, Camera->GetComponentRotation().Yaw, 0.0f).RotateVector(FVector(1.0f, 0.0f, 0.0f));
}

FVector ASonya::GetViewUpVector()
{
	return FRotator(Camera->GetComponentRotation().Pitch, 0.0f, 0.0f).RotateVector(FVector(0.0f, 0.0f, 1.0f));
}

void ASonya::MoveCamera(float DeltaTime)
{
	if (MyController->GetIsMoveCameraPressed())
	{
		FRotator RotationAmount = FRotator(MyController->GetPitchValue() * DeltaTime * CameraSpeed, MyController->GetRollValue() * DeltaTime * CameraSpeed, 0.0f);
		SpringArm->SetRelativeRotation(SpringArm->RelativeRotation + RotationAmount);

		if (SpringArm->RelativeRotation.Pitch > 80.0f)
		{
			SpringArm->RelativeRotation.Pitch = 80.0f;
		}
		else if (SpringArm->RelativeRotation.Pitch < -80.0f)
		{
			SpringArm->RelativeRotation.Pitch = -80.0f;
		}
	}
}

FRotator ASonya::OrientRotationToNormal(FVector UpVector, FVector Normal, FRotator CurrentRotation)
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

FRotator ASonya::GetInputRotation()
{
	FRotator InputRotation = FRotator(GetActorRotation().Pitch, Camera->GetComponentRotation().Yaw, GetActorRotation().Roll);
	return InputRotation;
}

void ASonya::Decelerate(float DeltaTime)
{
	if (!UsableNormalForce.IsNearlyZero() && !Velocity.IsNearlyZero())
	{
		FVector Normal = UsableNormalForce;
		Normal.Normalize();
		FVector VelocityAlongPlane = Velocity - (FVector::DotProduct(Velocity, Normal) * Normal);
		float Amount = 1.0f;

		//Velocity = Velocity - VelocityAlongPlane + VelocityAlongPlane * (1 - CurrentState->DriftingDeceleration * DeltaTime);
	
		if (DecelerationCurve && CurrentState->MaxSpeed > 0.0)
		{
			Amount = DecelerationCurve->GetFloatValue( VelocityAlongPlane.Size() / CurrentState->MaxSpeed);
		}

		FVector FrictionDirection = -VelocityAlongPlane;
		FrictionDirection.Normalize();
		float Friction = CurrentState->DriftingDeceleration * DeltaTime * Amount;

		if (Friction > UsableNormalForce.Size())
		{
			Friction = UsableNormalForce.Size();
		}

		if (Friction > VelocityAlongPlane.Size())
		{
			Friction = VelocityAlongPlane.Size();
		}

		//OutputVector(TEXT("Friction"), FrictionDirection * Friction);
		Velocity = Velocity + FrictionDirection * Friction;
	}
}

FVector ASonya::GetVelocityAlongPlane()
{
	FVector Normal = UsableNormalForce;
	Normal.Normalize();
	FVector VelocityAlongPlane = Velocity - (FVector::DotProduct(Velocity, Normal) * Normal);

	return VelocityAlongPlane;
}

void ASonya::Accelerate(float DeltaTime, FVector Direction)
{

}

void ASonya::RotateToMatchYaw(float DeltaTime, FVector Direction)
{
	// Speed up or slow down?

}

void ASonya::ApplyRotationInput(float DeltaTime)
{
	float PitchRotation = MyController->GetPitchValue() * CurrentState->MaxRotationAceleration.Pitch * DeltaTime;
	float RollRotation = MyController->GetRollValue() * CurrentState->MaxRotationAceleration.Roll * DeltaTime;

	AngularVelocity += FRotator(PitchRotation, 0.0f, RollRotation);

	//OutputRotator(TEXT("Angular Velocity"), AngularVelocity);
}

void ASonya::UpdateNormalForce(float DeltaTime)
{
	UsableNormalForce = UsableNormalForce - CurrentState->BaseImpact * UsableNormalForce.GetSafeNormal();
	if (UsableNormalForce.Size() < 0.0f)
	{
		UsableNormalForce = UsableNormalForce * 0.0f;
	}
}

void ASonya::OnCrouchPressed()
{
	SonyaMovement->Crouch();
}

void ASonya::OnCrouchReleased()
{
	SonyaMovement->UnCrouch();
}