// %*++***###*##**##++**+++*++*%%%%%%%+*%+#*+%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#=%%%#**#+#%
// ==----------------------------------------------------------------------=================+
// =------------------------------------::----------------------------------===---==========+
// ---------------------------------:-:--::::-::::-------------------=======================+
// =-------------------------------::::::::-::::-:::----------==============+===+++=========+
// ----------------------------::::::--:---=====----------===========++==++++++++++++++++++++
// ----------------------------:-----:---==++++++====-==========++++++++++++++++++++++++++++*
// -------------------------------------=+++++++=============++++++++++++++++++++++++++++++**
// -------------------------------------=++++*+========++++++++++++++++++++++++++++++++++++**
// ----------------------------:::::::--=+++++=======+++++++++++++++++++++++************++++*
// ---------------------::::::::::::::::-==+++===++++++++++++++++++++++++********###%%%##*++*
// -------:::::::::::::::::::::::::::::::-=====+####**+++++++++++++++++*********#%%%@@@@%%#**
// ------:-:::::::::::::::::::::::::::::::-====*%%%%#*++++++++++++++++++********##%@@@@@%%#**
// ----------::::::::::::::::::::::::::-=--====+#%%%*++++++++++++++++++++*********##%%%%%#***
// -------------=*=-:::::::::::::::::-=++======++***+++++++++++++++++++**************###*****
// -------------=*#=-------======++++*###*+=+=++=++++++++++*+++******************************
// =-----=======+*#*+++++++*****##########+=++++++++++***************************************
// +++++++++++****#################*****#*+=+++++++++****************************************
// ++**+++++++++++++======+++++++++++++****+=+++***################**************************
// *****+=--------::-::::::::::::::::::------=*#%%%%%%%%%%%%%%%%%%%#####*********************
// ******=-----------:::::::::::---:::::::::-=#%%%%@@@@@@@@@@@@@@%%%%###********************#
// ******=---------------:::::::::::-:::::::-*%%%@@@@@@@@@@@@@@@@@%%%%##********************#
// ****#*=-----------------:::::::::::::::::-=*%%@@@@@@@@@@@@@@@@@@%%##*********************#
// ******+===-------------::::::::::::---:::--=*#%%%@@@@@@@@@@@@@%%######**#**************###
// ==++==------------------:::::::::::::-------=+**##%%%@%%%%%%%%##########*****************#
// ==--------------------------::-:::::::::::---=++**##%%%%%%%%%%%##########*************####
// =--------------------------------:---::::--:--==+**###%#%%%%%%%%%%%#####**************####
// ====--------------------------:-------::-------==+++****###########******************#####
// ===--==------------------------------------::---==+++++******************************#####
// ===-------------------------------------:::-:----=+++********************************####%
// =====---------------------------------------------=++++******************************####%
// ======------------------==------------------------==+++***************************######%%
// =========-----===--------==------------------------==++********#*#####**#######*########%%

#include "load.h"
#include <cstdlib>
#include <crtdbg.h>	
#include <new>
#include "windows.h"
#include "tier0_orig.h"
#include <iostream>
#include "cvar.h"
#include <winternl.h>  // For UNICODE_STRING.
#include <fstream>
#include <filesystem>
#include <array>
#include <intrin.h>
#include "memory.h"
#include "filesystem.h"
#include "defs.h"
#include "factory.h"
#include "core.h"
#include "load.h"
#include "patcher.h"
#include "MinHook.h"
#include "TableDestroyer.h"
#include "bitbuf.h"
#include "in6addr.h"
#include "thirdparty/silver-bun/silver-bun.h"
#include <fcntl.h>
#include <io.h>
#include <streambuf>
#include "navmesh.h"
#include <psapi.h>
#include "logging.h"
#include "squirrel.h"
#include "predictionerror.h"
#include "netadr.h"
#include "sendmoveclampfix.h"
#pragma intrinsic(_ReturnAddress)

wchar_t kNtDll[] = L"ntdll.dll";
char kLdrRegisterDllNotification[] = "LdrRegisterDllNotification";
char kLdrUnregisterDllNotification[] = "LdrUnregisterDllNotification";
void* dll_notification_cookie_;

void Status_ConMsg(const char* text, ...)
// clang-format off
{
	char formatted[2048];
	va_list list;

	va_start(list, text);
	vsprintf_s(formatted, text, list);
	va_end(list);

	auto endpos = strlen(formatted);
	if (formatted[endpos - 1] == '\n')
		formatted[endpos - 1] = '\0'; // cut off repeated newline

	Msg("%s\n", formatted);
}
bool recursive2 = false;
signed __int64 __fastcall sub_1804735A0(char* a1, signed __int64 a2, const char* a3, va_list a4)
{
	signed __int64 result; // rax

	if (a2 <= 0)
		return 0i64;
	result = vsnprintf(a1, a2, a3, a4);
	if ((int)result < 0i64 || (int)result >= a2)
	{
		result = a2 - 1;
		a1[a2 - 1] = 0;
	}
	if (!recursive2) {
		recursive2 = true;
		Msg("%s\n", a1);
		recursive2 = false;
	}
	return result;
}
void sub_180473550(char* a1, signed __int64 a2, const char* a3, ...)
{
	int v5; // eax
	va_list ArgList; // [rsp+58h] [rbp+20h] BYREF

	va_start(ArgList, a3);
	if (a2 > 0)
	{
		v5 = vsnprintf(a1, a2, a3, ArgList);
		if (v5 < 0i64 || v5 >= a2)
			a1[a2 - 1] = 0;
	}
	std::cout << a1 << std::endl;

}
__declspec(dllexport) void whatever2() { Error(); };
typedef void (*CUtlBuffer__SetExternalBufferType)(__int64 a1, __int64 a2, __int64 a3, __int64 a4, char a5);
CUtlBuffer__SetExternalBufferType CUtlBuffer__SetExternalBufferOriginal;
char* textbuf = 0;
__int64 pdefsize = 0;
void __fastcall CUtlBuffer__SetExternalBuffer(__int64 a1, __int64 a2, __int64 a3, __int64 a4, char a5)
{
	static uintptr_t engineDS = uintptr_t(GetModuleHandleA("engine_ds.dll"));
	if (uintptr_t(_ReturnAddress()) == (engineDS + 0x169F03)) {
		if (!textbuf)
			textbuf = (char*)malloc(0x50238);
		a2 = (__int64)(textbuf);
		a3 = 0x50238;
	}
	return CUtlBuffer__SetExternalBufferOriginal(a1, a2, a3, a4, a5);
}
__int64 __fastcall sub_160130(__int64* a1, __int64* a2)
{
	__int64 result; // rax

	*a1 = (__int64)textbuf;
	result = pdefsize;
	*a2 = pdefsize;
	return result;
}

typedef char(__fastcall* ParsePDATAType)(__int64 a1, __int64 a2, __int64 a3, const char* a4);
ParsePDATAType ParsePDATAOriginal;

char __fastcall ParsePDEF(__int64 a1, __int64 a2, __int64 a3, const char* a4)
{
	char v10[53264];
	static auto whatthefuck = (__int64 (*)(__int64 a1, int* a2, __int64 a3, int a4, unsigned int a5, unsigned int a6, unsigned int a7))(uintptr_t(GetModuleHandleA("engine.dll")) + 0x42A450);
	a1 = (__int64)(textbuf);
	pdefsize = a2;
	auto ret = ParsePDATAOriginal(a1, a2, a3, a4);
	int whatever = a2;
	memcpy(v10, (void*)a1, sizeof(v10));
	int v4 = whatthefuck(a1, &whatever, (__int64)v10, a2, 1u, 0, 0);
	return ret;
}

long long* __fastcall unkfunc(long long* a1)
{
	*a1 = 0i64;
	a1[1] = 0i64;
	a1[2] = 0i64;
	a1[3] = 0i64;
	return a1;
}

BOOL RemoveItemsFromVTable(uintptr_t vTableAddr, size_t indexStart, size_t itemCount) {
	if (vTableAddr == 0) return FALSE;

	uintptr_t* vTable = reinterpret_cast<uintptr_t*>(vTableAddr);
	size_t vTableSize = 77;

	if (indexStart + itemCount > vTableSize) return FALSE;

	size_t elementsToShift = vTableSize - (indexStart + itemCount);

	DWORD oldProtect;
	if (!VirtualProtect(reinterpret_cast<LPVOID>(vTableAddr), vTableSize * sizeof(uintptr_t), PAGE_EXECUTE_READWRITE, &oldProtect)) {
		std::cerr << "Failed to change memory protection." << std::endl;
		return FALSE;
	}

	for (size_t i = 0; i < elementsToShift; ++i) {
		vTable[indexStart + i] = vTable[indexStart + itemCount + i];
	}

	memset(&vTable[vTableSize - itemCount], 0, itemCount * sizeof(uintptr_t));

	// Restore the original memory protection
	if (!VirtualProtect(reinterpret_cast<LPVOID>(vTableAddr), vTableSize * sizeof(uintptr_t), oldProtect, &oldProtect)) {
		std::cerr << "Failed to restore memory protection." << std::endl;
		return FALSE;
	}

	return TRUE;
}
struct SavedCall {
	__int64 a1;
	std::string a2;
	int a3;
};

std::vector<SavedCall> savedCalls;

bool shouldSave(const char* a2) {
	const char* commandsToSave[] = {
		"InitHUD",
		"FullyConnected",
		"MatchIsOver",
		"ChangeTitanBuildPoint",
		"ChangeCurrentTitanBuildPoint",
		"TutorialStatus",
		"ChangeCurrentTitanOID"
	};
	for (auto& command : commandsToSave) {
		if (strcmp(a2, command) == 0) {
			return true;
		}
	}
	return false;
}
typedef __int64 (*sub_629740Type)(__int64 a1, const char* a2, int a3);
sub_629740Type sub_629740Original;

__int64 __fastcall sub_629740(__int64 a1, const char* a2, int a3) {
	if (shouldSave(a2)) {
		savedCalls.push_back({ a1, a2, a3 });
		return -1;
	}
	else if (strcmp(a2, "RemoteWeaponReload") == 0) {
		for (auto& call : savedCalls) {
			sub_629740Original(call.a1, call.a2.c_str(), call.a3);
		}
		savedCalls.clear(); // Clear after processing
	}
	return sub_629740Original(a1, a2, a3);
}

