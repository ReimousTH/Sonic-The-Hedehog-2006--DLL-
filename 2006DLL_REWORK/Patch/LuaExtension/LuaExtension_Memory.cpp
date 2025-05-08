#include "LuaExtension_Memory.h"
#include <algorithm>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define TRIPLE_QUOTES(...) #__VA_ARGS__


namespace DebugLogV2{

	


	


	static const struct luaL_reg MEM [] = {
		{"ADD",Memory_ADD},
		{"GetPointer", Memory_GetPointer},
		{"SetPointerValue", Memory_SetPointerValue},
		{"SetBYTE", Memory_SetBYTE},
		{"GetBYTE", Memory_GetBYTE},
		{"SetDWORD", Memory_SetDWORD},
		{"GetDWORD", Memory_GetDWORD},
		{"SetFLOAT", Memory_SetFLOAT},
		{"GetFLOAT", Memory_GetFLOAT},
		{NULL, NULL}  /* sentinel */
	};



	const char* memoryScript = "\
							   Memory = { ptr = nil } \n\
							   \n\
							   function Memory:new(o) \n\
							   o = o or {} \n\
							   setmetatable(o, self) \n\
							   self.__index = self \n\
							   return o \n\
							   end \n\
							   \n\
							   function Memory:getPtr() \n\
							   return self.ptr \n\
							   end \n\
							   ";

	int MemoryLIB_GlobalInstall(lua_State* L)
	{
		luaL_openlib06(L,"memory",MEM,0);




		luaL_newmetatable06(L, "MemoryMeta");
		lua_pushstring06(L,"__index"); lua_pushvalue(L,-2); lua_settable06(L,-3); // __index = MemoryMeta
		lua_pushstring06(L,"ptr"); lua_pushlightuserdata(L,(void*)0); lua_settable06(L,-3); // __index = MemoryMeta


		lua_pushstring06(L, "__eq"); lua_pushcfunction06(L, Memory__eq); 	lua_settable06(L, -3);
		lua_pushstring06(L, "__neq"); lua_pushcfunction06(L, Memory__neq); 	lua_settable06(L, -3);
		lua_pushstring06(L, "__add"); lua_pushcfunction06(L, Memory__add); 	lua_settable06(L, -3);

		lua_pushstring06(L, "Move"); lua_pushcfunction06(L, Memory__Move); 	lua_settable06(L, -3);
		lua_pushstring06(L, "FMove"); lua_pushcfunction06(L, Memory__FMove); 	lua_settable06(L, -3);

		//later
		lua_pushstring06(L, "AND"); lua_pushcfunction06(L, Memory__AND); 	lua_settable06(L, -3);
		lua_pushstring06(L, "OR"); lua_pushcfunction06(L, Memory__OR); 	lua_settable06(L, -3);
		lua_pushstring06(L, "SBR"); lua_pushcfunction06(L, Memory__SBR); 	lua_settable06(L, -3);
		lua_pushstring06(L, "SBL"); lua_pushcfunction06(L, Memory__SBL); 	lua_settable06(L, -3);


		lua_pushstring06(L, "GetFLOAT"); lua_pushcfunction06(L, Memory__GetFLOAT); 	lua_settable06(L, -3);
		lua_pushstring06(L, "GetBYTE"); lua_pushcfunction06(L, Memory__GetBYTE); 	lua_settable06(L, -3);
		lua_pushstring06(L, "GetPointer"); lua_pushcfunction06(L, Memory__GetPointer); 	lua_settable06(L, -3);
		lua_pushstring06(L, "GetDWORD"); lua_pushcfunction06(L, Memory__GetDWORD); 	lua_settable06(L, -3);
		lua_pushstring06(L, "GetVector"); lua_pushcfunction06(L, Memory__GetVector); 	lua_settable06(L, -3);
		lua_pushstring06(L, "GetMatrix"); lua_pushcfunction06(L, Memory__GetXMMATRIX); 	lua_settable06(L, -3);

		lua_pushstring06(L, "GetString"); lua_pushcfunction06(L, Memory__GetSTRING); 	lua_settable06(L, -3);
		lua_pushstring06(L, "GetStringPTR"); lua_pushcfunction06(L, Memory__GetSTRINGPTR); 	lua_settable06(L, -3);

		lua_pushstring06(L, "SetDWORD"); lua_pushcfunction06(L, Memory__SetDWORD); 	lua_settable06(L, -3);
		lua_pushstring06(L, "SetFLOAT"); lua_pushcfunction06(L, Memory__SetFLOAT); 	lua_settable06(L, -3);
		lua_pushstring06(L, "SetBYTE"); lua_pushcfunction06(L, Memory__SetBYTE); 	lua_settable06(L, -3);
		lua_pushstring06(L, "SetPointer"); lua_pushcfunction06(L, Memory__SetPointer); 	lua_settable06(L, -3);
		lua_pushstring06(L, "SetVector"); lua_pushcfunction06(L, Memory__SetVector); 	lua_settable06(L, -3);
		lua_pushstring06(L, "SetMatrix"); lua_pushcfunction06(L, Memory__SetXMMATRIX); 	lua_settable06(L, -3);
		lua_pushstring06(L, "SetString"); lua_pushcfunction06(L, Memory__SetString); 	lua_settable06(L, -3);
		lua_pushstring06(L, "IsValidPTR"); lua_pushcfunction06(L, Memory__IsValidPTR); 	lua_settable06(L, -3);

		lua_pushstring06(L, "CallFunc"); lua_pushcfunction06(L, Memory_CallFunc); 	lua_settable06(L, -3);
		lua_pushstring06(L, "CallFuncF"); lua_pushcfunction06(L, Memory_CallFuncF); 	lua_settable06(L, -3);
		lua_pushstring06(L, "AsInt"); lua_pushcfunction06(L, Memory_AsInt); 	lua_settable06(L, -3);
		lua_pushstring06(L, "AsFloat"); lua_pushcfunction06(L, Memory_AsFloat); 	lua_settable06(L, -3);

		lua_pushstring06(L, "QueryProtect"); lua_pushcfunction06(L, Memory_QueryProtect); 	lua_settable06(L, -3);

		lua_pushstring06(L, "GetPTR"); lua_pushcfunction06(L, Memory__GetPTR); 	lua_settable06(L, -3);
		lua_pushstring06(L, "GetRTTI"); lua_pushcfunction06(L, Memory__GetRTTI); 	lua_settable06(L, -3);
		lua_pushstring06(L, "HOOK"); lua_pushcfunction06(L, Memory_HOOK); 	lua_settable06(L, -3);
		


		lua_pop(L,1);

		lua_register06(L,"Memory",Memory__NEW);


		luaL_newmetatable06(L, "HOOKCMeta");
		lua_pushstring06(L,"__index"); lua_pushvalue(L,-2); lua_settable06(L,-3); // __index = MemoryMeta
		lua_pushstring06(L,"lua_function"); lua_pushlightuserdata(L,(void*)0); lua_settable06(L,-3); // __index = MemoryMeta
		lua_pushstring06(L,"__gc"); lua_pushcfunction06(L,Memory_HOOK_OBJECT_GC); lua_settable06(L,-3); // __index = MemoryMeta
		lua_pop(L,1);


		lua_dostring06(L,TRIPLE_QUOTES(
			RTTI_META = {
				signature = 0,
				offset = 0,
				cdOffset = 0,
				name = "Empty"
			}
			function tostringu(str)
				return string.gsub(tostring(str),"userdata:","")
			end
			function RTTI_META:__tostring()
				return string.format("RTTI OBJECT : [sig: %s ,offs:%s,cdoffs:%s, %s]", tostringu(self.signature), tostringu(self.offset), tostringu(self.cdOffset), self.name)
			end
			function RTTI(signature,offset,cdoffset,name)
				local self = setmetatable({}, RTTI_META)
				self.signature = signature
				self.offset = offset
				self.cdOffset = cdOffset
				self.name = name
				return self
			end

		));

	





		return 0;
	}

