#include "common.h"

#define CVAR_ARCHIVE        1   // set to cause it to be saved to vars.rc
// used for system variables, not for player
// specific configurations
#define CVAR_USERINFO       2   // sent to server on connect or change
#define CVAR_SERVERINFO     4   // sent in response to front end requests
#define CVAR_SYSTEMINFO     8   // these cvars will be duplicated on all clients
#define CVAR_INIT           16  // don't allow change from console at all,
// but can be set from the command line
#define CVAR_LATCH          32  // will only change when C code next does
// a Cvar_Get(), so it can't be changed
// without proper initialization.  modified
// will be set, even though the value hasn't
// changed yet
#define CVAR_ROM            64  // display only, cannot be set by user at all
#define CVAR_USER_CREATED   128 // created by a set command
#define CVAR_TEMP           256 // can be set even when cheats are disabled, but is not archived
#define CVAR_CHEAT          512 // can not be changed if cheats are disabled
#define CVAR_NORESTART      1024    // do not clear when a cvar_restart is issued
#define CVAR_WOLFINFO       2048    // DHM - NERVE :: Like userinfo, but for wolf multiplayer info

#define CVAR_UNSAFE         4096    // ydnar: unsafe system cvars (renderer, sound settings, anything that might cause a crash)
#define CVAR_SERVERINFO_NOUPDATE        8192    // gordon: WONT automatically send this to clients, but server browsers will see it


#define MAX_STRING_CHARS    1024    // max length of a string passed to Cmd_TokenizeString
#define MAX_STRING_TOKENS   256     // max tokens resulting from Cmd_TokenizeString
#define MAX_TOKEN_CHARS     1024    // max length of an individual token

#define MAX_RELIABLE_COMMANDS 64

#define MAX_INFO_STRING     1024
#define MAX_INFO_KEY        1024
#define MAX_INFO_VALUE      1024

#define BIG_INFO_STRING     8192    // used for system info key only
#define BIG_INFO_KEY        8192
#define BIG_INFO_VALUE      8192

#define MAX_NAME_LENGTH     36      // max length of a client name

#define MAX_QPATH 64
#define MAX_OSPATH 256

typedef enum { qfalse, qtrue }    qboolean;

typedef struct cvar_s {
	char        *name;
	char        *string;
	char        *resetString;       // cvar_restart will reset to this value
	char        *latchedString;     // for CVAR_LATCH vars
	int flags;
	qboolean modified;              // set each time the cvar is changed
	int modificationCount;          // incremented each time the cvar is changed
	float value;                    // atof( string )
	int integer;                    // atoi( string )
	struct cvar_s *next;
	struct cvar_s *hashNext;
} cvar_t;

typedef enum {
	ET_GENERAL,
	ET_PLAYER,
	ET_CORPSE,
	ET_ITEM,
	ET_MISSILE,
	ET_MOVER,
	ET_PORTAL,
	ET_INVISIBLE,
	ET_SCRIPTMOVER,
	ET_TURRET //???
} entityTypes;

typedef struct {
	int unknown; //0
	byte* data; //4
	int maxsize; //8 (most likely maxsize value 16384)
	int cursize; //12 //value 129
	int readcount; //16 //value 16
	int bit; //20 //value 0
} msg_t; //size is 0x18 (24)

typedef enum {
	STEREO_CENTER,
	STEREO_LEFT,
	STEREO_RIGHT
} stereoFrame_t;

typedef enum {
	CA_UNINITIALIZED,
	CA_DISCONNECTED = 0,
	CA_CONNECTING,
	CA_CHALLENGING,
	CA_CONNECTED,
} connstate_t;

