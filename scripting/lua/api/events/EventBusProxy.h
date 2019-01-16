/*
 * EventBusProxy.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */

#pragma once

#include <vcmi/events/EventBus.h>

#include "../../LuaWrapper.h"

namespace scripting
{
namespace api
{
namespace events
{

//class SubscriptionProxy : public OpaqueWrapper<::events::EventSubscription, SubscriptionProxy>
//{
//public:
//	using Wrapper = OpaqueWrapper<::events::EventSubscription, SubscriptionProxy>;
//	static const std::vector<typename Wrapper::RegType> REGISTER;
//};

class EventBusProxy : public OpaqueWrapper<::events::EventBus, EventBusProxy>
{
public:
	using Wrapper = OpaqueWrapper<::events::EventBus, EventBusProxy>;
	static const std::vector<typename Wrapper::RegType> REGISTER;

	static int subscribeBefore(lua_State * L, ::events::EventBus * object);
	static int subscribeAfter(lua_State * L, ::events::EventBus * object);
};

}
}
}