	extern "C" Memory__CreateMetatableFields(lua_State* L,int value,int move){

		lua_pushstring06(L,"ptr");
		lua_pushlightuserdata(L,(void*)value);
		lua_settable06(L,-3);


		// required for save check
		lua_pushstring06(L,"move");
		lua_pushlightuserdata(L,(void*)move);
		lua_settable06(L,-3);
		return 0;
	}
	extern "C" Memory__CreateMetatable(lua_State* L,int value,int move){

		lua_newtable06(L); // Create a new table for the instance // local self = {}

		// Second table for metatable set
	//	lua_newtable06(L);
	//	lua_pushstring06(L, "__index");luaL_getmetatable06(L, "MemoryMeta");lua_settable06(L, -3); // { __index = MemoryMeta }
	//	lua_setmetatable06(L, -2); // setmetatable(self, { __index = MemoryMeta })

		luaL_getmetatable06(L, "MemoryMeta");
		lua_setmetatable06(L, -2); // setmetatable(self, { __index = MemoryMeta })


	    lua_pushstring06(L, "ptr");lua_pushlightuserdata(L, (void*)value);lua_settable06(L, -3); // self.ptr = ptr
		lua_pushstring06(L, "move");lua_pushlightuserdata(L, (void*)move);lua_settable06(L, -3); // self.move = move

	

		return 1; // Return the new table

	}
	extern "C" Memory__NEW(lua_State* L){

		int value = 0;
		if (lua_isuserdata(L,1)){
			value = (int)lua_touserdata(L,1);
		}
		else if (lua_isnumber(L,1)){
			value = lua_tonumber(L,1);
		}
		else if (lua_isstring(L,1)){

			const char* s = lua_tostring(L,1);
			const TCHAR* hexString = _T(s); // Hex string to convert
			value = _tcstoul(hexString, NULL, 16); // Convert hex string to unsigned long integer
		}
		else{
			value = lua_tonumber(L,1);
		}


		Memory__CreateMetatable(L,value,0);



		return 1;

	}


	extern "C" int Memory__FMove(lua_State* L)
	{
		lua_pushstring(L,"ptr");
		lua_gettable(L,1);
		int ptr =  (int)lua_touserdata(L,-1);

		lua_pushstring(L,"move");
		lua_gettable(L,1);
		int move =  (int)lua_touserdata(L,-1);




		if (lua_isuserdata(L,2)){
			move += (int)lua_touserdata(L,2);
		}
		else if (lua_isnumber(L,2)){
			move += lua_tonumber(L,2);
		}
		else if (lua_isstring(L,2)){

			const char* s = lua_tostring(L,2);
			const TCHAR* hexString = _T(s); // Hex string to convert
			move += _tcstoul(hexString, NULL, 16); // Convert hex string to unsigned long integer
		}
		else{
			move += lua_tonumber(L,2);
		}


		Memory__CreateMetatable(L,ptr+ move,0);



		return 1;
	}

