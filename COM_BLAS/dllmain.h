// dllmain.h : モジュール クラスの宣言です。

class CCOMBLASModule : public ATL::CAtlDllModuleT< CCOMBLASModule >
{
public :
	DECLARE_LIBID(LIBID_COMBLASLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_COMBLAS, "{5650ec59-f9dc-4d38-83b2-5828ff8c4d2a}")
};

extern class CCOMBLASModule _AtlModule;
