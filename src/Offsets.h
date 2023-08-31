#pragma once
#include "RE/Havok.h"

using tBSHashMapEventFindFunctor_RunFunctor = int32_t (*)(RE::BSSynchronizedClipGeneratorUtils::BSHashMapEventFindFunctor* a_this, const char* a_annotationText, RE::hkArray<char>& a_outArray);
static REL::Relocation<tBSHashMapEventFindFunctor_RunFunctor> BSHashMapEventFindFunctor_RunFunctor{ REL::VariantID(62389, 63330, 0xB18A50) };  // ADDF80, B02790, B18A50

using thkPointerMap_Get = bool (*)(RE::hkPointerMap<int32_t, int32_t>* a_map, int32_t a_key, int64_t& a_out);
static REL::Relocation<thkPointerMap_Get> hkPointerMap_Get{ REL::VariantID(56765, 57187, 0xA053E0) };  // 9CA890, 9EF090, A053E0

using thkPointerMap_GetWithDefault = int32_t (*)(RE::hkPointerMap<int32_t, int32_t>* a_map, int32_t a_key, int32_t a_default);
static REL::Relocation<thkPointerMap_GetWithDefault> hkPointerMap_GetWithDefault{ REL::VariantID(56768, 57195, 0xA05520) };  // 9CA9D0, 9EF1D0, A05520

using thkbClipGenerator_GetAnimDuration = float (*)(RE::hkbClipGenerator* a_this);
static REL::Relocation<thkbClipGenerator_GetAnimDuration> hkbClipGenerator_GetAnimDuration{ REL::VariantID(58613, 59263, 0xA482C0) };  // A0D770, A31F80, A482C0

using thkbClipTriggerArray_copyCtor = float (*)(RE::hkbClipTriggerArray* a_this, RE::hkbClipTriggerArray* a_other);
static REL::Relocation<thkbClipTriggerArray_copyCtor> hkbClipTriggerArray_copyCtor{ REL::VariantID(58596, 59246, 0xA45F20) };  // A0B3D0, A2FBE0, A45F20

using thkbClipGenerator_AddTrigger = float (*)(RE::hkbClipGenerator* a_this, float a_localTime, const RE::hkbEventBase& a_event, bool a_bRelativeToEndOfClip, bool a_bAcyclic, bool a_bIsAnnotation);
static REL::Relocation<thkbClipGenerator_AddTrigger> hkbClipGenerator_AddTrigger{ REL::VariantID(58614, 59264, 0xA48310) };  // A0D7C0, A31FD0, A48310

using thkaDefaultAnimationControl_SetAnimationBinding = void (*)(RE::hkaDefaultAnimationControl* a_this, RE::hkaAnimationBinding* a_binding);
static REL::Relocation<thkaDefaultAnimationControl_SetAnimationBinding> hkaDefaultAnimationControl_SetAnimationBinding{ REL::VariantID(63423, 64390, 0xB595A0) };  // B1E7C0, B43930, B595A0

using thkaSkeleton_CreateSynchronizedClipBinding = RE::hkaAnimationBinding* (*)(const RE::hkaSkeleton* a_skeleton, RE::hkaAnimationBinding* a_animationBinding, const char* a_synchronizedAnimationPrefix);
static REL::Relocation<thkaSkeleton_CreateSynchronizedClipBinding> hkaSkeleton_CreateSynchronizedClipBinding{ REL::VariantID(62393, 63334, 0xB18D20) };  // ADE250, B02A60, B18D20