	extern "C" Memory__eq(lua_State* L){

		lua_pushstring06(L,"ptr");lua_gettable(L,1);unsigned int V1 = (unsigned int)lua_touserdata(L,-1);
		lua_pushstring06(L,"ptr");lua_gettable(L,2);unsigned int V2 = (unsigned int)lua_touserdata(L,-1);



		lua_pushboolean(L,V1==V2);
		return 1;
	

	}

	extern "C" Memory__neq(lua_State* L)
	{
		lua_pushstring06(L,"ptr");lua_gettable(L,1);unsigned int V1 = (unsigned int)lua_touserdata(L,-1);
		lua_pushstring06(L,"ptr");lua_gettable(L,2);unsigned int V2 = (unsigned int)lua_touserdata(L,-1);



		
		lua_pushboolean(L,V1 != V2);
		return 1;
	}

	extern "C" int GetPointerOrHeximalArgument(lua_State* L,int arg){


		int emove = 0;
		if (lua_isuserdata(L,arg)){
			emove = (int)lua_touserdata(L,arg);
			//add MemoryMeta :)
		}
		else if (lua_isnumber(L,arg)){
			emove = lua_tonumber(L,arg);
		}
		else if (lua_isstring(L,arg)){

			const char* s = lua_tostring(L,arg);
			const TCHAR* hexString = _T(s); // Hex string to convert
			emove = _tcstoul(hexString, NULL, 16); // Convert hex string to unsigned long integer
		}
		else{
			emove = lua_tonumber(L,2);
		}

		return emove;


	}


	extern "C" int Memory__OR(lua_State* L)
	{
		lua_pushstring(L,"ptr");
		lua_gettable(L,1);
		int ptr =  (int)lua_touserdata(L,-1);

		lua_pushstring(L,"move");
		lua_gettable(L,1);
		int move =  (int)lua_touserdata(L,-1);
		int emove =  (int)lua_touserdata(L,-1);



		if (lua_isuserdata(L,2)){
			emove = (int)lua_touserdata(L,2);
		}
		else if (lua_isnumber(L,2)){
			emove = lua_tonumber(L,2);
		}
		else if (lua_isstring(L,2)){

			const char* s = lua_tostring(L,2);
			const TCHAR* hexString = _T(s); // Hex string to convert
			emove = _tcstoul(hexString, NULL, 16); // Convert hex string to unsigned long integer
		}
		else{
			emove = lua_tonumber(L,2);
		}


		Memory__CreateMetatable(L,ptr | emove,move);



		return 1;
	}

	extern "C" int Memory__AND(lua_State* L)
	{
		lua_pushstring(L,"ptr");lua_gettable(L,1);
		int ptr =  (int)lua_touserdata(L,-1);

		lua_pushstring(L,"move");lua_gettable(L,1);
		int move =  (int)lua_touserdata(L,-1);
		int emove =  (int)GetPointerOrHeximalArgument(L,2);

		Memory__CreateMetatable(L,ptr & emove,move);
		return 1;
	}

	extern "C" int Memory__add(lua_State* L)
	{

		lua_pushstring(L,"ptr");lua_gettable(L,1);unsigned int ptr1 = (unsigned int)lua_touserdata(L,-1);
		lua_pushstring(L,"move");lua_gettable(L,1);unsigned int move1 = (unsigned int)lua_touserdata(L,-1);

		lua_pushstring(L,"ptr");lua_gettable(L,2);unsigned int ptr2 = (unsigned int)lua_touserdata(L,-1);
		lua_pushstring(L,"move");lua_gettable(L,2);unsigned int move2 = (unsigned int)lua_touserdata(L,-1);

		Memory__CreateMetatable(L,ptr1+ptr2,move1+move2);


		return 1;
	}

	extern "C" int Memory__SBL(lua_State* L)
	{
		lua_pushstring(L,"ptr");lua_gettable(L,1);
		int ptr =  (int)lua_touserdata(L,-1);

		lua_pushstring(L,"move");lua_gettable(L,1);
		int move =  (int)lua_touserdata(L,-1);
		int emove =  (int)GetPointerOrHeximalArgument(L,2);

		Memory__CreateMetatable(L,ptr << emove,move);



		return 1;	
	}

