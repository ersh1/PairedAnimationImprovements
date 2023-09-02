#pragma once
#include "RE/Havok.h"

namespace Hooks
{
	class SynchronizedClipHooks
	{
	public:
		static void Hook()
		{
			auto& trampoline = SKSE::GetTrampoline();

			// hook both cases where the internal clip generator can be activated - add triggers from annotations, and remove the prefixes from the triggers not coming from annotations
			static REL::Relocation<uintptr_t> synchronizedClipActivateFunc{ REL::VariantID(62112, 63048, 0xB0CD30) };  // AD2320, AF6B30, B0CD30
			SKSE::AllocTrampoline(14);
			_BSSynchronizedClipGenerator_Activate_ActivateInternalClip = trampoline.write_call<5>(synchronizedClipActivateFunc.address() + REL::VariantOffset(0x14C, 0x14C, 0x158).offset(), BSSynchronizedClipGenerator_Activate_ActivateInternalClip);

			static REL::Relocation<uintptr_t> synchronizedClipUpdateFunc{ REL::VariantID(62114, 63050, 0xB0D0B0) };  // AD2690, AF6EA0, B0D0B0
			SKSE::AllocTrampoline(14);
			_BSSynchronizedClipGenerator_Update_ActivateInternalClip = trampoline.write_call<5>(synchronizedClipUpdateFunc.address() + REL::VariantOffset(0x5E, 0x5E, 0x5E).offset(), BSSynchronizedClipGenerator_Update_ActivateInternalClip);

			// nop out the creation of eventWithPrefixIdToEventWithoutPrefixIdMap inside synchronized clip because it's not needed anymore
			uint8_t patchNop5[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };  // nop
			REL::safe_write<uint8_t>(synchronizedClipActivateFunc.address() + REL::VariantOffset(0x4F, 0x4F, 0x4F).offset(), patchNop5);

			// skip the creation of synchronized clip bindings
			uint8_t patchClearRax[] = { 0x48, 0x31, 0xC0, 0x90, 0x90 };                                                   // call -> xor rax, rax
			static REL::Relocation<uintptr_t> createSynchronizedClipBindingsFunc{ REL::VariantID(63017, 63942, 0xB40550) };  // B05710, B2A980, B40550  hkbBehaviorGraph::CreateSynchronizedClipBindings
			REL::safe_write<uint8_t>(createSynchronizedClipBindingsFunc.address() + REL::VariantOffset(0x35, 0x37, 0x35).offset(), patchClearRax);  // with rax == nullptr, the function will return immediately
		}

	private:
		static void ConvertAnnotationsToTriggersForSynchronizedClip(RE::hkbAnimationBindingWithTriggers* a_animationBindingWithTriggers, const char* a_syncAnimPrefix, RE::BSSynchronizedClipGeneratorUtils::BSHashMapEventFindFunctor* a_functor);
		static void FixSynchronizedClipTriggers(RE::BSSynchronizedClipGenerator* a_synchronizedClipGenerator, const RE::hkbContext& a_context, RE::hkbClipGenerator* a_clipGenerator);

		static void BSSynchronizedClipGenerator_Activate_ActivateInternalClip(RE::BSSynchronizedClipGenerator* a_synchronizedClipGenerator, const RE::hkbContext& a_context, RE::hkbClipGenerator* a_clipGenerator);
		static void BSSynchronizedClipGenerator_Update_ActivateInternalClip(RE::BSSynchronizedClipGenerator* a_synchronizedClipGenerator, const RE::hkbContext& a_context, RE::hkbClipGenerator* a_clipGenerator);

	    static inline REL::Relocation<decltype(BSSynchronizedClipGenerator_Activate_ActivateInternalClip)> _BSSynchronizedClipGenerator_Activate_ActivateInternalClip;
		static inline REL::Relocation<decltype(BSSynchronizedClipGenerator_Update_ActivateInternalClip)> _BSSynchronizedClipGenerator_Update_ActivateInternalClip;
	};

	class LogHooks
	{
	public:
		static void Hook()
		{
			REL::Relocation<uintptr_t> BSSynchronizedClipGeneratorVtbl{ RE::VTABLE_BSSynchronizedClipGenerator[0] };
			_BSSynchronizedClipGenerator_Activate = BSSynchronizedClipGeneratorVtbl.write_vfunc(0x4, BSSynchronizedClipGenerator_Activate);
		}

	private:
		static void BSSynchronizedClipGenerator_Activate(RE::BSSynchronizedClipGenerator* a_this, const RE::hkbContext& a_context);

		static inline REL::Relocation<decltype(BSSynchronizedClipGenerator_Activate)> _BSSynchronizedClipGenerator_Activate;
	};

	void Install();
}