#if 0
typedef enum {
	CA_UNINITIALIZED,
	CA_DISCONNECTED,    // not talking to a server
	CA_AUTHORIZING,     // not used any more, was checking cd key
	CA_CONNECTING,      // sending request packets to the server
	CA_CHALLENGING,     // sending challenge packets to the server
	CA_CONNECTED,       // netchan_t established, getting gamestate
	CA_LOADING,         // only during cgame initialization, never during main loop
	CA_PRIMED,          // got gamestate, waiting for first frame
	CA_ACTIVE,          // game views should be displayed
	CA_CINEMATIC        // playing a cinematic or a static pic, not connected to a server
} connstate_t;
#endif
#define Q_COLOR_ESCAPE  '^'
#define Q_IsColorString( p )  ( p && *( p ) == Q_COLOR_ESCAPE && *( ( p ) + 1 ) && *( ( p ) + 1 ) != Q_COLOR_ESCAPE )

#define COLOR_BLACK     '0'
#define COLOR_RED       '1'
#define COLOR_GREEN     '2'
#define COLOR_YELLOW    '3'
#define COLOR_BLUE      '4'
#define COLOR_CYAN      '5'
#define COLOR_MAGENTA   '6'
#define COLOR_WHITE     '7'
#define COLOR_ORANGE    '8'
#define COLOR_MDGREY    '9'
#define COLOR_LTGREY    ':'
//#define COLOR_LTGREY	';'
#define COLOR_MDGREEN   '<'
#define COLOR_MDYELLOW  '='
#define COLOR_MDBLUE    '>'
#define COLOR_MDRED     '?'
#define COLOR_LTORANGE  'A'
#define COLOR_MDCYAN    'B'
#define COLOR_MDPURPLE  'C'
#define COLOR_NULL      '*'


#define COLOR_BITS  31
#define ColorIndex( c )   ( ( ( c ) - '0' ) & COLOR_BITS )

#define S_COLOR_BLACK       "^0"
#define S_COLOR_RED         "^1"
#define S_COLOR_GREEN       "^2"
#define S_COLOR_YELLOW      "^3"
#define S_COLOR_BLUE        "^4"
#define S_COLOR_CYAN        "^5"
#define S_COLOR_MAGENTA     "^6"
#define S_COLOR_WHITE       "^7"
#define S_COLOR_ORANGE      "^8"
#define S_COLOR_MDGREY      "^9"
#define S_COLOR_LTGREY      "^:"
//#define S_COLOR_LTGREY		"^;"
#define S_COLOR_MDGREEN     "^<"
#define S_COLOR_MDYELLOW    "^="
#define S_COLOR_MDBLUE      "^>"
#define S_COLOR_MDRED       "^?"
#define S_COLOR_LTORANGE    "^A"
#define S_COLOR_MDCYAN      "^B"
#define S_COLOR_MDPURPLE    "^C"
#define S_COLOR_NULL        "^*"

typedef int qhandle_t;
typedef int sfxHandle_t;
typedef int fileHandle_t;
typedef int clipHandle_t;

typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];
typedef vec_t vec5_t[5];
typedef unsigned char byte;

#define DotProduct( x,y )         ( ( x )[0] * ( y )[0] + ( x )[1] * ( y )[1] + ( x )[2] * ( y )[2] )
#define VectorSubtract( a,b,c )   ( ( c )[0] = ( a )[0] - ( b )[0],( c )[1] = ( a )[1] - ( b )[1],( c )[2] = ( a )[2] - ( b )[2] )
#define VectorAdd( a,b,c )        ( ( c )[0] = ( a )[0] + ( b )[0],( c )[1] = ( a )[1] + ( b )[1],( c )[2] = ( a )[2] + ( b )[2] )
#define VectorCopy( a,b )         ( ( b )[0] = ( a )[0],( b )[1] = ( a )[1],( b )[2] = ( a )[2] )
#define VectorScale( v, s, o )    ( ( o )[0] = ( v )[0] * ( s ),( o )[1] = ( v )[1] * ( s ),( o )[2] = ( v )[2] * ( s ) )
#define VectorMA( v, s, b, o )    ( ( o )[0] = ( v )[0] + ( b )[0] * ( s ),( o )[1] = ( v )[1] + ( b )[1] * ( s ),( o )[2] = ( v )[2] + ( b )[2] * ( s ) )