	extern "C" int Memory__SBR(lua_State* L)
	{
		lua_pushstring(L,"ptr");lua_gettable(L,1);
		int ptr =  (int)lua_touserdata(L,-1);

		lua_pushstring(L,"move");lua_gettable(L,1);
		int move =  (int)lua_touserdata(L,-1);
		int emove =  (int)GetPointerOrHeximalArgument(L,2);

		Memory__CreateMetatable(L,ptr >> emove,move);
		return 1;
	}

	
	extern "C" Memory__Move(lua_State* L){

		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		int ptr =  (int)lua_touserdata(L,-1);

		lua_pushstring06(L,"move");
		lua_gettable(L,1);
		int move =  (int)lua_touserdata(L,-1);



		if (lua_isuserdata(L,2)){
			move += (int)lua_touserdata(L,2);
		}
		else if (lua_isnumber(L,2)){
			move += lua_tonumber(L,2);
		}
		else if (lua_isstring(L,2)){

			const char* s = lua_tostring(L,2);
			const TCHAR* hexString = _T(s); // Hex string to convert
			move += _tcstoul(hexString, NULL, 16); // Convert hex string to unsigned long integer
		}
		else{
			move += lua_tonumber(L,2);
		}
		

		Memory__CreateMetatable(L,ptr,move);
	


		return 1;


	}

	extern "C" Memory__SET(lua_State* L,int type){

		int args = lua_gettop(L);

		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		int ptr =  (int)lua_touserdata(L,-1);


		lua_pushstring06(L,"move");
		lua_gettable(L,1);
		int move =  (int)lua_touserdata(L,-1);

	

		unsigned int value = 0;
		int arg_value_num = 2;

		if (args > 2){

			if (lua_isuserdata(L,2)){
				move += (int)lua_touserdata(L,2);
			}
			else if (lua_isnumber(L,2)){
				move += lua_tonumber(L,2);
			}
			else if (lua_isstring(L,2)){

				const char* s = lua_tostring(L,2);
				const TCHAR* hexString = _T(s); // Hex string to convert
				move += _tcstoul(hexString, NULL, 16); // Convert hex string to unsigned long integer
			}
			else{
				move += lua_tonumber(L,2);
			}

			arg_value_num = 3;
		}


		const char* string_param;
		if (lua_isuserdata(L,arg_value_num)){
			value = (unsigned int)lua_touserdata(L,arg_value_num);
		}
		//
		else if (lua_isnumber(L,arg_value_num)){
			if (type == 1){
				float hvalue = (float)lua_tonumber(L,arg_value_num);
				value = *(unsigned int*)&hvalue;
			}
			else{
					value = (unsigned int)lua_tonumber(L,arg_value_num);
			}
		}
		else if (lua_isstring(L,arg_value_num)){

			const char* s = lua_tostring(L,2);
			const TCHAR* hexString = _T(s); // Hex string to convert
			value= _tcstoul(hexString, NULL, 16); // Convert hex string to unsigned long integer
			string_param = s;
		}
		
		XMVECTOR* vec;
	
			switch (type){
				//DWORD 
			case  0: 
			case  1:
			case  3:
				*(unsigned int*)(ptr + move) = value;
				break;
			case  2:
				*(char*)(ptr + move) = value;
				break;
			case 5:
				vec = (XMVECTOR*)(ptr + move);
				lua_rawgeti(L,arg_value_num,1); vec->x = lua_tonumber(L,-1); lua_pop(L,1);
				lua_rawgeti(L,arg_value_num,2); vec->y = lua_tonumber(L,-1); lua_pop(L,1);
				lua_rawgeti(L,arg_value_num,3); vec->z = lua_tonumber(L,-1); lua_pop(L,1);
				lua_rawgeti(L,arg_value_num,4); vec->w = lua_tonumber(L,-1); lua_pop(L,1);
				break;
			case 6:
				strcpy((char*)(ptr + move),string_param);
				break;
			case 7:
				{
					XMMATRIX* matrix = (XMMATRIX*)(ptr + move);
					*matrix = XM_GetMatrix(L,arg_value_num);
				}
				break;
			}

	

	
		return 0;
	}
	extern "C" Memory__SetDWORD(lua_State* L){

		return Memory__SET(L,0);

	}

	extern "C" Memory__SetFLOAT(lua_State* L){

		return Memory__SET(L,1);

	}

	extern "C" Memory__SetBYTE(lua_State* L){

		return Memory__SET(L,2);

	}
	extern "C" Memory__SetPointer(lua_State* L){

		return Memory__SET(L,3);

	}

	extern "C" Memory__SetVector(lua_State* L){

		return Memory__SET(L,5);

	}

	extern "C" Memory__SetString(lua_State* L){
		return Memory__SET(L,6);
	}
	extern "C" Memory__SetXMMATRIX(lua_State* L){
		return Memory__SET(L,7);	
	}







