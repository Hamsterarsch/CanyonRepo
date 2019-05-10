#include "CanyonEditor.h"
#include "MultiBoxBuilder.h"
#include "CanyonEditorCommands.h"
#include "LevelEditor.h"
#include "AssetEditorToolkit.h"
#include "Widgets/SInfluenceDataEditor.h"
#include "TabManager.h"
#include "FileHelpers.h"
#include "PropertyEditorModule.h"
#include "DetailsCustom/InfluenceDataObjectDetailsCustom.h"

IMPLEMENT_MODULE(FCanyonEditorModule, CanyonEditor);

DEFINE_LOG_CATEGORY(LogCanyonEditor);

const FName FCanyonEditorModule::s_TabNameGlobalData{ TEXT("CanyonInfluenceEditorTab") };

#define LOCTEXT_NAMESPACE "CanyonEditor"


void FCanyonEditorModule::StartupModule()
{
	UE_LOG(LogCanyonEditor, Warning, TEXT("CanyonEditor Startup"));

	m_pEditorCommands = MakeShared<FUICommandList>();
	if (!m_pEditorCommands)
	{
		return;
	}

	FCanyonEditorCommands::Register();

	m_pEditorCommands->MapAction
	(
		FCanyonEditorCommands::Get().pCmdOpenInfluenceEditor,
		FExecuteAction::CreateRaw(this, &FCanyonEditorModule::PopupGlobalDataTab)
	);

	FGlobalTabmanager::Get()->RegisterTabSpawner
	                        (
		                        s_TabNameGlobalData,
		                        FOnSpawnTab::CreateRaw(this, &FCanyonEditorModule::HandleSpawnGlobalDataTab)
	                        )
	                        .SetDisplayName(LOCTEXT("InfluenceEditorTabTitle", "Influence Editor"))
	                        .SetTooltipText(LOCTEXT("InfluenceEditorTooltipText", "Open the influence editor tab."));
		
	FGlobalTabmanager::RegisterDefaultTabWindowSize(s_TabNameGlobalData, { 1000, 750 });
	
		
	{
		FLevelEditorModule &LevelEditorModule{ FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor") };
				
		auto NewMenuExtender{ MakeShared<FExtender>() };
			   
		NewMenuExtender->AddMenuBarExtension
		(
			"Window",
			EExtensionHook::Before,
			m_pEditorCommands,
			FMenuBarExtensionDelegate::CreateRaw(this, &FCanyonEditorModule::AddMenuBarButton)
		);
		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(NewMenuExtender);
	}


	//Customs registry
	{
		auto &PropertyModule{ FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor") };

		PropertyModule.RegisterCustomClassLayout
		(
			TEXT("InfluenceDataObject"),
			FOnGetDetailCustomizationInstance::CreateStatic
			(				
				&FInfluenceDataObjectDetailsCustom::MakeInstance
			)
		);

		PropertyModule.NotifyCustomizationModuleChanged();
	}

	
}

void FCanyonEditorModule::ShutdownModule()
{
	FCanyonEditorCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterTabSpawner(s_TabNameGlobalData);

	{
		auto &PropertyModule{ FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor") };

		PropertyModule.UnregisterCustomClassLayout(TEXT("InfluenceDataObject"));

	}

	UE_LOG(LogCanyonEditor, Warning, TEXT("CanyonEditor Shutdown"));
		
	
}


TSharedRef<SDockTab> FCanyonEditorModule::HandleSpawnGlobalDataTab(const FSpawnTabArgs& SpawnTabArgs)
{
	const TSharedRef<SDockTab> TabRef{ SNew(SDockTab).TabRole(ETabRole::MajorTab) };
	
	auto InfluenceEditorRef{ SNew(SInfluenceDataEditor) };
	TabRef->SetContent(InfluenceEditorRef);
	
	//Widget refs
	m_pInfluenceEditorWidget = InfluenceEditorRef;
	m_pInfluenceEditorTab = TabRef;

	TabRef->SetOnTabClosed(SDockTab::FOnTabClosedCallback::CreateRaw(this, &FCanyonEditorModule::HandleTabClose));

	return TabRef;


}

void FCanyonEditorModule::PopupGlobalDataTab()
{	
	UE_LOG(LogTemp, Warning, TEXT("Canyon Editor Command"));
	
	FGlobalTabmanager::Get()->InvokeTab(s_TabNameGlobalData);

	if(auto DataEditor{ m_pInfluenceEditorWidget.Pin() })
	{
		auto *pWidget{ DataEditor.Get() };
		pWidget->RefreshObjectData();
	}


}

void FCanyonEditorModule::AddMenuBarButton(FMenuBarBuilder& Builder)
{
	Builder.AddMenuEntry(FCanyonEditorCommands::Get().pCmdOpenInfluenceEditor);

	
}

void FCanyonEditorModule::HandleTabClose(TSharedRef<SDockTab> pDockTab)
{
	UE_LOG(LogCanyonEditor, Warning, TEXT("Canyon Editor: Closing gloabal data tab."));
	

}

#undef LOCTEXT_NAMESPACE
