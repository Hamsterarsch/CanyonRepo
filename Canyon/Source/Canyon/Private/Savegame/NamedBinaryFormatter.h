#pragma once
#include "BufferArchive.h"


/*	EXAMPLE USAGE 
 
	FBufferArchive Writer{ true, TEXT("Savegame") };
	
	//serializing uobjects is not supported
	
	int32 Version{ 11 };
	TArray<FVector> aPositions{ {1,0,0}, {0,1,0}, {0,0,1} };
	TMap<FString, int32> Map;
	Map.Add("FirstEntry", 1);
	Map.Add("SecondEntry", 2);
	Map.Add("ThirdEntry", 3);
	
	{
		Canyon::CNamedBinaryFormatter WriterFormatter{ Writer };
	
		WriterFormatter.AddField("Version", Version);
		
		WriterFormatter.EnterScope("LevelScope");
	
		WriterFormatter.AddField("aPositions", aPositions);
	
		WriterFormatter.EnterScope("InnerScope");
	
		WriterFormatter.AddField("Map", Map);
	
		WriterFormatter.LeaveScope();
	
		//close all scopes per standard
		//WriterFormatter.LeaveScope();
	}
	
	
	FMemoryReader Reader{ Writer, true };
	Canyon::CNamedBinaryFormatter ReaderFormatter{ Reader };
	
	
	auto VersionRead{ ReaderFormatter.ReadField<int32>("Version") };
	
	auto aPositionsRead{ ReaderFormatter.ReadField<TArray<FVector>>("LevelScope.aPositions") };
	
	auto MapRead{ ReaderFormatter.ReadField<TMap<FString, int32>>("LevelScope.InnerScope.Map") };

*/

namespace Canyon
{
	enum class ESlotType : uint8
	{
		Scope,
		Field,

	};

	struct SScope;
	struct SField;

	class CNamedBinaryFormatter
	{
	public:
		using t_ElementSizeType = uint64;


		CNamedBinaryFormatter(FArchive &TargetArchive);

		~CNamedBinaryFormatter();

		template<class T>
		void AddField(FString Name, const T &Value);

		//Fields nested in scopes are accessed by using a point,
		//eg. a field "DataEntry" inside the scope "InnerScope"
		//must be read using the token "InnerScope.DataEntry"
		template<class T>
		T ReadField(FString Name);

		void EnterScope(FString Name);

		void LeaveScope();

		void ToGlobalScope();

		
	private:
		const SField &ResolveFieldIdentifier(const FString &Token, const SScope &OuterScope) const;


		FArchive &m_TargetArchive;

		TArray<SScope> m_aScopeStack;

		
	};

	struct SField
	{
	public:
			SField() = default;

			SField(FString &&FieldName, uint64 FieldDataStartOffset = 0, uint64 FieldSize = 0);

			FArchive &Serialize(FArchive &Ar);
							

			FString Name;

			CNamedBinaryFormatter::t_ElementSizeType	Size,
														DataStartOffset;

			static SField s_Empty;

	};

	struct SScope
	{
	public:
		SScope() = default;

		SScope(FString &&ScopeName, uint64 ScopeDataStartOffset = 0);

		FArchive &Serialize(FArchive &Ar);


		FString	Name;

		CNamedBinaryFormatter::t_ElementSizeType DataStartOffset;

		TArray<SScope> aScopes;
		TArray<SField> aFields;

		
	};

	template <class T>  
	void CNamedBinaryFormatter::AddField(FString Name, const T &Value)
	{
		checkf(!m_TargetArchive.IsLoading(), TEXT("Trying to add a field with a loading archive as target, but only writing archives can add fields"));
		
		auto &CurrentScope{ m_aScopeStack.Top() };
		const auto FieldIndex{ CurrentScope.aFields.Emplace(std::move(Name), m_TargetArchive.Tell()) };

		//todo: are all loading archives respecting the "constness" of the object ?
		m_TargetArchive << const_cast<T &>(Value);
		CurrentScope.aFields[FieldIndex].Size = m_TargetArchive.Tell();


	}

	template <class T>
	T CNamedBinaryFormatter::ReadField(FString Name)
	{
		const auto &Field{ ResolveFieldIdentifier(Name, m_aScopeStack.Top()) };
	
		checkf(Field.Size != 0, TEXT("Trying to read a zero field. Either no data was written or the identifier does not exist"));
		checkf(m_TargetArchive.IsLoading(), TEXT("Fields can only be read from loading archives"));

		T Value;
		m_TargetArchive.Seek(Field.DataStartOffset);
		m_TargetArchive << Value;

		return Value;
		

	}


}

FArchive &operator<<(FArchive &Ar, Canyon::SScope &Scope);

FArchive &operator<<(FArchive &Ar, Canyon::SField &Field);
