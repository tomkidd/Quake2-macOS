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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * =======================================================================
 *
 * Local header for the refresher.
 *
 * =======================================================================
 */

#ifndef REF_LOCAL_H
#define REF_LOCAL_H

#include <stdio.h>
#include <ctype.h>
// come back to this -tkidd
//#include "glext.h" //Knightmare- MrG's shader waterwarp support
#include <math.h>

#include "../../ref_shared.h"
#include "qgl.h"


#ifndef GL_COLOR_INDEX8_EXT
 #define GL_COLOR_INDEX8_EXT GL_COLOR_INDEX
#endif

#ifndef GL_EXT_texture_filter_anisotropic
 #define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
 #define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#endif

#ifndef GL_VERSION_1_3
 #define GL_TEXTURE0 0x84C0
 #define GL_TEXTURE1 0x84C1
#endif

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

#ifndef GL_MULTISAMPLE_FILTER_HINT_NV
#define GL_MULTISAMPLE_FILTER_HINT_NV 0x8534
#endif

#define TEXNUM_LIGHTMAPS 1024
#define TEXNUM_SCRAPS 1152
#define TEXNUM_IMAGES 1153
#define    MAX_GLTEXTURES    4096 // Knightmare increased, was 1024
#define MAX_SCRAPS 1
#define BLOCK_WIDTH 128
#define BLOCK_HEIGHT 128
#define REF_VERSION "Yamagi Quake II OpenGL Refresher"
#define BACKFACE_EPSILON 0.01
#define LIGHTMAP_BYTES 4
#define MAX_LIGHTMAPS 128
#define GL_LIGHTMAP_FORMAT GL_RGBA

/* up / down */
#define PITCH 0

/* left / right */
#define YAW 1

/* fall over */
#define ROLL 2

extern viddef_t vid;


enum stereo_modes {
	STEREO_MODE_NONE,
	STEREO_MODE_OPENGL,
	STEREO_MODE_ANAGLYPH,
	STEREO_MODE_ROW_INTERLEAVED,
	STEREO_MODE_COLUMN_INTERLEAVED,
	STEREO_MODE_PIXEL_INTERLEAVED,
	STEREO_SPLIT_HORIZONTAL,
	STEREO_SPLIT_VERTICAL,
};

enum opengl_special_buffer_modes {
	OPENGL_SPECIAL_BUFFER_MODE_NONE,
	OPENGL_SPECIAL_BUFFER_MODE_STEREO,
	OPENGL_SPECIAL_BUFFER_MODE_STENCIL,
};

typedef struct image_s
{
	char name[MAX_QPATH];               /* game path, including extension */
	imagetype_t type;
	int width, height;                  /* source image */
	int upload_width, upload_height;    /* after power of two and picmip */
	int registration_sequence;          /* 0 = free */
	struct msurface_s *texturechain;    /* for sort-by-texture world drawing */
	int texnum;                         /* gl texture binding */
	float sl, tl, sh, th;               /* 0,0 - 1,1 unless part of the scrap */
	qboolean scrap;
	qboolean has_alpha;

	qboolean paletted;

    qboolean    is_cin;                    // Heffo - To identify a cin texture's image_t
    float        replace_scale_w;            // Knightmare- for scaling hi-res replacement images
    float        replace_scale_h;            // Knightmare- for scaling hi-res replacement images
} image_t;

//Harven MD3 ++
// commenting out for now -tkidd
#include "r_alias.h" // needs image_t struct
//Harven MD3 --


typedef enum
{
	rserr_ok,

	rserr_invalid_fullscreen,
	rserr_invalid_mode,

	rserr_unknown
} rserr_t;

#include "model.h"

void GL_BeginRendering(int *x, int *y, int *width, int *height);
void GL_EndRendering(void);

void GL_SetDefaultState( void );

void R_SetDefaultState(void);

extern float gldepthmin, gldepthmax;

typedef struct
{
	float x, y, z;
	float s, t;
	float r, g, b;
} glvert_t;

extern image_t gltextures[MAX_GLTEXTURES];
extern int numgltextures;

#define PARTICLE_TYPES 256