	extern int "C" Memory__GET(lua_State* L, int type){

		int args = lua_gettop(L);
		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		int ptr =  (int)lua_touserdata(L,-1);

		lua_pushstring06(L,"move");
		lua_gettable(L,1);
		int move =  (int)lua_touserdata(L,-1);


		if (args > 1) {

			if (lua_isuserdata(L,2)){
				move += (int)lua_touserdata(L,2);
			}
			else if (lua_isnumber(L,2)){
				move += lua_tonumber(L,2);
			}
			else if (lua_isstring(L,2)){

				const char* s = lua_tostring(L,2);
				const TCHAR* hexString = _T(s); // Hex string to convert
				move += _tcstoul(hexString, NULL, 16); // Convert hex string to unsigned long integer
			}
			else{
				move += lua_tonumber(L,2);
			}
		}
	

	

	
			switch (type){
				//DWORD 
			case  0 : 
				lua_pushnumber(L,*(unsigned int*)(ptr + move));
				break;
				//FLOAT
			case  1:
				lua_pushnumber(L,*(float*)(ptr + move));
				break;
				//BYTE
			case  2:
				lua_pushnumber(L,*(char*)(ptr + move));
				break;
				//pointer
			case  3:
				Memory__CreateMetatable(L,*(unsigned int*)(ptr + move),0);
				break;
			case 4:
				{
					
				
					struct RTTIClassHierarchyBase{
						std::type_info* typeDesc;
						size_t bcount;
						DWORD moffset;
						DWORD vftOffset;
						DWORD vftwOffset;
						DWORD attributes;
					};
					struct RTTIClassHierarchy{
						DWORD signature;
						DWORD attributes;
						size_t acount;
						RTTIClassHierarchyBase*** aclass;
					};
					struct RTTICompleteObjectLocator{
						DWORD signature;
						DWORD offset;
						DWORD cdOffset;
						std::type_info* typeDesc;
						RTTIClassHierarchy* hierarchyDesc;
					};
					if (ptr != 0 & *(unsigned int*)(ptr) > 0x82000000){
						int vft = *(unsigned int*)(ptr);
						RTTICompleteObjectLocator * vft_rtti = *(RTTICompleteObjectLocator **)(vft -4);
						const char* return_string = "NULL";
						if (vft && vft_rtti) return_string = vft_rtti->typeDesc->name();
						lua_getglobal06(L,"RTTI");
						lua_pushlightuserdata(L,(void*)vft_rtti->signature);
						lua_pushlightuserdata(L,(void*)vft_rtti->offset);
						lua_pushlightuserdata(L,(void*)vft_rtti->cdOffset);
						lua_pushstring06(L,vft_rtti->typeDesc->name());
						RTTIClassHierarchyBase* t = (*vft_rtti->hierarchyDesc->aclass)[0];
						

						lua_pcall06(L,4,1,0);
					}
					else{
						lua_getglobal06(L,"RTTI");
						lua_pushlightuserdata(L,(void*)0);
						lua_pushlightuserdata(L,(void*)0);
						lua_pushlightuserdata(L,(void*)0);
						lua_pushstring06(L,"NO VFT");
						lua_pcall06(L,4,1,0);
					}

			

		
				
				}
				break;
			case 5:
				{
				lua_getglobal06(L,"Vector");
				XMVECTOR* vec = (XMVECTOR*)(ptr + move);
				lua_pushnumber(L,vec->x);
				lua_pushnumber(L,vec->y);
				lua_pushnumber(L,vec->z);
				lua_pushnumber(L,vec->w);
				lua_pcall06(L,4,1,0);
				}
				break;
			case 7:
				{
					XMMATRIX_CREATEMETATABLE(L,*(XMMATRIX*)(ptr + move));
				}
				break;
			case 8:
				{
					lua_pushstring06(L,(const char*)(ptr + move));
				}
				break;
			case 9:
				{
					lua_pushstring06(L,*(const char**)(ptr + move));
				}
				break;
			}
	


		return 1;
	}


	extern "C" int Memory__GetDWORD(lua_State* L){

		return Memory__GET(L,0);

	}

	extern "C" int Memory__GetSTRINGPTR(lua_State* L)
	{
		return Memory__GET(L,9);
	}

	extern "C" int Memory__GetSTRING(lua_State* L)
	{
		return Memory__GET(L,8);
	}

	extern "C" int Memory__GetFLOAT(lua_State* L){

		return Memory__GET(L,1);

	}
	extern "C" int Memory__GetBYTE(lua_State* L){

		return Memory__GET(L,2);

	}
	extern "C" int Memory__GetPointer(lua_State* L){

		return Memory__GET(L,3);

	}


	extern "C" int Memory__GetVector(lua_State* L)
	{
		return Memory__GET(L,5);
	}


	extern "C" int Memory__GetXMMATRIX(lua_State* L)
	{
		return Memory__GET(L,7);
	}


	extern "C" int Memory__GetRTTI(lua_State* L)
	{
		return Memory__GET(L,4);
	}









	
	
	extern "C" int Memory__GetPTR(lua_State* L)
	{

		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		return 1;
	}

	
	extern "C" int Memory__IsValidPTR(lua_State* L)
	{

		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		unsigned int ptr =  (unsigned int)lua_touserdata(L,-1);


		//Save Check
		if (ptr > 0x40000000){

			lua_pushboolean(L,true);
		}
		else{
			lua_pushboolean(L,false);
		}



		return 1;

	}

	extern "C" Memory_GetBYTE(lua_State* L){

		int args = lua_gettop(L);
		UINT ptr = 0;
		UINT mv = 0; 

		if (args < 1){ lua_pushnil(L); return 1;}

		if (lua_isuserdata(L,1)){
			ptr = (UINT)lua_touserdata(L,1);
		}

		else{
			ptr = (UINT)lua_tonumber(L,1);
		}

		if (args > 1){
			mv = (UINT)lua_tonumber(L,2);
		}

	
		if ((ptr + mv) > mv && (ptr + mv) != 0 ){
			lua_pushnumber(L,*(char*)(ptr + mv));
		}
		else{
			lua_pushnil(L);
		}
		return 1;
	}
	extern "C" Memory_SetBYTE(lua_State* L){

		int args = lua_gettop(L);
		UINT ptr = 0;
		if (args < 2) return 0;

		if (lua_isuserdata(L,1)){
			ptr = (UINT)lua_touserdata(L,1);
		}
		else{
			ptr = lua_tonumber(L,1);
		}

		*(char*)(ptr) =  lua_tonumber(L,2);

		return 0;
	}


