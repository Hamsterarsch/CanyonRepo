// Fill out your copyright notice in the Description page of Project Settings.


#include "PlaceableBase.h"
#include "Misc/CanyonLogs.h"
#include "Components/StaticMeshCanyonComp.h"
#include "Misc/CanyonGM.h"
#include "WidgetComponent.h"
#include "WidgetBase/InfluenceDisplayWidgetBase.h"
#include "Components/CanyonMeshCollisionComp.h"


APlaceableBase::APlaceableBase()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
	m_pInfluenceWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("InfluenceWidget"));
	m_pInfluenceWidgetComp->SetupAttachment(GetRootComponent());

	m_pMeshCollisionComp = CreateDefaultSubobject<UCanyonMeshCollisionComp>(TEXT("ShapeCollider"));
	m_pMeshCollisionComp->SetupAttachment(GetRootComponent());


}


FString APlaceableBase::GetInfluenceQualifier(const TSubclassOf<APlaceableBase>& ForClass)
{
	auto *pClass{ ForClass.Get() };
	auto *pCdo{ Cast<APlaceableBase>(pClass->ClassDefaultObject) };
	
	auto InfluenceEnumVal{ pCdo->GetInfluenceEnumValue() };
	auto *pInfluenceEnumType{ pCdo->GetInfluenceEnumClass() };

	auto EnumDisplayText{ pInfluenceEnumType->GetDisplayNameTextByIndex(InfluenceEnumVal) };	
	return  EnumDisplayText.ToString();


}

float APlaceableBase::GetPlaceableNormalZMin() const
{
	return FMath::Cos(FMath::DegreesToRadians(m_PlaceableMaxSlopeAngle));


}

float APlaceableBase::GetPlaceableNormalZMax() const
{
	return FMath::Cos(FMath::DegreesToRadians(m_PlaceableMinSlopeAngle));


}

TArray<UActorComponent *> APlaceableBase::GetPlaceableMeshComps()
{
	return GetComponentsByClass(UStaticMeshCanyonComp::StaticClass());


}

UClass* APlaceableBase::GetInfluenceWidgetClass() const
{
	return m_InfluenceDisplayWidgetClass.Get();


}

int32 APlaceableBase::BeginInfluenceVisFor(const TSubclassOf<APlaceableBase> &TargetClass)
{
	auto *pOtherClass{ TargetClass.Get() };
	auto *pGM{ Cast<ACanyonGM>(GetWorld()->GetAuthGameMode()) };
	if(!pOtherClass || !pGM)
	{
		return 0;
	}
	
	auto Influence
	{
		pGM->GetInfluenceForPlaceable
		(
			GetInfluenceQualifier(pOtherClass),
			GetInfluenceQualifier(this->GetClass())
		)
	};

	if(Influence != 0)
	{
		SetInfluenceDisplayed(Influence);

	}

	return Influence;


}

int32 APlaceableBase::EndInfluenceVis()
{
	auto *pWidget{ Cast<UInfluenceDisplayWidgetBase>(m_pInfluenceWidgetComp->GetUserWidgetObject()) };
	if (pWidget)
	{
		return pWidget->EndDisplayInfluence();
	}

	return 0;


}

FString APlaceableBase::GetPlaceableCategory() const
{
	return GetInfluenceEnumClass()->GetDisplayNameTextByIndex(GetInfluenceEnumValue()).ToString();


}

void APlaceableBase::SetInfluenceDisplayed(const int32 Influence)
{
	auto *pWidget{ Cast<UInfluenceDisplayWidgetBase>(m_pInfluenceWidgetComp->GetUserWidgetObject()) };
	if (pWidget)
	{
		pWidget->BeginDisplayInfluence(Influence);
	}


}

void APlaceableBase::InitInfluenceDisplayWidget(UClass *pClass)
{
	m_pInfluenceWidgetComp->SetWidgetClass(pClass);
	m_pInfluenceWidgetComp->InitWidget();
	m_pInfluenceWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
		
	m_pInfluenceWidgetComp->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Collapsed);


}

void APlaceableBase::BeginPlay()
{
	Super::BeginPlay();

	if(auto *pClass{ m_InfluenceDisplayWidgetClass.Get() })
	{
		InitInfluenceDisplayWidget(pClass);
	}


}
