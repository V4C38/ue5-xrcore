// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "XR_Toolkit/Public/XRInteractionAxialMove.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeXRInteractionAxialMove() {}
// Cross Module References
	COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FTransform();
	UPackage* Z_Construct_UPackage__Script_XR_Toolkit();
	XR_TOOLKIT_API UClass* Z_Construct_UClass_UXRInteractionAxialMove();
	XR_TOOLKIT_API UClass* Z_Construct_UClass_UXRInteractionAxialMove_NoRegister();
	XR_TOOLKIT_API UClass* Z_Construct_UClass_UXRInteractionComponent();
	XR_TOOLKIT_API UEnum* Z_Construct_UEnum_XR_Toolkit_EAxialMoveTarget();
// End Cross Module References
	static FEnumRegistrationInfo Z_Registration_Info_UEnum_EAxialMoveTarget;
	static UEnum* EAxialMoveTarget_StaticEnum()
	{
		if (!Z_Registration_Info_UEnum_EAxialMoveTarget.OuterSingleton)
		{
			Z_Registration_Info_UEnum_EAxialMoveTarget.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_XR_Toolkit_EAxialMoveTarget, (UObject*)Z_Construct_UPackage__Script_XR_Toolkit(), TEXT("EAxialMoveTarget"));
		}
		return Z_Registration_Info_UEnum_EAxialMoveTarget.OuterSingleton;
	}
	template<> XR_TOOLKIT_API UEnum* StaticEnum<EAxialMoveTarget>()
	{
		return EAxialMoveTarget_StaticEnum();
	}
	struct Z_Construct_UEnum_XR_Toolkit_EAxialMoveTarget_Statics
	{
		static const UECodeGen_Private::FEnumeratorParam Enumerators[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[];
#endif
		static const UECodeGen_Private::FEnumParams EnumParams;
	};
	const UECodeGen_Private::FEnumeratorParam Z_Construct_UEnum_XR_Toolkit_EAxialMoveTarget_Statics::Enumerators[] = {
		{ "EAxialMoveTarget::OwningActor", (int64)EAxialMoveTarget::OwningActor },
		{ "EAxialMoveTarget::ThisComponent", (int64)EAxialMoveTarget::ThisComponent },
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UEnum_XR_Toolkit_EAxialMoveTarget_Statics::Enum_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/XRInteractionAxialMove.h" },
		{ "OwningActor.DisplayName", "Owning Actor" },
		{ "OwningActor.Name", "EAxialMoveTarget::OwningActor" },
		{ "ThisComponent.DisplayName", "AxialMove Component" },
		{ "ThisComponent.Name", "EAxialMoveTarget::ThisComponent" },
	};
#endif
	const UECodeGen_Private::FEnumParams Z_Construct_UEnum_XR_Toolkit_EAxialMoveTarget_Statics::EnumParams = {
		(UObject*(*)())Z_Construct_UPackage__Script_XR_Toolkit,
		nullptr,
		"EAxialMoveTarget",
		"EAxialMoveTarget",
		Z_Construct_UEnum_XR_Toolkit_EAxialMoveTarget_Statics::Enumerators,
		RF_Public|RF_Transient|RF_MarkAsNative,
		UE_ARRAY_COUNT(Z_Construct_UEnum_XR_Toolkit_EAxialMoveTarget_Statics::Enumerators),
		EEnumFlags::None,
		(uint8)UEnum::ECppForm::EnumClass,
		METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UEnum_XR_Toolkit_EAxialMoveTarget_Statics::Enum_MetaDataParams), Z_Construct_UEnum_XR_Toolkit_EAxialMoveTarget_Statics::Enum_MetaDataParams)
	};
	UEnum* Z_Construct_UEnum_XR_Toolkit_EAxialMoveTarget()
	{
		if (!Z_Registration_Info_UEnum_EAxialMoveTarget.InnerSingleton)
		{
			UECodeGen_Private::ConstructUEnum(Z_Registration_Info_UEnum_EAxialMoveTarget.InnerSingleton, Z_Construct_UEnum_XR_Toolkit_EAxialMoveTarget_Statics::EnumParams);
		}
		return Z_Registration_Info_UEnum_EAxialMoveTarget.InnerSingleton;
	}
	DEFINE_FUNCTION(UXRInteractionAxialMove::execOnRep_AxialMoveResult)
	{
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->OnRep_AxialMoveResult();
		P_NATIVE_END;
	}
	void UXRInteractionAxialMove::StaticRegisterNativesUXRInteractionAxialMove()
	{
		UClass* Class = UXRInteractionAxialMove::StaticClass();
		static const FNameNativePtrPair Funcs[] = {
			{ "OnRep_AxialMoveResult", &UXRInteractionAxialMove::execOnRep_AxialMoveResult },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
	}
	struct Z_Construct_UFunction_UXRInteractionAxialMove_OnRep_AxialMoveResult_Statics
	{
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UXRInteractionAxialMove_OnRep_AxialMoveResult_Statics::Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/XRInteractionAxialMove.h" },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UXRInteractionAxialMove_OnRep_AxialMoveResult_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UXRInteractionAxialMove, nullptr, "OnRep_AxialMoveResult", nullptr, nullptr, nullptr, 0, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00080401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UXRInteractionAxialMove_OnRep_AxialMoveResult_Statics::Function_MetaDataParams), Z_Construct_UFunction_UXRInteractionAxialMove_OnRep_AxialMoveResult_Statics::Function_MetaDataParams) };
	UFunction* Z_Construct_UFunction_UXRInteractionAxialMove_OnRep_AxialMoveResult()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UXRInteractionAxialMove_OnRep_AxialMoveResult_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UXRInteractionAxialMove);
	UClass* Z_Construct_UClass_UXRInteractionAxialMove_NoRegister()
	{
		return UXRInteractionAxialMove::StaticClass();
	}
	struct Z_Construct_UClass_UXRInteractionAxialMove_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const UECodeGen_Private::FBytePropertyParams NewProp_ObjectToMove_Underlying;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ObjectToMove_MetaData[];
#endif
		static const UECodeGen_Private::FEnumPropertyParams NewProp_ObjectToMove;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bConstrainX_MetaData[];
#endif
		static void NewProp_bConstrainX_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bConstrainX;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bConstrainY_MetaData[];
#endif
		static void NewProp_bConstrainY_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bConstrainY;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bConstrainZ_MetaData[];
#endif
		static void NewProp_bConstrainZ_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bConstrainZ;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_DistanceLimit_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_DistanceLimit;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_MovementSpeed_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_MovementSpeed;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_AxialMoveResult_MetaData[];
#endif
		static const UECodeGen_Private::FStructPropertyParams NewProp_AxialMoveResult;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_Origin_MetaData[];
#endif
		static const UECodeGen_Private::FStructPropertyParams NewProp_Origin;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UXRInteractionAxialMove_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UXRInteractionComponent,
		(UObject* (*)())Z_Construct_UPackage__Script_XR_Toolkit,
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UXRInteractionAxialMove_Statics::DependentSingletons) < 16);
	const FClassFunctionLinkInfo Z_Construct_UClass_UXRInteractionAxialMove_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_UXRInteractionAxialMove_OnRep_AxialMoveResult, "OnRep_AxialMoveResult" }, // 3805762257
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UXRInteractionAxialMove_Statics::FuncInfo) < 2048);
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UXRInteractionAxialMove_Statics::Class_MetaDataParams[] = {
		{ "BlueprintSpawnableComponent", "" },
		{ "ClassGroupNames", "XRToolkit" },
		{ "HideCategories", "Trigger PhysicsVolume" },
		{ "IncludePath", "XRInteractionAxialMove.h" },
		{ "ModuleRelativePath", "Public/XRInteractionAxialMove.h" },
	};
