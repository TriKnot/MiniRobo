#include "IKLegComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UIKLegComponent::UIKLegComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UIKLegComponent::BeginPlay()
{
	Super::BeginPlay();

	// Set the start offset for the step target relative to the leg root
	StepTargetStartOffset = StepTarget->GetComponentLocation() - GetComponentTransform().GetLocation();
}

void UIKLegComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// At least 2 bones are required for the leg to function
	if(Bones.Num() < 2)
	{
		return;
	}

	// Check if the step target should be moved or if it's already being moved
	if( bIsMovingStepTarget || ShouldMoveStepTarget() )
	{
		MoveStepTarget(DeltaTime);
	}

	// Solve the IK
	SolveIK();

	// Draw Debug
	DrawDebug();
}

void UIKLegComponent::Initialize(USphereComponent* InStepTarget, USphereComponent* InPole, TArray<TObjectPtr<UIKLegComponent>> InOtherLegs)
{
	// Reset 
	Bones.Empty();
	TotalLength = 0.0f;
	
	for (int32 i = 0; i < BoneCount + 1 ; i++) // +1 for the root bone
	{
		FBone Bone;
		Bone.BoneLength = (i == 0) ? 0 : BoneLength; // Root bone has no length
		Bone.Transform = GetComponentTransform();
		Bones.Add(Bone);
		TotalLength += Bone.BoneLength;
	}

	// Initialize other properties
	BonePositions.SetNum(Bones.Num());
	BoneRotations.SetNum(Bones.Num());
	Pole = InPole;
	StepTarget = InStepTarget;
	OtherLegs = InOtherLegs;
}

void UIKLegComponent::SolveIK()
{
	// Get all Bones Positions
	for(int32 i = 0; i < Bones.Num(); i++)
	{
		BonePositions[i] = Bones[i].Transform.GetLocation();
	}
	
	// Update Root Position
	BonePositions[0] = GetComponentTransform().GetLocation();
	
	// Loop Through all the bones and solve the IK
	for(int32 i = 0; i < Iterations; i++)
	{
		// Backwards
		BackwardsSolve();
	
		// Move Towards the Pole
		if(Pole)
			MoveTowardsPole();
		
		// Forwards
		ForwardsSolve();
		
		// Close enough ?
		if(FVector::Distance(BonePositions.Last(), EndEffectorTargetLocation) < Tolerance)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Iterations: %d"), i));
			break;
		}
	}
	
	// Set all the bone positions
	for(int32 i = 0; i < Bones.Num(); i++)
	{
		Bones[i].Transform.SetLocation(BonePositions[i]);
	}
	
	// Set all the bone rotations
	for(int32 i = 1; i < Bones.Num(); i++)
	{
		Bones[i].Transform.SetRotation((BonePositions[i - 1] - BonePositions[i]).ToOrientationQuat());
	}

	Bones.Last().Transform.SetLocation(EndEffectorTargetLocation);
}

void UIKLegComponent::ForwardsSolve()
{
	for(int32 j = 1; j < Bones.Num(); j++)
	{
		BonePositions[j] = BonePositions[j - 1] + (BonePositions[j] - BonePositions[j - 1]).GetSafeNormal() * Bones[j].BoneLength;
	}
}

void UIKLegComponent::BackwardsSolve()
{
	for(int32 j = Bones.Num() - 1; j > 0; j--)
	{
		// If this is the end effector
		if(j == Bones.Num() - 1)
		{
			// Set the end effector to the target location
			BonePositions[j] = EndEffectorTargetLocation;
		}
		else
		{
			BonePositions[j] = BonePositions[j + 1] + (BonePositions[j] - BonePositions[j + 1]).GetSafeNormal() * Bones[j].BoneLength;
		}
	}
}

void UIKLegComponent::MoveTowardsPole()
{
	if (!Pole) return;

	const FVector PolePosition = Pole->GetComponentLocation();
	for (int32 i = 1; i < Bones.Num() - 1; i++) // Skip the first and last bones
	{
		FVector CurrentJointPosition = BonePositions[i];
		FVector TowardsPole = PolePosition - CurrentJointPosition;
		
		const float MoveDistanceFraction = 0.01f; // 0.01 = 1% of the distance to the pole
		FVector MoveDirection = TowardsPole.GetSafeNormal();
		const FVector NewPosition = CurrentJointPosition + MoveDirection * TowardsPole.Size() * MoveDistanceFraction;
	
		// Update the bone's position
		BonePositions[i] = NewPosition;
	}
}

void UIKLegComponent::SetStepDirection(const FVector& InDirection) const
{
	// Set the step target's location
	FVector Direction = InDirection.GetSafeNormal();
	Direction.Z = 0.0f;

	Direction = GetComponentTransform().InverseTransformVectorNoScale(Direction);

	StepTarget->SetRelativeLocation(StepTargetStartOffset + Direction * StepDistance);
}

