// Fill out your copyright notice in the Description page of Project Settings.

#include "RTSPlayerEye.h"
#include "Misc/CanyonLogs.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "Placeables/PlaceableBase.h"
#include "RTSPlayerController.h"
#include "Engine/World.h"
#include "Misc/CollisionChannels.h"
#include "Misc/CanyonHelpers.h"
#include "Components/StaticMeshCanyonComp.h"
#include "Placeables/PlaceablePreview.h"
#include "UI/DeckState.h"
#include "UI/DeckStateRenderer.h"
#include "WidgetBase/MainHudWidgetBase.h"
#include "Misc/CanyonGM.h"


const FName ARTSPlayerEye::s_AxisMouseX{ TEXT("MouseX") };
const FName ARTSPlayerEye::s_AxisMouseY{ TEXT("MouseY") };

//Public--------------

ARTSPlayerEye::ARTSPlayerEye() :
	m_MouseShuffleSpeed{ 1 },
	m_KeyShuffleSpeed{ 1 },
	m_MouseTurnSpeed{ 1 },
	m_CameraMaxPitch{ 90 },
	m_CameraMinPitch{ 0 },
	m_BuildingRotationSteps{ 8 },
	m_bIsPlaceablePlaceable{ false },
	m_ZoomTargetDist{ 300 },
	m_ZoomTargetPitch{ -30 },
	m_CameraState{ this },
	m_PlacementState{ this }
{
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
	GetMovementComponent()->SetUpdatedComponent(GetRootComponent());

	GetCollisionComponent()->SetRelativeLocation({ 0,0,0 });

	m_pCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	m_pCameraSpringArm->SetupAttachment(GetRootComponent());
	m_pCameraSpringArm->SetRelativeLocation({ 0,0,0 });
	m_pCameraSpringArm->SetWorldRotation(FRotator{ -30, 0, 0 });

	m_pCameraSpringArm->bEditableWhenInherited = true;
	m_pCameraSpringArm->bEnableCameraLag = true;
	m_pCameraSpringArm->bEnableCameraRotationLag = true;

	m_pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	m_pCamera->SetupAttachment(m_pCameraSpringArm);
	m_pCamera->bEditableWhenInherited = true;

	m_aZoomNodes.Add(FZoomNode{ 300, 30 });

	m_pCursorRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CursorRoot"));
	//m_pCursorRoot->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	m_pCursorRoot->SetupAttachment(GetRootComponent());
	

}

