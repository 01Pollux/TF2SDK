
#include "TF2Config.hpp"
#include "Interfaces/GameData.hpp"
#include "Utils/Thunks.hpp"


// EngineClient
// ClientDLL
// IVModelInfo
// ICVar and IConCommandBase
#include "Engine/ClientDll.hpp"
#include "Engine/ModelInfo.hpp"
#include "Engine/Convar.hpp"

// ClientList
#include "Client/EntityList.hpp"

// ServerList
#include "Engine/EngineIFace.hpp"


TF2_NAMESPACE_BEGIN(::Interfaces)



#define TF2_INTERFACE(CONFIG, NAME)	std::tuple{ NAME##_Key, config.CONFIG.NAME, std::bit_cast<void**>(&NAME), static_cast<size_t>(IFaceType::CONFIG), #CONFIG##"::CreateInterface" }

void SDKManager::init(SG::IGameData* game_data, const Config& config, std::initializer_list<const char*> sig_list)
{
	set_gamedata(game_data);
	SDKManager::Manager = this;

	// Initialize any interfaces
	{
		enum class IFaceType
		{
			Engine,
			Client,
			Server,
			VSTDLib,

			Count
		};

		using CreateInterfaceFn = Utils::IFuncThunk<void*, const char*, int*>;
		CreateInterfaceFn iface_factory[static_cast<size_t>(IFaceType::Count)]{ };

		std::array ifaces{
			// engine library keys:
			TF2_INTERFACE(Engine, EngineClient),
			TF2_INTERFACE(Engine, EngineServer),
			TF2_INTERFACE(Engine, ModelInfo),

			// valve std library keys:
			std::tuple{ CVar_Key, config.Engine.Convar, std::bit_cast<void**>(&CVar), static_cast<size_t>(IFaceType::VSTDLib), "ValveSTD::CreateInterface" },

			// client library keys:
			TF2_INTERFACE(Client, ClientList),
			TF2_INTERFACE(Client, ClientDLL),

			// server library keys:
			TF2_INTERFACE(Server, ServerDLL),
		};

		m_GameData->PushFiles({ "/GameData/Interfaces" });
		for (auto& [name, is_on, res, type, iface_name] : ifaces)
		{
			if (!is_on)
				continue;

			if (!iface_factory[type])
				iface_factory[type] = m_GameData->ReadSignature(iface_name).get();

			*res = iface_factory[type](name, nullptr);
		}
	}

	if (sig_list.size() > 0)
		m_GameData->PushFiles(sig_list);
}

SDKManager::~SDKManager()
{
	delete m_GameData;
	m_GameData = nullptr;
	SDKManager::Manager = nullptr;
}


#undef TF2_KEYCONFIG
#undef TF2_INTERFACE

TF2_NAMESPACE_END();

