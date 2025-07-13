#define cls_realtime ((int*)0x15B8BC0) // 0x155F3E0)
#define cls_state ((int*)0x15B8AA0)
#define clc_stringData ((PCHAR)0x148FCBC)
#define clc_stringOffsets ((PINT)0x148DCBC)
#define clc_demoplaying ((PINT)0x18BA6CC)
#define cls_numglobalservers ((int*)0x15BD5E8)
#define cls_pingUpdateSource ((int*)0x1894A74)

#define cs0 (clc_stringData + clc_stringOffsets[0])
#define cs1 (clc_stringData + clc_stringOffsets[1])
#define gameWindow ((HWND*)0x1999D68)

static bool unlock_client_structure() {
	__try {
	XUNLOCK((void*)cls_realtime, sizeof(int));
	XUNLOCK((void*)cls_state, sizeof(int));
	XUNLOCK((void*)clc_demoplaying, 4);
	XUNLOCK((void*)cls_numglobalservers, sizeof(int)); // idk if needed
	XUNLOCK((void*)cls_pingUpdateSource, sizeof(int));  // idk if needed
	}
	__except (1) {
		return false;
	}

	return true;
}