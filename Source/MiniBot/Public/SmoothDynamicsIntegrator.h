#pragma once

#include "CoreMinimal.h"
#include "SmoothDynamicsIntegrator.generated.h"

UCLASS()
class MINIBOT_API USmoothDynamicsIntegrator : public UObject
{
	GENERATED_BODY()
public:
	USmoothDynamicsIntegrator();
	void Initialize(const FVector& InitialPosition, float Frequency, float Damping, float UnderShoot);
	FVector Update(float DeltaTime, const FVector& TargetPosition, FVector Velocity = FVector::ZeroVector);

private:
	FVector Previous;
	FVector Current;
	FVector Delta;
	float K1, K2, K3;

	// Parameters
	float ResponseFrequency;
	float ResponseDamping;
	float ResponseUnderShoot;
	FVector X0;
};