// TODO(mrsteyk): REMOVE
void* CVEngineServer_PrecacheModel_o = nullptr;
uintptr_t CVEngineServer_PrecacheModel(uintptr_t a1, const char* a2, char a3) {
	auto ret = reinterpret_cast<decltype(&CVEngineServer_PrecacheModel)>(CVEngineServer_PrecacheModel_o)(a1, a2, a3);

	// ты хуесос полнейший, вондерер, где логгер сука нормальный
	// this print is so aids
	//printf("[STK] CVEngineServer_PrecacheModel('%s', %d) = %p\n", a2, +a3, LPVOID(a1));
	//std::cout << "[STK] CVEngineServer_PrecacheModel('" << a2 << "', " << +a3 << ") = " << LPVOID(a1) << std::endl;

	return ret;
}

void __fastcall cl_DumpPrecacheStats(__int64 CClientState, const char* name) {
	/*
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	*/
	auto outhandle = CreateFileW(L"CONOUT$", GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (!name || !name[0]) {
		//std::cout << "Can only dump stats when active in a level" << std::endl;
		const char error[] = "Can only dump stats when active in a level\r\n";
		WriteFile(outhandle, error, sizeof(error) - 1, 0, 0);
		return;
	}

	uintptr_t items = 0;

	if (!strcmp(name, "modelprecache")) {
		items = CClientState + 0x19EF8;
	}
	else if (!strcmp(name, "genericprecache")) {
		items = CClientState + 0x1DEF8;
	}
	else if (!strcmp(name, "decalprecache")) {
		items = CClientState + 0x1FEF8;
	}

	using GetStringTable_t = uintptr_t(__fastcall*)(uintptr_t, const char*);
	static auto GetStringTable = GetStringTable_t(uintptr_t(GetModuleHandleA("engine.dll")) + 0x22B60);
	auto table = GetStringTable(CClientState, name);

	if (!items || !table) {
		//std::cout << "!items || !table" << std::endl;
		const char error[] = "!items || !table\r\n";
		WriteFile(outhandle, error, sizeof(error) - 1, 0, 0);
		return;
	}

	auto count = (*(int64_t(__fastcall**)(uintptr_t))(*(uintptr_t*)table + 24i64))(table);

	//std::cout << std::endl << "Precache table " << name << ":  " << count << " out of UNK slots used" << std::endl;
	char buf[1024];
	auto towrite = sprintf_s(buf, "Precache table %s:  %d out of UNK slots used\n", name, int(count));
	WriteFile(outhandle, buf, towrite, 0, 0);

	for (int i = 0; i < count; i++) {
		auto slot = items + (i * 16);

		auto name = (*(const char* (__fastcall**)(__int64, _QWORD))(*(_QWORD*)table + 72i64))(table, i);

		using CL_GetPrecacheUserData_t = uintptr_t(__fastcall*)(uintptr_t, uintptr_t);
		auto CL_GetPrecacheUserData = CL_GetPrecacheUserData_t(uintptr_t(GetModuleHandleA("engine.dll")) + 0x558C0);

		auto p = CL_GetPrecacheUserData(table, i);

		auto refcount = (*(uint32_t*)slot) >> 3;

		if (!name || !p) {
			continue;
		}

		//Status_ConMsg("%03i:  %s (%s):   ", i, name, "");
		/*
		if (i < 10) {
			std::cout << "00";
		}
		else if (i < 100) {
			std::cout << "0";
		}
		std::cout << i << ":  " << name << " ():    ";
		*/
		towrite = sprintf_s(buf, "%03i:  %s (%s):   ", i, name, "");
		WriteFile(outhandle, buf, towrite, 0, 0);
		if (refcount == 0) {
			//std::cout << "never used" << std::endl;
			const char msg[] = "never used\r\n";
			WriteFile(outhandle, msg, sizeof(msg) - 1, 0, 0);
		}
		else {
			//std::cout << refcount << " refs," << std::endl;
			towrite = sprintf_s(buf, "%i refcounts\r\n", refcount);
			WriteFile(outhandle, buf, towrite, 0, 0);
		}
	}

	FlushFileBuffers(outhandle);
}

struct string_nodebug {
	union {
		char inl[16]{ 0 };
		const char* ptr;
	};
	// ?????
	//uint32_t size = 0;
	//uint32_t capacity = 0;
	size_t size = 0;
	size_t big_size = 0;
};
static_assert(sizeof(string_nodebug) == 4*8, "AAA");
std::array<string_nodebug[2], 100> g_militia_bodies;
// #STR: "%sserver_%s%s", "-sharedservervpk"
typedef __int64 (*sub_136E70Type)(char* pPath);
sub_136E70Type sub_136E70Original;
__int64 __fastcall sub_136E70(char* pPath)
{
	auto ret = sub_136E70Original(pPath);
	reinterpret_cast<__int64(*)()>(uintptr_t(GetModuleHandleA("engine.dll")) + 0x19D730)();
	return ret;	
}
void* SetPreCache_o = nullptr;
__int64 __fastcall SetPreCache(__int64 a1, __int64 a2, char a3) {
	auto ret = reinterpret_cast<decltype(&SetPreCache)>(SetPreCache_o)(a1, a2, a3);

	using sub_1800F5680_t = char(__fastcall*)(const char* a1, __int64 a2, void* a3, void* a4);
	static auto sub_1800F5680 = sub_1800F5680_t(uintptr_t(GetModuleHandleA("engine_r1o.dll")) + 0xF5680);

	static auto array_start = uintptr_t(GetModuleHandleA("engine_r1o.dll")) + 0x2555C00;

	auto idx = (a1 - array_start) / 10064;
	// assert that no mod and no more than 100...
	auto elem = &g_militia_bodies[idx];
	auto militia_exists = sub_1800F5680("bodymodel_militia", a2, &elem[0]->ptr, &elem[0]->big_size);

	if (!*(_QWORD*)(a1 + 488))
		sub_1800F5680("armsmodel_imc", a2, PVOID(a1 + 472), PVOID(a1 + 488));

	auto armsmodel_militia_exists = sub_1800F5680("armsmodel_militia", a2, &elem[1]->ptr, &elem[1]->big_size);

	return ret;
}

void CHL2_Player_Precache(uintptr_t a1, uintptr_t a2) {
	static auto server_mod = uintptr_t(GetModuleHandleA("server.dll"));
	static auto byte_180C318A6 = (uint8_t*)(server_mod + 0xC318A6);

	if (*byte_180C318A6) {

		using sub_1804FE8B0_t = uintptr_t(__fastcall*)(uintptr_t, uintptr_t);
		auto sub_1804FE8B0 = sub_1804FE8B0_t(server_mod + 0x4FE8B0);

		static auto EffectDispatch_ptr = (uintptr_t*)(server_mod + 0xC310C8);
		auto EffectDispatch = *EffectDispatch_ptr;

		sub_1804FE8B0(a1, a2);

		(*(void(__fastcall**)(__int64, __int64, const char*, __int64, _QWORD))(*(_QWORD*)EffectDispatch + 64i64))(
			EffectDispatch,
			1,
			"waterripple",
			0xFFFFFFFFi64,
			0i64);
		(*(void(__fastcall**)(__int64, __int64, const char*, __int64, _QWORD))(*(_QWORD*)EffectDispatch + 64i64))(
			EffectDispatch,
			1,
			"watersplash",
			0xFFFFFFFFi64,
			0i64);

		static auto StaticClassSystem001_ptr = (uintptr_t*)(server_mod + 0xC31000);
		auto StaticClassSystem001 = *StaticClassSystem001_ptr;
		for (size_t i = 0; i < 100; i++) {
			auto v5 = (*(__int64(__fastcall**)(__int64, _QWORD))(*(_QWORD*)StaticClassSystem001 + 24i64))(StaticClassSystem001, i);
			auto elem = g_militia_bodies[i];

			// if (*(_QWORD*)(v5 + 448))
			{
				using PrecacheModel_t = uintptr_t(__fastcall*)(const void*);
				static auto PrecacheModel = PrecacheModel_t(server_mod + 0x3B6A40);

				for (size_t i_ = 0; i_ < 16; i_++) {
					// IMC
					if (*(_QWORD*)(v5 + 448))
					{
						auto v7 = (_BYTE*)(v5 + 432);
						if (*(_QWORD*)(v5 + 456) >= 0x10ui64)
							v7 = *(_BYTE**)v7;
						PrecacheModel(v7);
					}

					// MCOR
					if (elem[0].size)
					{
						const char* v7 = elem[0].inl;
						if (elem[0].big_size >= 0x10)
							v7 = elem[0].ptr;
						PrecacheModel(v7);
					}

					// armsmodel IMC
					if (*(_QWORD*)(v5 + 488))
					{
						auto v8 = (_BYTE*)(v5 + 472);
						if (*(_QWORD*)(v5 + 496) >= 0x10ui64)
							v8 = *(_BYTE**)v8;
						PrecacheModel(v8);
					}

					// armsmodel MCOR
					if (elem[1].size)
					{
						const char* v7 = elem[1].inl;
						if (elem[1].big_size >= 0x10)
							v7 = elem[1].ptr;
						PrecacheModel(v7);
					}
				}

				// cockpitmodel
				if (*(_QWORD*)(v5 + 528))
				{
					auto v9 = (_BYTE*)(v5 + 512);
					if (*(_QWORD*)(v5 + 536) >= 0x10ui64)
						v9 = *(_BYTE**)v9;
					PrecacheModel(v9);
				}
			}
		}
	}
}
bool isProcessingSendTables = false;
typedef char* (__cdecl* COM_StringCopyType)(char* in);
COM_StringCopyType COM_StringCopyOriginal;
char* __cdecl COM_StringCopy(char* in)
{
	if (isProcessingSendTables) {
		std::ofstream file("test.txt", std::ios::app);
		file << in << "\n";
		file.close();
	}
	return COM_StringCopyOriginal(in);
}
typedef char(__fastcall* DataTable_SetupReceiveTableFromSendTableType)(__int64, __int64);
DataTable_SetupReceiveTableFromSendTableType DataTable_SetupReceiveTableFromSendTableOriginal;
char __fastcall DataTable_SetupReceiveTableFromSendTable(__int64 a1, __int64 a2)
{
	isProcessingSendTables = true;
	char ret = DataTable_SetupReceiveTableFromSendTableOriginal(a1, a2);
	isProcessingSendTables = false;
	return ret;
}

bool ReadConnectPacket2015AndWriteConnectPacket2014(bf_read& msg, bf_write& buffer)
{
	char type = msg.ReadByte();
	if (type != 'A')
	{
		return -1;
	}

	//buffer.WriteLong(-1);
	buffer.WriteByte('A');

	int version = msg.ReadLong();
	if (version != 1040)
	{
		return -1;
	}
	buffer.WriteLong(1036); // 2014 version

	buffer.WriteLong(msg.ReadLong()); // hostVersion
	int32_t lastChallenge = msg.ReadLong();
	buffer.WriteLong(lastChallenge); // challengeNr
	buffer.WriteLong(msg.ReadLong()); // unknown 
	buffer.WriteLong(msg.ReadLong()); // unknown1
	msg.ReadLongLong(); // skip platformUserId

	char tempStr[256];
	msg.ReadString(tempStr, sizeof(tempStr));
	buffer.WriteString(tempStr); // name

	msg.ReadString(tempStr, sizeof(tempStr));
	buffer.WriteString(tempStr); // password

	msg.ReadString(tempStr, sizeof(tempStr));
	buffer.WriteString(tempStr); // unknown2

	int unknownCount = msg.ReadByte();
	buffer.WriteByte(unknownCount);
	for (int i = 0; i < unknownCount; i++)
	{
		buffer.WriteLongLong(msg.ReadLongLong()); // unknown3
	}

	msg.ReadString(tempStr, sizeof(tempStr));
	buffer.WriteString(tempStr); // serverFilter

	msg.ReadLong(); // skip playlistVersionNumber
	msg.ReadLong(); // skip persistenceVersionNumber
	msg.ReadLongLong(); // skip persistenceHash

	int numberOfPlayers = msg.ReadByte();
	buffer.WriteByte(numberOfPlayers);
	for (int i = 0; i < numberOfPlayers; i++)
	{
		// Read SplitPlayerConnect from 2015 packet
		int type = msg.ReadUBitLong(6);
		int count = msg.ReadByte();

		// Write SplitPlayerConnect to 2014 packet
		buffer.WriteUBitLong(type, 6);
		buffer.WriteByte(count);

		for (int j = 0; j < count; j++)
		{
			msg.ReadString(tempStr, sizeof(tempStr));
			buffer.WriteString(tempStr); // key

			msg.ReadString(tempStr, sizeof(tempStr));
			buffer.WriteString(tempStr); // value
		}
	}

	buffer.WriteByte(msg.ReadByte()); // lowViolence 

	//if (msg.ReadByte() != 1)
	//{
	//	return false;
	//}
	buffer.WriteByte(1);

	return !buffer.IsOverflowed() ? lastChallenge : -1;
}

typedef char (*ProcessConnectionlessPacketType)(unsigned int* a1, netpacket_s* a2);
ProcessConnectionlessPacketType ProcessConnectionlessPacketOriginal;
double lastReceived = 0.f;
char __fastcall ProcessConnectionlessPacket(unsigned int* a1, netpacket_s* a2)
{
	char buffer[1200] = { 0 };
	bf_write writer(reinterpret_cast<char*>(buffer), sizeof(buffer));

	if (((char*)a2->pData + 4)[0] == 'A' && ReadConnectPacket2015AndWriteConnectPacket2014(a2->message, writer) != -1)
	{
		if (lastReceived == a2->received)
			return false;
		lastReceived = a2->received;
		bf_read converted(reinterpret_cast<char*>(buffer), writer.GetNumBytesWritten());
		a2->message = converted;
		return ProcessConnectionlessPacketOriginal(a1, a2);
	}
	else
	{
		return ProcessConnectionlessPacketOriginal(a1, a2);
	}
}
typedef void (*CAI_NetworkManager__BuildStubType)(__int64 a1);
typedef void (*CAI_NetworkManager__LoadNavMeshType)(__int64 a1, __int64 a2, const char* a3);
CAI_NetworkManager__LoadNavMeshType CAI_NetworkManager__LoadNavMeshOriginal;
void __fastcall CAI_NetworkManager__LoadNavMesh(__int64 a1, __int64 a2, const char* a3)
{
	CAI_NetworkManager__LoadNavMeshOriginal(a1, a2, a3);
	*(int*)((uintptr_t)(GetModuleHandleA("server.dll")) + 0xC317F0) &= ~1;
	((CAI_NetworkManager__BuildStubType)(((uintptr_t)(GetModuleHandleA("server.dll"))) + 0x3664C0))(a1);
	((CAI_NetworkManager__BuildStubType)(((uintptr_t)(GetModuleHandleA("server.dll"))) + 0x3645f0))(a1);
}

std::vector<std::string> netMessages = {
	"Base_CmdKeyValues",
	"CLC_BaselineAck",
	"CLC_ClientInfo",
	"CLC_ClientSayText",
	"CLC_ClientTick",
	"CLC_CmdKeyValues",
	"CLC_DurangoVoiceData",
	"CLC_FileCRCCheck",
	"CLC_ListenEvents",
	"CLC_LoadingProgress",
	"CLC_Move",
	"CLC_PersistenceClientToken",
	"CLC_PersistenceRequestSave",
	"CLC_RespondCvarValue",
	"CLC_SaveReplay",
	"CLC_SplitPlayerConnect",
	"CLC_VoiceData",
	"CNetMessage",
	"NET_SetConVar",
	"NET_SignonState",
	"NET_SplitScreenUser",
	"NET_StringCmd",
	"SVC_BSPDecal",
	"SVC_ClassInfo",
	"SVC_CmdKeyValues",
	"SVC_CreateStringTable",
	"SVC_CrosshairAngle",
	"SVC_DurangoVoiceData",
	"SVC_EntityMessage",
	"SVC_FixAngle",
	"SVC_GameEvent",
	"SVC_GameEventList",
	"SVC_GetCvarValue",
	"SVC_Menu",
	"SVC_PersistenceBaseline",
	"SVC_PersistenceDefFile",
	"SVC_PersistenceNotifySaved",
	"SVC_PersistenceUpdateVar",
	"SVC_PlaylistChange",
	"SVC_Playlists",
	"SVC_Print",
	"SVC_SendTable",
	"SVC_ServerInfo",
	"SVC_ServerTick",
	"SVC_SetPause",
	"SVC_SetTeam",
	"SVC_Snapshot",
	"SVC_Sounds",
	"SVC_SplitScreen",
	"SVC_TempEntities",
	"SVC_UpdateStringTable",
	"SVC_UserMessage",
	"SVC_VoiceData"
};

// Original function pointer type
typedef __int64(*NET_OutOfBandPrintf_t)(int, void*, const char*, ...);
NET_OutOfBandPrintf_t Original_NET_OutOfBandPrintf = NULL;
typedef __int64 (*NET_SendPacketType)(
	__int64 a1,
	int a2,
	void* a3,
	_DWORD* a4,
	int a5,
	__int64 a6,
	char a7,
	int a8,
	char a9,
	__int64 a10,
	char a11);
NET_SendPacketType NET_SendPacketOriginal;
// Detour function
__int64 Detour_NET_OutOfBandPrintf(int sock, void* adr, const char* fmt, ...) {
	if (strcmp(fmt, "%c00000000000000") == 0) {
		static const char** gamemode = reinterpret_cast<const char** >((uintptr_t)(GetModuleHandleA("server.dll")) + 0xB68520);
		static const char* mapname = reinterpret_cast<const char*>((uintptr_t)(GetModuleHandleA("engine_ds.dll")) + 0x1C89A84);
		//MessageBoxA(NULL, *gamemode, mapname, 16);
		char msg[1200] = { 0 };
		bf_write startup(msg, 1200);
		startup.WriteLong(0xFFFFFFFFi64);
		startup.WriteByte(0x4Au);
		startup.WriteString(mapname);
		startup.WriteString(*gamemode);
		return NET_SendPacketOriginal(0, sock, adr, (uint32*)(startup.GetBasePointer()), startup.GetNumBytesWritten(), 0i64, 0, 0, 0, 0i64, 1);
	}

	va_list args;
	va_start(args, fmt);
	__int64 result = Original_NET_OutOfBandPrintf(sock, adr, fmt, args);
	va_end(args);
	return result;
}
typedef __int64(*Host_InitDedicatedType)(__int64 a1, __int64 a2, __int64 a3);
Host_InitDedicatedType Host_InitDedicatedOriginal;
typedef __int64 (*SendTable_CalcDeltaType)(
	__int64 a1,
	__int64 a2,
	__int64 a3,
	__int64 a4,
	int a5,
	__int64 a6,
	int a7,
	int a8,
	int a9);
SendTable_CalcDeltaType SendTable_CalcDeltaOriginal;
typedef __int64 (*CBaseServer__WriteDeltaEntitiesType)(__int64 a1, _DWORD* a2, __int64 a3, __int64 a4, __int64 a5, int a6, unsigned int a7);
CBaseServer__WriteDeltaEntitiesType CBaseServer__WriteDeltaEntitiesOriginal;
__int64 __fastcall CBaseServer__WriteDeltaEntities(__int64 a1, _DWORD* a2, __int64 a3, __int64 a4, __int64 a5, int a6, unsigned int a7)
{
	static uintptr_t engine = uintptr_t(GetModuleHandleA("engine.dll"));
	static uintptr_t engine_ds = uintptr_t(GetModuleHandleA("engine_ds.dll"));
	*(int*)(engine + 0x2966168) = 2; // force active
	memcpy((void*)(engine + 0x2E9253C), (void*)(engine_ds + 0x200D00C), static_cast<size_t>(static_cast<size_t>(0x2E9BA40) - static_cast<size_t>(0x2E9253C))); // prevent overflow
	memcpy((void*)(engine + 0x2965048), (void*)(engine_ds + 0x1C836A8), 32);
	return CBaseServer__WriteDeltaEntitiesOriginal(a1, a2, a3, a4, a5, a6, a7);
}
__int64 SendTable_CalcDelta(
	__int64 a1,
	__int64 a2,
	__int64 a3,
	__int64 a4,
	int a5,
	__int64 a6,
	int a7,
	int a8,
	int a9)
{
	if (a7 < 600)
		a7 = 600;
	if (a8 < 600)
		a8 = 600;
	return SendTable_CalcDeltaOriginal(a1, a2, a3, a4, a5, a6, a7, a8, a9);
}
typedef bool (*SendTable_EncodeType)(
	__int64 a1,
	__int64 a2,
	__int64 a3,
	int a4,
	__int64 pRecipients,
	char a6,
	__int64 a7,
	__int64 a8);
SendTable_EncodeType SendTable_EncodeOriginal;
bool __fastcall SendTable_Encode(
	__int64 a1,
	__int64 a2,
	__int64 a3,
	int a4,
	__int64 pRecipients,
	char a6,
	__int64 a7,
	__int64 a8)
{
	return SendTable_EncodeOriginal(a1, a2, a3, a4, pRecipients, 0, a7, a8);
}
typedef __int64 (*bf_write__WriteUBitLongType)(bf_write* a1, unsigned int a2, signed int a3);
bf_write__WriteUBitLongType bf_write__WriteUBitLongOriginal;
__int64 __fastcall bf_write__WriteUBitLong(bf_write* a1, unsigned int a2, signed int a3)
{
	static uintptr_t engineDS = uintptr_t(GetModuleHandleA("engine_ds.dll"));
	auto ret = bf_write__WriteUBitLongOriginal(a1, a2, a3);
	if (uintptr_t(_ReturnAddress()) == (engineDS+0x51018) || uintptr_t(_ReturnAddress()) == (engineDS + 0x5101D))
		bf_write__WriteUBitLongOriginal(a1, 0, 14);
	return ret;
}
using sub_EA0D0_t = void(__fastcall*)(const char*, ...);

__int64 Host_InitDedicated(__int64 a1, __int64 a2, __int64 a3)
{
	CModule engine("engine.dll", (uintptr_t)LoadLibraryA("engine.dll"));
	CModule engineDS("engine_ds.dll");
	//int i = 1;
	//while (true)
	//{
	//	CMemory engineDSPattern = engineDS.FindPatternSIMD(((uint8_t*)"\xF7\xCC\x58\xCC\xCC\xCC\x00"), "x?x???x", nullptr, i);
	//	if (!engineDSPattern)
	//		break;
	//	CMemory enginePattern = engine.FindPatternSIMD(((uint8_t*)"\xF7\xCC\x58\xCC\xCC\xCC\x00"), "x?x???x", nullptr, i - 1);
	//	std::vector<uint8_t> patchBytes;
	//	for (size_t j = 0; j < 7; ++j)
	//	{
	//		patchBytes.push_back(enginePattern.GetValue<uint8_t>());
	//		enginePattern.OffsetSelf(1);
	//	}
	//	engineDSPattern.Patch(patchBytes);
	//	i++;
	//}
	NET_CreateNetChannelOriginal = NET_CreateNetChannelType(engine.GetModuleBase() + 0x1F1B10);
	MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x13CA10), LPVOID(engine.GetModuleBase() + 0x1EC7B0), NULL); // NET_BufferToBufferCompress
	MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x13DD90), LPVOID(engine.GetModuleBase() + 0x1EDB40), NULL); // NET_BufferToBufferDecompress
	MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x144C60), LPVOID(engine.GetModuleBase() + 0x1F4AC0), NULL); // NET_ClearQueuedPacketsForChannel
	MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x141D60), &NET_CreateNetChannel, NULL); // NET_CreateNetChannel LPVOID(engine.GetModuleBase() + 0x1F1B10)
	MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x13F7A0), LPVOID(engine.GetModuleBase() + 0x1EF550), NULL); // NET_GetUDPPort
	MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x145440), LPVOID(engine.GetModuleBase() + 0x1F5220), NULL); // NET_Init
	MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x13C9D0), LPVOID(engine.GetModuleBase() + 0x1EC770), NULL); // NET_InitPostFork
	MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x13C8D0), LPVOID(engine.GetModuleBase() + 0x1EC660), NULL); // NET_IsDedicated
	MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x13C8C0), LPVOID(engine.GetModuleBase() + 0x1EC650), NULL); // NET_IsMultiplayer
	MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x13F7D0), LPVOID(engine.GetModuleBase() + 0x1EF580), NULL); // NET_ListenSocket
	Original_NET_OutOfBandPrintf = NET_OutOfBandPrintf_t(engine.GetModuleBase() + 0x1F47C0);
	MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x144960), LPVOID(Detour_NET_OutOfBandPrintf), NULL); // NET_OutOfBandPrintf
	MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x142ED0), LPVOID(engine.GetModuleBase() + 0x1F2CF0), NULL); // NET_ProcessSocket
	MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x1458A0), LPVOID(engine.GetModuleBase() + 0x1F5650), NULL); // NET_RemoveNetChannel
	MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x143390), LPVOID(engine.GetModuleBase() + 0x1F31B0), NULL); // NET_RunFrame
	NET_SendPacketOriginal = NET_SendPacketType(engine.GetModuleBase() + 0x1F4130);
	MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x1442D0), LPVOID(engine.GetModuleBase() + 0x1F4130), NULL); // NET_SendPacket
	MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x144D10), LPVOID(engine.GetModuleBase() + 0x1F4B70), NULL); // NET_SendQueuedPackets
	MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x1453D0), LPVOID(engine.GetModuleBase() + 0x1F51B0), NULL); // NET_SetMultiplayer
	MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x1434C0), LPVOID(engine.GetModuleBase() + 0x1F32E0), NULL); // NET_Shutdown
	MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x13FAB0), LPVOID(engine.GetModuleBase() + 0x1EF860), NULL); // NET_SleepUntilMessages
	MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x13C3E0), LPVOID(engine.GetModuleBase() + 0x1EC1B0), NULL); // NET_StringToAdr
	MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x13C100), LPVOID(engine.GetModuleBase() + 0x1EBED0), NULL); // NET_StringToSockaddr
	MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x146D50), LPVOID(engine.GetModuleBase() + 0x1F6B90), NULL); // INetMessage__WriteToBuffer
	MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x13B000), LPVOID(engine.GetModuleBase() + 0x1E9EA0), NULL); // CNetChan__CNetChan__dtor
	MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x017940), LPVOID(engine.GetModuleBase() + 0x028BC0), NULL); // CLC_SplitPlayerConnect__dtor
	MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x12F140), LPVOID(engine.GetModuleBase() + 0x1DC830), NULL); // SendTable_WriteInfos
	MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x71C0), &bf_write__WriteUBitLong, reinterpret_cast<LPVOID*>(&bf_write__WriteUBitLongOriginal)); // bf_write__WriteUBitLong
	MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x497F0), LPVOID(engine.GetModuleBase() + 0xD8420), NULL); // CBaseClient::ConnectionStart
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x62610), LPVOID(engine.GetModuleBase() + 0xF1930), NULL); // SV_PackEntity
	//MH_CreateHook(LPVOID(engine.GetModuleBase() + 0xE3A90), LPVOID(CreateFunction((void*)(engineDS.GetModuleBase() + 0x55340), (void*)(engineDS.GetModuleBase() + 0x1C76250))), NULL); // CFrameSnapshotManager::UsePreviouslySentPacket
	//MH_CreateHook(LPVOID(engine.GetModuleBase() + 0xE3B00), LPVOID(CreateFunction((void*)(engineDS.GetModuleBase() + 0x553B0), (void*)(engineDS.GetModuleBase() + 0x1C76250))), NULL); // CFrameSnapshotManager::GetPreviouslySentPacket
	//MH_CreateHook(LPVOID(engine.GetModuleBase() + 0xE6430), LPVOID(CreateFunction((void*)(engineDS.GetModuleBase() + 0x57D30), (void*)(engineDS.GetModuleBase() + 0x1C76250))), NULL); // CFrameSnapshotManager::CreatePackedEntity
	//MH_CreateHook(LPVOID(engine.GetModuleBase() + 0x161050), LPVOID(engineDS.GetModuleBase() + 0xBA290), NULL); // CBaseEdict::GetChangeAccessor
	//MH_CreateHook(LPVOID(engine.GetModuleBase() + 0x484850), LPVOID(engineDS.GetModuleBase() + 0x322930), NULL); // SV_PackEntity->writeBuf allocator
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x12FE80), LPVOID(engine.GetModuleBase() + 0x1DD560),  NULL); // SendTable_Encode
	//MH_CreateHook(LPVOID(engine.GetModuleBase() + 0xF12B0), LPVOID(engineDS.GetModuleBase() + 0x62100), NULL); // SV_EnsureInstanceBaseline football
	//MH_CreateHook(LPVOID(engine.GetModuleBase() + 0x101190), LPVOID(engineDS.GetModuleBase() + 0x714C0), NULL); // CChangeFrameList::ctor
	//MH_CreateHook(LPVOID(engine.GetModuleBase() + 0x15D5F0), LPVOID(engineDS.GetModuleBase() + 0xB7F50), NULL); // PackedEntity__SetServerAndClientClass
	//MH_CreateHook(LPVOID(engine.GetModuleBase() + 0x15D4C0), LPVOID(engineDS.GetModuleBase() + 0xB7E20), NULL); // PackedEntity__AllocAndCopyPadded
	//MH_CreateHook(LPVOID(engine.GetModuleBase() + 0x15D5C0), LPVOID(engineDS.GetModuleBase() + 0xB7F20), NULL); // PackedEntity__UnkReFunc
	//MH_CreateHook(LPVOID(engine.GetModuleBase() + 0x15DD10), LPVOID(engineDS.GetModuleBase() + 0xB8680), NULL); // PackedEntity::SetRecipients
	//MH_CreateHook(LPVOID(engine.GetModuleBase() + 0x1DDE70), LPVOID(engineDS.GetModuleBase() + 0x130790), NULL); // SendTable_CalcDelta
	//MH_CreateHook(LPVOID(engine.GetModuleBase() + 0x15D950), LPVOID(engineDS.GetModuleBase() + 0xB82B0), NULL); // PackedEntity::CompareRecipients
	//MH_CreateHook(LPVOID(engine.GetModuleBase() + 0xF1260), LPVOID(engineDS.GetModuleBase() + 0x620B0), NULL); // unknown edict change func

	
	//memcpy((void*)(engineDS.GetModuleBase() + 0x550760), (void*)(engine.GetModuleBase() + 0x7CB3F0), (0x550968 - 0x550760));
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x12FE80), &SendTable_Encode, reinterpret_cast<LPVOID*>(&SendTable_EncodeOriginal)); // SendTable_Encode
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x12FE80), LPVOID(engine.GetModuleBase() + 0x1DD560), NULL); // SendTable_Encode
	//MH_CreateHook(LPVOID(engine.GetModuleBase() + 0x1DDE70), LPVOID(engineDS.GetModuleBase() + 0x130790), reinterpret_cast<LPVOID*>(NULL));
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x116A70), LPVOID(engine.GetModuleBase() + 0x1C3C90), NULL); // CSendTablePrecalc::SetupFlatPropertyArray
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x50870), &CBaseServer__WriteDeltaEntities, NULL); // 
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x12F900), LPVOID(engine.GetModuleBase() + 0x01DCFE0), NULL); // SendTable_CullPropsFromProxies
	
	////MH_CreateHook(LPVOID(engine.GetModuleBase() + 0xDDF00), LPVOID(engineDS.GetModuleBase() + 0x4F710), NULL);
	//
	//MH_CreateHook(LPVOID(engine.GetModuleBase() + 0xCCAE0), LPVOID(engineDS.GetModuleBase() + 0x3D160), NULL);
	//MH_CreateHook(LPVOID(engine.GetModuleBase() + 0x161050), LPVOID(engineDS.GetModuleBase() + 0xBA290), NULL); // CBaseEdict::GetChangeAccessor
	//
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x116580), LPVOID(engine.GetModuleBase() + 0x1C37A0), NULL); // ???
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x12F3A0), LPVOID(engine.GetModuleBase() + 0x1DCA80), NULL); // ???
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x123240), LPVOID(engine.GetModuleBase() + 0x1D04E0), NULL); // ???
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x130240), LPVOID(engine.GetModuleBase() + 0x1DD920), NULL); // SendTable_WritePropList
	//
	
	//
	//
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x000000000571B0), LPVOID(CreateFunction((void*)(engine.GetModuleBase() + 0x000000000E58B0), (void*)(engine.GetModuleBase() + 0x7BBEA8))), NULL); // CFrameSnapshotManager::BuildSnapshotList
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x00000000057D30), LPVOID(CreateFunction((void*)(engine.GetModuleBase() + 0x000000000E6430), (void*)(engine.GetModuleBase() + 0x7BBEA8))), NULL); // CFrameSnapshotManager::CreatePackedEntity
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x00000000057D30), LPVOID((void*)(engine.GetModuleBase() + 0x000000000E6430)), NULL);
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x4FB10), LPVOID((void*)(engine.GetModuleBase() + 0xDE310)), NULL);
	//
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x000000000553B0), LPVOID(CreateFunction((void*)(engine.GetModuleBase() + 0x000000000E3B00), (void*)(engine.GetModuleBase() + 0x7BBEA8))), NULL); // CFrameSnapshotManager::GetPreviouslySentPacket
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x000000000584E0), LPVOID(CreateFunction((void*)(engine.GetModuleBase() + 0x000000000E6BE0), (void*)(engine.GetModuleBase() + 0x7BBEA8))), NULL); // CFrameSnapshotManager::ChangeLevel
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x00000000057630), LPVOID(CreateFunction((void*)(engine.GetModuleBase() + 0x000000000E5D30), (void*)(engine.GetModuleBase() + 0x7BBEA8))), NULL); // CFrameSnapshotManager::UnkFunc1
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x000000000586A0), LPVOID(CreateFunction((void*)(engine.GetModuleBase() + 0x000000000E6DA0), (void*)(engine.GetModuleBase() + 0x7BBEA8))), NULL); // CFrameSnapshotManager::TakeTickSnapshot
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x00000000055340), LPVOID(CreateFunction((void*)(engine.GetModuleBase() + 0x000000000E3A90), (void*)(engine.GetModuleBase() + 0x7BBEA8))), NULL); // CFrameSnapshotManager::UsePreviouslySentPacket
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x00000000057100), LPVOID(CreateFunction((void*)(engine.GetModuleBase() + 0x000000000E5800), (void*)(engine.GetModuleBase() + 0x7BBEA8))), NULL); // CFrameSnapshotManager::AddExplicitDelete
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x00000000058530), LPVOID(CreateFunction((void*)(engine.GetModuleBase() + 0x000000000E6C30), (void*)(engine.GetModuleBase() + 0x7BBEA8))), NULL); // CFrameSnapshotManager::CreateEmptySnapshot
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x00000000057A50), LPVOID(CreateFunction((void*)(engine.GetModuleBase() + 0x000000000E6150), (void*)(engine.GetModuleBase() + 0x7BBEA8))), NULL); // CFrameSnapshotManager::RemoveEntityReference
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x000000000552C0), LPVOID(CreateFunction((void*)(engine.GetModuleBase() + 0x000000000E3A10), (void*)(engine.GetModuleBase() + 0x7BBEA8))), NULL); // CFrameSnapshotManager::AddEntityReference
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x000000000553E0), LPVOID(CreateFunction((void*)(engine.GetModuleBase() + 0x000000000E3B30), (void*)(engine.GetModuleBase() + 0x7BBEA8))), NULL); // CFrameSnapshotManager::GetPackedEntity
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x000000000552D0), LPVOID(CreateFunction((void*)(engine.GetModuleBase() + 0x000000000E3A20), (void*)(engine.GetModuleBase() + 0x7BBEA8))), NULL); // CFrameSnapshotManager::ShouldForceRepack
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x00000000058900), LPVOID(CreateFunction((void*)(engine.GetModuleBase() + 0x000000000E7000), (void*)(engine.GetModuleBase() + 0x7BBEA8))), NULL); // CFrameSnapshotManager::dtor
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x00000000058360), LPVOID(CreateFunction((void*)(engine.GetModuleBase() + 0x000000000E6A60), (void*)(engine.GetModuleBase() + 0x7BBEA8))), NULL); // CFrameSnapshotManager::ReleaseReference
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x000000000548A0), LPVOID(CreateFunction((void*)(engine.GetModuleBase() + 0x000000000E30F0), (void*)(engine.GetModuleBase() + 0x7BBEA8))), NULL); // CFrameSnapshotManager::ReleaseReference
	//
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x000000000583C0), LPVOID(engine.GetModuleBase() + 0x000000000E6AC0), NULL); // CFrameSnapshotManager__UnkFunc9NotVcall
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x0000000003C540), LPVOID(engine.GetModuleBase() + 0x000000000CBFD0), NULL); // j_CFrameSnapshotManager__UnkFunc8NotVCall
	
	//MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x130DF0), LPVOID(engine.GetModuleBase() + 0x1DE4D0), NULL); // SendTable_WriteAllDeltaProps
	////MH_CreateHook(LPVOID(engineDS.GetModuleBase() + 0x130DF0), LPVOID(engine.GetModuleBase() + 0x1DE4D0), NULL); // SendTable_WriteAllDeltaProps
	//
	//*(uintptr_t*)(engineDS.GetModuleBase() + 0x5426C0) = *(uintptr_t*)(engine.GetModuleBase() + 0x7BBEA8); // set engineds framesnapshotmanager vtable(?) to engine one
	////*(uintptr_t*)(engine.GetModuleBase() + 0x7BBEA8)*(uintptr_t*)(engineDS.GetModuleBase() + 0x5426C0) =
	//CBaseServer__WriteDeltaEntitiesOriginal = CBaseServer__WriteDeltaEntitiesType(engine.GetModuleBase() + 0xDF020);

	//SendTable_CalcDeltaOriginal = SendTable_CalcDeltaType(engine.GetModuleBase() + 0x1DDE70);
	for (const auto& msg : netMessages) {
		std::string mangledName = ".?AV" + msg + "@@";
		LPVOID dsVtable = engineDS.GetVirtualMethodTable(mangledName.c_str());
		LPVOID vtable = engine.GetVirtualMethodTable(mangledName.c_str());

		if (dsVtable && vtable) {
			for (int i = 0; i < 14; ++i) {
				LPVOID pTarget = static_cast<LPVOID*>(dsVtable)[i];
				LPVOID pDetour = static_cast<LPVOID*>(vtable)[i];
				MH_CreateHook(pTarget, pDetour, NULL);
			}
		}
	}
	MH_EnableHook(MH_ALL_HOOKS);
	reinterpret_cast<char(__fastcall*)(__int64, CreateInterfaceFn)>((uintptr_t)(engine.GetModuleBase())+0x01A04A0)(0, (CreateInterfaceFn)(engineDS.GetModuleBase() + 0xE9000)); // connect nondedi engine
	reinterpret_cast<void(__fastcall*)(int, void*)>((uintptr_t)(engine.GetModuleBase())+0x47F580)(0, 0); // register nondedi engine cvars
	return Host_InitDedicatedOriginal(a1, a2, a3);
}
char* __fastcall sub_311910(char* a1, const char* a2, signed __int64 a3)
{
	char* result; // rax

	result = strncpy(a1, a2, a3);
	if (a3 > 0)
		a1[a3 - 1] = 0;
	return result;
}

