// Copyright 2022 (c) Microsoft. All rights reserved.

#include "VisualStudioToolsCommandlet.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "JsonObjectConverter.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "VisualStudioTools.h"

namespace VS {

static const FName NAME_Category = TEXT("Category");

static TArray<FProperty*> GetChangedPropertiesList(
	UStruct* InStruct, const uint8* DataPtr, const uint8* DefaultDataPtr)
{
	TArray<FProperty*> Result;

	const UClass* OwnerClass = Cast<UClass>(InStruct);

	// Walk only in the properties defined in the current class, the super classes are processed individually
	for (TFieldIterator<FProperty> It(OwnerClass, EFieldIteratorFlags::ExcludeSuper); It; ++It)
	{
		FProperty* Property = *It;
		for (int32 Idx = 0; Idx < Property->ArrayDim; Idx++)
		{
			const uint8* PropertyValue = Property->ContainerPtrToValuePtr<uint8>(DataPtr, Idx);
			const uint8* DefaultPropertyValue = Property->ContainerPtrToValuePtrForDefaults<uint8>(InStruct, DefaultDataPtr, Idx);

			if (!Property->Identical(PropertyValue, DefaultPropertyValue))
			{
				Result.Add(Property);
				break;
			}
		}
	}

	return Result;
}

static TArray<UClass*> FindBlueprintNativeParents(const UBlueprint* Blueprint)
{
	TArray<UClass*> Result;

	for (UClass* Super = Blueprint->GeneratedClass->GetSuperClass(); Super; Super = Super->GetSuperClass())
	{
		// Ignore the root `UObject` class and non-native parents.
		if (Super->HasAnyClassFlags(CLASS_Native) && Super->GetFName() != NAME_Object)
		{
			Result.Add(Super);
		}
	}

	return Result;
}

struct PropertyEntry
{
	FProperty* Property;
	TArray<int32> Blueprints;
};

struct FunctionEntry
{
	UFunction* Function;
	TArray<int32> Blueprints;
};

struct ClassEntry
{
	UClass* Class;
	TArray<int32> Blueprints;
	TMap<FString, PropertyEntry> Properties;
	TMap<FString, FunctionEntry> Functions;
};

using ClassMap = TMap<FString, ClassEntry>;

struct AssetIndex
{
	TSet<FString> AssetPathCache;
	ClassMap Classes;
	TArray<const UBlueprint*> Blueprints;