#define VectorClear( a )              ( ( a )[0] = ( a )[1] = ( a )[2] = 0 )
#define VectorNegate( a,b )           ( ( b )[0] = -( a )[0],( b )[1] = -( a )[1],( b )[2] = -( a )[2] )
#define VectorSet( v, x, y, z )       ( ( v )[0] = ( x ), ( v )[1] = ( y ), ( v )[2] = ( z ) )

#define Vector2Set( v, x, y )         ( ( v )[0] = ( x ),( v )[1] = ( y ) )
#define Vector2Copy( a,b )            ( ( b )[0] = ( a )[0],( b )[1] = ( a )[1] )
#define Vector2Subtract( a,b,c )      ( ( c )[0] = ( a )[0] - ( b )[0],( c )[1] = ( a )[1] - ( b )[1] )

#define Vector4Set( v, x, y, z, n )   ( ( v )[0] = ( x ),( v )[1] = ( y ),( v )[2] = ( z ),( v )[3] = ( n ) )
#define Vector4Copy( a,b )            ( ( b )[0] = ( a )[0],( b )[1] = ( a )[1],( b )[2] = ( a )[2],( b )[3] = ( a )[3] )
#define Vector4MA( v, s, b, o )       ( ( o )[0] = ( v )[0] + ( b )[0] * ( s ),( o )[1] = ( v )[1] + ( b )[1] * ( s ),( o )[2] = ( v )[2] + ( b )[2] * ( s ),( o )[3] = ( v )[3] + ( b )[3] * ( s ) )
#define Vector4Average( v, b, s, o )  ( ( o )[0] = ( ( v )[0] * ( 1 - ( s ) ) ) + ( ( b )[0] * ( s ) ),( o )[1] = ( ( v )[1] * ( 1 - ( s ) ) ) + ( ( b )[1] * ( s ) ),( o )[2] = ( ( v )[2] * ( 1 - ( s ) ) ) + ( ( b )[2] * ( s ) ),( o )[3] = ( ( v )[3] * ( 1 - ( s ) ) ) + ( ( b )[3] * ( s ) ) )

#define SnapVector( v ) {v[0] = ( (int)( v[0] ) ); v[1] = ( (int)( v[1] ) ); v[2] = ( (int)( v[2] ) );}

typedef struct {
	netsrc_t	sock;

	int			dropped;			// between last packet and previous

	netadr_t	remoteAddress;
	int			qport;				// qport value to write when transmitting

									// sequencing variables
	int			incomingSequence;
	int			outgoingSequence;

	// incoming fragment assembly buffer
	int			fragmentSequence;
	int			fragmentLength;
	byte		fragmentBuffer[MAX_MSGLEN];

	// outgoing fragment buffer
	// we need to space out the sending of large fragmented messages
	qboolean	unsentFragments;
	int			unsentFragmentStart;
	int			unsentLength;
	byte		unsentBuffer[MAX_MSGLEN];
} netchan_t;

typedef struct {

	int			clientNum;
	int			lastPacketSentTime;			// for retransmits during connection
	int			lastPacketTime;				// for timeouts

	netadr_t	serverAddress;
	int			connectTime;				// for connection retransmits
	int			connectPacketCount;			// for display on connection dialog
	char		serverMessage[MAX_STRING_TOKENS];	// for display on connection dialog

	int			challenge;					// from the server to use for connecting
	int			checksumFeed;				// from the server for checksum calculations

											// these are our reliable messages that go to the server
	int			reliableSequence;
	int			reliableAcknowledge;		// the last one the server has executed
	char		reliableCommands[MAX_RELIABLE_COMMANDS][MAX_STRING_CHARS];

	// server message (unreliable) and command (reliable) sequence
	// numbers are NOT cleared at level changes, but continue to
	// increase as long as the connection is valid

	// message sequence is used by both the network layer and the
	// delta compression layer
	int			serverMessageSequence;

	// reliable messages received from server
	int			serverCommandSequence;
	int			lastExecutedServerCommand;		// last server command grabbed or executed with CL_GetServerCommand
	char		serverCommands[MAX_RELIABLE_COMMANDS][MAX_STRING_CHARS];

	// file transfer from server
	fileHandle_t download;
	char		downloadTempName[MAX_OSPATH];
	char		downloadName[MAX_OSPATH];
	int			downloadNumber;
	int			downloadBlock;	// block we are waiting for
	int			downloadCount;	// how many bytes we got
	int			downloadSize;	// how many bytes we got
	char		downloadList[MAX_INFO_STRING]; // list of paks we need to download
	qboolean	downloadRestart;	// if true, we need to do another FS_Restart because we downloaded a pak

									// demo information
	char		demoName[MAX_QPATH];
	qboolean	spDemoRecording;
	qboolean	demorecording;
	qboolean	demoplaying;
	qboolean	demowaiting;	// don't record until a non-delta message is received
	qboolean	firstDemoFrameSkipped;
	fileHandle_t	demofile;

	int			timeDemoFrames;		// counter of rendered frames
	int			timeDemoStart;		// cls.realtime before first frame
	int			timeDemoBaseTime;	// each frame will be at this time + frameNum * 50

									// big stuff at end of structure so most offsets are 15 bits or less
	netchan_t	netchan;
} clientConnection_t;

