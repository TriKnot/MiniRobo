#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "IKLegComponent.generated.h"

USTRUCT(BlueprintType)
struct FBone
{
    GENERATED_BODY()

public:
    UPROPERTY(/*EditAnywhere, BlueprintReadWrite, Category = "IK"*/)
    FTransform Transform;

    UPROPERTY(/*EditAnywhere, BlueprintReadWrite, Category = "IK"*/)
    float BoneLength;

    UPROPERTY(/*EditAnywhere, BlueprintReadWrite, Category = "IK"*/)
    FVector AxisOfRotation; // Not used yet, but planned for future enhancements
};

UCLASS(BlueprintType, Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MINIBOT_API UIKLegComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    UIKLegComponent();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    // IK setup properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
    int32 BoneCount = 2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
    float BoneLength = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
    USphereComponent* StepTarget;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
    USphereComponent* Pole;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IK")
    FVector EndEffectorTargetLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
    int32 Iterations = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
    float Tolerance = 0.01f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IK")
    TArray<FBone> Bones;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK")
    float StepDistance = 100.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK")
    float StepHeight = 25.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK")
    float StepEaseCurveExponent = 2.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "IK")
    float TotalLength;

    void Initialize(USphereComponent* InStepTarget, USphereComponent* InPole, TArray<TObjectPtr<UIKLegComponent>> InOtherLegs);
    void MoveStepTarget(float DeltaTime);
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
    FVector PolePositionOffset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
    float EndEffectorMaxSpeed = 0.01f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float MaxStepHeighPercentage = 0.5f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IK", meta = (AllowPrivateAccess = "true"))
    TArray<TObjectPtr<UIKLegComponent>> OtherLegs;

    UPROPERTY()
    TArray<FVector> BonePositions;

    UPROPERTY()
    TArray<FQuat> BoneRotations;

    // Debug properties
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IKDebug")
    bool bDrawJoints = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IKDebug")
    bool bDrawBones = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IKDebug")
    bool bDrawEndEffectorTarget = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IKDebug")
    bool bDrawStepTarget = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IKDebug")
    bool bDrawStepDistance = false;

private:
    // IK functions
    void SolveIK();
    void ForwardsSolve();
    void BackwardsSolve();
    void MoveTowardsPole();

    void DrawDebug();
    bool ShouldMoveStepTarget();
    
    // Properties for managing dynamic step target movement
    bool bIsMovingStepTarget;
    float CurrentInterpolationTime = 0.0f;
    float InterpolationDuration = 0.15f;
    FVector StartStepLocation;
    FVector TargetStepLocation;
    FVector StepTargetStartOffset;
    
    UFUNCTION()
    bool IsMovingStepTarget() const { return bIsMovingStepTarget; }
    
public:
    // Functions for leg registration and step offset management
    UFUNCTION()
    void SetStepDirection(const FVector& InDirection) const;
    UFUNCTION()
    FVector GetStepTargetLocation() const { return StepTarget->GetComponentLocation(); }
    UFUNCTION()
    FVector GetEndEffectorLocation() const { return  Bones.Last().Transform.GetLocation(); }
    UFUNCTION()
    FVector GetStepTargetStartOffset() const { return StepTargetStartOffset; }

};