extern image_t *r_notexture;
#ifdef    ROQ_SUPPORT
extern    image_t        *r_rawtexture;
#endif // ROQ_SUPPORT

extern    image_t        *r_envmappic;
extern    image_t        *r_spheremappic;
extern    image_t        *r_causticpic;
extern    image_t        *r_shelltexture;
extern    image_t        *r_particlebeam;

//Knightmare- Psychospaz's enhanced particles
extern    image_t        *r_particletextures[PARTICLE_TYPES];

extern entity_t *currententity;
extern    int            r_worldframe; // Knightmare added for trans animations
extern model_t *currentmodel;
extern int r_visframecount;
extern int r_framecount;
extern cplane_t frustum[4];
extern int c_brush_polys, c_alias_polys;

// Knightmare- saveshot buffer
extern    byte    *saveshotdata;

extern int gl_filter_min, gl_filter_max;

/* view origin */
extern vec3_t vup;
extern vec3_t vpn;
extern vec3_t vright;
extern vec3_t r_origin;

/* screen size info */
extern refdef_t r_newrefdef;
extern int r_viewcluster, r_viewcluster2, r_oldviewcluster, r_oldviewcluster2;

extern    cvar_t    *gl_clear;
extern  cvar_t  *gl_driver;

extern cvar_t *r_norefresh;
extern cvar_t *gl_lefthand;
extern cvar_t *r_gunfov;
extern cvar_t *r_farsee;
extern cvar_t *r_drawentities;
extern cvar_t *r_drawworld;
extern cvar_t *r_speeds;
extern cvar_t *r_fullbright;
extern cvar_t *r_novis;
extern cvar_t *r_lerpmodels;
extern    cvar_t    *r_ignorehwgamma; // Knightmare- hardware gamma

extern    cvar_t    *r_waterwave;    // Knightmare- water waves
extern    cvar_t  *r_caustics;    // Barnes water caustics
extern    cvar_t  *r_glows;        // texture glows
extern    cvar_t    *r_saveshotsize;// Knightmare- save shot size option

// Knightmare- lerped dlights on models
extern    cvar_t    *r_model_shading;
extern    cvar_t    *r_model_dlights;

extern cvar_t *r_lightlevel;

// Knightmare- added Vic's overbright rendering
extern    cvar_t    *r_overbrightbits;

// Knightmare- added Psychospaz's console font size option
extern    cvar_t    *con_font;
extern    cvar_t    *con_font_size;

extern cvar_t *gl1_overbrightbits;

// Knigntmare- Psychospaz's chasecam
extern    cvar_t    *cl_3dcam;
extern    cvar_t    *cl_3dcam_angle;
extern    cvar_t    *cl_3dcam_dist;
extern    cvar_t    *cl_3dcam_alpha;
extern    cvar_t    *cl_3dcam_adjust;
// end Knightmare

extern cvar_t *gl1_palettedtexture;
extern cvar_t *gl1_pointparameters;

// this next group is stuff from kmq2. Not sure what all is used -tkidd
extern    cvar_t    *r_ext_swapinterval;
extern    cvar_t    *r_ext_multitexture;
extern    cvar_t    *r_ext_draw_range_elements;
extern    cvar_t    *r_ext_compiled_vertex_array;

extern    cvar_t    *r_ext_mtexcombine; // Vic's overbright rendering
extern    cvar_t    *r_stencilTwoSide; // Echon's two-sided stenciling
extern    cvar_t    *r_arb_vertex_buffer_object;
extern    cvar_t    *r_pixel_shader_warp; // allow disabling the nVidia water warp
extern    cvar_t    *r_trans_lightmaps; // allow disabling of lightmaps on trans and warp surfaces
extern    cvar_t    *r_solidalpha;            // allow disabling of trans33+trans66 surface flag combining

extern    cvar_t    *r_glass_envmaps; // Psychospaz's envmapping
extern    cvar_t    *r_trans_surf_sorting; // trans bmodel sorting
extern    cvar_t    *r_shelltype; // entity shells: 0 = solid, 1 = warp, 2 = spheremap

