#include "LuaExtension_XMMATRIX.h"


namespace DebugLogV2{

	

	#define STRINGIFY(x) #x
	#define TOSTRING(x) STRINGIFY(x)
	#define TRIPLE_QUOTES(...) #__VA_ARGS__


	int XMMATRIX_GlobalInstall(lua_State* L) {
	

		luaL_newmetatable06(L, "XMMATRIX_META");

		// Set __index metamethod
		lua_pushstring06(L, "__index");
		lua_pushcfunction06(L, index_handler);
		lua_settable06(L, -3);

		// Set __newindex metamethod
		lua_pushstring06(L, "__newindex");
		lua_pushcfunction06(L, newindex_handler);
		lua_settable06(L, -3);


		// Set __newindex metamethod
		lua_pushstring06(L, "Scaling");lua_pushcfunction06(L, XMMATRIX_Scaling);lua_settable06(L, -3);
		lua_pushstring06(L, "Transformation");lua_pushcfunction06(L, XMMATRIX_Transformation);lua_settable06(L, -3);

		// Pop the metatable from the stack
		lua_pop(L, 1);


		lua_register06(L, "XMMATRIX",XMMATRIX_NEW);

		return 0;
	}

	extern "C" int XMMATRIX_CREATEMETATABLE(lua_State* L, XMMATRIX mat) {
		// Create a new table to represent the matrix
		lua_newtable(L);

		// Create a table for each row
		for (int i = 0; i < 4; ++i) {
			lua_getglobal06(L,"Vector");
			lua_pcall06(L,0,1,0); // Create a table for the row
			for (int j = 0; j < 4; ++j) {
				lua_pushnumber(L, mat.r[i].v[j]); // Push the value
				lua_rawseti(L, -2, j + 1); // Set the value at index j+1
			}
			lua_rawseti(L, -2, i + 1); // Set the row table at index i+1
		}

		// Set the metatable for the matrix table
		luaL_getmetatable(L, "XMMATRIX_META");
		lua_setmetatable(L, -2);

		return 1; // Return the matrix table
	}

	extern "C" int XMMATRIX_NEW(lua_State* L) {
		XMMATRIX mat = XMMatrixIdentity(); // Default to identity matrix

		// If arguments are provided, populate the matrix
		if (lua_istable(L, 1)) {
			for (int row = 0; row < 4; ++row) {
				lua_rawgeti(L, 1, row + 1); // Get the row table
				if (lua_istable(L, -1)) {
					for (int col = 0; col < 4; ++col) {
						lua_rawgeti(L, -1, col + 1); // Get the column value
						mat.r[row].v[col] = (float)lua_tonumber(L, -1);
						lua_pop(L, 1); // Pop the column value
					}
				}
				lua_pop(L, 1); // Pop the row table
			}
		}

		return XMMATRIX_CREATEMETATABLE(L, mat);
	}



	float XM_GetNumber(lua_State* L, int arg,int arg_count,float def =  1.0) {
		float return_value = def;

		if (arg < arg_count && lua_isnumber(L, arg)) {
			return_value = (float)lua_tonumber(L, arg); 
		}
		return return_value;
	}
	XMVECTOR XM_GetVector(lua_State* L, int arg,int arg_count) {
		XMVECTOR scaling_vectgor = XMVectorSet(0,0,0,1);

		if (lua_istable(L,arg)){
			lua_rawgeti06(L,arg,1); scaling_vectgor.x = lua_tonumber06(L,-1); lua_pop(L,1);
			lua_rawgeti06(L,arg,2); scaling_vectgor.y = lua_tonumber06(L,-1); lua_pop(L,1);
			lua_rawgeti06(L,arg,3); scaling_vectgor.z = lua_tonumber06(L,-1); lua_pop(L,1);
			lua_rawgeti06(L,arg,4); scaling_vectgor.w = lua_tonumber06(L,-1); lua_pop(L,1);
		}
		else if (arg < arg_count){
			scaling_vectgor.x = XM_GetNumber(L,arg,arg_count);
			scaling_vectgor.y = XM_GetNumber(L,arg + 1,arg_count);
			scaling_vectgor.z = XM_GetNumber(L,arg + 2,arg_count);
			scaling_vectgor.w = XM_GetNumber(L,arg + 3,arg_count);
		}

		return scaling_vectgor;

	}
	XMMATRIX XM_GetMatrix(lua_State* L, int index) {
		XMMATRIX matrix;

		for (int row = 0; row < 4; ++row) {
			lua_rawgeti(L, index, row + 1); // Get the row table
			for (int col = 0; col < 4; ++col) {
				lua_rawgeti(L, -1, col + 1); // Get the column value
				matrix.r[row].v[col] = (float)lua_tonumber(L, -1);
				lua_pop(L, 1); // Pop the column value
			}
			lua_pop(L, 1); // Pop the row table
		}

		return matrix;
	}


