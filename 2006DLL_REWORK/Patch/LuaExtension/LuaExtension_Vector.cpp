#include "LuaExtension_Vector.h"

#define VectorLIBName "Vector"
#define VectorLIBMeta "VectorMeta"

namespace DebugLogV2{

	

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define TRIPLE_QUOTES(...) #__VA_ARGS__

	

	const char* VectorRLIBScript = TRIPLE_QUOTES(
		VectorMeta = {
			[1] = 0,
			[2] = 0,
			[3] = 0,
			[4] = 0,
			__index = function(t, key)
			if key == "x" then return t[1]
			elseif key == "y" then return t[2]
			elseif key == "z" then return t[3]
			elseif key == "w" then return t[4]
			else return rawget(t, key)
				end
				end,
				__newindex = function(t, key, value)
				if key == "x" then
					rawset(t, 1, value)
					elseif key == "y" then
					rawset(t, 2, value)
					elseif key == "z" then
					rawset(t, 3, value)
					elseif key == "w" then
					rawset(t, 4, value)
				else
				rawset(t, key, value)
				end
				end
	}
	function Vector(x, y, z, w)
		local self = setmetatable({}, VectorMeta)
		self[1] = x or 0
		self[2] = y or 0
		self[3] = z or 0
		self[4] = w or 0
		return self
		end

		function VectorMeta:__add(b)
		local result = Vector(
		self[1] + b[1],
		self[2] + b[2],
		self[3] + b[3],
		self[4] + b[4]
	)
		return result
		end

		function VectorMeta:__sub(b)
		local result = Vector(
		self[1] - b[1],
		self[2] - b[2],
		self[3] - b[3],
		self[4] - b[4]
	)
		return result
		end

		function VectorMeta:__tostring()
		return string.format("X:%.2f ,Y:%.2f, Z:%.2f, W:%.2f", self[1], self[2], self[3], self[4])
		end

	);

	int VectorRLIB_GlobalInstall(lua_State* L)
	{


		lua_dostring06(L,VectorRLIBScript);
		return 0;
	}






}