void ARTSPlayerEye::CreateNewPlacablePreview(TSubclassOf<APlaceableBase> NewPlaceableClass)
{
	auto *pClass{ NewPlaceableClass.Get() };
	if (!pClass)
	{
		UE_LOG(LogCanyonPlacement, Error, TEXT("Tried to create a new placeable with null class."))
		return;
	}

	DiscardCurrentPlaceablePreview();

	FHitResult Hit;
	if(TraceForTerrainUnderCursor(Hit, GetWorld()))
	{
		m_pCursorRoot->SetWorldLocation(Hit.ImpactPoint);
	}
	
	auto *pNewPlaceable{ APlaceablePreview::SpawnPlaceablePreview(GetWorld(), FTransform::Identity, NewPlaceableClass) };
	if(!pNewPlaceable)
	{
		UE_LOG(LogCanyonPlacement, Error, TEXT("Could not spawn new placeable from class."))
		return;
	}


	m_pPlaceablePreviewCurrent = pNewPlaceable;
	m_pPlaceablePreviewCurrent->AttachToComponent(m_pCursorRoot, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	m_pPlaceablePreviewCurrent->SetActorRelativeLocation({ 0,0,0 });
	
	m_pPlaceablePreviewCurrent->NotifyUnplaceable();
	m_bIsPlaceablePlaceable = false;

	m_PlacementState.HandleInput(EAbstractInputEvent::PlaceBuilding_Start);
	

}

#pragma region Movement
void ARTSPlayerEye::AddForwardMovement(const float AxisValue)
{
	if (FMath::IsNearlyZero(AxisValue))
	{
		return;

	}
	auto Forward = m_pCameraSpringArm->GetForwardVector();
	//todo: delta time
	AddActorWorldOffset(Forward.GetSafeNormal2D() * AxisValue * m_KeyShuffleSpeed);




}

void ARTSPlayerEye::AddRightMovement(const float AxisValue)
{
	if (FMath::IsNearlyZero(AxisValue))
	{
		return;

	}
	auto Right = m_pCameraSpringArm->GetRightVector();
	//todo: delta time
	AddActorWorldOffset(Right.GetSafeNormal2D() * AxisValue * m_KeyShuffleSpeed);
	   

}

void ARTSPlayerEye::AddForwardMovementFromMouse(float AxisValue)
{
	auto Forward = m_pCameraSpringArm->GetForwardVector();
	//Mouse based shuffle (todo: delta time)
	AddActorWorldOffset(Forward.GetSafeNormal2D() * -AxisValue * m_MouseShuffleSpeed * m_pCamera->AspectRatio);
	

}

void ARTSPlayerEye::AddRightMovementFromMouse(float AxisValue)
{
	auto Right = m_pCameraSpringArm->GetRightVector();
	if (FMath::IsNearlyZero(AxisValue))
	{
		return;

	}
	AddActorWorldOffset(Right.GetSafeNormal2D() * -AxisValue * m_MouseShuffleSpeed);

}

void ARTSPlayerEye::AddCameraYaw(const float AxisValue)
{
	if (FMath::IsNearlyZero(AxisValue))
	{
		return;

	}
	m_pCameraSpringArm->AddRelativeRotation(FRotator{ 0, AxisValue * m_KeyTurnSpeed, 0 });


}

void ARTSPlayerEye::AddCameraYawFromMouse(float AxisValue)
{
	if (FMath::IsNearlyZero(AxisValue))
	{
		return;

	}
	m_pCameraSpringArm->AddRelativeRotation(FRotator{ 0, AxisValue * m_MouseTurnSpeed, 0 });


}

void ARTSPlayerEye::AddCameraPitchFromMouse(float AxisValue)
{
	if (FMath::IsNearlyZero(AxisValue))
	{
		return;

	}

	auto SummedPitch = m_pCameraSpringArm->RelativeRotation.Pitch + AxisValue * m_MouseTurnSpeed * m_pCamera->AspectRatio;
	if (SummedPitch > -m_CameraMaxPitch && SummedPitch < -m_CameraMinPitch)
	{
		m_pCameraSpringArm->RelativeRotation.Pitch = SummedPitch;
	}


}

void ARTSPlayerEye::ZoomOut()
{
	if (m_ZoomIndex < (m_aZoomNodes.Num() - 1))
	{
		++m_ZoomIndex;
		auto RelativeRot = m_pCameraSpringArm->RelativeRotation;
		if (RelativeRot.Pitch > -m_aZoomNodes[m_ZoomIndex].m_PitchMax)
		{
			m_pCameraSpringArm->SetRelativeRotation(FRotator{ -m_aZoomNodes[m_ZoomIndex].m_PitchMax, m_pCameraSpringArm->RelativeRotation.Yaw, 0 });

		}


	}


}

void ARTSPlayerEye::ZoomIn()
{
	if (m_ZoomIndex > 0)
	{
		--m_ZoomIndex;
		auto RelativeRot = m_pCameraSpringArm->RelativeRotation;
		if (RelativeRot.Pitch < -m_aZoomNodes[m_ZoomIndex].m_PitchMax)
		{
			m_pCameraSpringArm->SetRelativeRotation(FRotator{ -m_aZoomNodes[m_ZoomIndex].m_PitchMax, RelativeRot.Yaw, 0 });

		}


	}


}
#pragma endregion

void ARTSPlayerEye::SetPreviewCursorPosWs(const FVector &NewPos)
{
	m_pCursorRoot->SetWorldLocation(NewPos);


}

void ARTSPlayerEye::UpdateCurrentPlaceablePreview()
{
	if(!m_pPlaceablePreviewCurrent)
	{
		return;
	}

	FVector NewRootPos;	
	bool bIsCurrentPlaceablePlaceable{ m_PlacementRuler.HandleBuildingRules(m_pPlaceablePreviewCurrent, NewRootPos) };
	m_pCursorRoot->SetWorldLocation(NewRootPos);				

	if(bIsCurrentPlaceablePlaceable)
	{
		if(!m_bIsPlaceablePlaceable)
		{
			m_pPlaceablePreviewCurrent->NotifyPlaceable();
			m_bIsPlaceablePlaceable = true;
		}
	}
	else
	{
		if(m_bIsPlaceablePlaceable)
		{
			m_pPlaceablePreviewCurrent->NotifyUnplaceable();
			m_bIsPlaceablePlaceable = false;			
		}		
	}
	

}

bool ARTSPlayerEye::TryCommitPlaceablePreview()
{
	if(!m_pPlaceablePreviewCurrent)
	{
		return false;


	}

	if(m_bIsPlaceablePlaceable)
	{
		//fetch preview data and destroy
		auto *pBuildingClass{ m_pPlaceablePreviewCurrent->GetPreviewedClass() };
		const auto PreviewedInfluence{ m_pPlaceablePreviewCurrent->GetCurrentInfluence() };

		m_pPlaceablePreviewCurrent->Destroy();
		m_pPlaceablePreviewCurrent = nullptr;

		//spawn building

		const auto Transform{ m_pCursorRoot->GetComponentTransform() };
		auto *pSpawned{ GetWorld()->SpawnActor<APlaceableBase>(pBuildingClass, Transform) };
		
		//Update deck state (has to be done before gm notify)
		const auto PlaceableCategory{ pSpawned->GetPlaceableCategory() };
		m_pDeckState->ClearCachedPlaceableForCategory(PlaceableCategory);
		m_pDeckState->ChargeCountDecrementFor(PlaceableCategory);

		//Update gm
		auto *pGm{ Cast<ACanyonGM>(GetWorld()->GetAuthGameMode()) };
		pGm->AddPointsCurrent(PreviewedInfluence);
								
		return true;


	}
	return false;

	
}

void ARTSPlayerEye::DiscardCurrentPlaceablePreview(const bool bIsInstigatedByPlayer)
{
	if(m_pPlaceablePreviewCurrent)
	{
		UE_LOG(LogCanyonPlacement, Log, TEXT("Discarding current placeable."));
		m_pPlaceablePreviewCurrent->Destroy();
	}
	m_bIsPlaceablePlaceable = false;


}

int32 ARTSPlayerEye::GetCurrentChargesForPlaceables() const
{
	 return m_pDeckState->GetChargesCurrent();


}

void ARTSPlayerEye::NotifyOnDisplayNewDecks()
{
	m_pDeckState->NotifyOnDisplayNewDecks();


}

bool ARTSPlayerEye::GetAreDecksSelectable() const
{
	return m_pDeckState->GetAreDecksSelectable();


}

//Protected------------------

void ARTSPlayerEye::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//m_pViewportClient = GetGameInstance()->GetGameViewportClient();
	//check(m_pViewportClient);
	//only on begin play


}

