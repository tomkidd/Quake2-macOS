/*
 * Copyright (C) 1997-2001 Id Software, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 * USA.
 *
 * =======================================================================
 *
 * Main header for the client
 *
 * =======================================================================
 */

#ifndef CL_CLIENT_H
#define CL_CLIENT_H

// 12/23/2001- increased this from 20
#define MAX_CLIENTWEAPONMODELS 64
#define	CMD_BACKUP 256 /* allow a lot of command backups for very fast systems */

/* the cl_parse_entities must be large enough to hold UPDATE_BACKUP frames of
   entities, so that when a delta compressed message arives from the server
   it can be un-deltad from the original */
#define    MAX_PARSE_ENTITIES    4096 //was 16384
//#define    MAX_PARSE_ENTITIES    1024

#define MAX_SUSTAINS		32
#define	PARTICLE_GRAVITY 40
#define BLASTER_PARTICLE_COLOR 0xe0
#define INSTANT_PARTICLE -10000.0
#define MIN_RAIL_LENGTH        1024
#define DEFAULT_RAIL_LENGTH    2048
#define DEFAULT_RAIL_SPACE    1

#define MIN_DECAL_LIFE 5

#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "../../common/header/common.h"

#include "../curl/header/download.h"
#include "../sound/header/sound.h"
#include "../sound/header/vorbis.h"
#include "../vid/header/ref.h"
#include "../vid/header/vid.h"

#include "screen.h"
#include "keyboard.h"
#include "console.h"

#include "cinematic.h"

//Knightmare added
#include "../../game/header/game.h"
trace_t SV_Trace (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, edict_t *passedict, int contentmask);
//end Knightmare

#define random()    ((rand () & 0x7fff) / ((float)0x7fff))
#define crandom()    (2.0 * (random() - 0.5))

//  added for Psychospaz's chasecam
vec3_t clientOrg; //lerped org of client for server->client side effects


int    color8red (int color8);
int    color8green (int color8);
int    color8blue (int color8);
//end Knightmare

typedef struct
{
	qboolean		valid; /* cleared if delta parsing was invalid */
	int				serverframe;
	int				servertime; /* server time the message is valid for (in msec) */
	int				deltaframe;
	byte			areabits[MAX_MAP_AREAS/8]; /* portalarea visibility bits */
	player_state_t	playerstate;
	int				num_entities;
	int				parse_entities; /* non-masked index into cl_parse_entities array */
} frame_t;

typedef struct
{
	entity_state_t	baseline; /* delta from this if not from a previous frame */
	entity_state_t	current;
	entity_state_t	prev; /* will always be valid, but might just be a copy of current */

	int			serverframe; /* if not current, this ent isn't in the frame */

	int			trailcount;	 /* for diminishing grenade trails */
	vec3_t		lerp_origin; /* for trails (variable hz) */

	int			fly_stoptime;
} centity_t;

typedef struct
{
	char	name[MAX_QPATH];
	char	cinfo[MAX_QPATH];

	struct image_s	*skin;

	struct image_s	*icon;
	char	iconname[MAX_QPATH];

	struct model_s	*model;

	struct model_s	*weaponmodel[MAX_CLIENTWEAPONMODELS];
} clientinfo_t;

extern char cl_weaponmodels[MAX_CLIENTWEAPONMODELS][MAX_QPATH];
extern int num_cl_weaponmodels;

/* the client_state_t structure is wiped
   completely at every server map change */