extern    cvar_t    *r_ext_texture_compression; // Heffo - ARB Texture Compression
extern    cvar_t    *r_lightcutoff;    //** DMP - allow dynamic light cutoff to be user-settable

extern    cvar_t    *r_screenshot_jpeg;            // Heffo - JPEG Screenshots
extern    cvar_t    *r_screenshot_jpeg_quality;    // Heffo - JPEG Screenshots

extern    cvar_t    *r_nosubimage;
extern    cvar_t    *r_bitdepth;
extern    cvar_t    *r_mode;
extern    cvar_t    *r_log;
extern    cvar_t    *r_lightmap;
extern    cvar_t    *r_shadows;
extern    cvar_t    *r_shadowalpha;
extern    cvar_t    *r_shadowrange;
extern    cvar_t    *r_shadowvolumes;
extern    cvar_t    *r_stencil; //  stenciling for color shells
extern    cvar_t    *r_transrendersort; // correct trasparent sorting
extern    cvar_t    *r_particle_lighting; //  particle lighting
extern    cvar_t    *r_particle_min;
extern    cvar_t    *r_particle_max;
extern    cvar_t    *r_particledistance;

extern    cvar_t    *r_dynamic;
extern    cvar_t    *r_monolightmap;
extern    cvar_t    *r_nobind;
extern    cvar_t    *r_round_down;
extern    cvar_t    *r_picmip;
extern    cvar_t    *r_skymip;
extern    cvar_t    *r_playermip;
extern    cvar_t    *r_showtris;
extern    cvar_t    *r_showbbox;    // Knightmare- show model bounding box
extern    cvar_t    *r_finish;
extern    cvar_t    *r_ztrick;
extern    cvar_t    *r_cull;
extern    cvar_t    *r_polyblend;
extern    cvar_t    *r_flashblend;
extern    cvar_t    *r_lightmaptype;
extern    cvar_t    *r_modulate;
extern    cvar_t    *r_drawbuffer;
extern    cvar_t    *r_3dlabs_broken;
extern    cvar_t    *r_swapinterval;
extern    cvar_t    *r_anisotropic;
extern    cvar_t    *r_anisotropic_avail;
extern    cvar_t    *r_texturemode;
extern    cvar_t    *r_texturealphamode;
extern    cvar_t    *r_texturesolidmode;
extern  cvar_t  *r_saturatelighting;
extern  cvar_t  *r_lockpvs;
extern    cvar_t    *r_intensity;

extern    cvar_t    *r_skydistance; //Knightmare- variable sky range
extern    cvar_t    *r_saturation;    //** DMP

#ifdef LIGHT_BLOOMS
extern  cvar_t  *r_bloom;
#endif


extern cvar_t *gl1_particle_min_size;
extern cvar_t *gl1_particle_max_size;
extern cvar_t *gl1_particle_size;
extern cvar_t *gl1_particle_att_a;
extern cvar_t *gl1_particle_att_b;
extern cvar_t *gl1_particle_att_c;

extern cvar_t *r_mode;
extern cvar_t *r_customwidth;
extern cvar_t *r_customheight;

extern cvar_t *gl_retexturing;

extern cvar_t *gl_nolerp_list;

extern cvar_t *gl_lightmap;
extern cvar_t *gl_shadows;
extern cvar_t *gl1_stencilshadow;
extern cvar_t *gl1_dynamic;
extern cvar_t *gl_nobind;
extern cvar_t *gl1_round_down;
extern cvar_t *gl1_picmip;
extern cvar_t *gl_showtris;
extern cvar_t *gl_showbbox;
extern cvar_t *gl_finish;
extern cvar_t *gl1_ztrick;
extern cvar_t *gl_zfix;
extern cvar_t *r_clear;
extern cvar_t *gl_cull;
extern cvar_t *gl1_polyblend;
extern cvar_t *gl1_flashblend;
extern cvar_t *r_modulate;
extern cvar_t *gl_drawbuffer;
extern cvar_t *r_vsync;
extern cvar_t *gl_anisotropic;
extern cvar_t *gl_texturemode;
extern cvar_t *gl1_texturealphamode;
extern cvar_t *gl1_texturesolidmode;
extern cvar_t *gl1_saturatelighting;
extern cvar_t *r_lockpvs;
extern cvar_t *gl_msaa_samples;

