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
	FZoomNode(float Distance = 300) :
	m_Distance(Distance)
	{
	}
		
	UPROPERTY(EditAnywhere, DisplayName="Distance")
		float m_Distance;

	UPROPERTY(EditAnywhere, DisplayName="Movement Speed Multiplier Delta")
		float m_MovementSpeedMultDelta;


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

	void BeginGame();

	UFUNCTION(BlueprintCallable)
		void CreateNewPlacablePreview(TSubclassOf<class APlaceableBase> NewPlaceableClass);

	void DebugAddChargesForCategory(const FString &Category, int32 Num);

	//Movement
	void AddForwardMovement(float AxisValue);

	void AddRightMovement(float AxisValue);

	void AddForwardMovementFromMouse(float AxisValue);

	void AddRightMovementFromMouse(float AxisValue);

	void AddCameraYaw(float AxisValue);

	void AddCameraYawFromMouse(float AxisValue);

	void AddCameraPitchFromMouse(float AxisValue);

	void ZoomOut();

	void ZoomIn();
	//End Movement

	void SetPreviewCursorPosWs(const FVector &NewPos);

	void UpdateCurrentPlaceablePreview();

	bool TryCommitPlaceablePreview();

	void DiscardCurrentPlaceablePreview(bool bIsIntigatedByPlayer = false);

	inline bool GetIsInPlacement() const { return m_PlacementState.GetIsInPlacement(); }

	int32 GetCurrentChargesForPlaceables() const;

	void NotifyOnDisplayNewDecks();

	bool GetAreDecksSelectable() const;

	inline float GetPlacementAbortSuccessTime() const { return m_PlacementAbortSuccessTime; }

	void OnLoose();

	void OnPointsRequiredChanged(int32 NewPoints);

	void OnPointsCurrentChanged(int32 NewPoints);

	void OnNextLevelAccessible();


	const static FName s_AxisMouseX;

	const static FName s_AxisMouseY;

	constexpr static ECollisionChannel s_CollisionLayerPlaceable{ ECC_GameTraceChannel1 };

	constexpr static ECollisionChannel s_CollisionLayerNonPlaceable{ ECC_GameTraceChannel2 };


protected:
	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	//Input
	virtual void SetupPlayerInputComponent(UInputComponent *pInputComponent) override;

	void ActionSelectStart();

	void ActionSelectEnd();

	void ActionContextStart();

	void ActionContextEnd();

	void EnterSeamlessRotation();

	void LeaveSeamlessRotation();

	void IncreaseBuildingRot();

	void DecreaseBuildingRot();
	//End Input
	
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

	UPROPERTY(EditDefaultsOnly, Category="Canyon|UI")
		TSubclassOf<class UPrettyWidget> m_LooseWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="Canyon|UI")
		TSubclassOf<class UInfluenceDisplayWidgetBase> m_PreviewInfluenceDisplayWidget;

	UPROPERTY(EditDefaultsOnly)
		//the maximum hold time (in seconds) of the placement abort button
		//that incurs an abort of the placement
		float m_PlacementAbortSuccessTime;

	UPROPERTY(EditDefaultsOnly, DisplayName = "Building Rotation Steps", Category="Controls")
		int32 m_BuildingRotationSteps;

	UPROPERTY(EditDefaultsOnly, Category="Controls")
		TArray<FZoomNode> m_aZoomNodes;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class UMainHudWidgetBase> m_MainHudClass;

	UPROPERTY()
		class UGameViewportClient *m_pViewportClient;

	UPROPERTY()
		USceneComponent *m_pCursorRoot;

	UPROPERTY()
		class APlaceablePreview *m_pPlaceablePreviewCurrent;

	UPROPERTY()
		class UDeckState *m_pDeckState;

	UPROPERTY()
		class UDeckStateRenderer *m_pDeckStateRenderer;

	UPROPERTY()
		class UPrettyWidget *m_pLooseWidget;

	UPROPERTY()
		class UMainHudWidgetBase *m_pMainHudWidget;
	

	bool m_bIsPlaceablePlaceable;
	FVector2D m_MouseShufflePreMousePos;
	float m_ZoomTargetDist;
	float m_ZoomTargetPitch;
	int32 m_ZoomIndex;
	FVector m_SeamlessRotationPrePos;
	float m_MovementSpeedMultCurrent;

	CCameraStateMachine m_CameraState;
	CPlacementStateMachine m_PlacementState;
	CPlacementRuler m_PlacementRuler;



};