typedef struct usercmd_s
{
	int serverTime; // 0x0  // pm + 4
	byte buttons;   // 0x4  // pm + 8   // console, chat, ads, attack, use
	byte wbuttons;  // 0x5  // pm + 9   // lean left, lean right, reload
	byte weapon;    // 0x6  // pm + 10
	byte gap_0x7;           // pm + 11
	int angles[3];  // 0x8  // pm + 12  [0] = 0x8, [1] = 0xC, [2] = 0x10
	signed char forwardmove;    // 0x14     // pm + 24
	signed char rightmove;      // 0x15     // pm + 25
	signed char upmove;         // 0x16     // pm + 26
	byte gap_0x17;                          // pm + 27
} usercmd_t;

typedef enum
{
	WEAPON_READY = 0x0,
	WEAPON_RAISING = 0x1,
	WEAPON_DROPPING = 0x2,
	WEAPON_FIRING = 0x3,
	WEAPON_RECHAMBERING = 0x4,
	WEAPON_RELOADING = 0x5,
	WEAPON_RELOADING_INTERUPT = 0x6,
	WEAPON_RELOAD_START = 0x7,
	WEAPON_RELOAD_START_INTERUPT = 0x8,
	WEAPON_RELOAD_END = 0x9,
	//...
} weaponstate_t;

typedef struct trace_s
{
	float fraction;     // 0x0
	vec3_t endpos;      // 0x4
	vec3_t normal;      // 0x10
	int surfaceFlags;   // 0x1C
	byte gap0x20[8];
	uint16_t entityNum; // 0x28
	uint16_t partName;  // 0x2A
	byte gap0x2C[2];
	byte allsolid;      // 0x2E
	byte startsolid;    // 0x2F
} trace_t;

typedef struct objective_s
{
	int state;
	vec3_t origin;
	int entNum;
	int teamNum;
	int icon;
} objective_t;

typedef enum
{
	PM_NORMAL = 0x0,
	PM_NORMAL_LINKED = 0x1,
	PM_NOCLIP = 0x2,
	PM_UFO = 0x3,
	PM_SPECTATOR = 0x4,
	PM_INTERMISSION = 0x5,
	PM_DEAD = 0x6,
	PM_DEAD_LINKED = 0x7,
} pmtype_t;

typedef struct hudelem_s
{
	byte gap[112];
} hudelem_t;

typedef struct hudElemState_s
{
	hudelem_t current[31];
	hudelem_t archival[31];
} hudElemState_t;

