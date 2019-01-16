/*
 * EventBusProxy.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"

#include "EventBusProxy.h"

#include "../../LuaStack.h"
#include "../../LuaCallWrapper.h"

namespace scripting
{
namespace api
{
namespace events
{

const std::vector<EventBusProxy::RegType> EventBusProxy::REGISTER =
{
	{
		"subscribeBefore",
		&EventBusProxy::subscribeBefore
	},
	{
		"subscribeAfter",
		&EventBusProxy::subscribeAfter
	}
};

int EventBusProxy::subscribeBefore(lua_State * L, ::events::EventBus * object)
{
	// subscription = subscribeBefore(eventName, functor)
}

int EventBusProxy::subscribeAfter(lua_State * L, ::events::EventBus * object)
{
	//subscription = subscribeAfter(eventName, functor)
}


}
}
}

