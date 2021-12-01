
#include "TF2Config.hpp"
#include "Interfaces/GameData.hpp"
#include "Utils/Thunks.hpp"


// EngineClient
// ClientDLL
// IVModelInfo
// ICVar and IConCommandBase
// ISpatialPartition
// IEngineTrace
// IVDebugOverlay
// IValveGlobalVars
// INetworkStringTableContainer
// IEngineSound
// IStudioRender
#include "Engine/ClientDll.hpp"
#include "Engine/ModelInfo.hpp"
#include "Engine/Convar.hpp"
#include "Engine/GameEnumerator.hpp"
#include "Engine/Trace.hpp"
#include "Engine/DebugOverlay.hpp"
#include "Engine/GlobalVars.hpp"
#include "Engine/NetStrTable.hpp"
#include "Engine/Sounds.hpp"
#include "Engine/ModelRenderer.hpp"
#include "Engine/GameEvent.hpp"
#include "Materials/StudioRender.hpp"

// ClientList
// IGameRules
// IPrediction
// IGameMovement
// IClientTools
#include "Client/EntityList.hpp"
#include "Client/GameRules.hpp"
#include "Client/Prediction.hpp"
#include "Client/ClientTools.hpp"

// ServerList
#include "Engine/EngineIFace.hpp"
#include "Server/ServerTools.hpp"

// VGui::ISurface
// VGui::IPanel
// VGui::IInput
// VGui::IEngineVGui
#include "VGui/Surface.hpp"
#include "VGui/Panel.hpp"
#include "VGui/Input.hpp"
#include "VGui/Engine.hpp"

// IMaterialSystem
// IMaterialSystemConfig
#include "Materials/MaterialSys.hpp"
#include "Materials/MaterialCfg.hpp"

// IVPhysics
// IPhysicsCollision
// IPhysicsSurfaceProps
#include "Physics/VPhysics.hpp"

TF2_NAMESPACE_BEGIN(::Interfaces)


#define TF2_INTERFACE(CONFIG, NAME)		std::tuple{ NAME##_Key, config.CONFIG.NAME, std::bit_cast<void**>(&NAME), static_cast<size_t>(IFaceType::CONFIG), #CONFIG##"::CreateInterface", NAME##_Sig }

__declspec(noinline) bool SDKManager::init(SG::IGameData* game_data, const Config& config, std::initializer_list<const char*> sig_list)
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
			VGui,
			Material,
			Physics,

			Count
		};

		using CreateInterfaceFn = Utils::IFuncThunk<void*, const char*, int*>;
		CreateInterfaceFn iface_factory[static_cast<size_t>(IFaceType::Count)]{ };
		
		std::array ifaces{
			// engine library keys:
			TF2_INTERFACE(Engine, EngineClient),
			TF2_INTERFACE(Engine, EngineServer),
			TF2_INTERFACE(Engine, ModelInfo),
			TF2_INTERFACE(Engine, SpatialPartition),
			TF2_INTERFACE(Engine, ClientTrace),
			TF2_INTERFACE(Engine, DebugOverlay),
			TF2_INTERFACE(Engine, GlobalVars),
			TF2_INTERFACE(Engine, EngineVGUI),
			TF2_INTERFACE(Engine, ClientSound),
			TF2_INTERFACE(Engine, ModelRenderer),
			TF2_INTERFACE(Engine, GameEventMgr),
			TF2_INTERFACE(Engine, StudioRender),

			// valve std library keys:
			std::tuple{ CVar_Key, config.Engine.Convar, std::bit_cast<void**>(&CVar), static_cast<size_t>(IFaceType::VSTDLib), "ValveSTD::CreateInterface", CVar_Sig },

			// client library keys:
			TF2_INTERFACE(Client, ClientList),
			TF2_INTERFACE(Client, ClientDLL),
			TF2_INTERFACE(Client, GameRules),
			TF2_INTERFACE(Client, Prediction),
			TF2_INTERFACE(Client, GameMovement),
			TF2_INTERFACE(Client, ClientTools),

			// server library keys:
			TF2_INTERFACE(Server, ServerDLL),
			TF2_INTERFACE(Server, ServerTools),

			// vgui library keys:
			TF2_INTERFACE(VGui, Surface),
			TF2_INTERFACE(VGui, Panel),
			TF2_INTERFACE(VGui, InputSys),

			// materialsystem library keys:
			TF2_INTERFACE(Material, MatSys),
			TF2_INTERFACE(Material, MatSysCfg),

			// physics library kyes:
			TF2_INTERFACE(Physics, VPhysics),
			TF2_INTERFACE(Physics, PhysicsCollision),
			TF2_INTERFACE(Physics, PhysicsSurfProp)
		};

#undef TF2_INTERFACE

		m_GameData->PushFiles({ "/GameData/Interfaces" });
		for (auto& [name, is_on, res, type, iface_name, is_sig] : ifaces)
		{
			if (!is_on)
				continue;

			if (!is_sig)
			{
				if (!iface_factory[type])
					iface_factory[type] = m_GameData->ReadSignature(iface_name).get();

				*res = iface_factory[type](name, nullptr);
			}
			else
			{
				*res = m_GameData->ReadSignature(name).get();
			}

			assert(*res);
			if (!*res)
				return false;
		}
	}

	if (sig_list.size() > 0)
		m_GameData->PushFiles(sig_list);
	return true;
}


SDKManager::~SDKManager()
{
	delete m_GameData;
	m_GameData = nullptr;
	SDKManager::Manager = nullptr;
}


TF2_NAMESPACE_END();
