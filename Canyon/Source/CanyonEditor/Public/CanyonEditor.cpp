#include "CanyonEditor.h"
#include "MultiBoxBuilder.h"
#include "CanyonEditorCommands.h"
#include "LevelEditor.h"
#include "AssetEditorToolkit.h"
#include "Widgets/SGlobalDataEditor.h"
#include "TabManager.h"
#include "FileHelpers.h"

IMPLEMENT_MODULE(FCanyonEditorModule, CanyonEditor);

DEFINE_LOG_CATEGORY(LogCanyonEditor);

const FName FCanyonEditorModule::s_TabNameGlobalData{ TEXT("CanyonEditorGlobalDataTab") };

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
		FCanyonEditorCommands::Get().TestCommand,
		FExecuteAction::CreateRaw(this, &FCanyonEditorModule::PopupGlobalDataTab)
	);

	FGlobalTabmanager::Get()->RegisterTabSpawner
	(
		s_TabNameGlobalData,
		FOnSpawnTab::CreateRaw(this, &FCanyonEditorModule::HandleSpawnGlobalDataTab)
	)
	.SetDisplayName(LOCTEXT("GlobalDataEditorTabTitle", "Global Data"))
	.SetTooltipText(LOCTEXT("GlobalDataEditorTooltipText", "Open the global data tab."));
		
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
	
	
}

void FCanyonEditorModule::ShutdownModule()
{
	FCanyonEditorCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterTabSpawner(s_TabNameGlobalData);

	UE_LOG(LogCanyonEditor, Warning, TEXT("CanyonEditor Shutdown"));
		
	
}


TSharedRef<SDockTab> FCanyonEditorModule::HandleSpawnGlobalDataTab(const FSpawnTabArgs& SpawnTabArgs)
{
	const TSharedRef<SDockTab> pMajorTab = SNew(SDockTab).TabRole(ETabRole::MajorTab);
	pMajorTab->SetContent(SNew(SGlobalDataEditor));
	m_pGlobalDataEditor = pMajorTab;

	pMajorTab->SetOnTabClosed(SDockTab::FOnTabClosedCallback::CreateRaw(this, &FCanyonEditorModule::HandleTabClose));

	return pMajorTab;


}

void FCanyonEditorModule::PopupGlobalDataTab()
{	
	UE_LOG(LogTemp, Warning, TEXT("Canyon Editor Command"));
	
	FGlobalTabmanager::Get()->InvokeTab(s_TabNameGlobalData);


}

void FCanyonEditorModule::AddMenuBarButton(FMenuBarBuilder& Builder)
{
	Builder.AddMenuEntry(FCanyonEditorCommands::Get().TestCommand);

	
}

void FCanyonEditorModule::HandleTabClose(TSharedRef<SDockTab> pDockTab)
{
	UE_LOG(LogCanyonEditor, Warning, TEXT("Canyon Editor: Closing gloabal data tab."));

	auto pChild{ pDockTab->GetContent() };
	
	if (pChild->GetType() == TEXT("SGlobalDataEditor"))
	{
		auto &GdEditor{ static_cast<SGlobalDataEditor &>(pChild.Get()) };

		auto *pDco{ GdEditor.GetTargetDco() };
		if(!pDco)
		{
			return;
		}

		pDco->Modify(true);

		auto *pPackage{ pDco->GetOutermost() };
		UEditorLoadingAndSavingUtils::SavePackages({ pPackage }, false);
	}
	

}

#undef LOCTEXT_NAMESPACE
