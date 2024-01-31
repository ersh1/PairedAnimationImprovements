#include "Hooks.h"
#include "Offsets.h"

namespace Hooks
{
    void SynchronizedClipHooks::ConvertAnnotationsToTriggersForSynchronizedClip(RE::hkbAnimationBindingWithTriggers* a_animationBindingWithTriggers, const char* a_syncAnimPrefix, RE::BSSynchronizedClipGeneratorUtils::BSHashMapEventFindFunctor* a_functor)
    {
		// reimplement the vanilla function, but instead of removing the prefix and letting all others through, we only let the ones through that have the prefix
		if (a_animationBindingWithTriggers) {
			if (const auto& binding = a_animationBindingWithTriggers->binding) {
				if (const auto& animation = binding->animation) {
					if (!a_syncAnimPrefix || !*a_syncAnimPrefix) {
						a_syncAnimPrefix = "NPC";
					}

					for (const auto& annotationTrack : animation->annotationTracks) {
						for (const auto& annotation : annotationTrack.annotations) {
							const auto prefixLength = strlen(a_syncAnimPrefix);
							if (strlen(annotation.text.data()) > prefixLength && !strncmp(a_syncAnimPrefix, annotation.text.data(), prefixLength)) {
								std::string_view annotationWithoutPrefix = annotation.text.data() + prefixLength;
								RE::hkArray<char> tempArray = {};
								tempArray._capacityAndFlags = static_cast<int32_t>(0x80000000);
								tempArray.reserve(0x80);
								tempArray._size = 1;
								const auto functorResult = BSHashMapEventFindFunctor_RunFunctor(a_functor, annotationWithoutPrefix.data(), tempArray);
								if (functorResult != -1) {
									RE::hkbStringEventPayload* payload = nullptr;
									if (tempArray.size() > 1) {
										payload = static_cast<RE::hkbStringEventPayload*>(hkHeapAlloc(sizeof(RE::hkbStringEventPayload)));
										memset(payload, 0, sizeof(RE::hkbStringEventPayload));
										payload->memSizeAndFlags = static_cast<uint16_t>(0xFFFF);
										SKSE::stl::emplace_vtable(payload);
										payload->data = tempArray.data();
									}

									a_animationBindingWithTriggers->triggers.push_back({ annotation.time, functorResult, payload });
								}
								tempArray.resize(0);
							}
						}
					}
				}
			}
		}
    }

