#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "MiniBotCharacter.generated.h"

UCLASS()
class MINIBOT_API AMiniBotCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMiniBotCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// Helper function to initialize leg components
	void SetupLegs();

public:
	// Camera and input setup
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	// Input Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);

	// Body mesh and IK setup
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Body")
	class UStaticMeshComponent* BodyMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IK")
	class USceneComponent* LegRoot;
	UPROPERTY()
	TArray<TObjectPtr<class UIKLegComponent>> Legs;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK")
	TObjectPtr<class UIKLegComponent> LegBack;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK")
	TObjectPtr<class UIKLegComponent> LegFrontRight;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK")
	TObjectPtr<class UIKLegComponent> LegFrontLeft;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK")
	TObjectPtr<class USphereComponent> LegStepTargetBack;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK")
	TObjectPtr<class USphereComponent> LegStepTargetFrontRight;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK")
	TObjectPtr<class USphereComponent> LegStepTargetFrontLeft;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK")
	TObjectPtr<class USphereComponent> LegPoleBack;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK")
	TObjectPtr<class USphereComponent> LegPoleFrontRight;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK")
	TObjectPtr<class USphereComponent> LegPoleFrontLeft;

public:
	// Smooth dynamics integrator for body motion
	UPROPERTY()
	class USmoothDynamicsIntegrator* BodyIntegrator;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Integrators", meta = (ClampMin = "0.0"))
	float BodyResponseFrequency;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Integrators", meta = (ClampMin = "0.0"))
	float BodyResponseDamping;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Integrators", meta = (ClampMin = "-1.0", ClampMax = "1.0"))
	float BodyResponseUnderShoot;

	
};
