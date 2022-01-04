#pragma once

#include <format>
#include <source_location>
#include <nlohmann/json.hpp>
#include <px/interfaces/InterfacesSys.hpp>

PX_NAMESPACE_BEGIN();

static constexpr const char* Interface_ILogger = "ILogger";

class IPlugin;

enum class PlLogType : unsigned char
{
	Msg,
	Dbg,
	Err,
	Ftl
};

struct LoggerInfo
{
	IPlugin*					Plugin;
	PlLogType					LogType;
	const std::source_location	SourceLoc;
	nlohmann::ordered_json		Info;

	LoggerInfo(IPlugin* pl, PlLogType ltype, const std::source_location loc, nlohmann::ordered_json&& msg) noexcept :
		Plugin(pl), LogType(ltype), SourceLoc(loc), Info(std::forward<nlohmann::ordered_json>(msg))
	{ }
};

class ILogger : public IInterface
{
public:
	static constexpr IPlugin* MainPlugin = nullptr;
	virtual void LogMessage(LoggerInfo&& linfo) abstract;

	void DbgEx(IPlugin* plugin, std::source_location loc, nlohmann::ordered_json&& info)
	{
		LogMessage({ plugin, PlLogType::Dbg, loc, std::forward<nlohmann::ordered_json>(info) });
	}

	void MsgEx(IPlugin* plugin, std::source_location loc, nlohmann::ordered_json&& info)
	{
		LogMessage({ plugin, PlLogType::Msg, loc, std::forward<nlohmann::ordered_json>(info) });
	}
	void ErrEx(IPlugin* plugin, std::source_location loc, nlohmann::ordered_json&& info)
	{
		LogMessage({ plugin, PlLogType::Err, loc, std::forward<nlohmann::ordered_json>(info) });
	}

	void FtlEx(IPlugin* plugin, std::source_location loc, nlohmann::ordered_json&& info)
	{
		LogMessage({ plugin, PlLogType::Ftl, loc, std::forward<nlohmann::ordered_json>(info) });
	}
};

PX_NAMESPACE_END();


#ifndef PX_NO_LOGGERS
	#define PX_INTERNAL_LOGGER_INST		px::Logger

	#define PX_INTERNAL_LOGGER_PL		px::ThisPlugin

	#define PX_LOGARG(name, ...)		{ name, __VA_ARGS__ }

	#define PX_MESSAGE(msg)				PX_LOGARG("Message", msg)

	#define PX_BEGIN_LOGCTX(...)		do { \
										constexpr std::source_location log_loc(std::source_location::current()); nlohmann::ordered_json log_ctx{ __VA_ARGS__ }

	#define PX_SET_LOGCTX(name, ...)	log_ctx[name] = __VA_ARGS__

	#define PX_GET_LOGCTX				log_ctx

	#define PX_END_LOGCTX(TYPE)			PX_INTERNAL_LOGGER_INST->TYPE##Ex(PX_INTERNAL_LOGGER_PL, log_loc, std::move(PX_GET_LOGCTX)); \
										} while (false)

	#ifdef PX_DEBUG
		#define PX_LOG_DEBUG(...)							\
				PX_BEGIN_LOGCTX(__VA_ARGS__);				\
				PX_END_LOGCTX(Dbg)

	#else
		#define PX_LOG_DEBUG(...)
	#endif

	#define PX_LOG_MESSAGE(...)								\
			PX_BEGIN_LOGCTX(__VA_ARGS__);					\
			PX_END_LOGCTX(Msg)																

	#define PX_LOG_ERROR(...)								\
			PX_BEGIN_LOGCTX(__VA_ARGS__);					\
			PX_END_LOGCTX(Err)																

	#define PX_LOG_FATAL(...)								\
			PX_BEGIN_LOGCTX(__VA_ARGS__);					\
			PX_END_LOGCTX(Ftl)																	

#else
	#define PX_INTERNAL_LOGGER_INST

	#define PX_INTERNAL_LOGGER_PL

	#define PX_LOGARG(name, ...)

	#define PX_MESSAGE(msg)

	#define PX_BEGIN_LOGCTX(data)	

	#define PX_SET_LOGINFO(name, ...) 

	#define PX_LOG_CTX	

	#define PX_END_LOGCTX

	#define PX_LOG_DEBUG(...)

	#define PX_LOG_MESSAGE(...)

	#define PX_LOG_ERROR(...) 

	#define PX_LOG_FATAL(...)
#endif