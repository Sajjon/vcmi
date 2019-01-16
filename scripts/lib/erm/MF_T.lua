local TriggerBase = require("core:erm.TriggerBase")

local trigger = TriggerBase.create()

local eventBus = EVENT_BUS;

local beforeApplyDamage = function(...)
	trigger:call()
end

eventBus.subscribeBefore("ApplyDamage", beforeApplyDamage)

return trigger
