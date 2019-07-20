#include "Components/MovingSceneRoot.h"
#include "Components/ArrowComponent.h"
#include "Curves/CurveFloat.h"
/*
UMovingSceneRoot::UMovingSceneRoot() :
	m_OnePosition{ 0, 1, 0 },
	m_OneMinusPosition{ 0, -1, 0 },
	m_CurrentIntervalPos{ 0 }
{
	PrimaryComponentTick.bCanEverTick = true;

#if WITH_EDITOR

	m_pToOneArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ToOneArrow"));
	m_pToOneArrow->SetupAttachment(this);
	SpanArrowComponent(RelativeLocation, m_OnePosition, m_pToOneArrow);

	m_pToOneMinusArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ToOneMinusArrow"));
	m_pToOneMinusArrow->SetupAttachment(this);
	SpanArrowComponent(RelativeLocation, m_OneMinusPosition, m_pToOneMinusArrow);
#endif

}

void UMovingSceneRoot::TickComponent
(
	const float DeltaTime, 
	ELevelTick TickType,
	FActorComponentTickFunction *ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	m_CurrentIntervalPos =  FMath::Fmod(m_CurrentIntervalPos + DeltaTime, m_IntervalLength);
	const auto LerpFactor{ m_pDynamics->GetFloatValue(m_CurrentIntervalPos) };

	auto NewRelativePos{ m_StartingRootPosition };
	NewRelativePos +=  m_OnePosition * FMath::Clamp(LerpFactor, 0.f, 1.f);
	NewRelativePos +=  m_OneMinusPosition * FMath::Clamp(LerpFactor, -1.f, 0.f);

	SetRelativeLocation(NewRelativePos);


}

void UMovingSceneRoot::BeginPlay()
{
	Super::BeginPlay();

	m_StartingRootPosition = RelativeLocation;


}


#if WITH_EDITOR
void UMovingSceneRoot::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	SpanArrowComponent(RelativeLocation, m_OnePosition, m_pToOneArrow);
	SpanArrowComponent(RelativeLocation, m_OneMinusPosition, m_pToOneMinusArrow);


}
#endif
	*/

void SpanArrowComponent(const FVector &From, const FVector &To, UArrowComponent *pArrow)
{
	pArrow->SetRelativeLocation(From);

	const auto Disp{ (To - From) };
	const auto Magnitude{ Disp.Size() };
	const auto Dir{ Disp.GetSafeNormal() };

	pArrow->SetRelativeRotation(Dir.Rotation());
	pArrow->SetRelativeScale3D(FVector{ Magnitude/100, 0, 0 });

	
}