typedef struct playerState_s
{
	int commandTime;        // 0x0
	pmtype_t pm_type;       // 0x4
	int bobCycle;           // 0x8
	int pm_flags;           // 0xC
	int pm_time;            // 0x10
	vec3_t origin;          // [0] = 0x14, [1] = 0x18, [2] = 0x1C
	vec3_t velocity;        // [0] = 0x20, [1] = 0x24, [2] = 0x28
	int weaponTime;         // 0x2c
	int weaponDelay;        // 0x30
	int grenadeTimeLeft;    // 0x34
	int iFoliageSoundTime;  // 0x38
	int gravity;            // 0x3C
	float leanf;            // 0x40
	int speed;              // 0x44
	vec3_t delta_angles;    // [0] = 0x48, [1] = 0x4C, [2] = 0x50
	int groundEntityNum;    // 0x54
	vec3_t vLadderVec;      // [0] = 0x58, [1] = 0x5C, [2] = 0x60
	int jumpTime;           // 0x64
	float jumpOriginZ;     // 0x68
	int legsTimer;          // 0x6C
	int legsAnim;           // 0x70
	int torsoTimer;         // 0x74
	int torsoAnim;          // 0x78
	int movementDir;        // 0x7C
	int eFlags;             // 0x80
	int eventSequence;      // 0x84
	int events[4];          // 0x88
	unsigned int eventParms[4]; // 0x98
	int oldEventSequence;       // 0xA8
	int clientNum;              // 0xAC
	unsigned int weapon;        // 0xB0
	weaponstate_t weaponstate;  // 0xB4
	float fWeaponPosFrac;       // 0xB8
	int viewmodelIndex;         // 0xBC
	vec3_t viewangles;          // 0xC0
	int viewHeightTarget;       // 0xCC
	float viewHeightCurrent;    // 0xD0
	int viewHeightLerpTime;     // 0xD4
	int viewHeightLerpTarget;   // 0xD8
	int viewHeightLerpDown;     // 0xDC
	int viewHeightLerpPosAdj;   // 0xE0
	int damageEvent;            // 0xe4
	int damageYaw;              // 0xe8
	int damagePitch;            // 0xec
	int damageCount;            // 0xf0
	int stats[6];               // 0xf4
	int ammo[64];      // 0x10c
	int ammoclip[64];  // 0x20c
	unsigned int weapons[2];    // 0x30c
	byte weaponslots[8];        // 0x314
	unsigned int weaponrechamber[2]; // 0x31c
	vec3_t mins;                // 0x324
	vec3_t maxs;                // 0x330
	int proneViewHeight;        // 0x33C
	int crouchViewHeight;       // 0x340
	int standViewHeight;        // 0x344
	int deadViewHeight;         // 0x348
	float walkSpeedScale;       // 0x34C // ADS
	float runSpeedScale;        // 0x350
	float proneSpeedScale;      // 0x354
	float crouchSpeedScale;     // 0x358
	float strafeSpeedScale;     // 0x35C
	float backSpeedScale;       // 0x360
	float leanSpeedScale;       // 0x364
	float proneDirection;       // 0x368
	float proneDirectionPitch;  // 0x36c
	float proneTorsoPitch;      // 0x370
	int viewlocked;             // 0x374
	int viewlocked_entNum;      // 0x378
	float friction;             // 0x37C
	int gunfx;                  // 0x380
	int serverCursorHint;       // 0x384
	int serverCursorHintVal;    // 0x388
	trace_t serverCursorHintTrace; // 0x38C
	byte gap_0x3BC[4];
	int iCompassFriendInfo;     // 0x3C0
	float fTorsoHeight;         // 0x3c4
	float fTorsoPitch;          // 0x3c8
	float fWaistPitch;          // 0x3cc
	int entityEventSequence;    // 0x3D0
	int weapAnim;               // 0x3d4
	float aimSpreadScale;       // 0x3d8
	int shellshockIndex;        // 0x3dc
	int shellshockTime;         // 0x3e0
	int shellshockDuration;     // 0x3e4
	objective_t objective[16]; // 0x3E8
	hudElemState_t hud;         // 0x5A8
	int deltaTime;              // 0x20C8
} playerState_t;