typedef struct
{
	int			timeoutcount;

	int			timedemo_frames;
	int			timedemo_start;

	qboolean	refresh_prepped; /* false if on new level or new ref dll */
	qboolean	sound_prepped; /* ambient sounds can start */
	qboolean	force_refdef; /* vid has changed, so we can't use a paused refdef */

	int			parse_entities; /* index (not anded off) into cl_parse_entities[] */

	usercmd_t	cmd;
	usercmd_t	cmds[CMD_BACKUP]; /* each mesage will send several old cmds */
	int			cmd_time[CMD_BACKUP]; /* time sent, for calculating pings */
#ifdef LARGE_MAP_SIZE // larger precision needed
    int            predicted_origins[CMD_BACKUP][3];    // for debug comparing against server
#else
	short		predicted_origins[CMD_BACKUP][3]; /* for debug comparing against server */
#endif
    
	float		predicted_step; /* for stair up smoothing */
	unsigned	predicted_step_time;

	vec3_t		predicted_origin; /* generated by CL_PredictMovement */
	vec3_t		predicted_angles;
	vec3_t		prediction_error;

	frame_t		frame; /* received from server */
	int			surpressCount; /* number of messages rate supressed */
	frame_t		frames[UPDATE_BACKUP];

	/* the client maintains its own idea of view angles, which are
	   sent to the server each frame.  It is cleared to 0 upon entering each level.
	   the server sends a delta each frame which is added to the locally
	   tracked view angles to account for standing on rotating objects,
	   and teleport direction changes */
	vec3_t		viewangles;

	int			time; /* this is the time value that the client is rendering at. always <= cls.realtime */
	float		lerpfrac; /* between oldframe and frame */

	refdef_t	refdef;

	vec3_t		v_forward, v_right, v_up; /* set when refdef.angles is set */

	/* transient data from server */
	char		layout[1024]; /* general 2D overlay */
	int			inventory[MAX_ITEMS];

	/* non-gameserver infornamtion */
	fileHandle_t cinematic_file;
	int			cinematictime; /* cls.realtime for first cinematic frame */
	int			cinematicframe;
	unsigned char	cinematicpalette[768];
	qboolean	cinematicpalette_active;

	/* server state information */
	qboolean	attractloop; /* running the attract loop, any key will menu */
	int			servercount; /* server identification for prespawns */
	char		gamedir[MAX_QPATH];
	int			playernum;

	char		configstrings[MAX_CONFIGSTRINGS][MAX_QPATH];

	/* locally derived information from server state */

	struct model_s	*model_draw[MAX_MODELS];

	struct cmodel_s	*model_clip[MAX_MODELS];

	struct sfx_s	*sound_precache[MAX_SOUNDS];

	struct image_s	*image_precache[MAX_IMAGES];

	clientinfo_t	clientinfo[MAX_CLIENTS];
	clientinfo_t	baseclientinfo;
} client_state_t;

extern	client_state_t	cl;

/* the client_static_t structure is persistant through
   an arbitrary number of server connections */
typedef enum
{
	ca_uninitialized,
	ca_disconnected,  /* not talking to a server */
	ca_connecting, /* sending request packets to the server */
	ca_connected, /* netchan_t established, waiting for svc_serverdata */
	ca_active /* game views should be displayed */
} connstate_t;

typedef enum
{
	dl_none,
	dl_model,
	dl_sound,
	dl_skin,
	dl_single
} dltype_t;

typedef enum {key_game, key_console, key_message, key_menu} keydest_t;

typedef struct
{
	connstate_t	state;
	keydest_t	key_dest;

    qboolean    consoleActive;
    
	int			framecount;
	int			realtime; /* always increasing, no clamping, etc */
	float		rframetime; /* seconds since last render frame */
	float		nframetime; /* network frame time */

	/* screen rendering information */
	float		disable_screen; /* showing loading plaque between levels */
								/* or changing rendering dlls */

	/* if time gets > 30 seconds ahead, break it */
	int			disable_servercount; /* when we receive a frame and cl.servercount */
									 /* > cls.disable_servercount, clear disable_screen */

	/* connection information */
	char		servername[256]; /* name of server from original connect */
	float		connect_time; /* for connection retransmits */

	int			quakePort; /* a 16 bit value that allows quake servers */
						   /* to work around address translating routers */
	netchan_t	netchan;
	int			serverProtocol; /* in case we are doing some kind of version hack */

	int			challenge; /* from the server to use for connecting */

	qboolean	forcePacket; /* Forces a package to be send at the next frame. */

	FILE		*download; /* file transfer from server */
	char		downloadtempname[MAX_OSPATH];
	char		downloadname[MAX_OSPATH];
	int			downloadnumber;
	dltype_t	downloadtype;
	size_t		downloadposition;
	int			downloadpercent;

	/* demo recording info must be here, so it isn't cleared on level change */
	qboolean	demorecording;
	qboolean	demowaiting; /* don't record until a non-delta message is received */
	FILE		*demofile;

#ifdef USE_CURL
	/* http downloading */
	dlqueue_t  downloadQueue; /* queues with files to download. */
	dlhandle_t HTTPHandles[MAX_HTTP_HANDLES]; /* download handles. */
	char	   downloadServer[512]; /* URL prefix to dowload from .*/
	char	   downloadServerRetry[512]; /* retry count. */
	char	   downloadReferer[32]; /* referer string. */
#endif

#ifdef    ROQ_SUPPORT
    // Cinematic information
    cinHandle_t        cinematicHandle;
#endif // ROQ_SUPPORT
    
} client_static_t;

