#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "MiniBotCharacter.generated.h"

class UIKLegComponent;
class USphereComponent;
class UCapsuleComponent;
class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class MINIBOT_API AMiniBotCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	
	/** Returns FollowCamera subobject **/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
public:
	// Sets default values for this pawn's properties
	AMiniBotCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	/** Shoot Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void Move(const FInputActionValue& Value);
	
	void Look(const FInputActionValue& Value);

	void Jump(const FInputActionValue& Value);

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IK")
	USceneComponent* LegRoot;

	UPROPERTY()
	TArray<TObjectPtr<UIKLegComponent>> Legs;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK")
	TObjectPtr<UIKLegComponent> LegBack;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK")
	TObjectPtr<UIKLegComponent> LegFrontRight;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK")
	TObjectPtr<UIKLegComponent> LegFrontLeft;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK")
	TObjectPtr<USphereComponent> LegStepTargetBack;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK")
	TObjectPtr<USphereComponent> LegStepTargetFrontRight;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK")
	TObjectPtr<USphereComponent> LegStepTargetFrontLeft;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK")
	TObjectPtr<USphereComponent> LegPoleBack;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK")
	TObjectPtr<USphereComponent> LegPoleFrontRight;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IK")
	TObjectPtr<USphereComponent> LegPoleFrontLeft;

	void SetupLegs();
	
};
