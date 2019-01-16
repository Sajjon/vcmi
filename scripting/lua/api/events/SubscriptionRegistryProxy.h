/*
 * SubscriptionRegistryProxy.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */

#pragma once

#include "../../LuaWrapper.h"

namespace scripting
{
namespace api
{
namespace events
{

template <typename E>
class SubscriptionRegistryProxy
{
public:
	static int subscribeBefore(lua_State * L, typename E::UDataType object)
	{

	}

	static int subscribeAfter(lua_State * L, typename E::UDataType object)
	{

	}
};


}
}
}
