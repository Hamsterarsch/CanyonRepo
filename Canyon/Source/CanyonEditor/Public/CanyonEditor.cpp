#include "CanyonEditor.h"

IMPLEMENT_GAME_MODULE(FCanyonEditorModule, CanyonEditor);

DEFINE_LOG_CATEGORY(LogCanyonEditor);

#define LOCTEXT_NAMESPACE "CanyonEditor"

void FCanyonEditorModule::StartupModule()
{
	UE_LOG(LogCanyonEditor, Log, TEXT("CanyonEditor Startup"));
	
	
}

void FCanyonEditorModule::ShutdownModule()
{
	UE_LOG(LogCanyonEditor, Log, TEXT("CanyonEditor Shutdown"));
		
	
}

#undef LOCTEXT_NAMESPACE