#include "shared.h"
#include "client.h"
#include "render.h"
/*
RE_RegisterShaderNoMip_t RE_RegisterShaderNoMip = (RE_RegisterShaderNoMip_t)0x4FCAE0;
RE_RegisterShader_t RE_RegisterShader = (RE_RegisterShader_t)0x4FCA80;
R_FindShader_t R_FindShader = (R_FindShader_t)0x4FC5C0;
SCR_DrawPic_t SCR_DrawPic = (SCR_DrawPic_t)0x4168D0;
SCR_AdjustFrom640_t SCR_AdjustFrom640 = (SCR_AdjustFrom640_t)0x416810;
SCR_DrawSmallStringExt_t SCR_DrawSmallStringExt = (SCR_DrawSmallStringExt_t)0x416A30;
SCR_DrawSmallChar_t SCR_DrawSmallChar = (SCR_DrawSmallChar_t)0x416980;
*/
SCR_DrawString_t SCR_DrawString = (SCR_DrawString_t)0x4E3F00;

RE_SetColor_t RE_SetColor = (RE_SetColor_t)0x4EA700;
/*
RB_SetGL2D_t RB_SetGL2D = (RB_SetGL2D_t)0x4D6CB0;
*/

void R_Init(void) {
	void(*call)(void);
	*(int*)&call = 0x4BF480;
	call();

	if (Cvar_VariableIntegerValue("r_borderless")) {
		int width, height;
		GetDesktopResolution(&width, &height);
		SetWindowLongA(*gameWindow, GWL_EXSTYLE, 0);
		SetWindowLongA(*gameWindow, GWL_STYLE, WS_VISIBLE | WS_POPUP);
		MoveWindow(*gameWindow, 0, 0, width, height, 0);
	}
}