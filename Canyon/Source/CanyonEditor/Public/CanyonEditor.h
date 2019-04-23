#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
//#include "UnrealEd.h"


DECLARE_LOG_CATEGORY_EXTERN(LogCanyonEditor, All, Log);


class FCanyonEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;

	virtual void ShutdownModule() override;
	
	
};