extern cvar_t *vid_fullscreen;
extern cvar_t *vid_gamma;

extern cvar_t *intensity;

extern int gl_solid_format;
extern int gl_alpha_format;
extern int gl_tex_solid_format;
extern int gl_tex_alpha_format;

extern int c_visible_lightmaps;
extern int c_visible_textures;

extern float r_world_matrix[16];


// entity sorting struct
typedef struct sortedelement_s sortedelement_t;
typedef struct sortedelement_s
{
    void  *data;
    vec_t len;
    vec3_t org;
    sortedelement_t *left, *right;
};

void R_TranslatePlayerSkin(int playernum);

// Knightmare- added some of Psychospaz's shortcuts
void ElementAddNode (sortedelement_t *base, sortedelement_t *thisElement);
int transCompare (const void *arg1, const void *arg2);

void R_MaxColorVec (vec3_t color);

//
// r_entity.c
//
extern    sortedelement_t *ents_trans;
extern    sortedelement_t *ents_viewweaps;
extern    sortedelement_t *ents_viewweaps_trans;

void R_DrawEntitiesOnList (sortedelement_t *list);
void R_DrawAllEntities (qboolean addViewWeaps);
void R_DrawAllEntityShadows (void);
void R_DrawSolidEntities ();

//
// r_particle.c
//
extern    sortedelement_t *parts_prerender;
void R_BuildParticleList (void);
void R_SortParticlesOnList (void);
void R_DrawParticles (sortedelement_t *list);
void R_DrawAllParticles (void);
void R_DrawDecals (void);
void R_DrawAllDecals (void);



void R_Bind(int texnum);

void R_TexEnv(GLenum value);

//void R_LightPoint (vec3_t p, vec3_t color);
void R_LightPoint (vec3_t p, vec3_t color, qboolean isEnt);
void R_LightPointDynamics (vec3_t p, vec3_t color, m_dlight_t *list, int *amount, int max);
void R_PushDlights(void);
void R_ShadowLight (vec3_t pos, vec3_t lightAdd);
void R_MarkLights (dlight_t *light, int bit, mnode_t *node);

extern model_t *r_worldmodel;
extern unsigned d_8to24table[256];
extern    float        d_8to24tablef[256][3]; //Knightmare added

extern int registration_sequence;

void V_AddBlend(float r, float g, float b, float a, float *v_blend);

void R_RenderView(refdef_t *fd);
void R_BeginFrame( float camera_separation );
void R_SwapBuffers( int );
void R_SetPalette ( const unsigned char *palette);
void R_ScreenShot(void); //R_ScreenShot_f
void R_DrawAliasMD2Model (entity_t *e);
void R_DrawAliasMD2ModelShadow (entity_t *e);
//Harven++ MD3
void R_DrawAliasModel(entity_t *e);
void R_DrawAliasModelShadow (entity_t *e);
//Harven-- MD3
void R_DrawBrushModel(entity_t *e);
void R_DrawSpriteModel(entity_t *e);
void R_DrawBeam(entity_t *e);
void R_DrawWorld(void);
void R_RenderDlights(void);
void R_DrawAlphaSurfaces(void);
void R_RenderBrushPoly(msurface_t *fa);
void R_InitParticleTextures(void);
void Draw_InitLocal(void); //R_DrawInitLocal
void R_SubdivideSurface(msurface_t *fa);
qboolean R_CullBox(vec3_t mins, vec3_t maxs);
void R_RotateForEntity(entity_t *e, qboolean full);
void R_MarkLeaves(void);

//
// r_alias_misc.c
//
//#define NUMVERTEXNORMALS    162
extern    float    r_avertexnormals[NUMVERTEXNORMALS][3];

// precalculated dot products for quantized angles
#define SHADEDOT_QUANT 16
extern    float    r_avertexnormal_dots[SHADEDOT_QUANT][256];

extern    float    *shadedots;

#define MAX_MODEL_DLIGHTS 32
extern    m_dlight_t    model_dlights[MAX_MODEL_DLIGHTS];
extern    int            model_dlights_num;

