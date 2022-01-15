#pragma once

#include <px/enumclass.hpp>
#include <px/bitbuf.hpp>
#include <px/interfaces/InterfacesSys.hpp>

_STD_BEGIN;

template<typename>
class function;

_STD_END;

PX_NAMESPACE_BEGIN();

static constexpr const char* Interface_EventManager = "IEventManager";

using EventCallback = std::function<void(bitbuf&)>;
using EventID = uint32_t;

class IEventManager : public IInterface
{
public:
	/// <summary>
	/// Add event listener, returns 0 if the event doesn't exists
	/// </summary>
	virtual EventID AddListener(const char* event_name, const EventCallback& callback) abstract;

	/// <summary>
	/// Remove an existsing event listener by name
	/// </summary>
	virtual void RemoveListener(const char* event_name, EventID id) abstract;

	/// <summary>
	/// Register an event
	/// </summary>
	virtual void AddEvent(const char* event_name) abstract;

	/// <summary>
	/// Delete an existsing event
	/// </summary>
	virtual void RemoveEvent(const char* event_name) abstract;

	/// <summary>
	/// Begin event's callback
	/// </summary>
	/// <returns>true if the event exists and there is atleast 1 listener</returns>
	virtual bool StartEvent(const char* event_name) abstract;

	/// <summary>
	/// End event's callback
	/// </summary>
	virtual void ExecuteEvent(const char* event_name, bitbuf&) abstract;
};

PX_NAMESPACE_END();
