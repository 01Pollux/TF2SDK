#pragma once

#include <stdint.h>
#include <cassert>

#include <px/version.hpp>

#define TF2_NAMESPACE_BEGIN(...)			\
namespace px::tf2##__VA_ARGS__	{

#define TF2_NAMESPACE_END() }

TF2_NAMESPACE_BEGIN();
TF2_NAMESPACE_END();

namespace tf2 = px::tf2;

#ifdef PX_EXPORTED
#define PX_SDK_TF2 __declspec(dllexport)
#else 
#define PX_SDK_TF2
#endif

#define TF2_EXPORT_INTERFACE(CLASS, NAME, KEY)	\
inline CLASS* NAME;								\
static constexpr inline bool NAME##_Sig = false;\
static constexpr const char* NAME##_Key = KEY

#define TF2_EXPORT_INTERFACE_S(CLASS, NAME, KEY)\
inline CLASS* NAME;								\
static constexpr inline bool NAME##_Sig = true;	\
static constexpr const char* NAME##_Key = KEY


namespace px
{
	class IGameData;
}

TF2_NAMESPACE_BEGIN(::interfaces);

static constexpr px::version TF2SDKVersion{ "1.2.0.0" };

class SDKManager
{
public:
	struct Config;

	PX_SDK_TF2 bool
		init(px::IGameData* game_data, const Config& config, const std::initializer_list<std::string>& sig_list = { });

	PX_SDK_TF2 ~SDKManager();

	void set_gamedata(px::IGameData* new_gamedata) noexcept
	{
		m_GameData = new_gamedata;
	}

	static px::IGameData* Get()
	{
		return Manager->m_GameData;
	}

private:
	px::IGameData* m_GameData;
	static inline SDKManager* Manager = nullptr;
};



struct SDKManager::Config
{
	struct 
	{
		/// <summary>
		/// IVEngineClient
		/// </summary>
		bool EngineClient : 1{ };

		/// <summary>
		/// IVEngineServer
		/// </summary>
		bool EngineServer : 1{ };

		/// <summary>
		/// IVModelInfo
		/// </summary>
		bool ModelInfo : 1{ };

		/// <summary>
		/// ICVar and IConCommandBase
		/// </summary>
		bool Convar : 1{ };

		/// <summary>
		/// ISpatialPartition
		/// </summary>
		bool SpatialPartition : 1{ };

		/// <summary>
		/// IEngineTrace
		/// </summary>
		bool ClientTrace : 1{ };

		/// <summary>
		/// IVDebugOverlay
		/// </summary>
		bool DebugOverlay : 1{ };

		/// <summary>
		/// IValveGlobalVars
		/// </summary>
		bool GlobalVars : 1{ };

		/// <summary>
		/// INetworkStringTableContainer
		/// </summary>
		bool NSTContainer : 1{ };

		/// <summary>
		/// VGui::IEngineVGui
		/// </summary>
		bool EngineVGUI : 1{ };

		/// <summary>
		/// IEngineSound
		/// </summary>
		bool ClientSound : 1{ };

		/// <summary>
		/// IVModelRender
		/// </summary>
		bool ModelRenderer : 1{ };

		/// <summary>
		/// IGameEventManager
		/// </summary>
		bool GameEventMgr : 1{ };

		/// <summary>
		/// IStudioRender
		/// </summary>
		bool StudioRender : 1{ };
	} Engine;

	struct
	{
		/// <summary>
		/// IBaseClientDLL
		/// </summary>
		bool ClientDLL : 1{ };

		/// <summary>
		/// IClientEntityList
		/// </summary>
		bool ClientList : 1{ };
		
		/// <summary>
		/// IGameRules
		/// </summary>
		bool GameRules : 1{ };

		/// <summary>
		/// IPrediction
		/// </summary>
		bool Prediction : 1{ };

		/// <summary>
		/// IGameMovement
		/// </summary>
		bool GameMovement : 1{ };

		/// <summary>
		/// IClientTools
		/// </summary>
		bool ClientTools : 1{ };
	} Client;

	struct
	{
		/// <summary>
		/// IBaseServertDLL
		/// </summary>
		bool ServerDLL : 1{ };

		/// <summary>
		/// IServerTools
		/// </summary>
		bool ServerTools : 1{ };
	} Server;

	struct
	{
		/// <summary>
		/// VGui::ISurface
		/// </summary>
		bool Surface : 1{ };

		/// <summary>
		/// VGui::IPanel
		/// </summary>
		bool Panel : 1{ };

		/// <summary>
		/// VGui::IInput
		/// </summary>
		bool InputSys : 1{ };
	} VGui;

	struct
	{
		/// <summary>
		/// IMaterialSystem
		/// </summary>
		bool MatSys : 1{ };

		/// <summary>
		/// IMaterialSystemConfig
		/// </summary>
		bool MatSysCfg : 1{ };
	} Material;

	struct
	{
		/// <summary>
		/// IVPhysics
		/// </summary>
		bool VPhysics : 1{ };

		/// <summary>
		/// IPhysicsCollision
		/// </summary>
		bool PhysicsCollision : 1{ };

		/// <summary>
		/// IPhysicsSurfaceProps
		/// </summary>
		bool PhysicsSurfProp : 1{ };
	} Physics;
};



TF2_NAMESPACE_END();