extern    vec3_t    shadelight;
extern    vec3_t    lightspot;
extern    float    shellFlowH, shellFlowV;

void    R_ShadowLight (vec3_t pos, vec3_t lightAdd);
void    R_SetShellBlend (qboolean toggle);
void    R_SetVertexOverbrights (qboolean toggle);
qboolean FlowingShell (void);
float    R_CalcEntAlpha (float alpha, vec3_t point);
float    R_CalcShadowAlpha (entity_t *e);
void    R_ShadowBlend (float alpha);
void    R_FlipModel (qboolean on);
void    R_SetBlendModeOn (image_t *skin);
void    R_SetBlendModeOff (void);
void    R_SetShadeLight (void);
void    R_DrawAliasModelBBox (vec3_t bbox[8], entity_t *e);

//
// r_backend.c
//
// Knightmare- MrG's Vertex array stuff
#define MAX_TEXTURE_UNITS    2 // 4
#define MAX_VERTICES    16384
#define MAX_INDICES        MAX_VERTICES * 4

/*
 typedef struct {
 unsigned        indexBuffer;
 unsigned        vertexBuffer;
 //    unsigned        normalBuffer;
 unsigned        colorBuffer;
 unsigned        texCoordBuffer[MAX_TEXTURE_UNITS];
 } vbo_t;
 
 #define VBO_OFFSET(i)        ((char *)NULL + (i))
 extern vbo_t        rb_vbo;
 */

#define VA_SetElem2(v,a,b)        ((v)[0]=(a),(v)[1]=(b))
#define VA_SetElem3(v,a,b,c)    ((v)[0]=(a),(v)[1]=(b),(v)[2]=(c))
#define VA_SetElem4(v,a,b,c,d)    ((v)[0]=(a),(v)[1]=(b),(v)[2]=(c),(v)[3]=(d))

#define VA_SetElem2v(v,a)    ((v)[0]=(a)[0],(v)[1]=(a)[1])
#define VA_SetElem3v(v,a)    ((v)[0]=(a)[0],(v)[1]=(a)[1],(v)[2]=(a)[2])
#define VA_SetElem4v(v,a)    ((v)[0]=(a)[0],(v)[1]=(a)[1],(v)[2]=(a)[2],(v)[3]=(a)[3])


extern unsigned    indexArray[MAX_INDICES];
extern float    texCoordArray[MAX_TEXTURE_UNITS][MAX_VERTICES][2];
//extern float    inTexCoordArray[MAX_VERTICES][2];
extern float    vertexArray[MAX_VERTICES][3];
extern float    colorArray[MAX_VERTICES][4];
extern unsigned rb_vertex, rb_index;
// end vertex array stuff

void RB_InitBackend (void);
float RB_CalcGlowColor (renderparms_t parms);
void RB_ModifyTextureCoords (float *inArray, float *vertexArray, int numVerts, renderparms_t parms);
void RB_CheckArrayOverflow (GLenum polyMode, int numVerts, int numIndex);
void RB_DrawArrays (GLenum polyMode);
void RB_DrawMeshTris (GLenum polyMode, int numTMUs);


//glpoly_t *WaterWarpPolyVerts(glpoly_t *p);
void R_EmitWaterPolys(msurface_t *fa, float alpha, qboolean light);
void R_InitDSTTex (void);
void R_AddSkySurface(msurface_t *fa);
void R_ClearSkyBox(void);
void R_DrawSkyBox(void);
void R_MarkLights(dlight_t *light, int bit, mnode_t *node);

//
// r_bloom.c
//
#ifdef LIGHT_BLOOMS
void R_BloomBlend( refdef_t *fd );
void R_InitBloomTextures( void );
#endif

void COM_StripExtension(char *in, char *out);

void    R_InitChars (void);

void R_SwapBuffers(int);

int Draw_GetPalette(void);

//void R_ResampleTexture(unsigned *in, int inwidth, int inheight,
//        unsigned *out, int outwidth, int outheight);
void R_ResampleTexture (void *indata, int inwidth, int inheight, void *outdata,  int outwidth, int outheight); //GL_ResampleTexture