void ARTSPlayerEye::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	m_pCameraSpringArm->TargetArmLength = FMath::FInterpTo(m_pCameraSpringArm->TargetArmLength, m_aZoomNodes[m_ZoomIndex].m_Distance, DeltaTime, 5);

	m_CameraState.Update();
	m_PlacementState.Update();

	//UpdateCursorRoot();

}

void ARTSPlayerEye::BeginPlay()
{
	Super::BeginPlay();

	//
	auto *pGM{ Cast<ACanyonGM>(GetWorld()->GetAuthGameMode()) };

	auto *pWidget{ CreateWidget<UMainHudWidgetBase>(GetWorld(), m_MainHudClass.Get()) };
	pWidget->AddToViewport();

	m_pDeckState = UDeckState::Construct(pGM);
	m_pDeckStateRenderer = UDeckStateRenderer::Construct(pGM, m_pDeckState, pWidget);

	NotifyOnDisplayNewDecks();
	
}


#pragma region Input
void ARTSPlayerEye::SetupPlayerInputComponent(UInputComponent *pInputComponent)
{
	//Super::SetupPlayerInputComponent(pInputComponent);

	pInputComponent->BindAxis(s_AxisMouseX);
	pInputComponent->BindAxis(s_AxisMouseY);

	pInputComponent->BindAxis(TEXT("MoveRight"), this, &ARTSPlayerEye::AddRightMovement);
	pInputComponent->BindAxis(TEXT("MoveForward"), this, &ARTSPlayerEye::AddForwardMovement);

	pInputComponent->BindAxis(TEXT("RotateCamera"), this, &ARTSPlayerEye::AddCameraYaw);

	pInputComponent->BindAction(TEXT("Select"), IE_Pressed, this, &ARTSPlayerEye::ActionSelectStart);
	pInputComponent->BindAction(TEXT("Select"), IE_Released, this, &ARTSPlayerEye::ActionSelectEnd);

	pInputComponent->BindAction(TEXT("ContextAction"), IE_Pressed, this, &ARTSPlayerEye::ActionContextStart);
	pInputComponent->BindAction(TEXT("ContextAction"), IE_Released, this, &ARTSPlayerEye::ActionContextEnd);

	pInputComponent->BindAction(TEXT("SeamlessRotate"), IE_Pressed, this, &ARTSPlayerEye::EnterSeamlessRotation);
	pInputComponent->BindAction(TEXT("SeamlessRotate"), IE_Released, this, &ARTSPlayerEye::LeaveSeamlessRotation);
	
	pInputComponent->BindAction(TEXT("ZoomOut"), IE_Pressed, this, &ARTSPlayerEye::ZoomOut);
	pInputComponent->BindAction(TEXT("ZoomIn"), IE_Pressed, this, &ARTSPlayerEye::ZoomIn);

	pInputComponent->BindAction(TEXT("IncreaseBuildingRot"), IE_Pressed, this, &ARTSPlayerEye::IncreaseBuildingRot);
	pInputComponent->BindAction(TEXT("DecreaseBuildingRot"), IE_Pressed, this, &ARTSPlayerEye::DecreaseBuildingRot);


}