	extern "C" Memory_QueryProtect(lua_State* L)
	{
		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		void* ptr =  (void*)lua_touserdata(L,-1);
		MEMORY_BASIC_INFORMATION data;
		VirtualQuery((void*)ptr, &data,0);
		lua_pushboolean(L,data.Protect);
		return 1;
	}

	void THREAD_REGISTER_CHANGE(int r3_value) {
		__asm{
			mr r3,r3_value
		}
	}

	extern "C" Memory_CallFuncF(lua_State* L){
		int args = lua_gettop(L);
		unsigned long long registers_values[9];
		double registers_values_float[9];
		char* registers_values_string[9]; //only to free 
		unsigned long long registers_values_or[31];

		int arg_count = 0;

		for (int i = 2; i <= args; i++) {

			if (lua_isnumber(L, i)) {
				registers_values[arg_count] = (int)lua_tonumber(L, i);
				registers_values_float[arg_count] = lua_tonumber(L, i); // Use double for float
				arg_count++;
			}

			else if (lua_istable(L,i)){

				lua_getmetatable06(L,i);
				luaL_getmetatable06(L,"Uint64Meta");
				if (lua_rawequal(L,-1,-2)){

					lua_pop(L,1);
					lua_pushstring06(L, "part1");
					lua_gettable(L, i);
					unsigned long value_1 = (unsigned long)lua_touserdata(L, -1);


					lua_pushstring06(L, "part2");
					lua_gettable(L, i);
					unsigned long value_2 = (unsigned long)lua_touserdata(L, -1);


					registers_values[arg_count] = ((unsigned long long)value_1 << 32) | value_2; 
					arg_count++;
				}

				lua_pop(L,1);
			}
			else if (lua_islightuserdata(L, i)) {

				registers_values[arg_count] = (unsigned long long)lua_touserdata(L, i);
				arg_count++;
			}
			else if (lua_isstring(L,i)){
				registers_values[arg_count] = (unsigned long long)lua_tostring(L, i);
				arg_count++;
			}
		}

		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		int ptr =  (int)lua_touserdata(L,-1);

		lua_pushstring06(L,"move");
		lua_gettable(L,1);
		int move =  (int)lua_touserdata(L,-1);

		int func_ptr = ptr + move;



		int return_value = 0;
		float return_value_float = 0.0;


		unsigned long long r3_value = registers_values[0];
		unsigned long long r4_value = registers_values[1];
		unsigned long long r5_value = registers_values[2];
		unsigned long long r6_value = registers_values[3];
		unsigned long long r7_value = registers_values[4];
		unsigned long long r8_value = registers_values[5];
		unsigned long long r9_value = registers_values[6];


		float f1_value = registers_values_float[0];
		float f2_value = registers_values_float[1];
		float f3_value = registers_values_float[2];
		float f4_value = registers_values_float[3];
		float f5_value = registers_values_float[4];
		float f6_value = registers_values_float[5];
		float f7_value = registers_values_float[6];



		__asm{
			mr r3,r3_value
				mr r4,r4_value
				mr r5,r5_value
				mr r6,r6_value
				mr r7,r7_value
				mr r8,r8_value
				mr r9,r9_value

				fmr fp1,f1_value
				fmr fp2,f2_value
				fmr fp3,f3_value
				fmr fp4,f4_value
				fmr fp5,f5_value
				fmr fp6,f6_value
				fmr fp7,f7_value

		}

		__asm{
			    mtctr func_ptr 
				bctrl
				mr return_value,r3
				fmr return_value_float,fp1
		}


		lua_pushnumber(L,return_value_float);
		return 1;

	}
	extern "C" Memory_CallFunc(lua_State* L)
	{
		
		int args = lua_gettop(L);
		unsigned long long registers_values[9];
		double registers_values_float[9];
		char* registers_values_string[9]; //only to free 
		unsigned long long registers_values_or[31];

		int arg_count = 0;

		for (int i = 2; i <= args; i++) {

			if (lua_isnumber(L, i)) {
				registers_values[arg_count] = (int)lua_tonumber(L, i);
				registers_values_float[arg_count] = lua_tonumber(L, i); // Use double for float
				arg_count++;
			}

			else if (lua_istable(L,i)){

				lua_getmetatable06(L,i);
				luaL_getmetatable06(L,"Uint64Meta");
				if (lua_rawequal(L,-1,-2)){

					lua_pop(L,1);
					lua_pushstring06(L, "part1");
					lua_gettable(L, i);
					unsigned long value_1 = (unsigned long)lua_touserdata(L, -1);


					lua_pushstring06(L, "part2");
					lua_gettable(L, i);
					unsigned long value_2 = (unsigned long)lua_touserdata(L, -1);


					registers_values[arg_count] = ((unsigned long long)value_1 << 32) | value_2; 
					arg_count++;
				}

				lua_pop(L,1);
			}
			else if (lua_islightuserdata(L, i)) {

				registers_values[arg_count] = (unsigned long long)lua_touserdata(L, i);
				arg_count++;
			}
			else if (lua_isstring(L,i)){
				registers_values[arg_count] = (unsigned long long)lua_tostring(L, i);
				arg_count++;
			}
		}

		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		int ptr =  (int)lua_touserdata(L,-1);

		lua_pushstring06(L,"move");
		lua_gettable(L,1);
		int move =  (int)lua_touserdata(L,-1);

		int func_ptr = ptr + move;
	

		
		int return_value = 0;
		float return_value_float = 0.0;
		

		unsigned long long r3_value = registers_values[0];
		unsigned long long r4_value = registers_values[1];
		unsigned long long r5_value = registers_values[2];
		unsigned long long r6_value = registers_values[3];
		unsigned long long r7_value = registers_values[4];
		unsigned long long r8_value = registers_values[5];
		unsigned long long r9_value = registers_values[6];


		float f1_value = registers_values_float[0];
		float f2_value = registers_values_float[1];
		float f3_value = registers_values_float[2];
		float f4_value = registers_values_float[3];
		float f5_value = registers_values_float[4];
		float f6_value = registers_values_float[5];
		float f7_value = registers_values_float[6];



		__asm{
			mr r3,r3_value
			mr r4,r4_value
			mr r5,r5_value
			mr r6,r6_value
			mr r7,r7_value
			mr r8,r8_value
			mr r9,r9_value

			fmr fp1,f1_value
			fmr fp2,f2_value
			fmr fp3,f3_value
			fmr fp4,f4_value
			fmr fp5,f5_value
			fmr fp6,f6_value
			fmr fp7,f7_value

		}

		__asm{
			mtctr func_ptr 
			bctrl
			mr return_value,r3
			fmr return_value_float,fp1
		}


		Memory__CreateMetatable(L,return_value,0);
		return 1;
	}

