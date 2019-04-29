#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "SDockTab.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCanyonEditor, All, Log);

class FCanyonEditorModule : public IModuleInterface
{
public:
	
	virtual void StartupModule() override;

	virtual void ShutdownModule() override;


private:
	TSharedRef<SDockTab> HandleSpawnGlobalDataTab(const class FSpawnTabArgs &SpawnTabArgs);
	
	void PopupGlobalDataTab();
	
	void AddMenuBarButton(class FMenuBarBuilder &Builder);

	void HandleTabClose(TSharedRef<SDockTab> pDockTab);


	static const FName s_TabNameGlobalData;

	TSharedPtr<class FUICommandList> m_pEditorCommands;

	TWeakPtr<SDockTab> m_pGlobalDataEditor;
	
};
