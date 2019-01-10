#include "fose\PluginAPI.h"
#include "fose_common\SafeWrite.h"

#define directInputAddr 0x11795C4
#define isHeldOffset 0x18F8

/* DirectX Scancodes */
#define LCTRL 29 + isHeldOffset
#define LSHIFT 42 + isHeldOffset
#define RCTRL 157 + isHeldOffset
#define RSHIFT 54 + isHeldOffset

void writeHooks();
bool versionCheck(const FOSEInterface* fose);

extern "C" {

	BOOL WINAPI DllMain(HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved) {
		return TRUE;
	}

	bool FOSEPlugin_Query(const FOSEInterface *fose, PluginInfo *info) {
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = "Quick Item Transfer";
		info->version = 1;
    
		return versionCheck(fose);
	}

	bool FOSEPlugin_Load(const FOSEInterface *fose) {
		writeHooks();
		return true;
	}

};

_declspec(naked) void hookContainer() {
	static const UInt32 retnAddr = 0x63BD85;
	static const UInt32 doMoveAddr = 0x63BE0C;
	_asm {
		mov ecx, dword ptr ds:[directInputAddr]
		cmp     byte ptr[ecx + LCTRL], 0
		jnz		dropOne
		cmp     byte ptr[ecx + RCTRL], 0
		jnz		dropOne
		cmp     byte ptr[ecx + LSHIFT], 0
		jnz		dropAll
		cmp     byte ptr[ecx + RSHIFT], 0
		jnz		dropAll
	originalcode :
		mov ecx, 0xF636D4
		jmp retnAddr

	dropOne:
		push 1
		jmp doMoveAddr

	dropAll:
		mov ecx, dword ptr DS : [0x10760A8]
		mov eax, dword ptr[ecx + 4]
		push eax
		jmp doMoveAddr

	}
}

_declspec(naked) void hookInventory() {
	static const UInt32 retnAddr = 0x654C5C;
	static const UInt32 doMoveAddr = 0x654C94;
	_asm {
		mov ecx, dword ptr ds : [directInputAddr]
		cmp     byte ptr[ecx + LCTRL], 0
		jnz		dropOne
		cmp     byte ptr[ecx + RCTRL], 0
		jnz		dropOne
		cmp     byte ptr[ecx + LSHIFT], 0
		jnz		dropAll
		cmp     byte ptr[ecx + RSHIFT], 0
		jnz		dropAll
	originalCode:
		mov ecx, 0xF636D4
		jmp retnAddr
		
	dropOne :
		push 1
		jmp doMoveAddr

	dropAll :
		mov ecx, dword ptr DS : [0x1076864]
		mov esi, dword ptr[ecx + 4]
		push esi
		jmp doMoveAddr
	}
}

_declspec(naked) void hookBarter() {
	static const UInt32 retnAddr = 0x634BDE;
	static const UInt32 doMoveAddr = 0x634C1B;
	_asm {
		mov ecx, dword ptr ds : [directInputAddr]
		cmp     byte ptr[ecx + LCTRL], 0
		jnz		dropOne
		cmp     byte ptr[ecx + RCTRL], 0
		jnz		dropOne
		cmp     byte ptr[ecx + LSHIFT], 0
		jnz		dropAll
		cmp     byte ptr[ecx + RSHIFT], 0
		jnz		dropAll
	originalCode :
		mov ecx, 0xF636D4
		jmp retnAddr

	dropOne:
		push 1
		jmp doMoveAddr

	dropAll:
		push edi
		jmp doMoveAddr
	}
}

void writeHooks() {
	WriteRelJump(0x63BD80, (UInt32)hookContainer);
	WriteRelJump(0x654C57, (UInt32)hookInventory);
	WriteRelJump(0x634BD9, (UInt32)hookBarter);
}

bool versionCheck(const FOSEInterface* fose) {
	if (fose->isEditor) return false;
	if (fose->foseVersion < FOSE_VERSION_INTEGER) {
		_ERROR("FOSE version too old (got %08X expected at least %08X)", fose->foseVersion, FOSE_VERSION_INTEGER);
		return false;
	}
	if (fose->runtimeVersion != FALLOUT_VERSION)
	{
		_ERROR("incorrect runtime version (got %08X need %08X)", fose->runtimeVersion, FALLOUT_VERSION);
		return false;
	}
	return true;
}
