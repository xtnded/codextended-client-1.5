#include "shared.h"
#include "client.h"
#include "version_info.h"

cvar_t* cl_running;

void CL_FOVLimit() {
	int fov = Cvar_VariableIntegerValue("cg_fov");
	int cheats = atoi(Info_ValueForKey(cs1, "sv_cheats"));

	char* sv_fov_min = Info_ValueForKey(cs1, "sv_fov_min");
	char* sv_fov_max = Info_ValueForKey(cs1, "sv_fov_max");
	int fovMin = strlen(sv_fov_min) ? atoi(sv_fov_min) : 80;
	int fovMax = strlen(sv_fov_max) ? atoi(sv_fov_max) : 95;

	if ((fov < fovMin || fov > fovMax) && cheats != 1) {
		Com_Printf("cg_fov \"%d\" is invalid. Allowed values: \"%d\" - \"%d\".\n", fov, fovMin, fovMax);
		Cvar_Set("cg_fov", "80");
	}
}

void CL_Frame(int msec) {

	if (!cl_running->integer) return;
	
	void CL_DiscordFrame();
	CL_DiscordFrame();

	CL_FOVLimit();
}
UINT_PTR pcl_frame = (UINT_PTR)0x412710;
void __declspec(naked) CL_FrameStub()
{
	__asm
	{
		push    eax
		call	pcl_frame
		call    CL_Frame
		add     esp, 4
		retn
	}
}

void Cmd_Borderless() {
	if (Cmd_Argc() > 1) {
		Cvar_Set("r_borderless", Cmd_Argv(1));

		if (Cvar_VariableIntegerValue("r_borderless")) {
			Cvar_Set("r_fullscreen", "0");
			Cvar_Set("r_mode", "-1");
			Cvar_Set("com_introplayed", "1");
		}

		void(*Cbuf_ExecuteText)(const char*);
		*(UINT32*)&Cbuf_ExecuteText = 0x42AAB0;
		char cmd[10];
		sprintf(cmd, "vid_restart", cmd);
		Cbuf_ExecuteText(cmd);
	}
	else {
		Com_Printf("\"r_borderless\" is:\"%d\" default: \"0\"\n", Cvar_VariableIntegerValue("r_borderless"));
	}
}

void CL_Init(void) {
	void(*oCL_Init)();
	*(int*)(&oCL_Init) = 0x413380;
	oCL_Init();

	cl_running = Cvar_Get("cl_running", "0", CVAR_ROM);
	Cvar_Get("cg_fov", "80", CVAR_ARCHIVE);

	Cvar_Set("version", va("COD MP 1.5x build %d %s %s win-x86", BUILDNUMBER, __DATE__, __TIME__));
	Cvar_Set("shortversion", va("1.5x (%d)", BUILDNUMBER));
	Cvar_Set("r_overbrightbits", "0");
	Cvar_Set("r_ignorehwgamma", "0");
	Cvar_Set("cl_languagewarnings", "0");
	Cvar_Set("cl_languagewarningsaserrors", "0");
	Cvar_Set("com_hunkmegs", "512");

	Cmd_AddCommand("r_borderless", Cmd_Borderless);
}