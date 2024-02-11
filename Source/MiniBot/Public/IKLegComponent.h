#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "IKLegComponent.generated.h"

class USphereComponent;

USTRUCT(BlueprintType)
struct FBone
{
    GENERATED_BODY()

public:
    UPROPERTY()
    FTransform Transform;

    UPROPERTY()
    float BoneLength;

    UPROPERTY() // Not used yet
    FVector AxisOfRotation;
};

UCLASS(BlueprintType, Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MINIBOT_API UIKLegComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    UIKLegComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
    int32 BoneCount = 2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
    float BoneLength = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
    USphereComponent* StepTarget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IK")
    FVector EndEffectorTargetLocation;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
    USphereComponent* Pole;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
    int32 Iterations = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
    float Tolerance = 0.01f;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "IK")
    float TotalLength;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
    FVector PolePositionOffset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
    float EndEffectorMaxSpeed = 0.01f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float MaxStepHeighPercentage = 0.5f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IK")
    TArray<FBone> Bones;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK")
    float StepDistance = 100.0f;

    UPROPERTY()
    TArray<FVector> BonePositions;

    UPROPERTY()
    TArray<FQuat> BoneRotations;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
    bool bDrawDebug;

    void Init(USphereComponent* InStepTarget, USphereComponent* InPole);
    void SolveIK();
    void DrawDebug();

    void MoveStepTarget(float DeltaTime);
    bool ShouldMoveStepTarget();

    void ForwardsSolve();
    void BackwardsSolve();
    void MoveTowardsPole();

private:
    UPROPERTY()
    bool bIsMovingStepTarget;
    UPROPERTY()
    float CurrentInterpolationTime = 0.0f;
    UPROPERTY()
    float InterpolationDuration = 0.2f;
    UPROPERTY()
    FVector StartStepLocation;
    UPROPERTY()
    FVector TargetStepLocation;

    UPROPERTY()
    FVector StepTargetStartOffset;



public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK")
    float StepHeight = 25.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK")
    float StepEaseCurveExponent = 2.0f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IK")
    TArray<TObjectPtr<UIKLegComponent>> Legs;
    UFUNCTION()
    bool IsMovingStepTarget() const { return bIsMovingStepTarget; }

    UFUNCTION()
    void RegisterOtherLegs(const TArray<UIKLegComponent*>& InLegs) { Legs = InLegs; }
    UFUNCTION()
    void RegisterOtherLeg(UIKLegComponent* InLeg) { Legs.AddUnique(InLeg); }
    UFUNCTION()
    void SetStepOffset(const FVector& InDirection) const;

    UFUNCTION()
    FVector GetEndEffectorRelativeLocation() const { return  Bones.Last().Transform.GetLocation() - GetComponentLocation(); }
    UFUNCTION()
    FVector GetStepTargetStartOffset() const { return StepTargetStartOffset; }
    
};