typedef struct
{
	playerState_t* ps;  // 0x0
	usercmd_t cmd;      // 0x4
	usercmd_t oldcmd;   // 0x1C
	int tracemask;      // 0x34
	int debugLevel;     // 0x38
	int numtouch;       // 0x3C
	int touchents[32];  // 0x40
	vec3_t mins;        // 0xC0
	vec3_t maxs;        // 0xCC
	byte watertype;     // 0xd8
	byte waterlevel;    // 0xd9
	byte gap_0xDA[2];
	float xyspeed;      // 0xDC
	int pmove_fixed;    // 0xE0
	int pmove_msec;     // 0xE4
	int proneChange;    // 0xE8
	void (*trace)(trace_t*, const vec3_t, const vec3_t, const vec3_t, const vec3_t, int, int);     // 0xec
	void (*trace2)(trace_t*, const vec3_t, const vec3_t, const vec3_t, const vec3_t, int, int);    // 0xF0
	void (*trace3)(trace_t*, const vec3_t, const vec3_t, const vec3_t, const vec3_t, int, int);    // 0xF4
	byte gap_0xF8[4];
} pmove_t;

typedef void(*Cvar_Set_t)(char*, char*);
typedef cvar_t* (*Cvar_Get_t)(const char*, const char*, int);
typedef cvar_t* (*Cvar_FindVar_t)(const char*);

extern Cvar_Set_t Cvar_Set;
extern Cvar_Get_t Cvar_Get;
extern Cvar_FindVar_t Cvar_FindVar;

char* Cvar_VariableString(const char*);
char* Cvar_VariableStringBuffer(const char* var_name, char* buffer, int bufsize);
int Cvar_VariableIntegerValue(const char* var_name);

void Q_strncpyz(char *dest, const char *src, int destsize);
void Com_sprintf(char *dest, int size, const char *fmt, ...);

static void(*Cmd_ArgvBuffer)(int, char*, int) = (void(*)(int, char*, int))0x4285E0; // UNUSED, WRONG OFFSET
char* Cmd_Argv(int index);
int Cmd_Argc();

void Info_SetValueForKey(char *s, const char *key, const char *value);
char *Info_ValueForKey(const char *s, const char *key);
void Info_NextPair(const char** head, char* key, char* value);

extern DWORD game_mp;
extern DWORD cgame_mp;

#define GAME_OFF(x) (game_mp + (x - 0x20000000))
#define CGAME_OFF(x) (cgame_mp + (x - 0x30000000))


/* net stuff */


#define EX_MASTER_NAME "xtnded.org"
extern netadr_t ex_master;
bool ex_master_resolve();

typedef void(*NET_OutOfBandPrint_t)(netsrc_t, netadr_t, const char*, ...);
extern NET_OutOfBandPrint_t NET_OutOfBandPrint;

typedef void(*NET_SendPacket_t)(netsrc_t,int,void*,netadr_t);
extern NET_SendPacket_t NET_SendPacket;

void Sys_SendPacket(int packet_size, void *packet, netadr_t to);

qboolean    NET_StringToAdr(const char *s, netadr_t *a);
const char  *NET_AdrToString(netadr_t a);


typedef void(__fastcall *CL_Netchan_Encode_t)(msg_t*);
extern CL_Netchan_Encode_t CL_Netchan_Encode;

typedef void(*Netchan_Transmit_t)(int chan, int length, const BYTE *data);
extern Netchan_Transmit_t Netchan_Transmit;

typedef void(*Netchan_TransmitNextFragment_t)(int chan);
extern Netchan_TransmitNextFragment_t Netchan_TransmitNextFragment;

typedef void(*MSG_initHuffman_t)(void);
extern MSG_initHuffman_t MSG_initHuffman;

//void MSG_Init(msg_t *buf, byte *data, int length);

char* trimSpaces(char* str);
char* Q_CleanStr(char* string, bool colors = false);

char* Com_CleanHostname(char* hostname, bool colors);
char* Com_CleanMapname(char* mapname);
const char* Com_GametypeName(char* gt, bool colors = false);
bool GetDesktopResolution(int* pHorizontal, int* pVertical);

#define PATCH_PUSH_STRING_PTR_VALUE(offset, new_str) \
	XUNLOCK((void*)offset, 10); \
	*(const char **)(offset + 1) = new_str;

#define PATCH_PUSH_INT_PTR_VALUE(offset, new_int) \
	XUNLOCK((void*)offset, 10); \
	*(int *)(offset + 1) = new_int;