	extern "C" Memory_AsInt(lua_State* L)
	{
		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		int ptr =  (int)lua_touserdata(L,-1);
		lua_pushnumber(L,ptr);
		return 1;


	}

	extern "C" Memory_AsFloat(lua_State* L)
	{
		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		int ptr =  (int)lua_touserdata(L,-1);
		lua_pushnumber(L,*(float*)&ptr);
		return 1;
	}



	extern "C" Memory_GetDWORD(lua_State* L){

		int args = lua_gettop(L);
		UINT ptr = 0;
		UINT mv = 0; 

		if (args < 1){ lua_pushnil(L); return 1;}

		if (lua_isuserdata(L,1)){
			ptr = (UINT)lua_touserdata(L,1);
		}

		else{
			ptr = (UINT)lua_tonumber(L,1);
		}

		if (args > 1){
			mv = (UINT)lua_tonumber(L,2);
		}

		if ((ptr + mv) > mv && (ptr + mv) != 0 ){
			lua_pushnumber(L,*(int*)(ptr + mv));
		}
		else{
			lua_pushnil(L);
		}
		return 1;
	}
	extern "C" Memory_SetDWORD(lua_State* L){

		int args = lua_gettop(L);
		UINT ptr = 0;
		if (args < 2) return 0;

		if (lua_isuserdata(L,1)){
			ptr = (UINT)lua_touserdata(L,1);
		}
		else{
			ptr = lua_tonumber(L,1);
		}

		*(unsigned int*)(ptr) =  lua_tonumber(L,2);
		
	
		return 0;
	}
	extern "C" Memory_GetFLOAT(lua_State* L){

		int args = lua_gettop(L);
		UINT ptr = 0;
		UINT mv = 0; 

		if (args < 1){ lua_pushnil(L); return 1;}

		if (lua_isuserdata(L,1)){
			ptr = (UINT)lua_touserdata(L,1);
		}

		else{
			ptr = (UINT)lua_tonumber(L,1);
		}

		if (args > 1){
			mv = (UINT)lua_tonumber(L,2);
		}



		void* result_ptr = 0;
		if ((ptr + mv) > mv && (ptr + mv) != 0 ){
			result_ptr = *(void**)(ptr + mv);
		}

		if ((ptr + mv) > mv && (ptr + mv) != 0 ){
			lua_pushnumber(L,*(float*)(ptr + mv));
		}
		else{
			lua_pushnil(L);
		}
		return 1;
	}
	extern "C" Memory_SetFLOAT(lua_State* L){

		int args = lua_gettop(L);
		UINT ptr = 0;
		if (args < 2) return 0;

		if (lua_isuserdata(L,1)){
			ptr = (UINT)lua_touserdata(L,1);
		}
		else{
			ptr = lua_tonumber(L,1);
		}
		*(float*)(ptr) =  lua_tonumber(L,2);
		return 0;
	}