extern client_static_t	cls;

/*Evil hack against too many power screen and power
  shield impact sounds. For example if the player
  fires his shotgun onto a Brain. */
extern int num_power_sounds;

/* cvars */
extern	cvar_t	*gl1_stereo_separation;
extern	cvar_t	*gl1_stereo_convergence;
extern	cvar_t	*gl1_stereo;
extern	cvar_t	*cl_gun;
extern    cvar_t    *cl_weapon_shells;
extern	cvar_t	*cl_add_blend;
extern	cvar_t	*cl_add_lights;
extern	cvar_t	*cl_add_particles;
extern	cvar_t	*cl_add_entities;
extern	cvar_t	*cl_predict;
extern	cvar_t	*cl_footsteps;
extern	cvar_t	*cl_noskins;
extern    cvar_t    *cl_blood;

// reduction factor for particle effects
extern    cvar_t    *cl_particle_scale;

// whether to adjust fov for wide aspect rattio
extern    cvar_t    *cl_widescreen_fov;

// whether to use texsurfs.txt footstep sounds
extern    cvar_t    *cl_footstep_override;

extern    cvar_t    *con_alpha; // Psychospaz's transparent console

// Psychospaz's changeable rail code
extern    cvar_t    *cl_railred;
extern    cvar_t    *cl_railgreen;
extern    cvar_t    *cl_railblue;
extern    cvar_t    *cl_railtype;
extern    cvar_t    *cl_rail_length;
extern    cvar_t    *cl_rail_space;

extern    cvar_t    *r_decals; // decal control
extern    cvar_t    *r_decal_life; // decal duration in seconds
extern    cvar_t    *con_font_size;
extern    cvar_t    *alt_text_color;

// Psychospaz's chasecam
extern    cvar_t    *cl_3dcam;
extern    cvar_t    *cl_3dcam_angle;
extern    cvar_t    *cl_3dcam_chase;
extern    cvar_t    *cl_3dcam_dist;
extern    cvar_t    *cl_3dcam_alpha;
extern    cvar_t    *cl_3dcam_adjust;

//Knightmare 12/28/2001- BramBo's FPS counter
extern    cvar_t    *cl_drawfps;

// whether to try to play OGGs instead of CD tracks
extern    cvar_t    *cl_ogg_music;
extern    cvar_t    *cl_rogue_music; // whether to play Rogue tracks
extern    cvar_t    *cl_xatrix_music; // whether to play Xatrix tracks
// end Knightmare

extern    cvar_t    *cl_servertrick;

extern	cvar_t	*cl_upspeed;
extern	cvar_t	*cl_forwardspeed;
extern	cvar_t	*cl_sidespeed;
extern	cvar_t	*cl_yawspeed;
extern	cvar_t	*cl_pitchspeed;
extern	cvar_t	*cl_run;
extern	cvar_t	*cl_anglespeedkey;
extern	cvar_t	*cl_shownet;
extern	cvar_t	*cl_showmiss;
extern	cvar_t	*cl_showclamp;
extern	cvar_t	*lookstrafe;
extern	cvar_t	*sensitivity;
extern    cvar_t    *menu_sensitivity;
extern    cvar_t    *menu_rotate;
extern    cvar_t    *menu_alpha;
extern    cvar_t    *hud_scale;
extern    cvar_t    *hud_width;
extern    cvar_t    *hud_height;
extern    cvar_t    *hud_alpha;

