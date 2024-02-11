#include "MiniBotCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "IKLegComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

AMiniBotCharacter::AMiniBotCharacter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character won't move in the direction of input
	
	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 550.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	
	// Debug
	GetArrowComponent()->SetScreenSize(1500);

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	SetupLegs();

}

void AMiniBotCharacter::BeginPlay()
{
	Super::BeginPlay();

	if(Controller)
	{
		// Add Input Mapping Context
		if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}

	// Register legs
	LegBack->RegisterOtherLegs({LegFrontRight, LegFrontLeft});
	LegFrontRight->RegisterOtherLegs({LegBack, LegFrontLeft});
	LegFrontLeft->RegisterOtherLegs({LegBack, LegFrontRight});

}


void AMiniBotCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Get Move Direction and velocity from character move component
	const FVector MoveDirection = GetCharacterMovement()->GetLastInputVector();
	for (const TObjectPtr<UIKLegComponent> Leg : Legs)
	{
		Leg->SetStepOffset(MoveDirection);
	}
}

void AMiniBotCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jump
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMiniBotCharacter::Jump);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMiniBotCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMiniBotCharacter::Look);
	}
}


void AMiniBotCharacter::Move(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		// Input is a Vector2D
		const FVector2D MovementVector = Value.Get<FVector2D>();

		// Get Control rotation
		const FRotator ControlRotation = Controller->GetControlRotation();

		// Add control rotation to movement input
		const FVector ForwardVector = FRotationMatrix(ControlRotation).GetScaledAxis(EAxis::X);
		const FVector RightVector = FRotationMatrix(ControlRotation).GetScaledAxis(EAxis::Y);
		
		AddMovementInput(ForwardVector, MovementVector.Y);
		AddMovementInput(RightVector, MovementVector.X);
	}
}

void AMiniBotCharacter::Look(const FInputActionValue& Value)
{

	if (Controller != nullptr)
	{
		// Input is a Vector2D
		const FVector2D LookAxisVector = Value.Get<FVector2D>();
		
		// Add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMiniBotCharacter::Jump(const FInputActionValue& Value)
{
}

void AMiniBotCharacter::SetupLegs()
{
	// Clear Legs array
	Legs.Empty();

	// Create the leg root
	LegRoot = CreateDefaultSubobject<USceneComponent>(TEXT("LegRoot"));
	LegRoot->SetupAttachment(GetCapsuleComponent());
	LegRoot->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	
	// Create legs
	LegBack = CreateDefaultSubobject<UIKLegComponent>(TEXT("LegBack"));
	LegBack->SetupAttachment(LegRoot);
	Legs.Add(LegBack);
	
	LegFrontRight = CreateDefaultSubobject<UIKLegComponent>(TEXT("LegFrontRight"));
	LegFrontRight->SetupAttachment(LegRoot);
	Legs.Add(LegFrontRight);
	
	LegFrontLeft = CreateDefaultSubobject<UIKLegComponent>(TEXT("LegFrontLeft"));
	LegFrontLeft->SetupAttachment(LegRoot);
	Legs.Add(LegFrontLeft);
	
	// Create leg step targets
	LegStepTargetBack = CreateDefaultSubobject<USphereComponent>(TEXT("LegStepTargetBack"));
	LegStepTargetBack->SetupAttachment(LegBack);
	LegStepTargetBack->SetSphereRadius(5.0f);
	LegStepTargetBack->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LegStepTargetFrontRight = CreateDefaultSubobject<USphereComponent>(TEXT("LegStepTargetFrontRight"));
	LegStepTargetFrontRight->SetupAttachment(LegFrontRight);
	LegStepTargetFrontRight->SetSphereRadius(5.0f);
	LegStepTargetFrontRight->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	LegStepTargetFrontLeft = CreateDefaultSubobject<USphereComponent>(TEXT("LegStepTargetFrontLeft"));
	LegStepTargetFrontLeft->SetupAttachment(LegFrontLeft);
	LegStepTargetFrontLeft->SetSphereRadius(5.0f);
	LegStepTargetFrontLeft->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// Create leg poles
	LegPoleBack = CreateDefaultSubobject<USphereComponent>(TEXT("LegPoleBack"));
	LegPoleBack->SetupAttachment(LegBack);
	LegPoleBack->SetSphereRadius(5.0f);
	LegPoleBack->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LegPoleFrontRight = CreateDefaultSubobject<USphereComponent>(TEXT("LegPoleFrontRight"));
	LegPoleFrontRight->SetupAttachment(LegFrontRight);
	LegPoleFrontRight->SetSphereRadius(5.0f);
	LegPoleFrontRight->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LegPoleFrontLeft = CreateDefaultSubobject<USphereComponent>(TEXT("LegPoleFrontLeft"));
	LegPoleFrontLeft->SetupAttachment(LegFrontLeft);
	LegPoleFrontLeft->SetSphereRadius(5.0f);
	LegPoleFrontLeft->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Initialize legs
	LegBack->Init(LegStepTargetBack, LegPoleBack);
	LegFrontRight->Init(LegStepTargetFrontRight, LegPoleFrontRight);
	LegFrontLeft->Init(LegStepTargetFrontLeft, LegPoleFrontLeft);

	// Register legs
	LegBack->RegisterOtherLegs({LegFrontRight, LegFrontLeft});
	LegFrontRight->RegisterOtherLegs({LegBack, LegFrontLeft});
	LegFrontLeft->RegisterOtherLegs({LegBack, LegFrontRight});
}
