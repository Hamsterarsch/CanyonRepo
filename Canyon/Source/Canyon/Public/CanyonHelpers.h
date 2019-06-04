#pragma once

#include "CoreMinimal.h"
#include "SoftObjectPtr.h"


template<class T>
UClass *SafeLoadClassPtr(const TSoftClassPtr<T> &ClassPtr)
{
	UClass *pClass = nullptr;
	if ((pClass = ClassPtr.Get()) == nullptr)
	{
		pClass = ClassPtr.LoadSynchronous();

	}

	return ensureMsgf(pClass, TEXT("Could not load class ptr from soft obj ptr")) ? pClass : nullptr;


}

template<class T>
T *SafeLoadObjectPtr(const TSoftObjectPtr<T> &ObjectPtr)
{
	UObject *pObject = nullptr;
	if ((pObject = ObjectPtr.Get()) == nullptr)
	{
		pObject = ObjectPtr.LoadSynchronous();

	}

	return ensureMsgf(pObject, TEXT("Could not load class ptr from soft obj ptr")) ? Cast<T>(pObject) : nullptr;


}