char __fastcall CBaseClient__ProcessClientInfo(__int64 a1, __int64 a2)
{
	char v4; // al
	int v5; // eax
	int v6; // eax
	int v7; // eax

	*(_DWORD*)(a1 + 940) = *(_DWORD*)(a2 + 32);
	v4 = *(_BYTE*)(a2 + 44);
	*(_DWORD*)(a1 + 976) = 0;
	*(_BYTE*)(a1 + 928) = v4;
	sub_311910((char*)(a1 + 648), (const char*)(a2 + 52), 256i64);
	*(_QWORD*)(a1 + 944) = *(unsigned int*)(a2 + 308);
	v5 = *(_DWORD*)(a2 + 312);
	*(_DWORD*)(a1 + 956) = 0;
	*(_DWORD*)(a1 + 952) = v5;
	v6 = *(_DWORD*)(a2 + 316);
	*(_DWORD*)(a1 + 964) = 0;
	*(_DWORD*)(a1 + 960) = v6;
	v7 = *(_DWORD*)(a2 + 320);
	*(_DWORD*)(a1 + 972) = 0;
	*(_DWORD*)(a1 + 968) = v7;
	if (*(_DWORD*)(a2 + 40) != (*(unsigned int(__fastcall**)(_QWORD))(**(_QWORD**)(a1 + 920) + 120i64))(*(_QWORD*)(a1 + 920)))
		(*(void(__fastcall**)(__int64))(*(_QWORD*)(a1 - 8) + 96i64))(a1 - 8);
	return 1;
}
typedef void (*COM_InitType)();
COM_InitType COM_InitOriginal;
typedef void (*SaveRestore_InitType)(__int64);
SaveRestore_InitType SaveRestore_Init;
void* pdataempty = 0;

