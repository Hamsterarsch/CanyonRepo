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
	ActionRotate_Inc,
	ActionRotate_Dec
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

	void AddCarryOverChargesToDeck(const struct FCarryOverCharges &CarryCharges);

	void NotifyBuildingSelectionChanged(int32 CountSelectedCurrent, int32 CountSelectedMax);;

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

	void IncreaseBuildingRot();

	void DecreaseBuildingRot();

	//End Movement

	void SetPreviewCursorPosWs(const FVector &NewPos);

	void UpdateCurrentPlaceablePreview();

	bool TryCommitPlaceablePreview();

	void DiscardCurrentPlaceablePreview(bool bIsPlayerInstigated = false);

	inline bool GetIsInPlacement() const { return m_PlacementState.GetIsInPlacement(); }

	int32 GetCurrentChargesForPlaceables() const;

	void NotifyOnNewDeckAvailable();

	UFUNCTION(BlueprintCallable)
		bool GetAreDecksSelectable() const;

	inline float GetPlacementAbortSuccessTime() const { return m_PlacementAbortSuccessTime; }

	void OnPointsRequiredChanged(int32 NewPoints);

	void OnPointsCurrentChanged(int32 NewPoints);

	void OnNextLevelAccessible();

	void SwitchToPlaceableSelectionMode();

	void SwitchToPlaceablePlacementMode();

	int32 GetDeckChargesCurrent() const;


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
	
	void OnExitCurrentMenu();

	void InputRotateBuildingIncrease();

	void InputRotateBuildingDecrease();

	void EnablePlacementZoomOverride();

	void DisablePlacementZoomOverride();
	//End Input

	UFUNCTION(BlueprintCallable)
		float GetMinZoomDist() const;

	UFUNCTION(BlueprintCallable)
		float GetMaxZoomDist() const;

	UFUNCTION(BlueprintCallable)
		float GetZoomDist() const;

	UFUNCTION(BlueprintImplementableEvent)
		void OnZoomChanged();

	UFUNCTION(BlueprintImplementableEvent)
		void OnBeginPreviewBuilding();

	UFUNCTION(BlueprintImplementableEvent)
		void OnCommitPreviewBuilding(int32 PointDelta);

	UFUNCTION(BlueprintImplementableEvent)
		void OnAbortPreviewBuilding();

	UFUNCTION(BlueprintImplementableEvent)
		void OnUnitTransferEnabled();
	
	UPROPERTY(EditAnywhere)
		class USpringArmComponent *m_pCameraSpringArm;

	UPROPERTY(EditAnywhere)
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
		class UMainHudWidgetBase *m_pMainHudWidget;
	

	bool m_bIsPlaceablePlaceable;
	FVector2D m_MouseShufflePreMousePos;
	float m_ZoomTargetDist;
	float m_ZoomTargetPitch;
	int32 m_ZoomIndex;
	FVector m_SeamlessRotationPrePos;
	float m_MovementSpeedMultCurrent;
	bool m_bForceZoomDuringPlacement;

	CCameraStateMachine m_CameraState;
	CPlacementStateMachine m_PlacementState;
	CPlacementRuler m_PlacementRuler;

private:
	FHitResult TraceForPlaceable();


};


