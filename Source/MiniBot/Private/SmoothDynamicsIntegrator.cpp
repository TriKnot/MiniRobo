#include "SmoothDynamicsIntegrator.h"
#include "Math/UnrealMathUtility.h"

USmoothDynamicsIntegrator::USmoothDynamicsIntegrator()
	:	K1(0), K2(0), K3(0),
		ResponseFrequency(1.0f), 
		ResponseDamping(1.0f),
		ResponseUnderShoot(0.0f),
		X0(FVector::ZeroVector)
{
}

void USmoothDynamicsIntegrator::Initialize(const FVector& InitialPosition, float Frequency, float Damping, float UnderShoot)
{
	X0 = InitialPosition;
	ResponseFrequency = Frequency;
	ResponseDamping = Damping;
	ResponseUnderShoot = UnderShoot;

	K1 = ResponseDamping / (PI * ResponseFrequency);
	K2 = 1 / ((2 * PI * ResponseFrequency) * (2 * PI * ResponseFrequency));
	K3 = ResponseUnderShoot * ResponseDamping / (2 * PI * ResponseFrequency);

	Previous = X0;
	Current = X0;
	Delta = FVector::ZeroVector;
}

FVector USmoothDynamicsIntegrator::Update(float DeltaTime, const FVector& TargetPosition, FVector Velocity)
{
	if (Velocity.IsZero())
	{
		Velocity = (TargetPosition - Previous) / DeltaTime;
	}
	Previous = TargetPosition;

	const float StableK2 = FMath::Max(K2, FMath::Max(DeltaTime * DeltaTime / 2 + DeltaTime * K1 / 2, DeltaTime * K1));
	Current = Current + DeltaTime * Delta;
	Delta = Delta + DeltaTime * (TargetPosition + K3 * Delta - Current - K1 * Delta) / StableK2;
	return Current;
}