// Function pointer types
using sub_1601A0_t = __int64(__fastcall*)(__int64);
using sub_14EF30_t = void(__fastcall*)(__int64, __int64, DWORD);
using sub_45420_t = void(__fastcall*)(__int64, int);
using sub_4AAD0_t = void(__fastcall*)(__int64);
using sub_16B0F0_t = char(__fastcall*)(__int64);

char __fastcall sub_4C460(__int64 a1)
{
	char result; // al
	__int64 v3; // rax
	const char* v4; // rax
	__int64 v5; // r9
	char v6; // [rsp+20h] [rbp-3048h]
	char v7[34872]; // [rsp+30h] [rbp-3038h] BYREF
	if (!pdataempty) {
		pdataempty = calloc(34872, 1);
	}
	// Set up function pointers
	static auto engine_mod = uintptr_t(GetModuleHandleA("engine_ds.dll"));
	static auto enginenotdedi_mod = uintptr_t(GetModuleHandleA("engine.dll"));
	static auto sub_1601A0 = sub_1601A0_t(engine_mod + 0x1601A0);
	static auto sub_14EF30 = sub_14EF30_t(enginenotdedi_mod + 0x1FEDB0);
	static auto sub_EA0D0 = sub_EA0D0_t(engine_mod + 0xEA0D0);
	static auto sub_45420 = sub_45420_t(engine_mod + 0x45420);
	static auto sub_4AAD0 = sub_4AAD0_t(engine_mod + 0x4AAD0);
	static auto qword_2097510 = (QWORD*)(engine_mod + 0x2097510);
	static auto sub_16B0F0 = sub_16B0F0_t(engine_mod + 0x16B0F0);
	sub_16B0F0(a1);
	if (true)
	{
		v3 = (__int64)pdataempty;
		sub_14EF30((__int64)v7, v3, 0);
		v4 = (const char*)(*(__int64(__fastcall**)(__int64))(*(_QWORD*)(a1 + 8) + 136i64))(a1 + 8);
		sub_EA0D0("Sending persistence baseline to %s\n", v4);
		v5 = 1;
		v6 = 0;
		(*(void(__fastcall**)(__int64, char*, _QWORD, __int64, char))(*(_QWORD*)(a1 + 8) + 224i64))(
			a1 + 8,
			v7,
			0i64,
			v5,
			v6);
		sub_45420(a1, 6);
		sub_4AAD0(a1);
		if (qword_2097510)
			*(_BYTE*)((*(int(__fastcall**)(__int64))(*(_QWORD*)(a1 + 8) + 112i64))(a1 + 8) + qword_2097510 + 4032) = 0;
		return 1;
	}

	return result;
}
__int64 __fastcall sub_14E980(__int64 a1, __int64 a2, __int64 a3, int a4)
{
	static auto ConstructPersistenceCachedDefFileMsg = (__int64 (*)(__int64 a1, int a2))(uintptr_t(GetModuleHandleA("engine.dll")) + 0x1FE890);
	return ConstructPersistenceCachedDefFileMsg(a1, 0);
}
void COM_Init()
{
	COM_InitOriginal();
	// SaveRestore__Init();
	SaveRestore_Init = SaveRestore_InitType(uintptr_t(GetModuleHandleA("engine.dll")) + 0x1410E0);
	SaveRestore_Init(uintptr_t(GetModuleHandleA("engine.dll")) + 0x2EC8590);
}
typedef void (*CL_Retry_fType)();
CL_Retry_fType CL_Retry_fOriginal;
void CL_Retry_f() {
	static uintptr_t engine = uintptr_t(GetModuleHandleA("engine.dll"));
	typedef void (*Cbuf_AddTextType)(int a1, const char* a2, unsigned int a3);
	static Cbuf_AddTextType Cbuf_AddText = (Cbuf_AddTextType)(engine + 0x102D50);
	if (*(int*)(engine + 0x2966168) == 2) {
		Cbuf_AddText(0, "connect localhost\n", 0);
	}
	else {
		return CL_Retry_fOriginal();
	}
}