void ARTSPlayerEye::ActionSelectStart()
{
	m_CameraState.HandleInput(EAbstractInputEvent::ActionSelect_Start);
	m_PlacementState.HandleInput(EAbstractInputEvent::ActionSelect_Start);

}

void ARTSPlayerEye::ActionSelectEnd()
{
	m_CameraState.HandleInput(EAbstractInputEvent::ActionSelect_End);
	m_PlacementState.HandleInput(EAbstractInputEvent::ActionSelect_End);

}

void ARTSPlayerEye::ActionContextStart()
{
	m_CameraState.HandleInput(EAbstractInputEvent::ActionContext_Start);
	m_PlacementState.HandleInput(EAbstractInputEvent::ActionContext_Start);


}

void ARTSPlayerEye::ActionContextEnd()
{
	m_CameraState.HandleInput(EAbstractInputEvent::ActionContext_End);
	m_PlacementState.HandleInput(EAbstractInputEvent::ActionContext_End);


}

void ARTSPlayerEye::EnterSeamlessRotation()
{
	m_CameraState.HandleInput(EAbstractInputEvent::ActionRotate_Start);


}

void ARTSPlayerEye::LeaveSeamlessRotation()
{
	m_CameraState.HandleInput(EAbstractInputEvent::ActionRotate_End);


}

void ARTSPlayerEye::IncreaseBuildingRot()
{
	if(m_pPlaceablePreviewCurrent)
	{
		const auto RotStep{ 360.f / m_BuildingRotationSteps };
	   
		m_pCursorRoot->AddWorldRotation( {0, RotStep, 0} );
	}
		

}

void ARTSPlayerEye::DecreaseBuildingRot()
{
	if(m_pPlaceablePreviewCurrent)
	{
		const auto RotStep{ 360.f / m_BuildingRotationSteps };

		m_pCursorRoot->AddWorldRotation( {0, -RotStep, 0} );
	}


}
#pragma endregion