image_t *R_LoadPic (char *name, byte *pic, int width, int height, imagetype_t type, int bits);
image_t *R_FindImage(char *name, imagetype_t type);
void R_TextureMode(char *string);
void R_ImageList_f(void);

void R_SetTexturePalette(unsigned palette[256]);

void R_InitFailedImgList (void);
void R_InitImages (void);
void R_ShutdownImages (void);
void R_FreeUnusedImages (void);

void R_InitImages(void);
void R_ShutdownImages(void);

void R_FreeUnusedImages(void);

void R_TextureAlphaMode(char *string);
void R_TextureSolidMode(char *string);
int Scrap_AllocBlock(int w, int h, int *x, int *y);

/* GL extension emulation functions */
void GL_DrawParticles( int num_particles );
void R_DrawParticles2(int n,
		const particle_t particles[],
		const unsigned colortable[768]);

//
// r_fog.c
//
void R_SetFog (void);
void R_InitFogVars (void);
void R_SetFogVars (qboolean enable, int model, int density,
                   int start, int end, int red, int green, int blue);

/*
 * GL config stuff
 */

enum {
    GLREND_DEFAULT        = 1 << 0,
    GLREND_MCD            = 1 << 1,
    
    GLREND_3DLABS        = 1 << 2,
    GLREND_GLINT_MX        = 1 << 3,
    GLREND_PCX1            = 1 << 4,
    GLREND_PCX2            = 1 << 5,
    GLREND_PERMEDIA2    = 1 << 6,
    GLREND_PMX            = 1 << 7,
    GLREND_POWERVR        = 1 << 8,
    GLREND_REALIZM        = 1 << 9,
    GLREND_RENDITION    = 1 << 10,
    GLREND_SGI            = 1 << 11,
    GLREND_SIS            = 1 << 12,
    GLREND_VOODOO        = 1 << 13,
    
    GLREND_NVIDIA        = 1 << 14,
    GLREND_GEFORCE        = 1 << 15,
    
    GLREND_ATI            = 1 << 16,
    GLREND_RADEON        = 1 << 17
};


typedef struct
{
    int         rendType;
	const char *renderer_string;
	const char *vendor_string;
	const char *version_string;
	const char *extensions_string;

	int major_version;
	int minor_version;

    qboolean    allowCDS;
    // max texture size
    int            max_texsize;
    int            max_texunits;

    // ----

    qboolean    vertexBufferObject;
    qboolean    multitexture;
    qboolean    mtexcombine;    // added Vic's overbright rendering
    
    qboolean    have_stencil;
    qboolean    extStencilWrap;
    qboolean    atiSeparateStencil;
    qboolean    extStencilTwoSide;
    
    qboolean    extCompiledVertArray;
    qboolean    drawRangeElements;
    
    // texture shader support
    qboolean    NV_texshaders;
    //qboolean    ATI_fragshaders;
    
    // anisotropic filtering
	qboolean anisotropic;
	qboolean npottextures;
	qboolean palettedtexture;
	qboolean pointparameters;

	// ----

	float max_anisotropy;
} glconfig_t;

typedef struct
{
	float inverse_intensity;
	qboolean fullscreen;

	int prev_mode;

	unsigned char *d_16to8table;

	int lightmap_textures;

	int currenttextures[MAX_TEXTURE_UNITS];
    unsigned int    currenttmu;
	GLenum currenttarget;

	float camera_separation;
	enum stereo_modes stereo_mode;

    // advanced state manager - MrG
    qboolean        texgen;
    
    qboolean        regCombiners;
    qboolean        sgis_mipmap;
    unsigned int    dst_texture;
    
    qboolean        envAdd;
    qboolean        stencilEnabled;
    
    qboolean        gammaRamp;
    
    qboolean        cullFace;
    qboolean        polygonOffsetFill; // Knightmare added
    qboolean        vertexProgram;
    qboolean        fragmentProgram;
    qboolean        alphaTest;
    qboolean        blend;
    qboolean        stencilTest;
    qboolean        depthTest;
    qboolean        scissorTest;
    
    qboolean        texRectangle;
    qboolean        in2d;
    qboolean        arraysLocked;
    // End - MrG
    
    GLenum            cullMode;
    GLenum            shadeModelMode;
    GLfloat            depthMin;
    GLfloat            depthMax;
    GLfloat            offsetFactor;
    GLfloat            offsetUnits;
    GLenum            alphaFunc;
    GLclampf        alphaRef;
    GLenum            blendSrc;
    GLenum            blendDst;
    GLenum            depthFunc;
    GLboolean        depthMask;
    
    qboolean        texture_compression; // Heffo - ARB Texture Compression
    
	qboolean stencil;
} glstate_t;

