#pragma once

static uint32_t& g_dwTlsIndex = *(uint32_t*)REL::VariantID(520865, 407383, 0x30A8C04).address();  // 2F50B74, 2FEB6F4, 30A8C04

namespace RE
{
	template <class Key, class Value>
	class hkMapBase
	{
	public:
		struct Pair
		{
			Key key;
			Value value;
		};

		Pair* _data;            // 00
		int32_t _sizeAndFlags;  // 08
		int32_t _hashMod;       // 0C
	};

	template <class Key, class Value>
	class hkMap : public hkMapBase<Key, Value>
	{
	public:
	};

	template <class Key, class Value, class Allocator = void>
	class hkPointerMap
	{
	protected:
		hkMap<Key, Value> _map; // 00
	};
	static_assert(sizeof(hkPointerMap<void*, void*>) == 0x10);

	class hkbEventPayload : public hkReferencedObject
	{
	public:
		inline static constexpr auto RTTI = RTTI_hkbEventPayload;
		inline static constexpr auto VTABLE = VTABLE_hkbEventPayload;
	};
	static_assert(sizeof(hkbEventPayload) == 0x10);

	class hkbStringEventPayload : public hkbEventPayload
	{
	public:
		inline static constexpr auto RTTI = RTTI_hkbStringEventPayload;
		inline static constexpr auto VTABLE = VTABLE_hkbStringEventPayload;

		// members
		hkStringPtr data;  // 10
	};
	static_assert(sizeof(hkbStringEventPayload) == 0x18);

	class hkbAnimationBindingWithTriggers : public hkReferencedObject
	{
	public:
		inline static constexpr auto RTTI = RTTI_hkbAnimationBindingWithTriggers;
		inline static constexpr auto VTABLE = VTABLE_hkbAnimationBindingWithTriggers;

		struct Trigger
		{
			Trigger(float a_time, int32_t a_eventId, hkbEventPayload* a_payload) :
				time(a_time),
				eventId(a_eventId),
				payload(a_payload)
			{}

			float time = 0.f;                             // 00
			uint32_t pad04 = 0;                           // 04
			int32_t eventId = -1;                         // 08
			hkRefPtr<hkbEventPayload> payload = nullptr;  // 10
		};
		static_assert(offsetof(Trigger, eventId) == 0x8);
		static_assert(sizeof(Trigger) == 0x18);

		// members
		hkRefPtr<hkaAnimationBinding> binding;  // 10
		hkArray<Trigger> triggers;              // 18
	};
	static_assert(sizeof(hkbAnimationBindingWithTriggers) == 0x28);

	class hkbSymbolIdMap : public hkReferencedObject
	{
	public:
		inline static constexpr auto RTTI = RTTI_hkbSymbolIdMap;
		inline static constexpr auto VTABLE = VTABLE_hkbSymbolIdMap;

		hkArray<int32_t> internalToExternalMap;
		hkPointerMap<int32_t, int32_t> externalToInternalMap;
	};

	class BSSynchronizedClipGenerator : public hkbGenerator
	{
	public:
		inline static constexpr auto RTTI = RTTI_BSSynchronizedClipGenerator;
		inline static constexpr auto VTABLE = VTABLE_BSSynchronizedClipGenerator;

		~BSSynchronizedClipGenerator() override;  // 00

		// members
		uint32_t unk48;
		uint32_t unk4C;
		hkbClipGenerator* clipGenerator;                                             // 50
		hkStringPtr syncAnimPrefix;                                                  // 58
		bool syncClipIgnoreMarkPlacement;                                            // 60
		float getToMarkTime;                                                         // 64
		float markErrorThreshold;                                                    // 68
		bool leadCharacter;                                                          // 6C
		bool reorientSupportChar;                                                    // 6D
		bool applyMotionFromRoot;                                                    // 6E
		class BGSSynchronizedAnimationInstance* synchronizedScene;                   // 70
		uint32_t unk78;                                                              // 78
		uint32_t unk7C;                                                              // 7C
		hkQsTransform startingWorldFromModel;                                        // 80
		hkQsTransform worldFromModelWithRootMotion;                                  // B0
		hkQsTransform unkTransform;                                                  // E0
		float getToMarkProgress;                                                     // 110
		hkaAnimationBinding* binding;                                                // 118
		hkPointerMap<int32_t, int32_t>* eventWithPrefixIdToEventWithoutPrefixIdMap;  // 120
		uint16_t synchronizedAnimationBindingIndex;                                  // 128
		bool doneReorientingSupportChar;                                             // 12A
		bool unk12B;                                                                 // 12B
		bool unk12C;                                                                 // 12C
	};
	static_assert(sizeof(BSSynchronizedClipGenerator) == 0x130);
	static_assert(offsetof(BSSynchronizedClipGenerator, synchronizedAnimationBindingIndex) == 0x128);

	namespace BSSynchronizedClipGeneratorUtils
	{
		class FindEventFunctor
		{
		public:
			inline static constexpr auto RTTI = RTTI_BSSynchronizedClipGeneratorUtils__FindEventFunctor;
			inline static constexpr auto VTABLE = VTABLE_BSSynchronizedClipGeneratorUtils__FindEventFunctor;

			virtual ~FindEventFunctor() = default;  // 00

			virtual void Unk_01([[maybe_unused]] const char* a_key) {}  // 01
		};
		static_assert(sizeof(FindEventFunctor) == 0x8);

