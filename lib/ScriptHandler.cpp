/*
 * ScriptHandler.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"

#include "ScriptHandler.h"

#include <vcmi/Services.h>

#include "CGameInterface.h"
#include "CScriptingModule.h"

#include "VCMIDirs.h"
#include "serializer/JsonDeserializer.h"
#include "serializer/JsonSerializer.h"
#include "filesystem/Filesystem.h"

static const std::vector<std::string> IMPLEMENTS_MAP =
{
	"ANYTHING",
	"BATTLE_EFFECT"
};

namespace scripting
{

ScriptImpl::ScriptImpl(const ScriptHandler * owner_)
	:owner(owner_),
	host(),
	implements(Implements::ANYTHING)
{

}

ScriptImpl::~ScriptImpl() = default;

void ScriptImpl::compile()
{
	code = host->compile(sourcePath, sourceText);

	if(host == owner->erm)
	{
		host = owner->lua;
		sourceText = code;
		code = host->compile(getName(), getSource());
	}
}

std::shared_ptr<Context> ScriptImpl::createContext(const Environment * env) const
{
	return host->createContextFor(this, env);
}

const std::string & ScriptImpl::getName() const
{
	return identifier;
}

const std::string & ScriptImpl::getSource() const
{
	return sourceText;
}

void ScriptImpl::performRegistration(::Services * services) const
{
	switch(implements)
	{
	case Implements::ANYTHING:
		break;
	case Implements::BATTLE_EFFECT:
		host->registerSpellEffect(services->spellEffects(), this);
		break;
	}
}

void ScriptImpl::serializeJson(JsonSerializeFormat & handler)
{
	handler.serializeString("source", sourcePath);
	handler.serializeEnum("implements", implements, Implements::ANYTHING, IMPLEMENTS_MAP);

	if(!handler.saving)
	{
		resolveHost();

		ResourceID sourcePathId("SCRIPTS/"+sourcePath);

		auto rawData = CResourceHandler::get()->load(sourcePathId)->readAll();

		sourceText = std::string((char *)rawData.first.get(), rawData.second);

		compile();
	}
}

void ScriptImpl::serializeJsonState(JsonSerializeFormat & handler)
{
	handler.serializeString("sourcePath", sourcePath);
	handler.serializeString("sourceText", sourceText);
	handler.serializeString("code", code);
	handler.serializeEnum("implements", implements, Implements::ANYTHING, IMPLEMENTS_MAP);

	if(!handler.saving)
	{
		resolveHost();
	}
}

void ScriptImpl::resolveHost()
{
	ResourceID sourcePathId(sourcePath);

	if(sourcePathId.getType() == EResType::ERM)
		host = owner->erm;
	else if(sourcePathId.getType() == EResType::LUA)
		host = owner->lua;
	else
		throw std::runtime_error("Unknown script language in:"+sourcePath);
}

PoolImpl::PoolImpl(const GameCb * gameCb_, const BattleCb * battleCb_, events::EventBus * eventBus_)
	: gameCb(gameCb_),
	battleCb(battleCb_),
	_eventBus(eventBus_)
{

}

std::shared_ptr<Context> PoolImpl::getContext(const Script * script)
{
	auto iter = cache.find(script);

	if(iter == cache.end())
	{
		auto context = script->createContext(this);
		cache[script] = context;

		auto key = script->getName();
		const JsonNode & scriptState = state[key];

		context->run(scriptState);

		return context;
	}
	else
	{
		return iter->second;
	}
}

const Services * PoolImpl::services() const
{
	return VLC;
}

const GameCb * PoolImpl::game() const
{
	return gameCb;
}

const BattleCb * PoolImpl::battle() const
{
	return battleCb;
}

::vstd::CLoggerBase * PoolImpl::logger() const
{
	return logMod;
}

::events::EventBus * PoolImpl::eventBus() const
{
    return _eventBus;
}

void PoolImpl::serializeState(const bool saving, JsonNode & data)
{
	if(saving)
	{
        for(auto & scriptAndContext : cache)
		{
			auto script = scriptAndContext.first;
			auto context = scriptAndContext.second;

			state[script->getName()] = context->saveState();

			data = state;
		}
	}
	else
	{
		state = data;
	}
}

ScriptHandler::ScriptHandler()
{
	boost::filesystem::path filePath = VCMIDirs::get().fullLibraryPath("scripting", "vcmiERM");
	erm = CDynLibHandler::getNewScriptingModule(filePath);

	filePath = VCMIDirs::get().fullLibraryPath("scripting", "vcmiLua");
	lua = CDynLibHandler::getNewScriptingModule(filePath);
}

ScriptHandler::~ScriptHandler() = default;

const Script * ScriptHandler::resolveScript(const std::string & name) const
{
	auto iter = objects.find(name);

	if(iter == objects.end())
	{
		logMod->error("Unknown script id '%s'", name);
		return nullptr;
	}
	else
	{
		return iter->second.get();
	}
}

std::vector<bool> ScriptHandler::getDefaultAllowed() const
{
	return std::vector<bool>();
}

std::vector<JsonNode> ScriptHandler::loadLegacyData(size_t dataSize)
{
	return std::vector<JsonNode>();
}

ScriptPtr ScriptHandler::loadFromJson(const JsonNode & json, const std::string & identifier) const
{
	ScriptPtr ret = std::make_shared<ScriptImpl>(this);

	JsonDeserializer handler(nullptr, json);
	ret->identifier = identifier;
	ret->serializeJson(handler);
	return ret;
}

void ScriptHandler::loadObject(std::string scope, std::string name, const JsonNode & data)
{
	auto object = loadFromJson(data, normalizeIdentifier(scope, "core", name));
	objects[object->identifier] = object;
}

void ScriptHandler::loadObject(std::string scope, std::string name, const JsonNode & data, size_t index)
{
	throw std::runtime_error("No legacy data load allowed for scripts");
}

void ScriptHandler::performRegistration(Services * services) const
{
	for(auto & keyValue : objects)
	{
		auto script = keyValue.second;
		script->performRegistration(services);
	}
}

void ScriptHandler::loadState(const JsonNode & state)
{
	objects.clear();

	const JsonNode & scriptsData = state["scripts"];

	for(auto & keyValue : scriptsData.Struct())
	{
		std::string name = keyValue.first;

		const JsonNode & scriptData = keyValue.second;

		ScriptPtr script = std::make_shared<ScriptImpl>(this);

		JsonDeserializer handler(nullptr, scriptData);
		script->serializeJsonState(handler);
		objects[name] = script;
	}
}

void ScriptHandler::saveState(JsonNode & state)
{
	JsonNode & scriptsData = state["scripts"];

	for(auto & keyValue : objects)
	{
		std::string name = keyValue.first;

		ScriptPtr script = keyValue.second;
		JsonNode scriptData;
		JsonSerializer handler(nullptr, scriptData);
		script->serializeJsonState(handler);

		scriptsData[name] = std::move(scriptData);
	}

}


}