#endif
	const UECodeGen_Private::FBytePropertyParams Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_ObjectToMove_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_ObjectToMove_MetaData[] = {
		{ "Category", "XRCore|Interaction" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n    * Determine whether to move the Actor owning this component, or this component only.\n    */" },
#endif
		{ "ModuleRelativePath", "Public/XRInteractionAxialMove.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Determine whether to move the Actor owning this component, or this component only." },
#endif
	};
#endif
	const UECodeGen_Private::FEnumPropertyParams Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_ObjectToMove = { "ObjectToMove", nullptr, (EPropertyFlags)0x0020080000000001, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UXRInteractionAxialMove, ObjectToMove), Z_Construct_UEnum_XR_Toolkit_EAxialMoveTarget, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_ObjectToMove_MetaData), Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_ObjectToMove_MetaData) }; // 3304269770
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_bConstrainX_MetaData[] = {
		{ "Category", "XRCore|Interaction|AxisConstraints" },
		{ "ModuleRelativePath", "Public/XRInteractionAxialMove.h" },
	};
#endif
	void Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_bConstrainX_SetBit(void* Obj)
	{
		((UXRInteractionAxialMove*)Obj)->bConstrainX = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_bConstrainX = { "bConstrainX", nullptr, (EPropertyFlags)0x0020080000000001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UXRInteractionAxialMove), &Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_bConstrainX_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_bConstrainX_MetaData), Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_bConstrainX_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_bConstrainY_MetaData[] = {
		{ "Category", "XRCore|Interaction|AxisConstraints" },
		{ "ModuleRelativePath", "Public/XRInteractionAxialMove.h" },
	};