    void SynchronizedClipHooks::FixSynchronizedClipTriggers(RE::BSSynchronizedClipGenerator* a_synchronizedClipGenerator, const RE::hkbContext& a_context, RE::hkbClipGenerator* a_clipGenerator)
    {
		if (!a_clipGenerator) {
			return;
		}

		// early out if the animation isn't fully loaded yet
		if (a_clipGenerator->userData == 8) {
			return;
		}

		const auto& hkbBehaviorGraph = a_context.behavior;
		if (!hkbBehaviorGraph) {
			logger::error("    == a_context.behavior is null! ==");
			return;
		}

		const auto& behaviorGraphData = hkbBehaviorGraph->data;
		if (!behaviorGraphData) {
			logger::error("    == a_context.behavior->data is null! ==");
			return;
		}

		const auto& stringData = behaviorGraphData->stringData;
		if (!stringData) {
			logger::error("    == a_context.behavior->data->stringData is null! ==");
			return;
		}

		if (stringData->eventNames.empty()) {
			logger::error("    == eventNames in stringData is empty! ==");
			return;
		}
		// create new binding with proper transformTrackToBoneIndices for this syncAnimPrefix
        if (!a_synchronizedClipGenerator->binding) {
			if (a_clipGenerator->binding) {
				const auto& setup = a_context.character->setup;
				if (setup) {
					const auto& animationSkeleton = setup->animationSkeleton;
					if (animationSkeleton) {
						a_synchronizedClipGenerator->binding = hkaSkeleton_CreateSynchronizedClipBinding(animationSkeleton.get(), a_clipGenerator->binding, a_synchronizedClipGenerator->syncAnimPrefix.data());
						const auto& animationControl = a_clipGenerator->animationControl;
						hkaDefaultAnimationControl_SetAnimationBinding(animationControl.get(), a_synchronizedClipGenerator->binding);
					}
				}
			}
		}

		const RE::BShkbAnimationGraph* animGraph = SKSE::stl::adjust_pointer<RE::BShkbAnimationGraph>(a_context.character, -0xC0);
		const auto projectDBData = static_cast<RE::BShkbHkxDB::ProjectDBData*>(animGraph->projectDBData);

		const auto symbolIdMap = reinterpret_cast<RE::hkbSymbolIdMap*>(a_context.behavior->eventIDMap.get());

		auto syncAnimPrefix = a_synchronizedClipGenerator->syncAnimPrefix.data();
		if (syncAnimPrefix == nullptr) {
			syncAnimPrefix = "NPC";
		}

		logger::trace("    == fixing up sync clip triggers in {}, prefix \"{}\" ==", a_clipGenerator->animationName.data(), syncAnimPrefix);

		const auto prefixLength = strlen(syncAnimPrefix);

		RE::BSSynchronizedClipGeneratorUtils::BSHashMapEventFindFunctor findFunctor;
		findFunctor.map = &projectDBData->eventNamesToIds;
		float animDuration = hkbClipGenerator_GetAnimDuration(a_clipGenerator);

		// reset triggers to original triggers
		if (a_clipGenerator->originalTriggers) {
			auto newClipTriggerArray = static_cast<RE::hkbClipTriggerArray*>(hkHeapAlloc(sizeof(RE::hkbClipTriggerArray)));
			memset(newClipTriggerArray, 0, sizeof(RE::hkbClipTriggerArray));
			hkbClipTriggerArray_copyCtor(newClipTriggerArray, a_clipGenerator->originalTriggers.get());
			a_clipGenerator->triggers = RE::hkRefPtr(newClipTriggerArray);
		}

		// don't create triggers from annotations if a_clipGenerator->flags has the 0x10 bit (FLAG_DONT_CONVERT_ANNOTATIONS_TO_TRIGGERS) set
		if ((a_clipGenerator->flags & 0x10) == 0) {
			// create triggers from annotations
			if (const auto& binding = a_clipGenerator->binding) {
				if (const auto& animation = binding->animation) {
					for (const auto& annotationTrack : animation->annotationTracks) {
						for (const auto& annotation : annotationTrack.annotations) {
							auto triggerTime = annotation.time - a_clipGenerator->cropStartAmountLocalTime;
							if (triggerTime >= 0.f && triggerTime <= animDuration) {
								if (strlen(annotation.text.data()) > prefixLength && !strncmp(syncAnimPrefix, annotation.text.data(), prefixLength)) {
									std::string_view annotationWithoutPrefix = annotation.text.data() + prefixLength;
									RE::hkArray<char> tempArray = {};
									tempArray._capacityAndFlags = static_cast<int32_t>(0x80000000);
									tempArray.reserve(0x80);
									tempArray._size = 1;
									const int32_t functorResult = BSHashMapEventFindFunctor_RunFunctor(&findFunctor, annotationWithoutPrefix.data(), tempArray);
									if (functorResult != -1) {
										RE::hkbStringEventPayload* payload = nullptr;
										if (tempArray.size() > 1) {
											payload = static_cast<RE::hkbStringEventPayload*>(hkHeapAlloc(sizeof(RE::hkbStringEventPayload)));
											memset(payload, 0, sizeof(RE::hkbStringEventPayload));
											payload->memSizeAndFlags = static_cast<uint16_t>(0xFFFF);
											SKSE::stl::emplace_vtable(payload);
											payload->data = tempArray.data();
										}

										auto eventId = functorResult;

										if (symbolIdMap) {
											if (eventId >= 0) {
												eventId = hkPointerMap_GetWithDefault(&symbolIdMap->externalToInternalMap, eventId + 1, -1);
											}
										}

										if (eventId != -1) {
											RE::hkbEventBase event;
											event.id = static_cast<RE::hkbEventBase::SystemEventIDs>(eventId);
											event.payload = RE::hkRefPtr(payload);

											hkbClipGenerator_AddTrigger(a_clipGenerator, triggerTime, event, false, false, true);
										}
									}
								}
							}
						}
					}
				}
			}
		}

		// fix prefixes
		if (a_clipGenerator->triggers) {
			for (auto& trigger : a_clipGenerator->triggers->triggers) {
				// only remove prefix from events that didn't come from annotations
				if (trigger.isAnnotation) {
				    continue;
				}
				int32_t id = trigger.event.id;

				if (id != -1) {
					if (id < stringData->eventNames.size()) {
						auto& name = stringData->eventNames[id];
						if (strlen(name.data()) > prefixLength && !strncmp(syncAnimPrefix, name.data(), prefixLength)) {
							std::string_view nameWithoutPrefix = name.data() + prefixLength;
							if (auto it = projectDBData->eventNamesToIds.find(nameWithoutPrefix); it != projectDBData->eventNamesToIds.end()) {
								const int32_t newExternalId = it->second;

								int32_t newId = hkPointerMap_GetWithDefault(&symbolIdMap->externalToInternalMap, newExternalId + 1, -1);

								if (newId >= 0) {
									logger::trace("    {} -> {}", stringData->eventNames[id].data(), stringData->eventNames[static_cast<int32_t>(newId)].data());
									trigger.event.id = static_cast<RE::hkbEventBase::SystemEventIDs>(newId);
								} else {
									logger::error("    failed to convert {} -> {}, internal id for {} not found!", stringData->eventNames[id].data(), stringData->eventNames[static_cast<int32_t>(newId)].data(), stringData->eventNames[static_cast<int32_t>(newId)].data());
								}
							} else {
								logger::error("    no match for {}", nameWithoutPrefix.data());
							}
						}
					} else {
						logger::error("    id ({}) out of bounds ({})!", id, stringData->eventNames.size());
					}
				} else {
					logger::error("    invalid id!");
				}
			}
		}

		logger::trace("    == done ==");
		
    }

