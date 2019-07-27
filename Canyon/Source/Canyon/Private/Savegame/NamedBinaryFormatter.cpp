#include "Savegame/NamedBinaryFormatter.h"

namespace Canyon
{
	CNamedBinaryFormatter::CNamedBinaryFormatter(FArchive& TargetArchive) :
		m_TargetArchive{ TargetArchive }
	{
		if(m_TargetArchive.IsLoading())
		{
			//load the scope index
			const auto ArchiveSize{ m_TargetArchive.TotalSize() };
			m_TargetArchive.Seek(ArchiveSize - sizeof(int64));

			int64 PosScopeData;
			m_TargetArchive << PosScopeData;

			m_TargetArchive.Seek(PosScopeData);
			m_TargetArchive << m_aScopeStack;

			return;
		}

		EnterScope(TEXT("Root"));

		
	}

	CNamedBinaryFormatter::~CNamedBinaryFormatter()
	{
		if(!m_TargetArchive.IsLoading())
		{
			ToGlobalScope();

			//write out scope index and start position
			auto PosScopeData{ m_TargetArchive.Tell() };
			m_TargetArchive << m_aScopeStack;
						
			m_TargetArchive <<  PosScopeData;



		}


	}

	void CNamedBinaryFormatter::EnterScope(FString Name)
	{
		SScope NewScope{ std::move(Name), static_cast<uint64>(m_TargetArchive.Tell()) };

		m_aScopeStack.Push(std::move(NewScope));
			   		 	  	  	 

	}

	void CNamedBinaryFormatter::LeaveScope()
	{
		//don't pop the root scope
		if(m_aScopeStack.Num() == 1)
		{
			return;


		}

		//pop the current scope and add it to its outer
		auto ScopeToLeave{ m_aScopeStack.Pop() };
		m_aScopeStack.Top().aScopes.Emplace(std::move(ScopeToLeave));


	}

	void CNamedBinaryFormatter::ToGlobalScope()
	{
		while(m_aScopeStack.Num() > 1)
		{
			LeaveScope();
				
		}


	}

	const SField &CNamedBinaryFormatter::ResolveFieldIdentifier(const FString &Token, const SScope &OuterScope) const
	{
		FString Left{}, Right{};
		
		if(Token.Split(".", &Left, &Right))
		{	
			auto *pScope
			{
				OuterScope.aScopes.FindByPredicate([&SearchedName = Left](const SScope &Elem)
				{
					return Elem.Name == SearchedName;


				})
			};

			if(!pScope)
			{
				return SField::s_Empty;
			}
			
			return ResolveFieldIdentifier(Right, *pScope);


		}

		for(auto &&Field : OuterScope.aFields)
		{
			if(Field.Name == Token)
			{
				return Field;
			}

		}

		return SField::s_Empty;


	}


//Field
	SField SField::s_Empty{ "", 0, 0 };

	SField::SField(FString&& FieldName, uint64 FieldDataStartOffset, uint64 FieldSize) :
		Name{ std::move(FieldName) },
		Size{ FieldSize },
		DataStartOffset{ FieldDataStartOffset }
	{
	}

	FArchive& SField::Serialize(FArchive& Ar)
	{
		Ar << Name;
		Ar << Size;
		Ar << DataStartOffset;

		return Ar;
		

	}

	SScope::SScope(FString &&ScopeName, uint64 ScopeDataStartOffset) :
		Name{ std::move(ScopeName) },
		DataStartOffset{ ScopeDataStartOffset }
	{
	}

	FArchive& SScope::Serialize(FArchive& Ar)
	{
		Ar << Name;
		Ar << DataStartOffset;
		
		Ar << aScopes;
		Ar << aFields;

		return Ar;


	}


}

FArchive& operator<<(FArchive& Ar, Canyon::SScope& Scope)
{
	return Scope.Serialize(Ar);


}

FArchive& operator<<(FArchive& Ar, Canyon::SField& Field)
{
	return Field.Serialize(Ar);


}