	extern "C" Memory_GetPointer(lua_State* L){

		int args = lua_gettop(L);
		UINT ptr = 0;
		UINT mv = 0; 

		if (args < 1){ lua_pushnil(L); return 1;}

		if (lua_isuserdata(L,1)){
			ptr = (UINT)lua_touserdata(L,1);
		}
	
		else{
			ptr = (UINT)lua_tonumber(L,1);
		}
		
		if (args > 1){
			mv = (UINT)lua_tonumber(L,2);
		}
		


		void* result_ptr = 0;
		if ((ptr + mv) > mv && (ptr + mv) != 0 ){
			result_ptr = *(void**)(ptr + mv);
		}
		
		if ((ptr + mv) > mv && (ptr + mv) != 0 ){
			lua_pushlightuserdata(L,*(void**)(ptr + mv));
		}
		else{
			lua_pushnil(L);
		}
		return 1;
	}
	extern "C" Memory_ADD(lua_State* L){

		int args = lua_gettop(L);
		UINT ptr = 0;
		UINT mv = 0; 

		if (args < 2){ lua_pushnil(L); return 1;}

		if (lua_isuserdata(L,1)){
			ptr = (UINT)lua_touserdata(L,1);
		}

		else{
			ptr = (UINT)lua_tonumber(L,1);
		}

		if (lua_isuserdata(L,2)){
			mv = (UINT)lua_touserdata(L,2);
		}

		else{
			mv = (UINT)lua_tonumber(L,2);
		}

		lua_pushlightuserdata(L,(void*)(ptr+mv));


		return 1;
	}
	extern "C" Memory_SetPointerValue(lua_State* L){



		int args = lua_gettop(L);
		UINT ptr = 0;
		UINT mv = 0; 





		if (args < 2){ lua_pushnil(L); return 1;}

		if (lua_isuserdata(L,1)){
			ptr = (UINT)lua_touserdata(L,1);
		}

		else{
			ptr = (UINT)lua_tonumber(L,1);
		}

		if (lua_isuserdata(L,2)){
			mv = (UINT)lua_touserdata(L,2);
		}

		else{
			mv = (UINT)lua_tonumber(L,2);
		}

		*(UINT*)ptr = mv;


		return 0;
	}

	unsigned int MEM_GetNumber(lua_State* L, int arg) {
		unsigned int return_value = 0;

		if (lua_isnumber(L, arg)) {
			return_value = (unsigned int)lua_tonumber(L, arg); // C
		} else if (lua_isuserdata(L, arg)) {
			return_value = (unsigned int)(size_t)lua_touserdata(L, arg); // Cast to unsigned int
		} else if (lua_isstring(L, arg)) {
			const char* str = lua_tostring(L, arg);
			char* endptr;
			return_value = strtoul(str, &endptr, 16); // Base 16 for hex
			if (*endptr != '\0' && *endptr != ' ') { 
				luaL_error(L, "Invalid hex string: '%s'", str);
				return 0; 
			}
			if (errno == ERANGE)
			{
				luaL_error(L,"Hex string '%s' is out of unsigned integer range.",str);
				return return_value; //Will not reach here
			}
		} else {
			// Handle invalid argument type
			luaL_error(L, "Argument %d must be a number, userdata, or hex string.", arg);
			return return_value;
		}

		return return_value;
	}


	//just shell
	HOOKV3EX(0,void*,LuaHookReturnVoid,(void*,void*,void*,void*,void*,void*,void*,double,double,double,double,double,double,double,double,double),(r3,r4,r5,r6,r7,r8,r9,fp1,fp2,fp3,fp4,fp5,fp6,fp7,fp8,fp9),void* r3,void* r4,void* r5,void* r6,void* r7,void* r8,void* r9,double fp1,double fp2,double fp3,double fp4,double fp5,double fp6,double fp7,double fp8,double fp9){
		return (void*)(0);
	}


	struct HOOK_OBJECT{
		int func_ref;
		void* addres_to;
		HookNew* ref_hook;
	};


	extern "C" int Memory_HOOK_OBJECT_NEW(lua_State* L,void* addres_to,int func_arg,HookNew* ref_hgook)
	{
		HOOK_OBJECT* hook = (HOOK_OBJECT*)lua_newuserdata06(L,4);
		luaL_getmetatable06(L, "HOOKCMeta");
		lua_setmetatable06(L,-2);

		hook->func_ref = func_arg;
		hook->addres_to = addres_to;
		hook->ref_hook = ref_hgook;


		return 1;
	}


	extern "C" Memory_HOOK_OBJECT_GC(lua_State* L)
	{
		
		HOOK_OBJECT* data = (HOOK_OBJECT*)lua_touserdata06(L,1);
		std::vector<HookNew*>* vec =  &(*HookNew::SaveBuffer)[data->addres_to];
		std::vector<HookNew*>::iterator it = std::find(vec->begin(),vec->end(),data->ref_hook);
		(*HookNew::SaveBuffer)[data->addres_to].erase(it);
		

		free(data->ref_hook);
		lua_unref(L,data->func_ref);
	
		return 0;
	}

	extern "C" Memory_HOOK(lua_State* L)
	{

		lua_pushstring06(L,"ptr");
		lua_gettable(L,1);
		void* addr_to =  (void*)lua_touserdata(L,-1);
		int run_pos = lua_tonumber(L,2);
		int regi = lua_tonumber(L,3);
		bool block = lua_toboolean(L,4);
		
		
		lua_pushvalue06(L,5);
		int lua_func = luaL_ref06(L,LUA_REGISTRYINDEX);

		HookNew* hook =  	HookNew::CreateHook(LuaHookReturnVoid,LuaHookReturnVoidMAP,addr_to,1,lua_func,L,run_pos,block,regi);
		Memory_HOOK_OBJECT_NEW(L,addr_to,lua_func,hook); 


	



		return 1;
	}

}