extern	cvar_t	*m_pitch;
extern	cvar_t	*m_yaw;
extern	cvar_t	*m_forward;
extern	cvar_t	*m_side;
extern	cvar_t	*freelook;
extern	cvar_t	*cl_lightlevel;
extern	cvar_t	*cl_paused;
extern	cvar_t	*cl_timedemo;

// Knightmare added
extern    cvar_t    *info_password;
extern    cvar_t    *info_spectator;
extern    cvar_t    *name;
extern    cvar_t    *skin;
extern    cvar_t    *rate;
//extern    cvar_t    *fov;
extern    cvar_t    *msg;
extern    cvar_t    *hand;
extern    cvar_t    *gender;
extern    cvar_t    *gender_auto;
// end Knightmare

extern	cvar_t	*cl_vwep;

// for the server to tell which version the client is
extern    cvar_t *cl_engine;
extern    cvar_t *cl_engine_version;

extern	cvar_t  *horplus;
extern	cvar_t	*cin_force43;

typedef struct
{
	int		key; /* so entities can reuse same entry */
	vec3_t	color;
	vec3_t	origin;
	float	radius;
	float	die; /* stop lighting after this time */
	float	decay; /* drop this each second */
	float	minlight; /* don't add when contributing less */
} cdlight_t;

extern	centity_t	cl_entities[MAX_EDICTS];
extern	cdlight_t	cl_dlights[MAX_DLIGHTS];

extern	entity_state_t	cl_parse_entities[MAX_PARSE_ENTITIES];

extern	netadr_t	net_from;
extern	sizebuf_t	net_message;

//void DrawString (int x, int y, char *s);
//void DrawStringScaled(int x, int y, char *s, float factor);
//void DrawAltString (int x, int y, char *s);    /* toggle high bit */
//void DrawAltStringScaled(int x, int y, char *s, float factor);
qboolean    CL_CheckOrDownloadFile (char *filename);

static float ClampCvar( float min, float max, float value );

// for use with the alt_text_color cvar
void TextColor (int colornum, int *red, int *green, int *blue);
qboolean StringSetParams (char modifier, int *red, int *green, int *blue, int *bold, int *shadow, int *italic, int *reset);
void Con_DrawString (int x, int y, char *s, int alpha);
void DrawStringGeneric (int x, int y, const char *string, int alpha, textscaletype_t scaleType, qboolean altBit);

//cl_scrn.c
typedef struct
{
    float x;
    float y;
    float avg;
} hudscale_t;

hudscale_t hudScale;

float scaledHud (float param);
float HudScale (void);
void InitHudScale (void);

void CL_AddNetgraph (void);

typedef struct cl_sustain
{
	int			id;
	int			type;
	int			endtime;
	int			nextthink;
	int			thinkinterval;
	vec3_t		org;
	vec3_t		dir;
	int			color;
	int			count;
	int			magnitude;
	void		(*think)(struct cl_sustain *self);
} cl_sustain_t;

void CL_ParticleSteamEffect2(cl_sustain_t *self);

void CL_TeleporterParticles (entity_state_t *ent);
void CL_ParticleEffect (vec3_t org, vec3_t dir, int color, int count);
void CL_ParticleEffect2 (vec3_t org, vec3_t dir, int color, int count);

void CL_ParticleEffect3 (vec3_t org, vec3_t dir, int color, int count);

// Psychospaz's enhanced particle code
void CL_ParticleEffectSplash (vec3_t org, vec3_t dir, int color, int count);
void CL_ElectricParticles (vec3_t org, vec3_t dir, int count);

// Psychospaz's mod detector
qboolean modType (char *name);
qboolean roguepath (void);
// utility function for protocol version
qboolean LegacyProtocol (void);

// Psychospaz's enhanced particle code
typedef struct
{
    qboolean    isactive;
    
    vec3_t        lightcol;
    float        light;
    float        lightvel;
} cplight_t;

#define P_LIGHTS_MAX 8
//end Knightmare

// Psychospaz's enhanced particle code
void SetParticleImages (void);