    void SynchronizedClipHooks::BSSynchronizedClipGenerator_Activate_ActivateInternalClip(RE::BSSynchronizedClipGenerator* a_synchronizedClipGenerator, const RE::hkbContext& a_context, RE::hkbClipGenerator* a_clipGenerator)
    {
		_BSSynchronizedClipGenerator_Activate_ActivateInternalClip(a_synchronizedClipGenerator, a_context, a_clipGenerator);

		FixSynchronizedClipTriggers(a_synchronizedClipGenerator, a_context, a_clipGenerator);
    }

    void SynchronizedClipHooks::BSSynchronizedClipGenerator_Update_ActivateInternalClip(RE::BSSynchronizedClipGenerator* a_synchronizedClipGenerator, const RE::hkbContext& a_context, RE::hkbClipGenerator* a_clipGenerator)
    {
		_BSSynchronizedClipGenerator_Update_ActivateInternalClip(a_synchronizedClipGenerator, a_context, a_clipGenerator);

		FixSynchronizedClipTriggers(a_synchronizedClipGenerator, a_context, a_clipGenerator);
    }

    void LogHooks::BSSynchronizedClipGenerator_Activate(RE::BSSynchronizedClipGenerator* a_this, const RE::hkbContext& a_context)
    {
		auto& stringData = a_context.behavior->data->stringData;

		auto animationBindingSet = reinterpret_cast<RE::hkbAnimationBindingSet*>(a_context.character->animationBindingSet.get());
		if (!animationBindingSet) {
			animationBindingSet = a_context.character->setup->animationBindingSet.get();
		}

		const auto symbolIdMap = reinterpret_cast<RE::hkbSymbolIdMap*>(a_context.behavior->eventIDMap.get());

		auto syncAnimPrefix = a_this->syncAnimPrefix.data();
		if (syncAnimPrefix == nullptr) {
			syncAnimPrefix = "NPC";
		}

		logger::trace("==== START {}, prefix \"{}\" ====", a_this->clipGenerator->animationName.data(), syncAnimPrefix);

		logger::trace("===triggers pre activation===");
		if (a_this->clipGenerator->triggers) {
			for (const auto& trigger : a_this->clipGenerator->triggers->triggers) {

				auto externalId = trigger.event.id;

				if (trigger.event.payload) {
					auto strPayload = reinterpret_cast<RE::hkbStringEventPayload*>(trigger.event.payload.get());
					logger::trace(" {}: {} with payload {}", trigger.localTime, stringData->eventNames[externalId].data(), strPayload->data.data());
				} else {
					logger::trace(" {}: {}", trigger.localTime, stringData->eventNames[externalId].data());
				}
			}
		} else {
			logger::trace(" no triggers pre activation");
		}

		logger::trace("===activating synchronized clip===");
		_BSSynchronizedClipGenerator_Activate(a_this, a_context);
		logger::trace("===done activating===");

		logger::trace("===original triggers===");
		if (a_this->clipGenerator->originalTriggers && !a_this->clipGenerator->originalTriggers->triggers.empty()) {
			for (const auto& trigger : a_this->clipGenerator->originalTriggers->triggers) {

				auto externalId = trigger.event.id;
				logger::trace(" {}: {}", trigger.localTime, stringData->eventNames[externalId].data());
			}
		} else {
			logger::trace(" no original triggers");
		}

	    RE::hkbAnimationBindingWithTriggers* animationBindingWithTriggers = nullptr;

		if (animationBindingSet && a_this->synchronizedAnimationBindingIndex != static_cast<uint16_t>(-1)) {
			animationBindingWithTriggers = animationBindingSet->bindings[a_this->synchronizedAnimationBindingIndex];
		}

		logger::trace("===triggers from binding===");
		if (animationBindingWithTriggers && !animationBindingWithTriggers->triggers.empty()) {
			for (const auto& trigger : animationBindingWithTriggers->triggers) {
				if (trigger.eventId >= 0) {

					auto id = hkPointerMap_GetWithDefault(&symbolIdMap->externalToInternalMap, trigger.eventId + 1, -1);

					if (trigger.payload) {
						auto strPayload = reinterpret_cast<RE::hkbStringEventPayload*>(trigger.payload.get());
						logger::trace(" {}: {} with payload {}", trigger.time, stringData->eventNames[id].data(), strPayload->data.data());
					} else {
						logger::trace(" {}: {}", trigger.time, stringData->eventNames[id].data());
					}
				} else {
					logger::error(" invalid external id {}", stringData->eventNames[trigger.eventId].data());
				}
			}
		} else {
		    logger::trace(" no triggers from binding");
		}		

		logger::trace("===final triggers===");
		if (a_this->clipGenerator->triggers && !a_this->clipGenerator->triggers->triggers.empty()) {
			for (const auto& trigger : a_this->clipGenerator->triggers->triggers) {

				auto externalId = trigger.event.id;

				if (trigger.event.payload) {
					auto strPayload = reinterpret_cast<RE::hkbStringEventPayload*>(trigger.event.payload.get());
					logger::trace(" {}: {} with payload {}", trigger.localTime, stringData->eventNames[externalId].data(), strPayload->data.data());
				} else {
					logger::trace(" {}: {}", trigger.localTime, stringData->eventNames[externalId].data());
				}
			}
		} else {
			logger::trace(" no final triggers");
		}

		logger::trace("==== END {}, prefix \"{}\" ====", a_this->clipGenerator->animationName.data(), syncAnimPrefix);
    }

    void Install()
	{
		logger::info("Hooking...");

		SynchronizedClipHooks::Hook();
#ifndef NDEBUG
		LogHooks::Hook();
#endif

		logger::info("...success");
	}
}
