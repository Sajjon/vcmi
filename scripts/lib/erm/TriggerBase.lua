local TriggerBase =
{
	create = function()
		local ret =
		{
			fn = {}
			--y = {}
			--e = {}
			call = function(self)
				for _, fn in ipairs(self.fn) do
					fn()
				end
			end
		}
		return ret
	end
}

return TriggerBase