typedef __int64 (*CBaseServer__FillServerInfoType)(__int64 a1, __int64 a2);
CBaseServer__FillServerInfoType CBaseServer__FillServerInfoOriginal;
__int64 __fastcall CBaseServer__FillServerInfo(__int64 a1, __int64 a2)
{
	static const char* real_gamedir = "r1_dlc1";
	auto ret = CBaseServer__FillServerInfoOriginal(a1, a2);

	*(const char**)(a2 + 72) = real_gamedir;
	return ret;
}
struct physics_performanceparams_t
{
	int maxCollisionsPerObjectPerTimestep;
	int maxCollisionChecksPerTimestep;
	float maxVelocity;
	float maxAngularVelocity;
	float lookAheadTimeObjectsVsWorld;
	float lookAheadTimeObjectsVsObject;
	float minFrictionMass;
	float maxFrictionMass;
};
typedef __int64 (*sub_1800257E0Type)(void* a1, physics_performanceparams_t* a2);
sub_1800257E0Type sub_1800257E0Original;

__int64 __fastcall sub_1800257E0(void* a1, physics_performanceparams_t* a2)
{
	a2->maxCollisionsPerObjectPerTimestep = 0xa;
	a2->maxCollisionChecksPerTimestep = 0x000004b0;

	return sub_1800257E0Original(a1, a2);
}
typedef void (*IVP_Environment__set_delta_PSI_timeType)(void* thisptr, float psi_time);
IVP_Environment__set_delta_PSI_timeType IVP_Environment__set_delta_PSI_timeOriginal;
void __fastcall IVP_Environment__set_delta_PSI_time(void* thisptr, float psi_time)
{
	IVP_Environment__set_delta_PSI_timeOriginal(thisptr, 0.015151516f);
}
typedef void (*sub_180031610Type)(__int64 a1, float a2);
sub_180031610Type sub_180031610Original;
void __fastcall sub_180031610(__int64 a1, float a2) {
	sub_180031610Original(a1, a2);
}
typedef __int64 (*CBaseEntity__VPhysicsInitNormalType)(void* a1, unsigned int a2, unsigned int a3, char a4, __int64 a5);
__int64 __fastcall C_BaseEntity__VPhysicsInitNormal(_QWORD* a1, unsigned int a2, unsigned int a3, char a4, __int64 a5)
{
	static auto CBaseEntity__SetMoveType = reinterpret_cast<void(*)(void* a1, __int64 a2, __int64 a3)>(uintptr_t(GetModuleHandleA("client.dll")) + 0x02F5B30);
	CBaseEntity__SetMoveType(a1, 5, 3);
	return 0;
}
CBaseEntity__VPhysicsInitNormalType CBaseEntity__VPhysicsInitNormalOriginal;
__int64 __fastcall CBaseEntity__VPhysicsInitNormal(void* a1, unsigned int a2, unsigned int a3, char a4, __int64 a5)
{
	static uintptr_t serverbase = uintptr_t(GetModuleHandleA("server.dll"));
	static auto CBaseEntity__SetMoveType = reinterpret_cast<void(*)(void* a1, __int64 a2, __int64 a3)>(uintptr_t(GetModuleHandleA("server.dll")) + 0x3B3200);

	
	if (uintptr_t(_ReturnAddress()) == (serverbase + 0xb63fd)) {
		
		return CBaseEntity__VPhysicsInitNormalOriginal(a1, a2, a3, a4, a5);
	}
	CBaseEntity__SetMoveType(a1, 5, 3);
	//std::cout << " rva: " << std::hex << uintptr_t(_ReturnAddress()) - serverbase << std::endl;

	return 0;
}

