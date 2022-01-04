
#include <px/interfaces/GameData.hpp>
#include <tf2/config.hpp>
#include <tf2/utils/Thunks.hpp>


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
#include <tf2/engine/ClientDll.hpp>
#include <tf2/engine/ModelInfo.hpp>
#include <tf2/engine/Convar.hpp>
#include <tf2/engine/GameEnumerator.hpp>
#include <tf2/engine/Trace.hpp>
#include <tf2/engine/DebugOverlay.hpp>
#include <tf2/engine/GlobalVars.hpp>
#include <tf2/engine/NetStrTable.hpp>
#include <tf2/engine/Sounds.hpp>
#include <tf2/engine/ModelRenderer.hpp>
#include <tf2/engine/GameEvent.hpp>
#include <tf2/materials/StudioRender.hpp>

// ClientList
// IGameRules
// IPrediction
// IGameMovement
// IClientTools
#include <tf2/client/EntityList.hpp>
#include <tf2/client/GameRules.hpp>
#include <tf2/client/Prediction.hpp>
#include <tf2/client/ClientTools.hpp>

// ServerList
#include <tf2/engine/EngineIFace.hpp>
#include <tf2/server/ServerTools.hpp>

// VGui::ISurface
// VGui::IPanel
// VGui::IInput
// VGui::IEngineVGui
#include <tf2/vgui/Surface.hpp>
#include <tf2/vgui/Panel.hpp>
#include <tf2/vgui/Input.hpp>
#include <tf2/vgui/Engine.hpp>

// IMaterialSystem
// IMaterialSystemConfig
#include <tf2/materials/MaterialSys.hpp>
#include <tf2/materials/MaterialCfg.hpp>

// IVPhysics
// IPhysicsCollision
// IPhysicsSurfaceProps
#include <tf2/physics/VPhysics.hpp>

TF2_NAMESPACE_BEGIN(::interfaces)


#define TF2_INTERFACE(CONFIG, NAME)		std::tuple{ NAME##_Key, config.CONFIG.NAME, std::bit_cast<void**>(&NAME), static_cast<size_t>(IFaceType::CONFIG), #CONFIG##"::CreateInterface", NAME##_Sig }

bool SDKManager::init(px::IGameData* game_data, const Config& config, const std::initializer_list<std::string>& sig_list)
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

		using CreateInterfaceFn = utils::FuncThunk<void*, const char*, int*>;
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

		m_GameData->PushFiles({ "/GameData/interfaces" });
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