	bool ProcessAsset(UObject* InAsset)
	{
		if (!InAsset)
		{
			return false;
		}

		FString AssetPath = InAsset->GetPathName();

		bool AlreadyAdded = false;
		AssetPathCache.Add(AssetPath, &AlreadyAdded);
		if (AlreadyAdded)
		{
			return false;
		}

		const UBlueprint* Blueprint = Cast<UBlueprint>(InAsset);
		UBlueprintGeneratedClass* BPGC = Blueprint ? Cast<UBlueprintGeneratedClass>(Blueprint->GeneratedClass) : nullptr;

		if (!Blueprint || !BPGC)
		{
			return false;
		}

		int32 BlueprintIndex = Blueprints.Num();

		TArray<UClass*> Parents = FindBlueprintNativeParents(Blueprint);
		UObject* GeneratedClassCDO = BPGC->ClassDefaultObject;

		bool hasAnyParent = Parents.Num() > 0;
		for (UClass* Parent : Parents)
		{
			FString ParentName = Parent->GetFName().ToString();
			if (!Classes.Contains(ParentName))
			{
				Classes.Add(ParentName).Class = Parent;
			}

			ClassEntry& entry = Classes[ParentName];

			entry.Blueprints.Add(BlueprintIndex);

			UObject* SuperClassCDO = Parent->GetDefaultObject(false);
			TArray<FProperty*> ChangedProperties = GetChangedPropertiesList(Parent, (uint8*)GeneratedClassCDO, (uint8*)SuperClassCDO);

			for (FProperty* Property : ChangedProperties)
			{
				FString PropertyName = Property->GetFName().ToString();
				if (!entry.Properties.Contains(PropertyName))
				{
					entry.Properties.Add(PropertyName).Property = Property;
				}

				PropertyEntry& propEntry = entry.Properties[PropertyName];
				propEntry.Blueprints.Add(BlueprintIndex);
			}
		}

		bool hasAnyFunctions = false;
		for (UFunction* Fn : BPGC->CalledFunctions)
		{
			if (!Fn->HasAnyFunctionFlags(EFunctionFlags::FUNC_Native))
			{
				continue;
			}

			hasAnyFunctions = true;

			UClass* Owner = Fn->GetOwnerClass();
			FString OwnerName = Owner->GetFName().ToString();
			if (!Classes.Contains(OwnerName))
			{
				Classes.Add(OwnerName).Class = Owner;
			}

			ClassEntry& entry = Classes[OwnerName];

			FString FnName = Fn->GetFName().ToString();
			if (!entry.Functions.Contains(FnName))
			{
				entry.Functions.Add(FnName).Function = Fn;
			}

			FunctionEntry& funcEntry = entry.Functions[FnName];
			funcEntry.Blueprints.Add(BlueprintIndex);
		}

		if (hasAnyParent || hasAnyFunctions)
		{
			check(Blueprints.Add(Blueprint) == BlueprintIndex);
			return true;
		}

		return false;
	}
};

using JsonWriter = TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>;

static bool ShouldSerializePropertyValue(FProperty* Property)
{
	if (FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
	{
		return true;
	}
	
	if (FNumericProperty* NumericProperty = CastField<FNumericProperty>(Property))
	{
		UEnum* EnumDef = NumericProperty->GetIntPropertyEnum();
		if (EnumDef != NULL)
		{
			return true;
		}

		if (NumericProperty->IsFloatingPoint())
		{
			return true;
		}
		
		if (NumericProperty->IsInteger())
		{
			return true;
		}
	}
	
	if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
	{
		return true;
	}
	
	if (FStrProperty* StringProperty = CastField<FStrProperty>(Property))
	{
		return true;
	}
	
	if (FTextProperty* TextProperty = CastField<FTextProperty>(Property))
	{
		return true;
	}

	return false;
}

static void SerializeBlueprints(TSharedRef<JsonWriter>& Json, TArray<const UBlueprint*> Items)
{
	Json->WriteArrayStart();
	for (const UBlueprint* Blueprint : Items)
	{
		Json->WriteObjectStart();

		Json->WriteValue(TEXT("name"), Blueprint->GetName());
		Json->WriteValue(TEXT("path"), Blueprint->GetPathName());
		Json->WriteObjectEnd();
	}
	Json->WriteArrayEnd();
}

static void SerializeProperties(TSharedRef<JsonWriter>& Json, ClassEntry& Entry, TArray<const UBlueprint*>& Blueprints)
{
	Json->WriteArrayStart();
	for (auto& Item : Entry.Properties)
	{
		auto& PropName = Item.Key;
		auto& PropEntry = Item.Value;
		FProperty* Property = PropEntry.Property;

		Json->WriteObjectStart();

		Json->WriteValue(TEXT("name"), PropName);

		Json->WriteIdentifierPrefix(TEXT("metadata"));
		{
			Json->WriteObjectStart();
			if (Property->HasMetaData(NAME_Category))
			{
				Json->WriteValue(TEXT("categories"), Property->GetMetaData(NAME_Category));
			}
			Json->WriteObjectEnd();
		}

		Json->WriteIdentifierPrefix(TEXT("values"));
		{
			Json->WriteArrayStart();
			for (auto& ClassMap : PropEntry.Blueprints)
			{
				Json->WriteObjectStart();

				Json->WriteValue(TEXT("blueprint"), ClassMap);

				const UBlueprint* Blueprint = Blueprints[ClassMap];
				UObject* GeneratedClassCDO = CastChecked<UBlueprintGeneratedClass>(Blueprint->GeneratedClass)->ClassDefaultObject;

				const uint8* PropData = PropEntry.Property->ContainerPtrToValuePtr<uint8>(GeneratedClassCDO);

				if (ShouldSerializePropertyValue(PropEntry.Property))
				{
					TSharedPtr<FJsonValue> JsonValue = FJsonObjectConverter::UPropertyToJsonValue(Property, PropData);
					FJsonSerializer::Serialize(JsonValue.ToSharedRef(), TEXT("value"), Json);
				}

				Json->WriteObjectEnd();
			}
			Json->WriteArrayEnd();
		}

		Json->WriteObjectEnd();
	}
	Json->WriteArrayEnd();
}

static void SerializeFunctions(TSharedRef<JsonWriter>& Json, ClassEntry& Entry)
{
	Json->WriteArrayStart();
	for (auto& Item : Entry.Functions)
	{
		auto& Name = Item.Key;
		auto& FnEntry = Item.Value;
		Json->WriteObjectStart();
		Json->WriteValue(TEXT("name"), Name);
		Json->WriteValue(TEXT("blueprints"), FnEntry.Blueprints);
		Json->WriteObjectEnd();
	}
	Json->WriteArrayEnd();
}

static void SerializeClasses(TSharedRef<JsonWriter>& Json, ClassMap& Items, TArray<const UBlueprint*> Blueprints)
{
	Json->WriteArrayStart();
	for (auto& Item : Items)
	{
		auto& ClassName = Item.Key;
		auto& Entry = Item.Value;
		Json->WriteObjectStart();
		Json->WriteValue(TEXT("name"), FString::Printf(TEXT("%s%s"), Entry.Class->GetPrefixCPP(), *Entry.Class->GetName()));

		Json->WriteValue(TEXT("blueprints"), Entry.Blueprints);

		Json->WriteIdentifierPrefix(TEXT("properties"));
		SerializeProperties(Json, Entry, Blueprints);

		Json->WriteIdentifierPrefix(TEXT("functions"));
		SerializeFunctions(Json, Entry);

		Json->WriteObjectEnd();
	}
	Json->WriteArrayEnd();
}

static void SerializeToIndex(AssetIndex Index, FArchive& IndexFile)
{
	TSharedRef<JsonWriter> Json = JsonWriter::Create(&IndexFile);

	Json->WriteObjectStart();

	Json->WriteIdentifierPrefix(TEXT("blueprints"));
	SerializeBlueprints(Json, Index.Blueprints);

	Json->WriteIdentifierPrefix(TEXT("classes"));
	SerializeClasses(Json, Index.Classes, Index.Blueprints);

	Json->WriteObjectEnd();
	Json->Close();
}

void RunAssetScan(FArchive& OutArchive)
{
	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();

	TArray<FAssetData> BlueprintAssets;
	FARFilter ClassFilter;
	ClassFilter.bRecursiveClasses = true;
	ClassFilter.ClassNames.Add(UBlueprint::StaticClass()->GetFName());

	AssetRegistry.GetAssets(ClassFilter, BlueprintAssets);

	AssetIndex Index;

	bool IndexUpdated = false;
	for (const FAssetData& Data : BlueprintAssets)
	{
		IndexUpdated |= Index.ProcessAsset(Data.GetAsset());
	}

	UE_LOG(LogVisualStudioTools, Display, TEXT("Found %d blueprints."), Index.Blueprints.Num());
	SerializeToIndex(Index, OutArchive);
}
}

UVisualStudioToolsCommandlet::UVisualStudioToolsCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = false;
	ShowErrorCount = false;
}

int32 UVisualStudioToolsCommandlet::Main(const FString& Params)
{
	UE_LOG(LogVisualStudioTools, Display, TEXT("Init VS Tools cmdlet."));

	FString FullPath;
	if (!FParse::Value(FCommandLine::Get(), TEXT("output"), FullPath))
	{
		UE_LOG(LogVisualStudioTools, Error, TEXT("Missing file output param."));
		return -1;
	}

	auto IndexFile = TUniquePtr<FArchive>(IFileManager::Get().CreateFileWriter(*FullPath));
	if (!IndexFile)
	{
		UE_LOG(LogVisualStudioTools, Error, TEXT("Failed to write index to %s."), *FullPath);
		return -1;
	}

	if (!FPaths::IsProjectFilePathSet())
	{
		UE_LOG(LogVisualStudioTools, Error, TEXT("You must launch with a project file to be able to update it."));
		return -1;
	}

	VS::RunAssetScan(*IndexFile);

	return 0;
}
