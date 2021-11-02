#pragma once

#include <stdint.h>
#include "User/Version.hpp"

#define TF2_NAMESPACE_BEGIN(...)			\
namespace ShadowGarden::TF2##__VA_ARGS__	{

#define TF2_NAMESPACE_END() }

TF2_NAMESPACE_BEGIN();
TF2_NAMESPACE_END();

namespace SG = ShadowGarden;
namespace TF2 = SG::TF2;


#ifdef SG_EXPORTED
#define SG_SDK_TF2 __declspec(dllexport)
#else 
#define SG_SDK_TF2
#endif


#define TF2_EXPORT_INTERFACE(CLASS, NAME, KEY)	\
inline CLASS* NAME;								\
static constexpr inline bool NAME##_Sig = false;\
static constexpr const char* NAME##_Key = KEY

#define TF2_EXPORT_INTERFACE_S(CLASS, NAME, KEY)\
inline CLASS* NAME;								\
static constexpr inline bool NAME##_Sig = true;	\
static constexpr const char* NAME##_Key = KEY


namespace ShadowGarden
{
	class IGameData;
}


TF2_NAMESPACE_BEGIN(::Interfaces);


static constexpr SG::Version TF2SDKVersion{ 1, 0, 0, 0 };


class SDKManager
{
public:
	struct Config;

	SG_SDK_TF2
	void init(SG::IGameData* game_data, const Config& config, std::initializer_list<const char*> sig_list);

	SG_SDK_TF2
	~SDKManager();

	void set_gamedata(SG::IGameData* new_gamedata) noexcept
	{
		m_GameData = new_gamedata;
	}

	static SG::IGameData* Get()
	{
		return Manager->m_GameData;
	}

private:
	SG::IGameData* m_GameData;
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
		bool EngineVGUI;

		/// <summary>
		/// IEngineSound
		/// </summary>
		bool ClientSound;
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
		bool Surface;

		/// <summary>
		/// VGui::IPanel
		/// </summary>
		bool Panel;

		/// <summary>
		/// VGui::IInput
		/// </summary>
		bool InputSys;
	} VGui;
};



TF2_NAMESPACE_END();