typedef void* (*CEntityFactoryDictionary__CreateType)(void* thisptr, const char* pClassName);
CEntityFactoryDictionary__CreateType CEntityFactoryDictionary__CreateOriginal;
void* CEntityFactoryDictionary__Create(void* thisptr, const char* pClassName) {
	static uintptr_t mapload = uintptr_t(GetModuleHandleA("server.dll")) + 0x1432a2;
	static uintptr_t serverbase = uintptr_t(GetModuleHandleA("server.dll"));

	bool override2 = false;
	if (strstr(pClassName, "prop_physics") != NULL) {// && uintptr_t(_ReturnAddress()) != mapload) {
		if (uintptr_t(_ReturnAddress()) == mapload)
			return 0;
		pClassName = "prop_dynamic_override";
		override2 = true;
	}
	//std::cout << "spawned: " << pClassName << " override: " << (override2 ? "true" : "false") << " retaddr: " << ((uintptr_t(_ReturnAddress()) == mapload) ? "true" : "false") << " rva: " << uintptr_t(_ReturnAddress()) - serverbase << std::endl;
	return CEntityFactoryDictionary__CreateOriginal(thisptr, pClassName);
}

typedef void (*ConCommandConstructorType)(
	ConCommandR1* newCommand, const char* name, void (*callback)(const CCommand&), const char* helpString, int flags, void* parent);

void RegisterConCommand(const char* commandName, void (*callback)(const CCommand&), const char* helpString, int flags) {
	static ConCommandConstructorType conCommandConstructor = (ConCommandConstructorType)(uintptr_t(GetModuleHandleA("engine.dll")) + 0x4808F0);
	ConCommandR1* newCommand = new ConCommandR1;
	
	conCommandConstructor(newCommand, commandName, callback, helpString, flags, nullptr);
}
LDR_DLL_LOADED_NOTIFICATION_DATA* GetModuleNotificationData(const wchar_t* moduleName)
{
	HMODULE hMods[1024];
	DWORD cbNeeded;
	MODULEINFO modInfo;

	if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods), &cbNeeded))
	{
		for (DWORD i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			wchar_t szModName[MAX_PATH];
			if (GetModuleFileNameEx(GetCurrentProcess(), hMods[i], szModName, sizeof(szModName) / sizeof(wchar_t)))
			{
				if (wcsstr(szModName, moduleName) != 0)
				{
					if (GetModuleInformation(GetCurrentProcess(), hMods[i], &modInfo, sizeof(modInfo)))
					{
						LDR_DLL_LOADED_NOTIFICATION_DATA* notificationData = new LDR_DLL_LOADED_NOTIFICATION_DATA();
						notificationData->Flags = 0;

						UNICODE_STRING* fullDllName = new UNICODE_STRING();
						fullDllName->Buffer = new wchar_t[MAX_PATH];
						wcscpy_s(fullDllName->Buffer, MAX_PATH, szModName);
						fullDllName->Length = (USHORT)wcslen(szModName) * sizeof(wchar_t);
						fullDllName->MaximumLength = MAX_PATH * sizeof(wchar_t);
						notificationData->FullDllName = fullDllName;

						UNICODE_STRING* baseDllName = new UNICODE_STRING();
						baseDllName->Buffer = new wchar_t[MAX_PATH];
						_wsplitpath_s(szModName, NULL, 0, NULL, 0, baseDllName->Buffer, MAX_PATH, NULL, 0);
						baseDllName->Length = (USHORT)wcslen(baseDllName->Buffer) * sizeof(wchar_t);
						baseDllName->MaximumLength = MAX_PATH * sizeof(wchar_t);
						notificationData->BaseDllName = baseDllName;

						notificationData->DllBase = modInfo.lpBaseOfDll;
						notificationData->SizeOfImage = modInfo.SizeOfImage;
						return notificationData;
					}
				}
			}
		}
	}

	return nullptr;
}