		class BSHashMapEventFindFunctor : public FindEventFunctor
		{
		public:
			inline static constexpr auto RTTI = RTTI___BSHashMapEventFindFunctor;
			inline static constexpr auto VTABLE = VTABLE___BSHashMapEventFindFunctor;

			BSHashMapEventFindFunctor() { stl::emplace_vtable(this); }

			~BSHashMapEventFindFunctor() override {}  // 00

			// override (FindEventFunctor)
			void Unk_01([[maybe_unused]] const char* a_key) override {}  // 01

			// members
			BSTHashMap<BSFixedString, uint32_t>* map;  // 08
		};
		static_assert(sizeof(BSHashMapEventFindFunctor) == 0x10);
	}

	class hkResource : public hkReferencedObject
	{
	public:
		inline static constexpr auto RTTI = RTTI_hkResource;
		inline static constexpr auto VTABLE = VTABLE_hkResource;
	};
	static_assert(sizeof(hkResource) == 0x10);

	class hkLoader : public hkReferencedObject
	{
	public:
		inline static constexpr auto RTTI = RTTI_hkLoader;
		inline static constexpr auto VTABLE = VTABLE_hkLoader;

		// members
		hkArray<hkResource*> loadedData;  // 10
	};
	static_assert(sizeof(hkLoader) == 0x20);

	namespace BShkbHkxDB
	{
		class DBData : public hkLoader
		{
		public:
			inline static constexpr auto RTTI = RTTI_BShkbHkxDB__DBData;
			inline static constexpr auto VTABLE = VTABLE_BShkbHkxDB__DBData;

			// members
			uint64_t unk20;  // 20
		};
		static_assert(sizeof(DBData) == 0x28);

		struct HashedData
		{
			RE::AnimationFileManagerSingleton::AnimationFileInfo animationFileInfo;  // 00
			uint32_t unk0C;                                                          // 0C
			uint32_t unk10;                                                          // 10
			uint32_t unk14;                                                          // 14
			uint32_t unk18;                                                          // 18
			uint32_t unk1C;                                                          // 1C
		};
		static_assert(sizeof(HashedData) == 0x20);

		struct HashedBehaviorData
		{
			HashedData hashedAnimData;   // 00
			BSResource::Stream* stream;  // 20
			DBData* data;                // 28
		};
		static_assert(sizeof(HashedBehaviorData) == 0x30);

		class ProjectDBData : public hkLoader
		{
		public:
			inline static constexpr auto RTTI = RTTI_BShkbHkxDB__ProjectDBData;
			inline static constexpr auto VTABLE = VTABLE_BShkbHkxDB__ProjectDBData;

			// members
			uint32_t unk20;
			uint32_t unk24;
			BSTArray<HashedBehaviorData*> hashedBehaviors;  // 28
			BSTArray<HashedData> hashedAnimations;          // 40
			uint32_t unk58;
			uint32_t unk5C;
			uint32_t unk60;
			uint32_t unk64;
			uint32_t unk68;
			uint32_t unk6C;
			BSTHashMap<BSFixedString, uint32_t> variableNamesToIds;  // variable name -> event id
			BSTHashMap<BSFixedString, uint32_t> eventNamesToIds;     // event name -> event id. This one is read from when handling anim events.
			BSTArray<char*> eventNames;                              // D0 - all anim events (~2000 total)
			BSTArray<char*> unkE8;                                   // E8 - state names?
			uint64_t unk100;                                         // 100
			uint64_t unk108;                                         // 108
			uint64_t unk110;                                         // 110
			uint64_t unk118;                                         // 118
			uint64_t unk120;                                         // 120
			uint64_t unk128;                                         // 128
			uint64_t unk130;                                         // 130
			uint64_t unk138;                                         // 138
			uint64_t unk140;                                         // 140
			uint64_t unk148;                                         // 148
			hkbAnimationBindingSet* bindings;                        // 150
			hkbSymbolIdMap* characterPropertyIdMap;                  // 158
			hkbProjectData* projectData;                             // 160
			uint32_t unk168;                                         // 168
			uint32_t unk16C;                                         // 16C
			uint64_t unk170;                                         // 170
			uint64_t unk178;                                         // 178
		};
		static_assert(offsetof(ProjectDBData, eventNamesToIds) == 0xA0);
		static_assert(offsetof(ProjectDBData, projectData) == 0x160);
		static_assert(sizeof(ProjectDBData) == 0x180);
	}

	class hkMemoryRouter
	{
	public:
		uint64_t unk00;             // 00
		uint64_t unk08;             // 08
		uint64_t unk10;             // 10
		uint64_t unk18;             // 18
		uint64_t unk20;             // 20
		uint64_t unk28;             // 28
		uint64_t unk30;             // 30
		uint64_t unk38;             // 38
		uint64_t unk40;             // 40
		uint64_t unk48;             // 48
		hkMemoryAllocator* temp;    // 50
		hkMemoryAllocator* heap;    // 58
		hkMemoryAllocator* debug;   // 60
		hkMemoryAllocator* solver;  // 68
		void* userData;             // 70
	};
	static_assert(offsetof(hkMemoryRouter, heap) == 0x58);
}

inline RE::hkMemoryRouter& hkGetMemoryRouter() { return *(RE::hkMemoryRouter*)(uintptr_t)SKSE::WinAPI::TlsGetValue(g_dwTlsIndex); }
inline void* hkHeapAlloc(int numBytes) { return hkGetMemoryRouter().heap->BlockAlloc(numBytes); }