typedef struct
{
	int internal_format;
	int current_lightmap_texture;

	msurface_t *lightmap_surfaces[MAX_LIGHTMAPS];

	int allocated[BLOCK_WIDTH];

	/* the lightmap texture data needs to be kept in
	   main memory so texsubimage can update properly */
	byte lightmap_buffer[4 * BLOCK_WIDTH * BLOCK_HEIGHT];
} gllightmapstate_t;

extern glconfig_t gl_config;
extern glstate_t gl_state;

/*
 * Updates the gamma ramp.
 */
void RI_UpdateGamma(void);

/*
 * Enables or disabes the vsync.
 */
void RI_SetVsync(void);

/*
 * Shuts the GL context down.
 */
void RI_ShutdownContext(void);

/*
 * Returns the address of the GL function proc,
 * or NULL if the function is not found.
 */
void *RI_GetProcAddress (const char* proc);

//
// r_glstate.c
//
void    GL_Stencil (qboolean enable, qboolean shell);
void    R_ParticleStencil (int passnum);
qboolean GL_HasStencil (void);

void    GL_Enable (GLenum cap);
void    GL_Disable (GLenum cap);
void    GL_ShadeModel (GLenum mode);
void    GL_TexEnv (GLenum value);
void    GL_CullFace (GLenum mode);
void    GL_PolygonOffset (GLfloat factor, GLfloat units);
void    GL_AlphaFunc (GLenum func, GLclampf ref);
void    GL_BlendFunc (GLenum src, GLenum dst);
void    GL_DepthFunc (GLenum func);
void    GL_DepthMask (GLboolean mask);
void    GL_DepthRange (GLfloat rMin, GLfloat rMax);
void    GL_LockArrays (int numVerts);
void    GL_UnlockArrays (void);
void    GL_EnableTexture (unsigned tmu);
void    GL_DisableTexture (unsigned tmu);
void    GL_EnableMultitexture(qboolean enable);
void    GL_SelectTexture (unsigned tmu);
void    GL_Bind (int texnum);
void    GL_MBind (unsigned tmu, int texnum);
void    GL_SetDefaultState (void);

/*
 ====================================================================
 
 IMPORTED FUNCTIONS
 
 ====================================================================
 */

void    VID_Error (int err_level, char *str, ...);

void    SetParticleImages (void); // Knightmare added

void    Cmd_AddCommand (char *name, void(*cmd)(void));
void    Cmd_RemoveCommand (char *name);
int        Cmd_Argc (void);
char    *Cmd_Argv (int i);
void    Cbuf_ExecuteText (int exec_when, char *text);

void    VID_Printf (int print_level, char *str, ...);

// files will be memory mapped read only
// the returned buffer may be part of a larger pak file,
// or a discrete file from anywhere in the quake search path
// a -1 return means the file does not exist
// NULL can be passed for buf to just determine existance
char    **FS_ListPak (char *find, int *num);
int        FS_LoadFile (char *name, void **buf);
void    FS_FreeFile (void *buf);

// gamedir will be the current directory that generated
// files should be stored to, ie: "f:\quake\id1"
char    *FS_Gamedir (void);

cvar_t    *Cvar_Get (char *name, char *value, int flags);
cvar_t    *Cvar_Set (char *name, char *value );
void     Cvar_SetValue (char *name, float value);

qboolean    VID_GetModeInfo (int *width, int *height, int mode);
void        VID_NewWindow (int width, int height);
// Knightmare- added import of text color for renderer
void        TextColor (int colornum, int *red, int *green, int *blue);


#endif