#endif
	void Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_bConstrainY_SetBit(void* Obj)
	{
		((UXRInteractionAxialMove*)Obj)->bConstrainY = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_bConstrainY = { "bConstrainY", nullptr, (EPropertyFlags)0x0020080000000001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UXRInteractionAxialMove), &Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_bConstrainY_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_bConstrainY_MetaData), Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_bConstrainY_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_bConstrainZ_MetaData[] = {
		{ "Category", "XRCore|Interaction|AxisConstraints" },
		{ "ModuleRelativePath", "Public/XRInteractionAxialMove.h" },
	};
#endif
	void Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_bConstrainZ_SetBit(void* Obj)
	{
		((UXRInteractionAxialMove*)Obj)->bConstrainZ = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_bConstrainZ = { "bConstrainZ", nullptr, (EPropertyFlags)0x0020080000000001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UXRInteractionAxialMove), &Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_bConstrainZ_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_bConstrainZ_MetaData), Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_bConstrainZ_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_DistanceLimit_MetaData[] = {
		{ "Category", "XRCore|Interaction" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n    * The maximum Distance allowed to travel from the origin location.\n    */" },
#endif
		{ "ModuleRelativePath", "Public/XRInteractionAxialMove.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "The maximum Distance allowed to travel from the origin location." },
#endif
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_DistanceLimit = { "DistanceLimit", nullptr, (EPropertyFlags)0x0020080000000001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UXRInteractionAxialMove, DistanceLimit), METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_DistanceLimit_MetaData), Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_DistanceLimit_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_MovementSpeed_MetaData[] = {
		{ "Category", "XRCore|Interaction" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n    * Movement Speed.\n    */" },
#endif
		{ "ModuleRelativePath", "Public/XRInteractionAxialMove.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Movement Speed." },
#endif
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_MovementSpeed = { "MovementSpeed", nullptr, (EPropertyFlags)0x0020080000000001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UXRInteractionAxialMove, MovementSpeed), METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_MovementSpeed_MetaData), Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_MovementSpeed_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_AxialMoveResult_MetaData[] = {
		{ "ModuleRelativePath", "Public/XRInteractionAxialMove.h" },
	};
#endif
	const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_AxialMoveResult = { "AxialMoveResult", "OnRep_AxialMoveResult", (EPropertyFlags)0x0020080100000020, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UXRInteractionAxialMove, AxialMoveResult), Z_Construct_UScriptStruct_FTransform, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_AxialMoveResult_MetaData), Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_AxialMoveResult_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_Origin_MetaData[] = {
		{ "ModuleRelativePath", "Public/XRInteractionAxialMove.h" },
	};
#endif
	const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_Origin = { "Origin", nullptr, (EPropertyFlags)0x0020080000000020, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UXRInteractionAxialMove, Origin), Z_Construct_UScriptStruct_FTransform, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_Origin_MetaData), Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_Origin_MetaData) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UXRInteractionAxialMove_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_ObjectToMove_Underlying,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_ObjectToMove,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_bConstrainX,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_bConstrainY,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_bConstrainZ,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_DistanceLimit,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_MovementSpeed,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_AxialMoveResult,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UXRInteractionAxialMove_Statics::NewProp_Origin,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UXRInteractionAxialMove_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UXRInteractionAxialMove>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UXRInteractionAxialMove_Statics::ClassParams = {
		&UXRInteractionAxialMove::StaticClass,
		"Engine",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		Z_Construct_UClass_UXRInteractionAxialMove_Statics::PropPointers,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		UE_ARRAY_COUNT(FuncInfo),
		UE_ARRAY_COUNT(Z_Construct_UClass_UXRInteractionAxialMove_Statics::PropPointers),
		0,
		0x00B000A4u,
		METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UXRInteractionAxialMove_Statics::Class_MetaDataParams), Z_Construct_UClass_UXRInteractionAxialMove_Statics::Class_MetaDataParams)
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UXRInteractionAxialMove_Statics::PropPointers) < 2048);
	UClass* Z_Construct_UClass_UXRInteractionAxialMove()
	{
		if (!Z_Registration_Info_UClass_UXRInteractionAxialMove.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UXRInteractionAxialMove.OuterSingleton, Z_Construct_UClass_UXRInteractionAxialMove_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_UXRInteractionAxialMove.OuterSingleton;
	}
	template<> XR_TOOLKIT_API UClass* StaticClass<UXRInteractionAxialMove>()
	{
		return UXRInteractionAxialMove::StaticClass();
	}

	void UXRInteractionAxialMove::ValidateGeneratedRepEnums(const TArray<struct FRepRecord>& ClassReps) const
	{
		static const FName Name_AxialMoveResult(TEXT("AxialMoveResult"));
		static const FName Name_Origin(TEXT("Origin"));

		const bool bIsValid = true
			&& Name_AxialMoveResult == ClassReps[(int32)ENetFields_Private::AxialMoveResult].Property->GetFName()
			&& Name_Origin == ClassReps[(int32)ENetFields_Private::Origin].Property->GetFName();

		checkf(bIsValid, TEXT("UHT Generated Rep Indices do not match runtime populated Rep Indices for properties in UXRInteractionAxialMove"));
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UXRInteractionAxialMove);
	UXRInteractionAxialMove::~UXRInteractionAxialMove() {}
	struct Z_CompiledInDeferFile_FID_XR_Testground_Plugins_XR_Toolkit_XR_Toolkit_Source_XR_Toolkit_Public_XRInteractionAxialMove_h_Statics
	{
		static const FEnumRegisterCompiledInInfo EnumInfo[];
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FEnumRegisterCompiledInInfo Z_CompiledInDeferFile_FID_XR_Testground_Plugins_XR_Toolkit_XR_Toolkit_Source_XR_Toolkit_Public_XRInteractionAxialMove_h_Statics::EnumInfo[] = {
		{ EAxialMoveTarget_StaticEnum, TEXT("EAxialMoveTarget"), &Z_Registration_Info_UEnum_EAxialMoveTarget, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 3304269770U) },
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_XR_Testground_Plugins_XR_Toolkit_XR_Toolkit_Source_XR_Toolkit_Public_XRInteractionAxialMove_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_UXRInteractionAxialMove, UXRInteractionAxialMove::StaticClass, TEXT("UXRInteractionAxialMove"), &Z_Registration_Info_UClass_UXRInteractionAxialMove, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UXRInteractionAxialMove), 1631162467U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_XR_Testground_Plugins_XR_Toolkit_XR_Toolkit_Source_XR_Toolkit_Public_XRInteractionAxialMove_h_16806752(TEXT("/Script/XR_Toolkit"),
		Z_CompiledInDeferFile_FID_XR_Testground_Plugins_XR_Toolkit_XR_Toolkit_Source_XR_Toolkit_Public_XRInteractionAxialMove_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_XR_Testground_Plugins_XR_Toolkit_XR_Toolkit_Source_XR_Toolkit_Public_XRInteractionAxialMove_h_Statics::ClassInfo),
		nullptr, 0,
		Z_CompiledInDeferFile_FID_XR_Testground_Plugins_XR_Toolkit_XR_Toolkit_Source_XR_Toolkit_Public_XRInteractionAxialMove_h_Statics::EnumInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_XR_Testground_Plugins_XR_Toolkit_XR_Toolkit_Source_XR_Toolkit_Public_XRInteractionAxialMove_h_Statics::EnumInfo));
PRAGMA_ENABLE_DEPRECATION_WARNINGS
