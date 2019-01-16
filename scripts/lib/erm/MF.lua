local MF = {}

local MF_D = function(x, p1)

end

MF.D = function(self, x, ...)
	local argc = select('#', ...)

	if argc == 1 then
		return MF_D(x, ...)
	end
end

local MF_F = function(x, p1)

end

MF.F = function(self, x, ...)
	local argc = select('#', ...)

	if argc == 1 then
		return MF_F(x, ...)
	end
end

return MF