void UIKLegComponent::DrawDebug()
{
	// Draw the Joint Positions
	if(bDrawJoints)
	{
		for(int32 i = 0; i < Bones.Num(); i++)
		{
			if(i == 0)
			{
				UKismetSystemLibrary::DrawDebugSphere(this, BonePositions[i], 5.0f, 12, FColor::Red, 0.0f, 0.0f);
			}
			else if(i == Bones.Num() - 1)
			{
				UKismetSystemLibrary::DrawDebugSphere(this, BonePositions[i], 5.0f, 12, FColor::Green, 0.0f, 0.0f);
			}
			else
			{
				UKismetSystemLibrary::DrawDebugSphere(this, BonePositions[i], 5.0f, 12, FColor::Blue, 0.0f, 0.0f);
			}
		}
	}
	if(bDrawBones)
	{
		// Draw the bones
		for(int32 i = 0; i < Bones.Num(); i++)
		{
			FVector Start = Bones[i].Transform.GetLocation();
			FVector End = Start + (Bones[i].Transform.GetRotation().GetForwardVector() * Bones[i].BoneLength);
			FColor Color;
			if(i == 0)
			{
				Color = FColor::Red;
			}
			else if(i == Bones.Num() - 1)
			{
				Color = FColor::Green;
			}
			else
			{
				Color = FColor::Blue;
			}
			UKismetSystemLibrary::DrawDebugArrow(this, Start, End, 50.0f, Color, 0.0f, 0.0f);
		}
	}
	
	// Draw the end effector target
	if(bDrawEndEffectorTarget)
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), EndEffectorTargetLocation, 5.0f, 12, FColor::Yellow, false, 0.0f);

	// Draw the step target
	if(bDrawStepTarget)
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), StepTarget->GetComponentLocation(), 5.0f, 12, FColor::Purple, false, 0.0f);

	// Draw step distance around the step target
	if(bDrawStepDistance)
		UKismetSystemLibrary::DrawDebugCircle(GetWorld(), StepTarget->GetComponentLocation(), StepDistance, 12, FColor::White,  0.0f, 1.0f, FVector(0, 1, 0), FVector(1, 0, 0), false);
}

void UIKLegComponent::MoveStepTarget(float DeltaTime)
{
	if (!StepTarget) return;

	if (CurrentInterpolationTime == 0.0f) // Check if interpolation needs to be started or if it's already started
	{
		// Line Trace down to find the ground
		FHitResult HitResult;
		const FVector StartLocation = StepTarget->GetComponentLocation() + FVector::UpVector * TotalLength * MaxStepHeighPercentage;
		const FVector EndLocation = StepTarget->GetComponentLocation() + FVector::DownVector * TotalLength * MaxStepHeighPercentage;
		if(UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartLocation, EndLocation, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>({GetOwner()}), EDrawDebugTrace::None, HitResult, true))
		{
			TargetStepLocation = HitResult.Location;
		}
		else // If no ground is found, stretch it downwards TODO: This should be handled differently
		{
			TargetStepLocation = EndLocation;
		}
		StartStepLocation = EndEffectorTargetLocation; // Set the start location for interpolation
		bIsMovingStepTarget = true; 
	}
	
	CurrentInterpolationTime += DeltaTime;

	const float Alpha = FMath::Clamp(CurrentInterpolationTime / InterpolationDuration, 0.0f, 1.0f);

	// Lerp between the start and target locations using the eased alpha
	const float EasedHorizontalAlpha = FMath::InterpEaseInOut(0.0f, 1.0f, Alpha, StepEaseCurveExponent); 
	FVector HorizontalLocation = FMath::Lerp(StartStepLocation, TargetStepLocation, EasedHorizontalAlpha);

	// Calculate the vertical offset using a sine wave
	float VerticalOffset = FMath::Sin(Alpha * PI) * StepHeight;
	FVector FinalLocation = HorizontalLocation + FVector(0, 0, VerticalOffset);
	
	// Update the end effector target location with the new position, including the vertical offset
	EndEffectorTargetLocation = FinalLocation;
	
	// Reset interpolation time if the target is reached or exceeded
	if (Alpha >= 1.0f)
	{
		CurrentInterpolationTime = 0.0f; 
		bIsMovingStepTarget = false; 
	}
}

bool UIKLegComponent::ShouldMoveStepTarget()
{
	// Check that no other legs are currently moving the step target
	for (const TObjectPtr<UIKLegComponent> OtherLeg : OtherLegs)
	{
		if(OtherLeg->IsMovingStepTarget())
		{
			return false;
		}
	}
	
	// Calculate distances
	const FVector StepTargetLocation = StepTarget->GetComponentLocation();

	// If end effector target is too far from the step target
	if(FVector::Distance(EndEffectorTargetLocation, StepTargetLocation) > StepDistance)
	{
		return true;
	}
	// If the last bone is too far from the step target
	if(FVector::Distance(StepTargetLocation, Bones.Last().Transform.GetLocation()) > StepDistance)
	{
		return true;
	}
	// If EndEffectorTargetLocation if further than total length
	if(FVector::Distance(EndEffectorTargetLocation, Bones[0].Transform.GetLocation()) > TotalLength)
	{
		return true;
	}

	return false;
}