	extern "C" int XMMATRIX_Scaling(lua_State* L) {

		int arg_count = lua_gettop(L);
		XMVECTOR scaling = XM_GetVector(L, 2, arg_count);
		XMMATRIX matrix = XM_GetMatrix(L, 1);
		XMMATRIX scalingMatrix = XMMatrixScaling(scaling.x, scaling.y, scaling.z);
		XMMATRIX result = XMMatrixMultiply(matrix, scalingMatrix);

		return XMMATRIX_CREATEMETATABLE(L, result);
	}

	extern "C" int XMMATRIX_Transformation(lua_State* L)
	{

		int arg_count = lua_gettop(L);

		XMVECTOR scalingOrigin =  XM_GetVector(L,2,arg_count);
		XMVECTOR rotationOrigin = XM_GetVector(L,3,arg_count);
		XMVECTOR scalingOrientationQuaternion = XM_GetVector(L,4,arg_count);
		XMVECTOR scale = XM_GetVector(L,5,arg_count);
		XMVECTOR Rotation = XM_GetVector(L,6,arg_count);
		XMVECTOR Translation = XM_GetVector(L,7,arg_count);


		XMMATRIX transformationMatrix = XMMatrixTransformation(
			scalingOrigin,                     // Scaling origin
			scalingOrientationQuaternion,      // Scaling orientation quaternion
			scale,                             // Scaling factors
			rotationOrigin,                    // Rotation origin
			Rotation,                // Rotation quaternion
			Translation                           // Translation vector (position)
		);

		return XMMATRIX_CREATEMETATABLE(L,transformationMatrix);
	}

	extern "C" int index_handler(lua_State* L) {
		const char* key = lua_tostring(L, 2);

		// Handle access to the "r" field
		if (strcmp(key, "r") == 0) {
			lua_pushvalue(L, 1); // Push the matrix table
			return 1;
		}

		// Handle access to rows (e.g., "r1", "r2", etc.)
		if (strncmp(key, "r", 1) == 0) {
			int row = atoi(key + 1) - 1;
			if (row >= 0 && row < 4) {
				lua_rawgeti(L, 1, row + 1); // Get the row table
				return 1;
			}
		}

		// Handle other keys (e.g., custom fields)
		lua_getmetatable(L, 1); // Get the metatable
		lua_pushvalue(L, 2); // Push the key
		lua_rawget(L, -2); // Get the value from the metatable
		return 1;
	}

	extern "C" int newindex_handler(lua_State* L) {
		const char* key = lua_tostring(L, 2);
		double value = lua_tonumber(L, 3);

		// Handle assignment to rows (e.g., "r1", "r2", etc.)
		if (strncmp(key, "r", 1) == 0) {
			int row = atoi(key + 1) - 1;
			if (row >= 0 && row < 4) {
				lua_rawgeti(L, 1, row + 1); // Get the row table
				if (lua_istable(L, -1)) {
					for (int col = 0; col < 4; ++col) {
						lua_rawgeti(L, -1, col + 1); // Get the column value
						if (lua_isnumber(L, -1)) {
							lua_pushnumber(L, value); // Push the new value
							lua_rawseti(L, -3, col + 1); // Set the new value
						}
						lua_pop(L, 1); // Pop the column value
					}
				}
				lua_pop(L, 1); // Pop the row table
			}
		}

		// Handle assignment to other keys (e.g., custom fields)
		lua_pushvalue(L, 2); // Push the key
		lua_pushvalue(L, 3); // Push the value
		lua_rawset(L, 1); // Set the value in the matrix table

		return 0;
	}




}