void __stdcall LoaderNotificationCallback(
	unsigned long notification_reason,
	const LDR_DLL_NOTIFICATION_DATA* notification_data,
	void* context) {
	if (notification_reason != LDR_DLL_NOTIFICATION_REASON_LOADED)
		return;
	doBinaryPatchForFile(notification_data->Loaded);
	if (std::wstring((wchar_t*)notification_data->Loaded.BaseDllName->Buffer, notification_data->Loaded.BaseDllName->Length).find(L"server.dll") != std::string::npos) {
		doBinaryPatchForFile(*GetModuleNotificationData(L"vstdlib"));
		uintptr_t vTableAddr = reinterpret_cast<uintptr_t>(GetModuleHandleA("server.dll")) + 0x807220;
		RemoveItemsFromVTable(vTableAddr, 35, 2);
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("server.dll") + 0x143A10), &CServerGameDLL__DLLInit, (LPVOID*)&CServerGameDLL__DLLInitOriginal);
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("server.dll") + 0x71E0BC), &hkcalloc_base, NULL);
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("server.dll") + 0x71E99C), &hkmalloc_base, NULL);
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("server.dll") + 0x71E9FC), &hkrealloc_base, NULL);
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("server.dll") + 0x72B480), &hkrecalloc_base, NULL);
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("server.dll") + 0x721000), &hkfree_base, NULL);
		//MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("server.dll") + 0x364D00), &CAI_NetworkManager__LoadNavMesh, reinterpret_cast<LPVOID*>(&CAI_NetworkManager__LoadNavMeshOriginal));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA(VSCRIPT_DLL) + (IsDedicatedServer() ? 0x6A80 : 0x6A60)), &CScriptVM__ctor, reinterpret_cast<LPVOID*>(&CScriptVM__ctororiginal));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA(VSCRIPT_DLL) + (IsDedicatedServer() ? 0x1210 : 0x1210)), &CScriptManager__CreateNewVM, reinterpret_cast<LPVOID*>(&CScriptManager__CreateNewVMOriginal));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA(VSCRIPT_DLL) + (IsDedicatedServer() ? 0x1640 : 0x1630)), &CScriptVM__GetUnknownVMPtr, reinterpret_cast<LPVOID*>(&CScriptVM__GetUnknownVMPtrOriginal));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA(VSCRIPT_DLL) + (IsDedicatedServer() ? 0x1600 : 0x15F0)), &CScriptManager__DestroyVM, reinterpret_cast<LPVOID*>(&CScriptManager__DestroyVMOriginal));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA(VSCRIPT_DLL) + (IsDedicatedServer() ? 0xCDD0 : 0xCDB0)), &CSquirrelVM__RegisterFunctionGuts, reinterpret_cast<LPVOID*>(&CSquirrelVM__RegisterFunctionGutsOriginal));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA(VSCRIPT_DLL) + (IsDedicatedServer() ? 0xD670 : 0xD650)), &CSquirrelVM__PushVariant, reinterpret_cast<LPVOID*>(&CSquirrelVM__PushVariantOriginal));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA(VSCRIPT_DLL) + (IsDedicatedServer() ? 0xD7D0 : 0xD7B0)), &CSquirrelVM__ConvertToVariant, reinterpret_cast<LPVOID*>(&CSquirrelVM__ConvertToVariantOriginal));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA(VSCRIPT_DLL) + (IsDedicatedServer() ? 0xB130 : 0xB110)), &CSquirrelVM__ReleaseValue, reinterpret_cast<LPVOID*>(&CSquirrelVM__ReleaseValueOriginal));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA(VSCRIPT_DLL) + (IsDedicatedServer() ? 0xA210 : 0xA1F0)), &CSquirrelVM__SetValue, reinterpret_cast<LPVOID*>(&CSquirrelVM__SetValueOriginal));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA(VSCRIPT_DLL) + (IsDedicatedServer() ? 0x9C60 : 0x9C40)), &CSquirrelVM__SetValueEx, reinterpret_cast<LPVOID*>(&CSquirrelVM__SetValueExOriginal));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA(VSCRIPT_DLL) + (IsDedicatedServer() ? 0xBE80 : 0xBE60)), &CSquirrelVM__TranslateCall, reinterpret_cast<LPVOID*>(&CSquirrelVM__TranslateCallOriginal));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("server.dll") + 0x507560), &ServerClassInit_DT_BasePlayer, reinterpret_cast<LPVOID*>(&ServerClassInit_DT_BasePlayerOriginal));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("server.dll") + 0x51DFE0), &ServerClassInit_DT_Local, reinterpret_cast<LPVOID*>(&ServerClassInit_DT_LocalOriginal));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("server.dll") + 0x5064F0), &ServerClassInit_DT_LocalPlayerExclusive, reinterpret_cast<LPVOID*>(&ServerClassInit_DT_LocalPlayerExclusiveOriginal));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("server.dll") + 0x593270), &ServerClassInit_DT_TitanSoul, reinterpret_cast<LPVOID*>(&ServerClassInit_DT_TitanSoulOriginal));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("server.dll") + 0x629740), &sub_629740, reinterpret_cast<LPVOID*>(&sub_629740Original));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("server.dll") + 0x3A1EC0), &CBaseEntity__SendProxy_CellOrigin, reinterpret_cast<LPVOID*>(NULL));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("server.dll") + 0x3A2020), &CBaseEntity__SendProxy_CellOriginXY, reinterpret_cast<LPVOID*>(NULL));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("server.dll") + 0x3A2130), &CBaseEntity__SendProxy_CellOriginZ, reinterpret_cast<LPVOID*>(NULL));
		//MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("server.dll") + 0x3C8B70), &CBaseEntity__VPhysicsInitNormal, reinterpret_cast<LPVOID*>(&CBaseEntity__VPhysicsInitNormalOriginal));

		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("engine.dll") + 0x1FDA50), &CLC_Move__ReadFromBuffer, reinterpret_cast<LPVOID*>(&CLC_Move__ReadFromBufferOriginal));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("engine.dll") + 0x1F6F10), &CLC_Move__WriteToBuffer, reinterpret_cast<LPVOID*>(NULL));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("engine.dll") + 0xCA730), &CBaseServer__FillServerInfo, reinterpret_cast<LPVOID*>(&CBaseServer__FillServerInfoOriginal));
		//MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("server.dll") + 0x3667D0), &CAI_NetworkManager__DelayedInit, reinterpret_cast<LPVOID*>(&CAI_NetworkManager__DelayedInitOriginal));
		//MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("server.dll") + 0x36BC30), &sub_36BC30, reinterpret_cast<LPVOID*>(&sub_36BC30Original));
		//MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("server.dll") + 0x36C150), &sub_36C150, reinterpret_cast<LPVOID*>(&sub_36C150Original));
		//MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("server.dll") + 0x3669C0), &CAI_NetworkManager__FixupHints, reinterpret_cast<LPVOID*>(&CAI_NetworkManager__FixupHintsOriginal));
		//MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("server.dll") + 0x31CE90), &unkallocfunc, reinterpret_cast<LPVOID*>(&unkallocfuncoriginal));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("server.dll") + 0x25A8E0), &CEntityFactoryDictionary__Create, reinterpret_cast<LPVOID*>(&CEntityFactoryDictionary__CreateOriginal));
		//MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("server.dll") + 0x363A50), &sub_363A50, reinterpret_cast<LPVOID*>(&sub_363A50Original));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("engine.dll") + 0x21F9C0), &CEngineVGui__Init, reinterpret_cast<LPVOID*>(&CEngineVGui__InitOriginal));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("engine.dll") + 0x21EB70), &CEngineVGui__HideGameUI, reinterpret_cast<LPVOID*>(&CEngineVGui__HideGameUIOriginal));
		
		
		RegisterConCommand("toggleconsole", ToggleConsoleCommand, "Toggles the console", (1 << 17));


		RegisterConCommand("script", script_cmd, "Execute Squirrel code in server context", FCVAR_GAMEDLL);
		RegisterConCommand("script_client", script_client_cmd, "Execute Squirrel code in client context", FCVAR_NONE);
		RegisterConCommand("script_ui", script_ui_cmd, "Execute Squirrel code in UI context", FCVAR_NONE);


		//MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("server.dll") + 0x364140), &sub_364140, reinterpret_cast<LPVOID*>(NULL));
		
		//MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("vphysics.dll") + 0x257E0), &sub_1800257E0, reinterpret_cast<LPVOID*>(&sub_1800257E0Original));
		//MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("vphysics.dll") + 0xE77F0), &IVP_Environment__set_delta_PSI_time, reinterpret_cast<LPVOID*>(&IVP_Environment__set_delta_PSI_timeOriginal));
		//MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("vphysics.dll") + 0x31610), &sub_180031610, reinterpret_cast<LPVOID*>(&sub_180031610Original));
		

		MH_CreateHook((LPVOID)GetProcAddress(GetModuleHandleA("vstdlib.dll"), "VStdLib_GetICVarFactory"), &VStdLib_GetICVarFactory, NULL);
		if (!IsDedicatedServer()) {
			MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA(ENGINE_DLL) + 0x136860), &Status_ConMsg, NULL);
			MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA(ENGINE_DLL) + 0x1BF500), &Status_ConMsg, NULL);
			MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA(ENGINE_DLL) + 0x4735A0), &sub_1804735A0, NULL);
			MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA(ENGINE_DLL) + 0x8E6D0), &Status_ConMsg, NULL);
			MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA(ENGINE_DLL) + 0x22610), &Status_ConMsg, NULL);
			MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA(ENGINE_DLL) + 0x1170A0), &COM_Init, reinterpret_cast<LPVOID*>(&COM_InitOriginal));
			MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA(ENGINE_DLL) + 0x55C00), &CL_Retry_f, reinterpret_cast<LPVOID*>(&CL_Retry_fOriginal));
			MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA(ENGINE_DLL) + 0x8EAF0), &Con_ColorPrintf, NULL);
			MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("launcher.dll") + 0xB640), &CSquirrelVM__PrintFunc1, NULL);
			MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("launcher.dll") + 0xB6F0), &CSquirrelVM__PrintFunc2, NULL);
			MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("launcher.dll") + 0xB7A0), &CSquirrelVM__PrintFunc3, NULL);
			MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("engine.dll") + 0x23E20), &SVC_Print_Process_Hook, NULL);

			//MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA(ENGINE_DLL) + 0x473550), &sub_180473550, NULL);

			//MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA(ENGINE_DLL) + 0x1168B0), &COM_StringCopy, reinterpret_cast<LPVOID*>(&COM_StringCopyOriginal));
			//MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA(ENGINE_DLL) + 0x1C79A0), &DataTable_SetupReceiveTableFromSendTable, reinterpret_cast<LPVOID*>(&DataTable_SetupReceiveTableFromSendTableOriginal));
		}
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("engine.dll") + 0x136E70), &sub_136E70, reinterpret_cast<LPVOID*>(&sub_136E70Original));
		//MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA(ENGINE_DLL) + 0x1C79A0), &sub_1801C79A0, reinterpret_cast<LPVOID*>(&sub_1801C79A0Original));
		//
		//
		//diMH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA(ENGINE_DLL) + 0x1D9E70), &MatchRecvPropsToSendProps_R, reinterpret_cast<LPVOID*>(NULL));
		//MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA(ENGINE_DLL) + 0x217C30), &sub_180217C30, NULL);
		// Cast the function pointer to the function at 0x4E80

		// TODO(mrsteyk): REMOVE
		if (!IsDedicatedServer())
		{
			auto engine_mod = GetModuleHandleA("engine.dll");
			auto CVEngineServer_PrecacheModel_ptr = uintptr_t(engine_mod) + 0x0FC4D0;
			MH_CreateHook(LPVOID(CVEngineServer_PrecacheModel_ptr), &CVEngineServer_PrecacheModel, &CVEngineServer_PrecacheModel_o);
		}

		// Fix precache start
		{
			LoadLibraryA("engine_r1o.dll");
			auto r1oe_mod = uintptr_t(GetModuleHandleA("engine_r1o.dll"));
			auto server_mod = uintptr_t(GetModuleHandleA("server.dll"));
			// Cache bodymodel_militia to our own array...
			//MH_CreateHook(LPVOID(r1oe_mod + 0xF5790), &SetPreCache, &SetPreCache_o);
			// Rebuild CHL2_Player's precache to take our stuff into account
			MH_CreateHook(LPVOID(server_mod + 0x41E070), &CHL2_Player_Precache, 0);
		}
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("engine.dll") + 0x72360), &cl_DumpPrecacheStats, NULL);
		MH_EnableHook(MH_ALL_HOOKS);
		std::cout << "did hooks" << std::endl;

	}

	if (IsDedicatedServer() && (std::wstring((wchar_t*)notification_data->Loaded.BaseDllName->Buffer, notification_data->Loaded.BaseDllName->Length).find(L"engine_ds.dll") != std::string::npos)) {
		uintptr_t offsets[] = {
			// move g_ServerGlobalVariables_nTimestampNetworkingBase + 4
			0x552fc,
			// move g_ServerGlobalVariables_nTimestampRandomizeWindow + 4
			0x55303,
			// move g_ServerGlobalVariables_mapname_pszValue + 4
			0x5ec5f,
			// move g_ServerGlobalVariables_mapversion + 4
			0x286ba,
			0x286cb,
			0x2898c,
			0x5e7aa,
			// move g_ServerGlobalVariables_startspot + 4
			0x5ec81,
			// move g_ServerGlobalVariables_bMapLoadFailed + 4
			0xaa088,
			// move g_ServerGlobalVariables_deathmatch + 4
			0x5ec41,
			0x5ec4b,
			// move g_ServerGlobalVariables_coop + 4
			0x5ec2c,
			// move g_ServerGlobalVariables_maxEntities + 4
			0x5e823,
			0x61C10,
			// move g_ServerGlobalVariables_serverCount + 4
			0xa05d2,
			0xa178e,
			// move g_ServerGlobalVariables_pEdicts + 4
			0x5DBC7,
			0x5dc0c,
			0x5e89d,
			0x61c86
		};
		HMODULE hModule = GetModuleHandleA("engine_ds.dll");
		//MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("dedicated.dll") + 0x84720), &ReadFileFromFilesystemHook, reinterpret_cast<LPVOID*>(&readFileFromFilesystem));

		for (auto offset : offsets) {
			int* address = reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(hModule) + offset);

			DWORD oldProtect;
			if (!VirtualProtect(address, sizeof(uintptr_t), PAGE_EXECUTE_READWRITE, &oldProtect)) {
				continue;
			}

			*address += 4;

			DWORD temp;
			VirtualProtect(address, sizeof(uintptr_t), oldProtect, &temp);

			FlushInstructionCache(GetCurrentProcess(), address, sizeof(uintptr_t));
		}

		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("engine_ds.dll") + 0x1693D0), &ParsePDEF, reinterpret_cast<LPVOID*>(&ParsePDATAOriginal));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("engine_ds.dll") + 0x3259C0), &CUtlBuffer__SetExternalBuffer, reinterpret_cast<LPVOID*>(&CUtlBuffer__SetExternalBufferOriginal));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("engine_ds.dll") + 0x160130), &sub_160130, reinterpret_cast<LPVOID*>(NULL));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("dedicated.dll") + 0x1752B0), &ReadFileFromVPKHook, reinterpret_cast<LPVOID*>(&readFileFromVPK));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("dedicated.dll") + 0x750F0), &ReadFromCacheHook, reinterpret_cast<LPVOID*>(&readFromCache));

		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("engine_ds.dll") + 0x433C0), &ProcessConnectionlessPacket, reinterpret_cast<LPVOID*>(&ProcessConnectionlessPacketOriginal));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("engine_ds.dll") + 0xA1B90), &Host_InitDedicated, reinterpret_cast<LPVOID*>(&Host_InitDedicatedOriginal));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("engine_ds.dll") + 0x45C00), &CBaseClient__ProcessClientInfo, reinterpret_cast<LPVOID*>(NULL));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("engine_ds.dll") + 0x4C460), &sub_4C460, reinterpret_cast<LPVOID*>(NULL));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("engine_ds.dll") + 0x14E980), &sub_14E980, reinterpret_cast<LPVOID*>(NULL));
		uintptr_t tier0_base = reinterpret_cast<uintptr_t>(GetModuleHandleA("tier0_r1.dll"));

		//MH_CreateHook(reinterpret_cast<LPVOID>(tier0_base + 0xC490), &HookedAlloc, reinterpret_cast<void**>(&g_originalAlloc));
		//MH_CreateHook(reinterpret_cast<LPVOID>(tier0_base + 0xC600), &HookedFree, reinterpret_cast<void**>(&g_originalFree));
		//MH_CreateHook(reinterpret_cast<LPVOID>(tier0_base + 0xDDE0), &HookedRealloc, reinterpret_cast<void**>(&g_originalRealloc));
		//MH_CreateHook(reinterpret_cast<LPVOID>(tier0_base + 0xB350), &HookedGetSize, reinterpret_cast<void**>(&g_originalGetSize));
		//MH_CreateHook(reinterpret_cast<LPVOID>(tier0_base + 0xC930), &HookedRegionAlloc, reinterpret_cast<void**>(&g_originalRegionAlloc));
		//
		MH_EnableHook(MH_ALL_HOOKS);
		//static CustomBuffer customBuffer;
		//std::cout.rdbuf(&customBuffer);

	}
	if (std::wstring((wchar_t*)notification_data->Loaded.BaseDllName->Buffer, notification_data->Loaded.BaseDllName->Length).find(L"engine.dll") != std::string::npos) {
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA(ENGINE_DLL) + 0x127C70), &FileSystem_UpdateAddonSearchPaths, reinterpret_cast<LPVOID*>(&FileSystem_UpdateAddonSearchPathsTypeOriginal));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("filesystem_stdio.dll") + 0x6A420), &ReadFileFromVPKHook, reinterpret_cast<LPVOID*>(&readFileFromVPK));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("filesystem_stdio.dll") + 0x9C20), &ReadFromCacheHook, reinterpret_cast<LPVOID*>(&readFromCache));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("filesystem_stdio.dll") + 0x16250), &AddVPKFile, reinterpret_cast<LPVOID*>(&AddVPKFileOriginal));
		//MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("filesystem_stdio.dll") + 0x4BC0), &CBaseFileSystem__CSearchPath__SetPath, reinterpret_cast<LPVOID*>(&CBaseFileSystem__CSearchPath__SetPathOriginal));
		//MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("filesystem_stdio.dll") + 0x13D60), &CZipPackFile__Prepare, reinterpret_cast<LPVOID*>(&CZipPackFile__PrepareOriginal));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("filesystem_stdio.dll") + 0x9AB70), &fs_sprintf_hook, reinterpret_cast<LPVOID*>(NULL));
		
		
	}
	if (std::wstring((wchar_t*)notification_data->Loaded.BaseDllName->Buffer, notification_data->Loaded.BaseDllName->Length).find(L"client.dll") != std::string::npos) {
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("client.dll") + 0x21FE50), &PredictionErrorFn, reinterpret_cast<LPVOID*>(NULL));
		//MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("client.dll") + 0x029840), &C_BaseEntity__VPhysicsInitNormal, reinterpret_cast<LPVOID*>(NULL));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("client.dll") + 0x27F2C0), &sub_18027F2C0, reinterpret_cast<LPVOID*>(&sub_18027F2C0Original));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("engine.dll") + 0x56A450), &vsnprintf_l_hk, NULL);
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("client.dll") + 0x744864), &vsnprintf_l_hk, NULL);
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("engine.dll") + 0x102D50), &Cbuf_AddText, reinterpret_cast<LPVOID*>(&Cbuf_AddTextOriginal));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("engine.dll") + 0x4801B0), &ConVar_PrintDescription, reinterpret_cast<LPVOID*>(&ConVar_PrintDescriptionOriginal));
		MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleA("engine.dll") + 0x4722E0), &sub_1804722E0, 0);
		
		MH_EnableHook(MH_ALL_HOOKS);
	}

}