typedef struct particle_s
{
	struct particle_s	*next;
    
    cplight_t    lights[P_LIGHTS_MAX];
    
    float        start;
	float		time;

	vec3_t		org;
	vec3_t		vel;
	vec3_t		accel;
    
//    float        color;
//    float        colorvel;
    vec3_t        color;
    vec3_t        colorvel;

	float		alpha;
	float		alphavel;
    
    float        size;
    float        sizevel;
    
    vec3_t        angle;
    
    int            image;
    int            flags;
    
    vec3_t        oldorg;
    float        temp;
    int            src_ent;
    int            dst_ent;
    
#ifdef DECALS
    int                decalnum;
    decalpolys_t    *decal;
#endif
    struct particle_s    *link;
    
    void        (*think)(struct cparticle_t *p, vec3_t org, vec3_t angle, float *alpha, float *size, int *image, float *time);
    qboolean    thinknext;
} cparticle_t;

void CL_ClearEffects (void);
void CL_UnclipDecals (void); // Knightmare added
void CL_ReclipDecals (void); // Knightmare added
void CL_ClearTEnts (void);
void CL_BlasterTrail (vec3_t start, vec3_t end, int red, int green, int blue,
                      int reddelta, int greendelta, int bluedelta);

// Hyperblaster glows
void CL_HyperBlasterEffect (vec3_t start, vec3_t end, vec3_t angle, int red, int green, int blue,
                            int reddelta, int greendelta, int bluedelta, float len, float size);
void CL_HyperBlasterTrail (vec3_t start, vec3_t end, int red, int green, int blue, int reddelta, int greendelta, int bluedelta);
void CL_BlasterTracer (vec3_t origin, vec3_t angle, int red, int green, int blue, float len, float size);
//end Knightmare

void CL_QuadTrail (vec3_t start, vec3_t end);
void CL_RailTrail (vec3_t start, vec3_t end, qboolean isRed);
void CL_BubbleTrail (vec3_t start, vec3_t end);
void CL_FlagTrail (vec3_t start, vec3_t end, qboolean isred, qboolean isgreen);

void CL_IonripperTrail (vec3_t start, vec3_t end);

void vectoangles2 (vec3_t value1, vec3_t angles);

void CL_BlasterParticles (vec3_t org, vec3_t dir, int count, int red, int green, int blue,
                          int reddelta, int greendelta, int bluedelta);

void CL_BlasterParticles2 (vec3_t org, vec3_t dir, unsigned int color);
void CL_BlasterTrail2 (vec3_t start, vec3_t end);
void CL_DebugTrail (vec3_t start, vec3_t end);
void CL_SmokeTrail (vec3_t start, vec3_t end, int colorStart, int colorRun, int spacing);
void CL_Flashlight (int ent, vec3_t pos);
void CL_ForceWall (vec3_t start, vec3_t end, int color);
void CL_FlameEffects (centity_t *ent, vec3_t origin);
void CL_GenericParticleEffect (vec3_t org, vec3_t dir, int color, int count, int numcolors, int dirspread, float alphavel);
void CL_BubbleTrail2 (vec3_t start, vec3_t end, int dist);
void CL_HeatbeamParticles (vec3_t start, vec3_t end);
void CL_ParticleSteamEffect (vec3_t org, vec3_t dir, int red, int green, int blue,
                             int reddelta, int greendelta, int bluedelta, int count, int magnitude);
// Psychospaz's enhanced particle code
void CL_TrackerTrail (vec3_t start, vec3_t end);
void CL_Tracker_Explode(vec3_t origin);
// Psychospaz's enhanced particle code
void CL_TagTrail (vec3_t start, vec3_t end, int color8);
void CL_ColorFlash (vec3_t pos, int ent, float intensity, float r, float g, float b);
void CL_Tracker_Shell(vec3_t origin);
void CL_MonsterPlasma_Shell(vec3_t origin);
void CL_ColorExplosionParticles (vec3_t org, int color, int run);
// Psychospaz's enhanced particle code
void CL_ParticleSmokeEffect (vec3_t org, vec3_t dir, float size);
void CL_Widowbeamout (cl_sustain_t *self);
void CL_Nukeblast (cl_sustain_t *self);
void CL_WidowSplash (vec3_t org);

int CL_ParseEntityBits (unsigned *bits);
void CL_ParseDelta (entity_state_t *from, entity_state_t *to, int number, int bits);
void CL_ParseFrame (void);

