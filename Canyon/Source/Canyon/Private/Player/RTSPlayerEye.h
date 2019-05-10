// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "CameraState.h"
#include "PlacementState.h"
#include "Placeables/PlacementRuler.h"
#include "RTSPlayerEye.generated.h"

enum class EAbstractInputEvent : size_t
{
	ActionSelect_Start,
	ActionSelect_End,
	ActionContext_Start,
	ActionContext_End,
	ActionRotate_Start,
	ActionRotate_End,
	PlaceBuilding_Start,
	PlaceInfra_Start,
	PlayeInfra_End


};

enum class ERTSInputState
{
	Idle,
	Placement,
	CameraMouseTurn,
	CameraMouseShuffle,
	PlacementCameraMouseTurn,
	Menu


};

USTRUCT(BlueprintType)
struct FZoomNode
{
	GENERATED_BODY()

public:	
	FZoomNode(float Distance = 300, float Pitch = -30) 
	{
		m_Distance = Distance;
		m_PitchMax =  Pitch;
	};
		
	UPROPERTY(EditAnywhere, DisplayName="Distance")
		float m_Distance;

	UPROPERTY(EditAnywhere, DisplayName="Max Pitch")
		float m_PitchMax;


};

/**
 * 
 */
UCLASS(CustomConstructor)
class CANYON_API ARTSPlayerEye : public ASpectatorPawn
{
	GENERATED_BODY()

public:
	ARTSPlayerEye();

	UFUNCTION(BlueprintCallable)
		void CreateNewPlacablePreview(TSubclassOf<class APlaceableBase> NewPlaceableClass);

	void AddForwardMovement(float AxisValue);

	void AddRightMovement(float AxisValue);

	void AddForwardMovementFromMouse(float AxisValue);

	void AddRightMovementFromMouse(float AxisValue);

	void AddCameraYaw(float AxisValue);

	void AddCameraYawFromMouse(float AxisValue);

	void AddCameraPitchFromMouse(float AxisValue);

	void ZoomOut();

	void ZoomIn();


	void SetPreviewCursorPosWs(const FVector &NewPos);

	void UpdateCurrentPlaceablePreview();

	bool TryCommitPlaceablePreview();

	void DiscardCurrentPlaceablePreview();


	const static FName s_AxisMouseX;

	const static FName s_AxisMouseY;

	constexpr static ECollisionChannel s_CollisionLayerPlaceable{ ECC_GameTraceChannel1 };

	constexpr static ECollisionChannel s_CollisionLayerNonPlaceable{ ECC_GameTraceChannel2 };


protected:
	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	bool GetClosestPlaceablePositionForCurrentPlaceable(FVector &OutPosition, FHitResult *pOutHit = nullptr);

	bool TraceForTerrainUnderCursor(FHitResult &OutHit);


	virtual void SetupPlayerInputComponent(UInputComponent *pInputComponent) override;

	void ActionSelectStart();

	void ActionSelectEnd();

	void ActionContextStart();

	void ActionContextEnd();

	void EnterSeamlessRotation();

	void LeaveSeamlessRotation();

	
	UPROPERTY(VisibleAnywhere)
		class USpringArmComponent *m_pCameraSpringArm;

	UPROPERTY(VisibleAnywhere)
		class UCameraComponent *m_pCamera;
	
	UPROPERTY(EditDefaultsOnly, DisplayName="Mouse Shuffle Speed", Category="Controls")
		float m_MouseShuffleSpeed;

	UPROPERTY(EditDefaultsOnly, DisplayName = "Key Shuffle Speed", Category="Controls")
		float m_KeyShuffleSpeed;

	UPROPERTY(EditDefaultsOnly, DisplayName = "Mouse Turn Speed", Category="Controls")
		float m_MouseTurnSpeed;

	UPROPERTY(EditDefaultsOnly, DisplayName = "Key Turn Speed", Category="Controls")
		float m_KeyTurnSpeed;

	UPROPERTY(EditDefaultsOnly, DisplayName = "Max Camera Pitch", Category="Controls")
		float m_CameraMaxPitch;

	UPROPERTY(EditDefaultsOnly, DisplayName = "Min Camera Pitch", Category="Controls")
		float m_CameraMinPitch;
	
	UPROPERTY(EditDefaultsOnly, Category="Controls")
		TArray<FZoomNode> m_aZoomNodes;

	UPROPERTY()
		class UGameViewportClient *m_pViewportClient;

	UPROPERTY()
		USceneComponent *m_pCursorRoot;

	UPROPERTY()
		class APlaceablePreview *m_pPlaceablePreviewCurrent;

	bool m_bWasPlaceablePlaceable;
	FVector2D m_MouseShufflePreMousePos;
	float m_ZoomTargetDist;
	float m_ZoomTargetPitch;
	int32 m_ZoomIndex;
	FVector m_SeamlessRotationPrePos;
	FVector m_CursorRootLastPlaceablePos;

	CCameraStateMachine m_CameraState;
	CPlacementStateMachine m_PlacementState;
	CPlacementRuler m_PlacementRuler;



};
