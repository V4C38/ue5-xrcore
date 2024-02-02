// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "XRInteractionAxialMove.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class UXRInteractionAxialMove;
#ifdef XR_TOOLKIT_XRInteractionAxialMove_generated_h
#error "XRInteractionAxialMove.generated.h already included, missing '#pragma once' in XRInteractionAxialMove.h"
#endif
#define XR_TOOLKIT_XRInteractionAxialMove_generated_h

#define FID_XR_Testground_Plugins_XR_Toolkit_XR_Toolkit_Source_XR_Toolkit_Public_XRInteractionAxialMove_h_9_DELEGATE \
XR_TOOLKIT_API void FOnMovementUpdate_DelegateWrapper(const FMulticastScriptDelegate& OnMovementUpdate, UXRInteractionAxialMove* Sender, float Progress);


#define FID_XR_Testground_Plugins_XR_Toolkit_XR_Toolkit_Source_XR_Toolkit_Public_XRInteractionAxialMove_h_10_DELEGATE \
XR_TOOLKIT_API void FOnMovementLimitReached_DelegateWrapper(const FMulticastScriptDelegate& OnMovementLimitReached, UXRInteractionAxialMove* Sender);


#define FID_XR_Testground_Plugins_XR_Toolkit_XR_Toolkit_Source_XR_Toolkit_Public_XRInteractionAxialMove_h_23_SPARSE_DATA
#define FID_XR_Testground_Plugins_XR_Toolkit_XR_Toolkit_Source_XR_Toolkit_Public_XRInteractionAxialMove_h_23_SPARSE_DATA_PROPERTY_ACCESSORS
#define FID_XR_Testground_Plugins_XR_Toolkit_XR_Toolkit_Source_XR_Toolkit_Public_XRInteractionAxialMove_h_23_EDITOR_ONLY_SPARSE_DATA_PROPERTY_ACCESSORS
#define FID_XR_Testground_Plugins_XR_Toolkit_XR_Toolkit_Source_XR_Toolkit_Public_XRInteractionAxialMove_h_23_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execOnRep_AxialMoveResult); \
	DECLARE_FUNCTION(execGetMovementProgress);


#define FID_XR_Testground_Plugins_XR_Toolkit_XR_Toolkit_Source_XR_Toolkit_Public_XRInteractionAxialMove_h_23_ACCESSORS
#define FID_XR_Testground_Plugins_XR_Toolkit_XR_Toolkit_Source_XR_Toolkit_Public_XRInteractionAxialMove_h_23_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUXRInteractionAxialMove(); \
	friend struct Z_Construct_UClass_UXRInteractionAxialMove_Statics; \
public: \
	DECLARE_CLASS(UXRInteractionAxialMove, UXRInteractionComponent, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/XR_Toolkit"), NO_API) \
	DECLARE_SERIALIZER(UXRInteractionAxialMove) \
	enum class ENetFields_Private : uint16 \
	{ \
		NETFIELD_REP_START=(uint16)((int32)Super::ENetFields_Private::NETFIELD_REP_END + (int32)1), \
		AxialMoveResult=NETFIELD_REP_START, \
		RootTransform, \
		NETFIELD_REP_END=RootTransform	}; \
	NO_API virtual void ValidateGeneratedRepEnums(const TArray<struct FRepRecord>& ClassReps) const override;


#define FID_XR_Testground_Plugins_XR_Toolkit_XR_Toolkit_Source_XR_Toolkit_Public_XRInteractionAxialMove_h_23_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UXRInteractionAxialMove(UXRInteractionAxialMove&&); \
	NO_API UXRInteractionAxialMove(const UXRInteractionAxialMove&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UXRInteractionAxialMove); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UXRInteractionAxialMove); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(UXRInteractionAxialMove) \
	NO_API virtual ~UXRInteractionAxialMove();


#define FID_XR_Testground_Plugins_XR_Toolkit_XR_Toolkit_Source_XR_Toolkit_Public_XRInteractionAxialMove_h_19_PROLOG
#define FID_XR_Testground_Plugins_XR_Toolkit_XR_Toolkit_Source_XR_Toolkit_Public_XRInteractionAxialMove_h_23_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_XR_Testground_Plugins_XR_Toolkit_XR_Toolkit_Source_XR_Toolkit_Public_XRInteractionAxialMove_h_23_SPARSE_DATA \
	FID_XR_Testground_Plugins_XR_Toolkit_XR_Toolkit_Source_XR_Toolkit_Public_XRInteractionAxialMove_h_23_SPARSE_DATA_PROPERTY_ACCESSORS \
	FID_XR_Testground_Plugins_XR_Toolkit_XR_Toolkit_Source_XR_Toolkit_Public_XRInteractionAxialMove_h_23_EDITOR_ONLY_SPARSE_DATA_PROPERTY_ACCESSORS \
	FID_XR_Testground_Plugins_XR_Toolkit_XR_Toolkit_Source_XR_Toolkit_Public_XRInteractionAxialMove_h_23_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_XR_Testground_Plugins_XR_Toolkit_XR_Toolkit_Source_XR_Toolkit_Public_XRInteractionAxialMove_h_23_ACCESSORS \
	FID_XR_Testground_Plugins_XR_Toolkit_XR_Toolkit_Source_XR_Toolkit_Public_XRInteractionAxialMove_h_23_INCLASS_NO_PURE_DECLS \
	FID_XR_Testground_Plugins_XR_Toolkit_XR_Toolkit_Source_XR_Toolkit_Public_XRInteractionAxialMove_h_23_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> XR_TOOLKIT_API UClass* StaticClass<class UXRInteractionAxialMove>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_XR_Testground_Plugins_XR_Toolkit_XR_Toolkit_Source_XR_Toolkit_Public_XRInteractionAxialMove_h


#define FOREACH_ENUM_EAXIALMOVETARGET(op) \
	op(EAxialMoveTarget::OwningActor) \
	op(EAxialMoveTarget::ThisComponent) 

enum class EAxialMoveTarget : uint8;
template<> struct TIsUEnumClass<EAxialMoveTarget> { enum { Value = true }; };
template<> XR_TOOLKIT_API UEnum* StaticEnum<EAxialMoveTarget>();

PRAGMA_ENABLE_DEPRECATION_WARNINGS