void CL_ParseTEnt (void);
void CL_ParseConfigString (void);
void CL_PlayBackgroundTrack (void); // Knightmare added
void CL_AddMuzzleFlash (void);
void CL_AddMuzzleFlash2 (void);
void SmokeAndFlash(vec3_t origin);

void CL_SetLightstyle (int i);

void CL_RunParticles (void);
void CL_RunDLights (void);
void CL_RunLightStyles (void);

void CL_CalcViewValues(void);
void CL_AddEntities (void);
void CL_AddDLights (void);
void CL_AddTEnts (void);
void CL_AddLightStyles (void);

void CL_PrepRefresh (void);
void CL_RegisterSounds (void);

void CL_Quit_f (void);

void IN_Accumulate (void);

void CL_ParseLayout (void);

/*
 ====================================================================
 
 IMPORTED FUNCTIONS
 
 ====================================================================
 */

// called when the renderer is loaded
qboolean    R_Init ( void *hinstance, void *wndproc, char *reason );

// called before the renderer is unloaded
void    R_Shutdown (void);

// All data that will be used in a level should be
// registered before rendering any frames to prevent disk hits,
// but they can still be registered at a later time
// if necessary.
//
// EndRegistration will free any remaining data that wasn't registered.
// Any model_s or skin_s pointers from before the BeginRegistration
// are no longer valid after EndRegistration.
//
// Skins and images need to be differentiated, because skins
// are flood filled to eliminate mip map edge errors, and pics have
// an implicit "pics/" prepended to the name. (a pic name that starts with a
// slash will not use the "pics/" prefix or the ".pcx" postfix)
void    R_BeginRegistration (char *map);
struct model_s *R_RegisterModel (char *name);
struct image_s *R_RegisterSkin (char *name);
struct image_s *R_DrawFindPic (char *name);

void    R_FreePic (char *name); // Knightmare added
void    R_SetSky (char *name, float rotate, vec3_t axis);
void    R_EndRegistration (void);

void    R_RenderFrame (refdef_t *fd);

void    R_SetParticlePicture (int num, char *name); // Knightmare added

void    R_DrawGetPicSize (int *w, int *h, char *name);    // will return 0 0 if not found
void    R_DrawPic (int x, int y, char *name);
// added alpha for Psychospaz's transparent console
void    R_DrawStretchPic (int x, int y, int w, int h, char *name, float alpha);
void    R_DrawScaledPic (int x, int y, float scale, float alpha, char *name);
// added char scaling from Quake2Max
void    R_DrawChar (float x, float y, int c, float scale, int red, int green, int blue, int alpha, qboolean italic, qboolean last);
void    R_DrawTileClear (int x, int y, int w, int h, char *name);
void    R_DrawFill (int x, int y, int w, int h, int c);
void    R_DrawFill2 (int x, int y, int w, int h, int red, int green, int blue, int alpha);
void    R_DrawFadeScreen (void);

void    R_GrabScreen (void); // screenshots for savegames
void    R_ScaledScreenshot (char *name); //  screenshots for savegames

#ifdef DECALS
int        R_MarkFragments (const vec3_t origin, const vec3_t axis[3], float radius, int maxPoints, vec3_t *points, int maxFragments, markFragment_t *fragments);
#endif

void    R_SetFogVars (qboolean enable, int model, int density, int start, int end, int red, int green, int blue);

float    R_CharMapScale (void); // Knightmare added char scaling from Quake2Max

// Draw images for cinematic rendering (which can have a different palette). Note that calls
#ifdef ROQ_SUPPORT
void    R_DrawStretchRaw (int x, int y, int w, int h, const byte *raw, int rawWidth, int rawHeight);
#else
void    R_DrawStretchRaw (int x, int y, int w, int h, int cols, int rows, byte *data);
#endif // ROQ_SUPPORT

/*
 ** video mode and refresh state management entry points
 */
void    R_SetPalette (const unsigned char *palette);    // NULL = game palette
void    R_BeginFrame (float camera_separation);
void    GLimp_EndFrame (void);

void    GLimp_AppActivate (qboolean activate);



void CL_Init (void);

void CL_FixUpGender(void);
void CL_Disconnect (void);
void CL_Disconnect_f (void);
void CL_GetChallengePacket (void);
void CL_PingServers_f (void);
void CL_Snd_Restart_f (void);
void CL_RequestNextDownload (void);
void CL_ResetPrecacheCheck (void);

typedef struct
{
	int			down[2]; /* key nums holding it down */
	unsigned	downtime; /* msec timestamp */
	unsigned	msec; /* msec down this frame */
	int			state;
} kbutton_t;

extern	kbutton_t	in_mlook, in_klook;
extern 	kbutton_t 	in_strafe;
extern 	kbutton_t 	in_speed;

void CL_InitInput (void);
void CL_RefreshCmd(void);
void CL_SendCmd (void);
void CL_RefreshMove(void);
void CL_SendMove (usercmd_t *cmd);

void CL_ClearState (void);

void CL_ReadPackets (void);

int  CL_ReadFromServer (void);
void CL_WriteToServer (usercmd_t *cmd);
void CL_BaseMove (usercmd_t *cmd);

void IN_CenterView (void);

float CL_KeyState (kbutton_t *key);
char *Key_KeynumToString (int keynum);

void CL_WriteDemoMessage (void);
void CL_Stop_f (void);
void CL_Record_f (void);

extern	char *svc_strings[256];

void CL_ParseServerMessage (void);
void CL_LoadClientinfo (clientinfo_t *ci, char *s);
void SHOWNET(char *s);
void CL_ParseClientinfo (int player);
void CL_Download_f (void);

//
// cl_download.c
//
void CL_RequestNextDownload (void);
qboolean CL_CheckOrDownloadFile (char *filename);
void CL_Download_f (void);
void CL_ParseDownload (void);

extern	int			gun_frame;

extern	struct model_s	*gun_model;

qboolean loadingMessage;

char loadingMessages[96];

float loadingPercent;

void V_Init (void);
void V_RenderView( float stereo_separation );
void V_AddEntity (entity_t *ent);
// Psychospaz's enhanced particle code
void V_AddParticle (vec3_t org, vec3_t angle, vec3_t color, float alpha,
                    int alpha_src, int alpha_dst, float size, int image, int flags);
#ifdef DECALS
void V_AddDecal (vec3_t org, vec3_t angle, vec3_t color, float alpha,
                 int alpha_src, int alpha_dst, float size, int image, int flags, decalpolys_t *decal);
#endif

void V_AddLight (vec3_t org, float intensity, float r, float g, float b);
void V_AddLightStyle (int style, float r, float g, float b);

void CL_RegisterTEntSounds (void);
void CL_RegisterTEntModels (void);
void CL_SmokeAndFlash(vec3_t origin);


void CL_InitPrediction (void);
void CL_PredictMove (void);
void CL_CheckPredictionError (void);
//Knightmare added
trace_t CL_Trace (vec3_t start, vec3_t end, float size,  int contentmask);
trace_t CL_BrushTrace (vec3_t start, vec3_t end, float size,  int contentmask);
trace_t CL_PMTrace (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end);
trace_t CL_PMSurfaceTrace (int playernum, vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, int contentmask);

cdlight_t *CL_AllocDlight (int key);
void CL_BigTeleportParticles (vec3_t org);
void CL_RocketTrail (vec3_t start, vec3_t end, centity_t *old);
void CL_DiminishingTrail (vec3_t start, vec3_t end, centity_t *old, int flags);
void CL_FlyEffect (centity_t *ent, vec3_t origin);
void CL_BfgParticles (entity_t *ent);
void CL_AddParticles (void);
void CL_EntityEvent (entity_state_t *ent);
void CL_TrapParticles (entity_t *ent);

void UI_Init (void);
void UI_Keydown (int key);
void UI_Draw (void);
void UI_ForceMenuOff (void);
void UI_AddToServerList (netadr_t adr, char *info);
void M_Menu_Main_f (void);

void CL_ParseInventory (void);
void CL_KeyInventory (int key);
void CL_DrawInventory (void);

void CL_PredictMovement (void);
trace_t CL_PMTrace(vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end);

#endif
