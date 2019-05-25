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
 * This file implements all specialized client side effects.  E.g.
 * weapon effects, enemy effects, flash, etc.
 *
 * =======================================================================
 */

#include "header/client.h"
#include "header/particles.h"

void CL_LogoutEffect(vec3_t org, int type);
void CL_ItemRespawnParticles(vec3_t org);
void thinkDecalAlpha (cparticle_t *p, vec3_t org, vec3_t angle, float *alpha, float *size, int *image, float *time);
void vectoanglerolled (vec3_t value1, float angleyaw, vec3_t angles);

void CL_ClearLightStyles(void);
void CL_ClearDlights(void);
void CL_ClearParticles(void);

//Knightmare- Psychospaz's smoke
void CL_GunSmokeEffect (vec3_t org, vec3_t dir);

static vec3_t avelocities[NUMVERTEXNORMALS];
extern struct model_s *cl_mod_smoke;
extern struct model_s *cl_mod_flash;

//here i convert old 256 color to RGB
const byte default_pal[768] =
{
    0,0,0,15,15,15,31,31,31,47,47,47,63,63,63,75,75,75,91,91,91,107,107,107,123,123,123,139,139,139,155,155,155,171,171,171,187,187,187,203,203,203,219,219,219,235,235,235,99,75,35,91,67,31,83,63,31,79,59,27,71,55,27,63,47,
    23,59,43,23,51,39,19,47,35,19,43,31,19,39,27,15,35,23,15,27,19,11,23,15,11,19,15,7,15,11,7,95,95,111,91,91,103,91,83,95,87,79,91,83,75,83,79,71,75,71,63,67,63,59,59,59,55,55,51,47,47,47,43,43,39,
    39,39,35,35,35,27,27,27,23,23,23,19,19,19,143,119,83,123,99,67,115,91,59,103,79,47,207,151,75,167,123,59,139,103,47,111,83,39,235,159,39,203,139,35,175,119,31,147,99,27,119,79,23,91,59,15,63,39,11,35,23,7,167,59,43,
    159,47,35,151,43,27,139,39,19,127,31,15,115,23,11,103,23,7,87,19,0,75,15,0,67,15,0,59,15,0,51,11,0,43,11,0,35,11,0,27,7,0,19,7,0,123,95,75,115,87,67,107,83,63,103,79,59,95,71,55,87,67,51,83,63,
    47,75,55,43,67,51,39,63,47,35,55,39,27,47,35,23,39,27,19,31,23,15,23,15,11,15,11,7,111,59,23,95,55,23,83,47,23,67,43,23,55,35,19,39,27,15,27,19,11,15,11,7,179,91,79,191,123,111,203,155,147,215,187,183,203,
    215,223,179,199,211,159,183,195,135,167,183,115,151,167,91,135,155,71,119,139,47,103,127,23,83,111,19,75,103,15,67,91,11,63,83,7,55,75,7,47,63,7,39,51,0,31,43,0,23,31,0,15,19,0,7,11,0,0,0,139,87,87,131,79,79,
    123,71,71,115,67,67,107,59,59,99,51,51,91,47,47,87,43,43,75,35,35,63,31,31,51,27,27,43,19,19,31,15,15,19,11,11,11,7,7,0,0,0,151,159,123,143,151,115,135,139,107,127,131,99,119,123,95,115,115,87,107,107,79,99,99,
    71,91,91,67,79,79,59,67,67,51,55,55,43,47,47,35,35,35,27,23,23,19,15,15,11,159,75,63,147,67,55,139,59,47,127,55,39,119,47,35,107,43,27,99,35,23,87,31,19,79,27,15,67,23,11,55,19,11,43,15,7,31,11,7,23,
    7,0,11,0,0,0,0,0,119,123,207,111,115,195,103,107,183,99,99,167,91,91,155,83,87,143,75,79,127,71,71,115,63,63,103,55,55,87,47,47,75,39,39,63,35,31,47,27,23,35,19,15,23,11,7,7,155,171,123,143,159,111,135,151,99,
    123,139,87,115,131,75,103,119,67,95,111,59,87,103,51,75,91,39,63,79,27,55,67,19,47,59,11,35,47,7,27,35,0,19,23,0,11,15,0,0,255,0,35,231,15,63,211,27,83,187,39,95,167,47,95,143,51,95,123,51,255,255,255,255,255,
    211,255,255,167,255,255,127,255,255,83,255,255,39,255,235,31,255,215,23,255,191,15,255,171,7,255,147,0,239,127,0,227,107,0,211,87,0,199,71,0,183,59,0,171,43,0,155,31,0,143,23,0,127,15,0,115,7,0,95,0,0,71,0,0,47,
    0,0,27,0,0,239,0,0,55,55,255,255,0,0,0,0,255,43,43,35,27,27,23,19,19,15,235,151,127,195,115,83,159,87,51,123,63,27,235,211,199,199,171,155,167,139,119,135,107,87,159,91,83
};

//this initializes all particle images - mods play with this...
void SetParticleImages (void)
{
    R_SetParticlePicture(particle_generic,        "gfx/particles/basic.tga");
    R_SetParticlePicture(particle_smoke,        "gfx/particles/smoke.tga");
    R_SetParticlePicture(particle_blood,        "gfx/particles/blood.tga");
    R_SetParticlePicture(particle_blooddrop,    "gfx/particles/blood_drop.tga");
    R_SetParticlePicture(particle_blooddrip,    "gfx/particles/blood_drip.tga");
    R_SetParticlePicture(particle_redblood,        "gfx/particles/blood_red.tga");
    R_SetParticlePicture(particle_bubble,        "gfx/particles/bubble.tga");
    R_SetParticlePicture(particle_blaster,        "gfx/particles/blaster.tga");
    R_SetParticlePicture(particle_beam,            "gfx/particles/beam.tga");
    R_SetParticlePicture(particle_beam2,        "gfx/particles/beam2.tga"); // only used for railgun
    R_SetParticlePicture(particle_lightning,    "gfx/particles/lightning.tga");
    R_SetParticlePicture(particle_lensflare,    "gfx/particles/lensflare.tga");
    R_SetParticlePicture(particle_lightflare,    "gfx/particles/lightflare.jpg");
    R_SetParticlePicture(particle_inferno,        "gfx/particles/inferno.tga");
    R_SetParticlePicture(particle_shield,        "gfx/particles/shield.jpg");
    //animations
    //rocket explosion
    R_SetParticlePicture(particle_rflash,        "gfx/particles/r_flash.tga");
    R_SetParticlePicture(particle_rexplosion1,    "gfx/particles/r_explod_1.tga");
    R_SetParticlePicture(particle_rexplosion2,    "gfx/particles/r_explod_2.tga");
    R_SetParticlePicture(particle_rexplosion3,    "gfx/particles/r_explod_3.tga");
    R_SetParticlePicture(particle_rexplosion4,    "gfx/particles/r_explod_4.tga");
    R_SetParticlePicture(particle_rexplosion5,    "gfx/particles/r_explod_5.tga");
    R_SetParticlePicture(particle_rexplosion6,    "gfx/particles/r_explod_6.tga");
    R_SetParticlePicture(particle_rexplosion7,    "gfx/particles/r_explod_7.tga");
    //disruptor explosion
    R_SetParticlePicture(particle_dexplosion1,    "gfx/particles/d_explod_1.tga");
    R_SetParticlePicture(particle_dexplosion2,    "gfx/particles/d_explod_2.tga");
    R_SetParticlePicture(particle_dexplosion3,    "gfx/particles/d_explod_3.tga");
    
    R_SetParticlePicture(particle_bulletmark,    "gfx/decals/bulletmark.tga");
    R_SetParticlePicture(particle_shadow,        "gfx/decals/shadow.tga");
    R_SetParticlePicture(particle_burnmark,        "gfx/decals/burnmark.tga");
    R_SetParticlePicture(particle_blooddecal1,    "gfx/decals/blood_1.tga");
    R_SetParticlePicture(particle_blooddecal2,    "gfx/decals/blood_2.tga");
    R_SetParticlePicture(particle_blooddecal3,    "gfx/decals/blood_3.tga");
    R_SetParticlePicture(particle_blooddecal4,    "gfx/decals/blood_4.tga");
    R_SetParticlePicture(particle_blooddecal5,    "gfx/decals/blood_5.tga");
    R_SetParticlePicture(particle_footprint,    "gfx/decals/footprint.tga");
}

int particleBlood(void)
{
    return particle_blooddecal1 + rand()%5;
}

void pRotateThink (cparticle_t *p, vec3_t org, vec3_t angle, float *alpha, float *size, int *image, float *time);

int    color8red (int color8)
{
    return (default_pal[color8*3+0]);
}
int    color8green (int color8)
{
    return (default_pal[color8*3+1]);;
}
int    color8blue (int color8)
{
    return (default_pal[color8*3+2]);;
}

#define MAX_TEX_SURF 2048 // was 256
struct texsurf_s
{
    int        step_id;
    char    tex[32];
};
typedef struct texsurf_s texsurf_t;
texsurf_t tex_surf[MAX_TEX_SURF];
int    num_texsurfs;

extern cparticle_t *active_particles, *free_particles;

void
CL_AddMuzzleFlash(void)
{
	vec3_t fv, rv;
	cdlight_t *dl;
	int i, weapon;
	centity_t *pl;
	int silenced;
	float volume;
	char soundname[64];

	i = MSG_ReadShort(&net_message);

	if ((i < 1) || (i >= MAX_EDICTS))
	{
		Com_Error(ERR_DROP, "CL_AddMuzzleFlash: bad entity");
	}

	weapon = MSG_ReadByte(&net_message);
	silenced = weapon & MZ_SILENCED;
	weapon &= ~MZ_SILENCED;

	pl = &cl_entities[i];

	dl = CL_AllocDlight(i);
	VectorCopy(pl->current.origin, dl->origin);
	AngleVectors(pl->current.angles, fv, rv, NULL);
	VectorMA(dl->origin, 18, fv, dl->origin);
	VectorMA(dl->origin, 16, rv, dl->origin);

	if (silenced)
	{
		dl->radius = 100.0f + (randk() & 31);
	}

	else
	{
		dl->radius = 200.0f + (randk() & 31);
	}

	dl->minlight = 32;
	dl->die = cl.time;

	if (silenced)
	{
		volume = 0.2f;
	}

	else
	{
		volume = 1;
	}

	switch (weapon)
	{
		case MZ_BLASTER:
			dl->color[0] = 1;
			dl->color[1] = 1;
			dl->color[2] = 0;
			S_StartSound(NULL, i, CHAN_WEAPON,
				S_RegisterSound("weapons/blastf1a.wav"), volume, ATTN_NORM, 0);
			break;
		case MZ_BLUEHYPERBLASTER:
			dl->color[0] = 0;
			dl->color[1] = 0;
			dl->color[2] = 1;
			S_StartSound(NULL, i, CHAN_WEAPON,
				S_RegisterSound("weapons/hyprbf1a.wav"), volume, ATTN_NORM, 0);
			break;
		case MZ_HYPERBLASTER:
			dl->color[0] = 1;
			dl->color[1] = 1;
			dl->color[2] = 0;
			S_StartSound(NULL, i, CHAN_WEAPON,
				S_RegisterSound("weapons/hyprbf1a.wav"), volume, ATTN_NORM, 0);
			break;
		case MZ_MACHINEGUN:
			dl->color[0] = 1;
			dl->color[1] = 1;
			dl->color[2] = 0;
			Com_sprintf(soundname, sizeof(soundname), "weapons/machgf%lub.wav",
				(randk() % 5) + 1);
			S_StartSound(NULL, i, CHAN_WEAPON, S_RegisterSound(
						soundname), volume, ATTN_NORM, 0);
			break;
		case MZ_SHOTGUN:
			dl->color[0] = 1;
			dl->color[1] = 1;
			dl->color[2] = 0;
			S_StartSound(NULL, i, CHAN_WEAPON,
				S_RegisterSound("weapons/shotgf1b.wav"), volume, ATTN_NORM, 0);
			S_StartSound(NULL, i, CHAN_AUTO,
				S_RegisterSound("weapons/shotgr1b.wav"), volume, ATTN_NORM, 0.1f);
			break;
		case MZ_SSHOTGUN:
			dl->color[0] = 1;
			dl->color[1] = 1;
			dl->color[2] = 0;
			S_StartSound(NULL, i, CHAN_WEAPON,
				S_RegisterSound("weapons/sshotf1b.wav"), volume, ATTN_NORM, 0);
			break;
		case MZ_CHAINGUN1:
			dl->radius = 200.0f + (randk() & 31);
			dl->color[0] = 1;
			dl->color[1] = 0.25;
			dl->color[2] = 0;
			Com_sprintf(soundname, sizeof(soundname), "weapons/machgf%lub.wav",
				(randk() % 5) + 1);
			S_StartSound(NULL, i, CHAN_WEAPON, S_RegisterSound(
						soundname), volume, ATTN_NORM, 0);
			break;
		case MZ_CHAINGUN2:
			dl->radius = 225.0f + (randk() & 31);
			dl->color[0] = 1;
			dl->color[1] = 0.5;
			dl->color[2] = 0;
			dl->die = cl.time + 0.1;  /* long delay */
			Com_sprintf(soundname, sizeof(soundname), "weapons/machgf%lub.wav",
				(randk() % 5) + 1);
			S_StartSound(NULL, i, CHAN_WEAPON, S_RegisterSound(
						soundname), volume, ATTN_NORM, 0);
			Com_sprintf(soundname, sizeof(soundname), "weapons/machgf%lub.wav",
				(randk() % 5) + 1);
			S_StartSound(NULL, i, CHAN_WEAPON, S_RegisterSound(
						soundname), volume, ATTN_NORM, 0.05);
			break;
		case MZ_CHAINGUN3:
			dl->radius = 250.0f + (randk() & 31);
			dl->color[0] = 1;
			dl->color[1] = 1;
			dl->color[2] = 0;
			dl->die = cl.time + 0.1;  /* long delay */
			Com_sprintf(soundname, sizeof(soundname), "weapons/machgf%lub.wav",
				(randk() % 5) + 1);
			S_StartSound(NULL, i, CHAN_WEAPON, S_RegisterSound(
						soundname), volume, ATTN_NORM, 0);
			Com_sprintf(soundname, sizeof(soundname), "weapons/machgf%lub.wav",
				(randk() % 5) + 1);
			S_StartSound(NULL, i, CHAN_WEAPON, S_RegisterSound(
						soundname), volume, ATTN_NORM, 0.033f);
			Com_sprintf(soundname, sizeof(soundname), "weapons/machgf%lub.wav",
				(randk() % 5) + 1);
			S_StartSound(NULL, i, CHAN_WEAPON, S_RegisterSound(
						soundname), volume, ATTN_NORM, 0.066f);
			break;
		case MZ_RAILGUN:
			dl->color[0] = 0.5;
			dl->color[1] = 0.5;
			dl->color[2] = 1.0;
			S_StartSound(NULL, i, CHAN_WEAPON,
				S_RegisterSound("weapons/railgf1a.wav"), volume, ATTN_NORM, 0);
			break;
		case MZ_ROCKET:
			dl->color[0] = 1;
			dl->color[1] = 0.5;
			dl->color[2] = 0.2;
			S_StartSound(NULL, i, CHAN_WEAPON,
				S_RegisterSound("weapons/rocklf1a.wav"), volume, ATTN_NORM, 0);
			S_StartSound(NULL, i, CHAN_AUTO,
				S_RegisterSound("weapons/rocklr1b.wav"), volume, ATTN_NORM, 0.1f);
			break;
		case MZ_GRENADE:
			dl->color[0] = 1;
			dl->color[1] = 0.5;
			dl->color[2] = 0;
			S_StartSound(NULL, i, CHAN_WEAPON,
				S_RegisterSound("weapons/grenlf1a.wav"), volume, ATTN_NORM, 0);
			S_StartSound(NULL, i, CHAN_AUTO,
				S_RegisterSound("weapons/grenlr1b.wav"), volume, ATTN_NORM, 0.1f);
			break;
		case MZ_BFG:
			dl->color[0] = 0;
			dl->color[1] = 1;
			dl->color[2] = 0;
			S_StartSound(NULL, i, CHAN_WEAPON,
				S_RegisterSound("weapons/bfg__f1y.wav"), volume, ATTN_NORM, 0);
			break;

		case MZ_LOGIN:
			dl->color[0] = 0;
			dl->color[1] = 1;
			dl->color[2] = 0;
			dl->die = cl.time + 1;
			S_StartSound(NULL, i, CHAN_WEAPON,
				S_RegisterSound("weapons/grenlf1a.wav"), 1, ATTN_NORM, 0);
			CL_LogoutEffect(pl->current.origin, weapon);
			break;
		case MZ_LOGOUT:
			dl->color[0] = 1;
			dl->color[1] = 0;
			dl->color[2] = 0;
			dl->die = cl.time + 1;
			S_StartSound(NULL, i, CHAN_WEAPON,
				S_RegisterSound("weapons/grenlf1a.wav"), 1, ATTN_NORM, 0);
			CL_LogoutEffect(pl->current.origin, weapon);
			break;
		case MZ_RESPAWN:
			dl->color[0] = 1;
			dl->color[1] = 1;
			dl->color[2] = 0;
			dl->die = cl.time + 1.0;
			S_StartSound(NULL, i, CHAN_WEAPON,
				S_RegisterSound("weapons/grenlf1a.wav"), 1, ATTN_NORM, 0);
			CL_LogoutEffect(pl->current.origin, weapon);
			break;
		case MZ_PHALANX:
			dl->color[0] = 1;
			dl->color[1] = 0.5;
			dl->color[2] = 0.5;
			S_StartSound(NULL, i, CHAN_WEAPON,
				S_RegisterSound("weapons/plasshot.wav"), volume, ATTN_NORM, 0);
			break;
		case MZ_IONRIPPER:
			dl->color[0] = 1;
			dl->color[1] = 0.5;
			dl->color[2] = 0.5;
			S_StartSound(NULL, i, CHAN_WEAPON,
				S_RegisterSound("weapons/rippfire.wav"), volume, ATTN_NORM, 0);
			break;
		case MZ_ETF_RIFLE:
			dl->color[0] = 0.9f;
			dl->color[1] = 0.7f;
			dl->color[2] = 0;
			S_StartSound(NULL, i, CHAN_WEAPON,
				S_RegisterSound("weapons/nail1.wav"), volume, ATTN_NORM, 0);
			break;
		case MZ_SHOTGUN2:
			dl->color[0] = 1;
			dl->color[1] = 1;
			dl->color[2] = 0;
			S_StartSound(NULL, i, CHAN_WEAPON,
				S_RegisterSound("weapons/shotg2.wav"), volume, ATTN_NORM, 0);
			break;
		case MZ_HEATBEAM:
			dl->color[0] = 1;
			dl->color[1] = 1;
			dl->color[2] = 0;
			dl->die = cl.time + 100;
			break;
		case MZ_BLASTER2:
			dl->color[0] = 0;
			dl->color[1] = 1;
			dl->color[2] = 0;
			S_StartSound(NULL, i, CHAN_WEAPON,
				S_RegisterSound("weapons/blastf1a.wav"), volume, ATTN_NORM, 0);
			break;
		case MZ_TRACKER:
			/* negative flashes handled the same in gl/soft until CL_AddDLights */
			dl->color[0] = -1;
			dl->color[1] = -1;
			dl->color[2] = -1;
			S_StartSound(NULL, i, CHAN_WEAPON,
				S_RegisterSound("weapons/disint2.wav"), volume, ATTN_NORM, 0);
			break;
		case MZ_NUKE1:
			dl->color[0] = 1;
			dl->color[1] = 0;
			dl->color[2] = 0;
			dl->die = cl.time + 100;
			break;
		case MZ_NUKE2:
			dl->color[0] = 1;
			dl->color[1] = 1;
			dl->color[2] = 0;
			dl->die = cl.time + 100;
			break;
		case MZ_NUKE4:
			dl->color[0] = 0;
			dl->color[1] = 0;
			dl->color[2] = 1;
			dl->die = cl.time + 100;
			break;
		case MZ_NUKE8:
			dl->color[0] = 0;
			dl->color[1] = 1;
			dl->color[2] = 1;
			dl->die = cl.time + 100;
			break;
	}
}

void
CL_AddMuzzleFlash2(void)
{
	int ent;
	vec3_t origin;
	unsigned flash_number;
	cdlight_t *dl;
	vec3_t forward, right;
	char soundname[64];

	ent = MSG_ReadShort(&net_message);

	if ((ent < 1) || (ent >= MAX_EDICTS))
	{
		Com_Error(ERR_DROP, "CL_AddMuzzleFlash2: bad entity");
	}

	flash_number = MSG_ReadByte(&net_message);

	if (flash_number > 210)
	{
		Com_DPrintf("CL_AddMuzzleFlash2: bad offset");
		return;
	}

	/* locate the origin */
	AngleVectors(cl_entities[ent].current.angles, forward, right, NULL);
	origin[0] = cl_entities[ent].current.origin[0] + forward[0] *
				monster_flash_offset[flash_number][0] + right[0] *
				monster_flash_offset[flash_number][1];
	origin[1] = cl_entities[ent].current.origin[1] + forward[1] *
				monster_flash_offset[flash_number][0] + right[1] *
				monster_flash_offset[flash_number][1];
	origin[2] = cl_entities[ent].current.origin[2] + forward[2] *
				monster_flash_offset[flash_number][0] + right[2] *
				monster_flash_offset[flash_number][1] +
				monster_flash_offset[flash_number][2];

	dl = CL_AllocDlight(ent);
	VectorCopy(origin, dl->origin);
	dl->radius = 200.0f + (randk() & 31);
	dl->minlight = 32;
	dl->die = cl.time;

	switch (flash_number)
	{
		case MZ2_INFANTRY_MACHINEGUN_1:
		case MZ2_INFANTRY_MACHINEGUN_2:
		case MZ2_INFANTRY_MACHINEGUN_3:
		case MZ2_INFANTRY_MACHINEGUN_4:
		case MZ2_INFANTRY_MACHINEGUN_5:
		case MZ2_INFANTRY_MACHINEGUN_6:
		case MZ2_INFANTRY_MACHINEGUN_7:
		case MZ2_INFANTRY_MACHINEGUN_8:
		case MZ2_INFANTRY_MACHINEGUN_9:
		case MZ2_INFANTRY_MACHINEGUN_10:
		case MZ2_INFANTRY_MACHINEGUN_11:
		case MZ2_INFANTRY_MACHINEGUN_12:
		case MZ2_INFANTRY_MACHINEGUN_13:
			dl->color[0] = 1;
			dl->color[1] = 1;
			dl->color[2] = 0;
			CL_ParticleEffect(origin, vec3_origin, 0, 40);
			CL_SmokeAndFlash(origin);
			S_StartSound(NULL, ent, CHAN_WEAPON,
				S_RegisterSound("infantry/infatck1.wav"), 1, ATTN_NORM, 0);
			break;

		case MZ2_SOLDIER_MACHINEGUN_1:
		case MZ2_SOLDIER_MACHINEGUN_2:
		case MZ2_SOLDIER_MACHINEGUN_3:
		case MZ2_SOLDIER_MACHINEGUN_4:
		case MZ2_SOLDIER_MACHINEGUN_5:
		case MZ2_SOLDIER_MACHINEGUN_6:
		case MZ2_SOLDIER_MACHINEGUN_7:
		case MZ2_SOLDIER_MACHINEGUN_8:
			dl->color[0] = 1;
			dl->color[1] = 1;
			dl->color[2] = 0;
			CL_ParticleEffect(origin, vec3_origin, 0, 40);
			CL_SmokeAndFlash(origin);
			S_StartSound(NULL, ent, CHAN_WEAPON,
				S_RegisterSound("soldier/solatck3.wav"), 1, ATTN_NORM, 0);
			break;

		case MZ2_GUNNER_MACHINEGUN_1:
		case MZ2_GUNNER_MACHINEGUN_2:
		case MZ2_GUNNER_MACHINEGUN_3:
		case MZ2_GUNNER_MACHINEGUN_4:
		case MZ2_GUNNER_MACHINEGUN_5:
		case MZ2_GUNNER_MACHINEGUN_6:
		case MZ2_GUNNER_MACHINEGUN_7:
		case MZ2_GUNNER_MACHINEGUN_8:
			dl->color[0] = 1;
			dl->color[1] = 1;
			dl->color[2] = 0;
			CL_ParticleEffect(origin, vec3_origin, 0, 40);
			CL_SmokeAndFlash(origin);
			S_StartSound(NULL, ent, CHAN_WEAPON,
				S_RegisterSound("gunner/gunatck2.wav"), 1, ATTN_NORM, 0);
			break;

		case MZ2_ACTOR_MACHINEGUN_1:
		case MZ2_SUPERTANK_MACHINEGUN_1:
		case MZ2_SUPERTANK_MACHINEGUN_2:
		case MZ2_SUPERTANK_MACHINEGUN_3:
		case MZ2_SUPERTANK_MACHINEGUN_4:
		case MZ2_SUPERTANK_MACHINEGUN_5:
		case MZ2_SUPERTANK_MACHINEGUN_6:
		case MZ2_TURRET_MACHINEGUN:
			dl->color[0] = 1;
			dl->color[1] = 1;
			dl->color[2] = 0;

			CL_ParticleEffect(origin, vec3_origin, 0, 40);
			CL_SmokeAndFlash(origin);
			S_StartSound(NULL, ent, CHAN_WEAPON,
				S_RegisterSound("infantry/infatck1.wav"), 1, ATTN_NORM, 0);
			break;

		case MZ2_BOSS2_MACHINEGUN_L1:
		case MZ2_BOSS2_MACHINEGUN_L2:
		case MZ2_BOSS2_MACHINEGUN_L3:
		case MZ2_BOSS2_MACHINEGUN_L4:
		case MZ2_BOSS2_MACHINEGUN_L5:
		case MZ2_CARRIER_MACHINEGUN_L1:
		case MZ2_CARRIER_MACHINEGUN_L2:
			dl->color[0] = 1;
			dl->color[1] = 1;
			dl->color[2] = 0;

			CL_ParticleEffect(origin, vec3_origin, 0, 40);
			CL_SmokeAndFlash(origin);
			S_StartSound(NULL, ent, CHAN_WEAPON,
				S_RegisterSound("infantry/infatck1.wav"), 1, ATTN_NONE, 0);
			break;

		case MZ2_SOLDIER_BLASTER_1:
		case MZ2_SOLDIER_BLASTER_2:
		case MZ2_SOLDIER_BLASTER_3:
		case MZ2_SOLDIER_BLASTER_4:
		case MZ2_SOLDIER_BLASTER_5:
		case MZ2_SOLDIER_BLASTER_6:
		case MZ2_SOLDIER_BLASTER_7:
		case MZ2_SOLDIER_BLASTER_8:
		case MZ2_TURRET_BLASTER:
			dl->color[0] = 1;
			dl->color[1] = 1;
			dl->color[2] = 0;
			S_StartSound(NULL, ent, CHAN_WEAPON,
				S_RegisterSound("soldier/solatck2.wav"), 1, ATTN_NORM, 0);
			break;

		case MZ2_FLYER_BLASTER_1:
		case MZ2_FLYER_BLASTER_2:
			dl->color[0] = 1;
			dl->color[1] = 1;
			dl->color[2] = 0;
			S_StartSound(NULL, ent, CHAN_WEAPON,
				S_RegisterSound("flyer/flyatck3.wav"), 1, ATTN_NORM, 0);
			break;

		case MZ2_MEDIC_BLASTER_1:
			dl->color[0] = 1;
			dl->color[1] = 1;
			dl->color[2] = 0;
			S_StartSound(NULL, ent, CHAN_WEAPON,
				S_RegisterSound("medic/medatck1.wav"), 1, ATTN_NORM, 0);
			break;

		case MZ2_HOVER_BLASTER_1:
			dl->color[0] = 1;
			dl->color[1] = 1;
			dl->color[2] = 0;
			S_StartSound(NULL, ent, CHAN_WEAPON,
				S_RegisterSound("hover/hovatck1.wav"), 1, ATTN_NORM, 0);
			break;

		case MZ2_FLOAT_BLASTER_1:
			dl->color[0] = 1;
			dl->color[1] = 1;
			dl->color[2] = 0;
			S_StartSound(NULL, ent, CHAN_WEAPON,
				S_RegisterSound("floater/fltatck1.wav"), 1, ATTN_NORM, 0);
			break;

		case MZ2_SOLDIER_SHOTGUN_1:
		case MZ2_SOLDIER_SHOTGUN_2:
		case MZ2_SOLDIER_SHOTGUN_3:
		case MZ2_SOLDIER_SHOTGUN_4:
		case MZ2_SOLDIER_SHOTGUN_5:
		case MZ2_SOLDIER_SHOTGUN_6:
		case MZ2_SOLDIER_SHOTGUN_7:
		case MZ2_SOLDIER_SHOTGUN_8:
			dl->color[0] = 1;
			dl->color[1] = 1;
			dl->color[2] = 0;
			CL_SmokeAndFlash(origin);
			S_StartSound(NULL, ent, CHAN_WEAPON,
				S_RegisterSound("soldier/solatck1.wav"), 1, ATTN_NORM, 0);
			break;

		case MZ2_TANK_BLASTER_1:
		case MZ2_TANK_BLASTER_2:
		case MZ2_TANK_BLASTER_3:
			dl->color[0] = 1;
			dl->color[1] = 1;
			dl->color[2] = 0;
			S_StartSound(NULL, ent, CHAN_WEAPON,
				S_RegisterSound("tank/tnkatck3.wav"), 1, ATTN_NORM, 0);
			break;

		case MZ2_TANK_MACHINEGUN_1:
		case MZ2_TANK_MACHINEGUN_2:
		case MZ2_TANK_MACHINEGUN_3:
		case MZ2_TANK_MACHINEGUN_4:
		case MZ2_TANK_MACHINEGUN_5:
		case MZ2_TANK_MACHINEGUN_6:
		case MZ2_TANK_MACHINEGUN_7:
		case MZ2_TANK_MACHINEGUN_8:
		case MZ2_TANK_MACHINEGUN_9:
		case MZ2_TANK_MACHINEGUN_10:
		case MZ2_TANK_MACHINEGUN_11:
		case MZ2_TANK_MACHINEGUN_12:
		case MZ2_TANK_MACHINEGUN_13:
		case MZ2_TANK_MACHINEGUN_14:
		case MZ2_TANK_MACHINEGUN_15:
		case MZ2_TANK_MACHINEGUN_16:
		case MZ2_TANK_MACHINEGUN_17:
		case MZ2_TANK_MACHINEGUN_18:
		case MZ2_TANK_MACHINEGUN_19:
			dl->color[0] = 1;
			dl->color[1] = 1;
			dl->color[2] = 0;
			CL_ParticleEffect(origin, vec3_origin, 0, 40);
			CL_SmokeAndFlash(origin);
			Com_sprintf(soundname, sizeof(soundname), "tank/tnkatk2%c.wav",
				'a' + (char)(randk() % 5));
			S_StartSound(NULL, ent, CHAN_WEAPON, 
				S_RegisterSound(soundname), 1, ATTN_NORM, 0);
			break;

		case MZ2_CHICK_ROCKET_1:
		case MZ2_TURRET_ROCKET:
			dl->color[0] = 1;
			dl->color[1] = 0.5f;
			dl->color[2] = 0.2f;
			S_StartSound(NULL, ent, CHAN_WEAPON,
				S_RegisterSound("chick/chkatck2.wav"), 1, ATTN_NORM, 0);
			break;

		case MZ2_TANK_ROCKET_1:
		case MZ2_TANK_ROCKET_2:
		case MZ2_TANK_ROCKET_3:
			dl->color[0] = 1;
			dl->color[1] = 0.5f;
			dl->color[2] = 0.2f;
			S_StartSound(NULL, ent, CHAN_WEAPON,
				S_RegisterSound("tank/tnkatck1.wav"), 1, ATTN_NORM, 0);
			break;

		case MZ2_SUPERTANK_ROCKET_1:
		case MZ2_SUPERTANK_ROCKET_2:
		case MZ2_SUPERTANK_ROCKET_3:
		case MZ2_BOSS2_ROCKET_1:
		case MZ2_BOSS2_ROCKET_2:
		case MZ2_BOSS2_ROCKET_3:
		case MZ2_BOSS2_ROCKET_4:
		case MZ2_CARRIER_ROCKET_1:
			dl->color[0] = 1;
			dl->color[1] = 0.5f;
			dl->color[2] = 0.2f;
			S_StartSound(NULL, ent, CHAN_WEAPON,
				S_RegisterSound("tank/rocket.wav"), 1, ATTN_NORM, 0);
			break;

		case MZ2_GUNNER_GRENADE_1:
		case MZ2_GUNNER_GRENADE_2:
		case MZ2_GUNNER_GRENADE_3:
		case MZ2_GUNNER_GRENADE_4:
			dl->color[0] = 1;
			dl->color[1] = 0.5;
			dl->color[2] = 0;
			S_StartSound(NULL, ent, CHAN_WEAPON,
				S_RegisterSound("gunner/gunatck3.wav"), 1, ATTN_NORM, 0);
			break;

		case MZ2_GLADIATOR_RAILGUN_1:
		case MZ2_CARRIER_RAILGUN:
		case MZ2_WIDOW_RAIL:
			dl->color[0] = 0.5;
			dl->color[1] = 0.5;
			dl->color[2] = 1.0;
			break;

		case MZ2_MAKRON_BFG:
			dl->color[0] = 0.5;
			dl->color[1] = 1;
			dl->color[2] = 0.5;
			break;

		case MZ2_MAKRON_BLASTER_1:
		case MZ2_MAKRON_BLASTER_2:
		case MZ2_MAKRON_BLASTER_3:
		case MZ2_MAKRON_BLASTER_4:
		case MZ2_MAKRON_BLASTER_5:
		case MZ2_MAKRON_BLASTER_6:
		case MZ2_MAKRON_BLASTER_7:
		case MZ2_MAKRON_BLASTER_8:
		case MZ2_MAKRON_BLASTER_9:
		case MZ2_MAKRON_BLASTER_10:
		case MZ2_MAKRON_BLASTER_11:
		case MZ2_MAKRON_BLASTER_12:
		case MZ2_MAKRON_BLASTER_13:
		case MZ2_MAKRON_BLASTER_14:
		case MZ2_MAKRON_BLASTER_15:
		case MZ2_MAKRON_BLASTER_16:
		case MZ2_MAKRON_BLASTER_17:
			dl->color[0] = 1;
			dl->color[1] = 1;
			dl->color[2] = 0;
			S_StartSound(NULL, ent, CHAN_WEAPON,
				S_RegisterSound("makron/blaster.wav"), 1, ATTN_NORM, 0);
			break;

		case MZ2_JORG_MACHINEGUN_L1:
		case MZ2_JORG_MACHINEGUN_L2:
		case MZ2_JORG_MACHINEGUN_L3:
		case MZ2_JORG_MACHINEGUN_L4:
		case MZ2_JORG_MACHINEGUN_L5:
		case MZ2_JORG_MACHINEGUN_L6:
			dl->color[0] = 1;
			dl->color[1] = 1;
			dl->color[2] = 0;
			CL_ParticleEffect(origin, vec3_origin, 0, 40);
			CL_SmokeAndFlash(origin);
			S_StartSound(NULL, ent, CHAN_WEAPON,
				S_RegisterSound("boss3/xfire.wav"), 1, ATTN_NORM, 0);
			break;

		case MZ2_JORG_MACHINEGUN_R1:
		case MZ2_JORG_MACHINEGUN_R2:
		case MZ2_JORG_MACHINEGUN_R3:
		case MZ2_JORG_MACHINEGUN_R4:
		case MZ2_JORG_MACHINEGUN_R5:
		case MZ2_JORG_MACHINEGUN_R6:
			dl->color[0] = 1;
			dl->color[1] = 1;
			dl->color[2] = 0;
			CL_ParticleEffect(origin, vec3_origin, 0, 40);
			CL_SmokeAndFlash(origin);
			break;

		case MZ2_JORG_BFG_1:
			dl->color[0] = 0.5;
			dl->color[1] = 1;
			dl->color[2] = 0.5;
			break;

		case MZ2_BOSS2_MACHINEGUN_R1:
		case MZ2_BOSS2_MACHINEGUN_R2:
		case MZ2_BOSS2_MACHINEGUN_R3:
		case MZ2_BOSS2_MACHINEGUN_R4:
		case MZ2_BOSS2_MACHINEGUN_R5:
		case MZ2_CARRIER_MACHINEGUN_R1:
		case MZ2_CARRIER_MACHINEGUN_R2:

			dl->color[0] = 1;
			dl->color[1] = 1;
			dl->color[2] = 0;

			CL_ParticleEffect(origin, vec3_origin, 0, 40);
			CL_SmokeAndFlash(origin);
			break;

		case MZ2_STALKER_BLASTER:
		case MZ2_DAEDALUS_BLASTER:
		case MZ2_MEDIC_BLASTER_2:
		case MZ2_WIDOW_BLASTER:
		case MZ2_WIDOW_BLASTER_SWEEP1:
		case MZ2_WIDOW_BLASTER_SWEEP2:
		case MZ2_WIDOW_BLASTER_SWEEP3:
		case MZ2_WIDOW_BLASTER_SWEEP4:
		case MZ2_WIDOW_BLASTER_SWEEP5:
		case MZ2_WIDOW_BLASTER_SWEEP6:
		case MZ2_WIDOW_BLASTER_SWEEP7:
		case MZ2_WIDOW_BLASTER_SWEEP8:
		case MZ2_WIDOW_BLASTER_SWEEP9:
		case MZ2_WIDOW_BLASTER_100:
		case MZ2_WIDOW_BLASTER_90:
		case MZ2_WIDOW_BLASTER_80:
		case MZ2_WIDOW_BLASTER_70:
		case MZ2_WIDOW_BLASTER_60:
		case MZ2_WIDOW_BLASTER_50:
		case MZ2_WIDOW_BLASTER_40:
		case MZ2_WIDOW_BLASTER_30:
		case MZ2_WIDOW_BLASTER_20:
		case MZ2_WIDOW_BLASTER_10:
		case MZ2_WIDOW_BLASTER_0:
		case MZ2_WIDOW_BLASTER_10L:
		case MZ2_WIDOW_BLASTER_20L:
		case MZ2_WIDOW_BLASTER_30L:
		case MZ2_WIDOW_BLASTER_40L:
		case MZ2_WIDOW_BLASTER_50L:
		case MZ2_WIDOW_BLASTER_60L:
		case MZ2_WIDOW_BLASTER_70L:
		case MZ2_WIDOW_RUN_1:
		case MZ2_WIDOW_RUN_2:
		case MZ2_WIDOW_RUN_3:
		case MZ2_WIDOW_RUN_4:
		case MZ2_WIDOW_RUN_5:
		case MZ2_WIDOW_RUN_6:
		case MZ2_WIDOW_RUN_7:
		case MZ2_WIDOW_RUN_8:
			dl->color[0] = 0;
			dl->color[1] = 1;
			dl->color[2] = 0;
			S_StartSound(NULL, ent, CHAN_WEAPON,
				S_RegisterSound("tank/tnkatck3.wav"), 1, ATTN_NORM, 0);
			break;

		case MZ2_WIDOW_DISRUPTOR:
			dl->color[0] = -1;
			dl->color[1] = -1;
			dl->color[2] = -1;
			S_StartSound(NULL, ent, CHAN_WEAPON,
				S_RegisterSound("weapons/disint2.wav"), 1, ATTN_NORM, 0);
			break;

		case MZ2_WIDOW_PLASMABEAM:
		case MZ2_WIDOW2_BEAMER_1:
		case MZ2_WIDOW2_BEAMER_2:
		case MZ2_WIDOW2_BEAMER_3:
		case MZ2_WIDOW2_BEAMER_4:
		case MZ2_WIDOW2_BEAMER_5:
		case MZ2_WIDOW2_BEAM_SWEEP_1:
		case MZ2_WIDOW2_BEAM_SWEEP_2:
		case MZ2_WIDOW2_BEAM_SWEEP_3:
		case MZ2_WIDOW2_BEAM_SWEEP_4:
		case MZ2_WIDOW2_BEAM_SWEEP_5:
		case MZ2_WIDOW2_BEAM_SWEEP_6:
		case MZ2_WIDOW2_BEAM_SWEEP_7:
		case MZ2_WIDOW2_BEAM_SWEEP_8:
		case MZ2_WIDOW2_BEAM_SWEEP_9:
		case MZ2_WIDOW2_BEAM_SWEEP_10:
		case MZ2_WIDOW2_BEAM_SWEEP_11:
			dl->radius = 300.0f + (randk() & 100);
			dl->color[0] = 1;
			dl->color[1] = 1;
			dl->color[2] = 0;
			dl->die = cl.time + 200;
			break;
	}
}

void
CL_TeleporterParticles(entity_state_t *ent)
{
    int            i;
    
    for (i = 0; i < 8; i++)
    {
        setupParticle (
                       0,    0,    0,
                       ent->origin[0]-16+(rand()&31),    ent->origin[1]-16+(rand()&31),    ent->origin[2]-16+(rand()&31),
                       crand()*14,        crand()*14,        80 + (rand()&7),
                       0,        0,        0,
                       230+crand()*25,    125+crand()*25,    25+crand()*25,
                       0,        0,        0,
                       1,        -0.5,
                       GL_SRC_ALPHA, GL_ONE,
                       2,        0,
                       particle_generic,
                       PART_GRAVITY,
                       NULL,0);
    }
}

void
CL_LogoutEffect(vec3_t org, int type)
{
    int            i;
    vec3_t    color;
    
    for (i=0 ; i<500 ; i++)
    {
        if (type == MZ_LOGIN)// green
        {
            color[0] = 20;
            color[1] = 200;
            color[2] = 20;
        }
        else if (type == MZ_LOGOUT)// red
        {
            color[0] = 200;
            color[1] = 20;
            color[2] = 20;
        }
        else// yellow
        {
            color[0] = 200;
            color[1] = 200;
            color[2] = 20;
        }
        
        setupParticle (
                       0,    0,    0,
                       org[0] - 16 + frand()*32,    org[1] - 16 + frand()*32,    org[2] - 24 + frand()*56,
                       crand()*20,            crand()*20,            crand()*20,
                       0,        0,        0,
                       color[0],        color[1],        color[2],
                       0,    0,    0,
                       1,        -1.0 / (1.0 + frand()*0.3),
                       GL_SRC_ALPHA, GL_ONE,
                       1,            0,
                       particle_generic,
                       PART_GRAVITY,
                       NULL,0);
    }
}

void
CL_ItemRespawnParticles(vec3_t org)
{
    int            i;
    
    for (i=0 ; i<64 ; i++)
    {
        setupParticle (
                       0,    0,    0,
                       org[0] + crand()*8,    org[1] + crand()*8,    org[2] + crand()*8,
                       crand()*8,            crand()*8,            crand()*8,
                       0,        0,        PARTICLE_GRAVITY*0.2,
                       0,        150+rand()*25,        0,
                       0,    0,    0,
                       1,        -1.0 / (1.0 + frand()*0.3),
                       GL_SRC_ALPHA, GL_ONE,
                       1,            0,
                       particle_generic,
                       PART_GRAVITY,
                       NULL,0);
    }
}

void
CL_ExplosionParticles(vec3_t org)
{
	int i, j;
	cparticle_t *p;
	float time;

	time = (float)cl.time;

	for (i = 0; i < 256; i++)
	{
		if (!free_particles)
		{
			return;
		}

		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;

		p->time = time;
		p->color = 0xe0 + (randk() & 7);

		for (j = 0; j < 3; j++)
		{
			p->org[j] = org[j] + ((randk() % 32) - 16);
			p->vel[j] = (randk() % 384) - 192;
		}

		p->accel[0] = p->accel[1] = 0;
		p->accel[2] = -PARTICLE_GRAVITY;
		p->alpha = 1.0;

		p->alphavel = -0.8f / (0.5f + frandk() * 0.3f);
	}
}

void
CL_BigTeleportParticles(vec3_t org)
{
    int            i, index;
    float        angle, dist;
    static int colortable0[4] = {10,50,150,50};
    static int colortable1[4] = {150,150,50,10};
    static int colortable2[4] = {50,10,10,150};
    
    for (i=0; i<(1024/cl_particle_scale->value); i++) // was 4096
    {
        
        index = rand()&3;
        angle = M_PI*2*(rand()&1023)/1023.0;
        dist = rand()&31;
        setupParticle (
                       0,    0,    0,
                       org[0]+cos(angle)*dist,    org[1] + sin(angle)*dist,org[2] + 8 + (rand()%90),
                       cos(angle)*(70+(rand()&63)),sin(angle)*(70+(rand()&63)),-100 + (rand()&31),
                       -cos(angle)*100,    -sin(angle)*100,PARTICLE_GRAVITY*4,
                       colortable0[index],    colortable1[index],    colortable2[index],
                       0,    0,    0,
                       1,        -0.1 / (0.5 + frand()*0.3),
                       GL_SRC_ALPHA, GL_ONE,
                       5,        0.15 / (0.5 + frand()*0.3),     // was 2, 0.05
                       particle_generic,
                       0,
                       NULL,0);
    }
}

/*
 *  Wall impact puffs
 */
#define pBlasterMaxVelocity 100
#define pBlasterMinSize 1.0
#define pBlasterMaxSize 5.0

void pBlasterThink (cparticle_t *p, vec3_t org, vec3_t angle, float *alpha, float *size, int *image, float *time)
{
    vec_t  length;
    vec3_t len;
    float clipsize = 1.0;
    VectorSubtract(p->angle, org, len);
    
    *size *= (float)(pBlasterMaxSize/VectorLength(len)) * 1.0/((4-*size));
    *size += *time * p->sizevel;
    
    if (*size > pBlasterMaxSize)
        *size = pBlasterMaxSize;
    if (*size < pBlasterMinSize)
        *size = pBlasterMinSize;
    
    pBounceThink (p, org, angle, alpha, &clipsize, image, time); // was size
    
    length = VectorNormalize(p->vel);
    if (length>pBlasterMaxVelocity)
        VectorScale(p->vel,    pBlasterMaxVelocity,    p->vel);
    else
        VectorScale(p->vel,    length, p->vel);
    
    /*    vec3_t len;
     VectorSubtract(p->angle, org, len);
     
     *size *= (float)(pBlasterMaxSize/VectorLength(len)) * 1.0/((4-*size));
     if (*size > pBlasterMaxSize)
     *size = pBlasterMaxSize;
     
     p->thinknext = true;*/
}

void
CL_BlasterParticles (vec3_t org, vec3_t dir, int count, int red, int green, int blue,
                          int reddelta, int greendelta, int bluedelta)
{
    int            i;
    //float        d;
    float speed = .75;
    cparticle_t *p;
    vec3_t        origin;
    
    for (i = 0; i < count; i++)
    {
        VectorSet(origin,
                  org[0] + dir[0]*(1 + random()*3 + pBlasterMaxSize/2.0),
                  org[1] + dir[1]*(1 + random()*3 + pBlasterMaxSize/2.0),
                  org[2] + dir[2]*(1 + random()*3 + pBlasterMaxSize/2.0)
                  );
        
        p = setupParticle (
                           org[0],    org[1],    org[2],
                           origin[0],    origin[1],    origin[2],
                           (dir[0]*75 + crand()*40)*speed,    (dir[1]*75 + crand()*40)*speed,    (dir[2]*75 + crand()*40)*speed,
                           0,        0,        0,
                           red,        green,        blue,
                           reddelta,    greendelta,    bluedelta,
                           1,        -0.5 / (0.5 + frand()*0.3),
                           GL_SRC_ALPHA, GL_ONE,
                           4,    -0.5,
                           particle_generic,
                           PART_GRAVITY,
                           pBlasterThink,true);
        
        /*    d = rand()&5;
         p = setupParticle (
         org[0],    org[1],    org[2],
         org[0]+((rand()&5)-2)+d*dir[0],    org[1]+((rand()&5)-2)+d*dir[1],    org[2]+((rand()&5)-2)+d*dir[2],
         (dir[0]*50 + crand()*20)*speed,    (dir[1]*50 + crand()*20)*speed,    (dir[2]*50 + crand()*20)*speed,
         0,            0,            0,
         red,        green,        blue,
         reddelta,    greendelta,    bluedelta,
         1,        -1.0 / (0.5 + frand()*0.3),
         GL_SRC_ALPHA, GL_ONE,
         4,        -1.0,
         particle_generic,
         PART_GRAVITY,
         pBlasterThink,true);*/
    }
    if (p) // added light effect
        addParticleLight (p, 150, 0, ((float)red)/255, ((float)green)/255, ((float)blue)/255);
}

void
CL_BlasterTrail (vec3_t start, vec3_t end, int red, int green, int blue,
                 int reddelta, int greendelta, int bluedelta)
{
    vec3_t        move;
    vec3_t        vec;
    float        len;
    int            dec;
    
    VectorCopy (start, move);
    VectorSubtract (end, start, vec);
    len = VectorNormalize (vec);
    
    dec = 4 * cl_particle_scale->value;
    VectorScale (vec, dec, vec);
    
    // FIXME: this is a really silly way to have a loop
    while (len > 0)
    {
        len -= dec;
        
        setupParticle (
                       0,    0,    0,
                       move[0] + crand(),    move[1] + crand(),    move[2] + crand(),
                       crand()*5,    crand()*5,    crand()*5,
                       0,        0,        0,
                       red,    green,    blue,
                       reddelta,    greendelta,    bluedelta,
                       1,        -1.0 / (0.5 + frand()*0.3),
                       GL_SRC_ALPHA, GL_ONE,
                       4,            -6,
                       particle_generic,
                       0,
                       NULL,0);
        
        VectorAdd (move, vec, move);
    }
}

void
CL_QuadTrail(vec3_t start, vec3_t end)
{
	vec3_t move;
	vec3_t vec;
	int len;
	int dec;
	float time;

	time = (float)cl.time;

	VectorCopy(start, move);
	VectorSubtract(end, start, vec);
	len = (int)VectorNormalize(vec);

	dec = 5;
	VectorScale(vec, 5, vec);

	while (len > 0)
	{
		len -= dec;

        setupParticle (
                       0,    0,    0,
                       move[0] + crand()*16,    move[1] + crand()*16,    move[2] + crand()*16,
                       crand()*5,    crand()*5,    crand()*5,
                       0,        0,        0,
                       0,        0,        200,
                       0,    0,    0,
                       1,        -1.0 / (0.8+frand()*0.2),
                       GL_SRC_ALPHA, GL_ONE,
                       1,            0,
                       particle_generic,
                       0,
                       NULL,0);

		VectorAdd(move, vec, move);
	}
}

void
CL_FlagTrail (vec3_t start, vec3_t end, qboolean isred, qboolean isgreen)
{
	vec3_t move;
	vec3_t vec;
	int len;
	int dec;
	float time;

	time = (float)cl.time;

	VectorCopy(start, move);
	VectorSubtract(end, start, vec);
	len = (int)VectorNormalize(vec);

	dec = 5;
	VectorScale(vec, 5, vec);

	while (len > 0)
	{
		len -= dec;

        setupParticle (
                       0,    0,    0,
                       move[0] + crand()*16, move[1] + crand()*16, move[2] + crand()*16,
                       crand()*5,    crand()*5, crand()*5,
                       0,        0,        0,
                       (isred)?255:0, (isgreen)?255:0, (!isred && !isgreen)?255:0,
                       0,    0,    0,
                       1,        -1.0 / (0.8+frand()*0.2),
                       GL_SRC_ALPHA, GL_ONE,
                       1,            0,
                       particle_generic,
                       0,
                       NULL,0);
        
		VectorAdd(move, vec, move);
	}
}

void
CL_DiminishingTrail(vec3_t start, vec3_t end, centity_t *old, int flags)
{
    cparticle_t *p;
	vec3_t move;
	vec3_t vec;
    float        len, oldlen;
	int j;
	float dec;
	float orgscale;
	float velscale;
	float time;

	time = (float)cl.time;

	VectorCopy(start, move);
	VectorSubtract(end, start, vec);
	len = oldlen = VectorNormalize(vec);

    dec = (flags & EF_ROCKET) ? 10 : 2;
    dec *= cl_particle_scale->value;
	VectorScale(vec, dec, vec);

	if (old->trailcount > 900)
	{
		orgscale = 4;
		velscale = 15;
	}
	else if (old->trailcount > 800)
	{
		orgscale = 2;
		velscale = 10;
	}
	else
	{
		orgscale = 1;
		velscale = 5;
	}

	while (len > 0)
	{
		len -= dec;

        if (!free_particles)
        {
            return;
        }
        
        if (flags & EF_ROCKET)
        {
            if (CM_PointContents(move,0) & MASK_WATER)
                setupParticle (
                               0,    0,    crand()*360,
                               move[0],    move[1],    move[2],
                               crand()*9,    crand()*9,    crand()*9+5,
                               0,        0,        0,
                               255,    255,    255,
                               0,    0,    0,
                               0.75,        -0.2 / (1 + frand() * 0.2),
                               GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                               1+random()*3,    1,
                               particle_bubble,
                               PART_TRANS|PART_SHADED,
                               pExplosionBubbleThink,true);
            else
                setupParticle (
                               crand()*180, crand()*100, 0,
                               move[0],    move[1],    move[2],
                               crand()*5,    crand()*5,    crand()*5,
                               0,        0,        5,
                               255,    255,    255,
                               -50,    -50,    -50,
                               1,        -0.5,
                               GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                               5,            15,
                               particle_smoke,
                               PART_TRANS|PART_SHADED,
                               pRotateThink, true);
        }
        else
        {
            /* drop less particles as it flies */
            if ((randk() & 1023) < old->trailcount)
            {
                if (flags & EF_GIB)
                {
                    if (cl_blood->value > 1)
                        p = setupParticle (
                                           0,    0,    random()*360,
                                           move[0] + crand()*orgscale,    move[1] + crand()*orgscale,    move[2] + crand()*orgscale,
                                           crand()*velscale,    crand()*velscale,    crand()*velscale,
                                           0,        0,        0,
                                           255,    0,        0,
                                           0,        0,        0,
                                           0.75,    -0.75 / (1+frand()*0.4),
                                           GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                                           3 + random()*2,            0,
                                           particle_blooddrop,
                                           PART_OVERBRIGHT|PART_GRAVITY|PART_SHADED,
                                           pBloodThink,true);
                    //NULL,0);
                    else
                        p = setupParticle (
                                           0,    0,    0,
                                           move[0] + crand()*orgscale,    move[1] + crand()*orgscale,    move[2] + crand()*orgscale,
                                           crand()*velscale,    crand()*velscale,    crand()*velscale,
                                           0,        0,        0,
                                           255,    0,        0,
                                           0,        0,        0,
                                           1,        -1.0 / (1+frand()*0.4),
                                           GL_SRC_ALPHA, GL_ONE,
                                           5,            -1,
                                           particle_blood,
                                           PART_GRAVITY|PART_SHADED,
                                           pBloodThink,true);
                    //NULL,0);
                    if (crand() < (double)0.0001F)
                        p->flags |= PART_LEAVEMARK;
                }
                else if (flags & EF_GREENGIB)
                {
                    p = setupParticle (
                                       0,    0,    0,
                                       move[0] + crand()*orgscale,    move[1] + crand()*orgscale,    move[2] + crand()*orgscale,
                                       crand()*velscale,    crand()*velscale,    crand()*velscale,
                                       0,        0,        0,
                                       255,    180,    50,
                                       0,        0,        0,
                                       1,        -0.5 / (1+frand()*0.4),
                                       GL_SRC_ALPHA, GL_ONE,
                                       5,            -1,
                                       particle_blood,
                                       PART_OVERBRIGHT|PART_GRAVITY|PART_SHADED,
                                       pBloodThink,true);
                    //NULL,0);
                    if (crand() < (double)0.0001F)
                        p->flags |= PART_LEAVEMARK;
                }
                else if (flags & EF_GRENADE) // no overbrights on grenade trails
                {
                    if (CM_PointContents(move,0) & MASK_WATER)
                        setupParticle (
                                       0,    0,    crand()*360,
                                       move[0],    move[1],    move[2],
                                       crand()*9,    crand()*9,    crand()*9+5,
                                       0,        0,        0,
                                       255,    255,    255,
                                       0,    0,    0,
                                       0.75,        -0.2 / (1 + frand() * 0.2),
                                       GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                                       1+random()*3,    1,
                                       particle_bubble,
                                       PART_TRANS|PART_SHADED,
                                       pExplosionBubbleThink,true);
                    else
                        setupParticle (
                                       crand()*180, crand()*50, 0,
                                       move[0] + crand()*orgscale,    move[1] + crand()*orgscale,    move[2] + crand()*orgscale,
                                       crand()*velscale,    crand()*velscale,    crand()*velscale,
                                       0,        0,        20,
                                       255,    255,    255,
                                       0,        0,        0,
                                       0.5,        -0.5,
                                       GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                                       5,            5,
                                       particle_smoke,
                                       PART_TRANS|PART_SHADED,
                                       pRotateThink,true);
                }
                else
                {
                    setupParticle (
                                   crand()*180, crand()*50, 0,
                                   move[0] + crand()*orgscale,    move[1] + crand()*orgscale,    move[2] + crand()*orgscale,
                                   crand()*velscale,    crand()*velscale,    crand()*velscale,
                                   0,        0,        20,
                                   255,        255,        255,
                                   0,    0,    0,
                                   0.5,        -0.5,
                                   GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                                   5,            5,
                                   particle_smoke,
                                   PART_OVERBRIGHT|PART_TRANS|PART_SHADED,
                                   pRotateThink,true);
                }
            }
            
            old->trailcount -= 5;
            
            if (old->trailcount < 100)
            {
                old->trailcount = 100;
            }
        }

		VectorAdd(move, vec, move);
	}
}

void
MakeNormalVectors(vec3_t forward, vec3_t right, vec3_t up)
{
	float d;

	/* this rotate and negate guarantees a 
	   vector not colinear with the original */
	right[1] = -forward[0];
	right[2] = forward[1];
	right[0] = forward[2];

	d = DotProduct(right, forward);
	VectorMA(right, -d, forward, right);
	VectorNormalize(right);
	CrossProduct(right, forward, up);
}

void
CL_RocketTrail(vec3_t start, vec3_t end, centity_t *old)
{
	vec3_t move;
	vec3_t vec;
    float        len, totallen;
	int dec;
	float time;

	time = (float)cl.time;

	/* smoke */
	CL_DiminishingTrail(start, end, old, EF_ROCKET);

	/* fire */
	VectorCopy(start, move);
	VectorSubtract(end, start, vec);
	totallen = len = (int)VectorNormalize(vec);

    dec = 1*cl_particle_scale->value;
	VectorScale(vec, dec, vec);

	while (len > 0)
	{
		len -= dec;

		if (!free_particles)
		{
			return;
		}

		if ((randk() & 7) == 0)
		{
            setupParticle (
                           0,    0,    0,
                           move[0] + crand()*5,    move[1] + crand()*5,    move[2] + crand()*5,
                           crand()*20,    crand()*20,    crand()*20,
                           0,        0,        20,
                           255,    255,    200,
                           0,    -50,    0,
                           1,        -1.0 / (1+frand()*0.2),
                           GL_SRC_ALPHA, GL_ONE,
                           2,            -2,
                           particle_blaster,
                           PART_GRAVITY,
                           NULL,0);
		}

		VectorAdd(move, vec, move);
	}
    
    VectorCopy (start, move);
    VectorSubtract (end, start, vec);
    totallen = len = VectorNormalize (vec);
    dec = 1.5*cl_particle_scale->value;
    VectorScale (vec, dec, vec);
    /*    len = totallen;
     VectorCopy (start, move);
     dec = 1.5;//*cl_particle_scale->value;
     VectorScale (vec, dec, vec);*/
    
    while (len > 0)
    {
        len -= dec;
        
        //flame
        setupParticle (
                       crand()*180, crand()*100, 0,
                       move[0],    move[1],    move[2],
                       crand()*5,    crand()*5,    crand()*5,
                       0,        0,        5,
                       255,    225,    200,
                       -50,    -50,    -50,
                       0.75,        -3,
                       GL_SRC_ALPHA, GL_ONE,
                       5,            5,
                       particle_inferno,
                       0,
                       pRotateThink, true);
        
        VectorAdd (move, vec, move);
    }
}

/*
 ===============
 FartherPoint
 Returns true if the first vector
 is farther from the viewpoint.
 ===============
 */
qboolean FartherPoint (vec3_t pt1, vec3_t pt2)
{
    vec3_t        distance1, distance2;
    
    VectorSubtract(pt1, cl.refdef.vieworg, distance1);
    VectorSubtract(pt2, cl.refdef.vieworg, distance2);
    return (VectorLength(distance1) > VectorLength(distance2));
}

#define DEVRAILSTEPS 2
//this is the length of each piece...
#define RAILTRAILSPACE 15

void CL_RailSprial (vec3_t start, vec3_t end, qboolean isRed)
{
    vec3_t        move;
    vec3_t        vec;
    float        len;
    vec3_t        right, up;
    int            i;
    float        d, c, s;
    vec3_t        dir;
    
    // Draw from closest point
    if (FartherPoint(start, end)) {
        VectorCopy (end, move);
        VectorSubtract (start, end, vec);
    }
    else {
        VectorCopy (start, move);
        VectorSubtract (end, start, vec);
    }
    len = VectorNormalize (vec);
    len = min (len, cl_rail_length->value);  // cap length
    MakeNormalVectors (vec, right, up);
    
    VectorScale(vec, cl_rail_space->value*cl_particle_scale->value, vec);
    
    for (i=0; i<len; i += cl_rail_space->value*cl_particle_scale->value)
    {
        d = i * 0.1;
        c = cos(d);
        s = sin(d);
        
        VectorScale (right, c, dir);
        VectorMA (dir, s, up, dir);
        
        setupParticle (
                       0,    0,    0,
                       move[0] + dir[0]*3,    move[1] + dir[1]*3,    move[2] + dir[2]*3,
                       dir[0]*6,    dir[1]*6,    dir[2]*6,
                       0,        0,        0,
                       (isRed)?255:cl_railred->value,    (isRed)?20:cl_railgreen->value,    (isRed)?20:cl_railblue->value,
                       0,    0,    0,
                       1,        -1.0,
                       GL_SRC_ALPHA, GL_ONE,
                       3,    0,
                       particle_generic,
                       0,
                       NULL,0);
        
        VectorAdd (move, vec, move);
    }
}

void pDevRailThink (cparticle_t *p, vec3_t org, vec3_t angle, float *alpha, float *size, int *image, float *time)
{
    int i;
    vec3_t len;
    VectorSubtract(p->angle, org, len);
    
    *size *= (float)(SplashSize/VectorLength(len)) * 0.5/((4-*size));
    if (*size > SplashSize)
        *size = SplashSize;
    
    //setting up angle for sparks
    {
        float time1, time2;
        
        time1 = *time;
        time2 = time1*time1;
        
        for (i=0;i<2;i++)
            angle[i] = 3*(p->vel[i]*time1 + (p->accel[i])*time2);
        angle[2] = 3*(p->vel[2]*time1 + (p->accel[2]-PARTICLE_GRAVITY)*time2);
    }
    
    p->thinknext = true;
}

void CL_DevRailTrail (vec3_t start, vec3_t end, qboolean isRed)
{
    vec3_t        move;
    vec3_t        vec, point;
    float        len;
    int            dec, i=0;
    
    // Draw from closest point
    if (FartherPoint(start, end)) {
        VectorCopy (end, move);
        VectorSubtract (start, end, vec);
    }
    else {
        VectorCopy (start, move);
        VectorSubtract (end, start, vec);
    }
    len = VectorNormalize (vec);
    len = min (len, cl_rail_length->value);  // cap length
    VectorCopy(vec, point);
    
    dec = 4;
    VectorScale (vec, dec, vec);
    
    // FIXME: this is a really silly way to have a loop
    while (len > 0)
    {
        len -= dec;
        i++;
        
        if (i>=DEVRAILSTEPS)
        {
            for (i=3;i>0;i--)
                setupParticle (
                               point[0],    point[1],    point[2],
                               move[0],    move[1],    move[2],
                               0,        0,        0,
                               0,        0,        0,
                               (isRed)?255:cl_railred->value,    (isRed)?20:cl_railgreen->value,    (isRed)?20:cl_railblue->value,
                               0,        -90,    -30,
                               0.75,        -.75,
                               GL_SRC_ALPHA, GL_ONE,
                               dec*DEVRAILSTEPS*TWOTHIRDS,    0,
                               particle_beam2,
                               PART_DIRECTION,
                               NULL,0);
        }
        
        setupParticle (
                       0,    0,    0,
                       move[0],    move[1],    move[2],
                       crand()*10,    crand()*10,    crand()*10+20,
                       0,        0,        0,
                       (isRed)?255:cl_railred->value,    (isRed)?20:cl_railgreen->value,    (isRed)?20:cl_railblue->value,
                       0,    0,    0,
                       1,        -0.75 / (0.5 + frand()*0.3),
                       GL_SRC_ALPHA, GL_ONE,
                       2,            -0.25,
                       0,
                       PART_GRAVITY|PART_SPARK,
                       pDevRailThink,true);
        
        setupParticle (
                       crand()*180, crand()*100, 0,
                       move[0],    move[1],    move[2],
                       crand()*10,    crand()*10,    crand()*10+20,
                       0,        0,        5,
                       255,    255,    255,
                       0,    0,    0,
                       0.25,        -0.25,
                       GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                       5,            10,
                       particle_smoke,
                       PART_TRANS|PART_GRAVITY|PART_OVERBRIGHT,
                       pRotateThink, true);
        
        VectorAdd (move, vec, move);
    }
}


void
CL_RailTrail(vec3_t start, vec3_t end, qboolean isRed)
{
	vec3_t move, last;
	vec3_t vec, point;
	int i;
    int            beamred, beamgreen, beamblue;
    float        len;//, dec;
    qboolean    colored = (cl_railtype->value!=0);

#ifdef DECALS // railgun impact decal
    VectorSubtract (end, start, vec);
    VectorNormalize(vec);
    CL_ParticleRailDecal (end, vec, 7, isRed);
#endif
    
    if (cl_railtype->value == 2)
    {
        CL_DevRailTrail (start, end, isRed);
        return;
    }
    // Draw from closest point
    if (FartherPoint(start, end)) {
        VectorCopy (end, move);
        VectorSubtract (start, end, vec);
    }
    else {
        VectorCopy (start, move);
        VectorSubtract (end, start, vec);
    }
    len = VectorNormalize (vec);
    if (cl_railtype->value == 0)
        len = min (len, cl_rail_length->value);  // cap length
    VectorCopy (vec, point);
    VectorScale (vec, RAILTRAILSPACE, vec);
    //MakeNormalVectors (vec, right, up);
    
    if (colored) {
        if (isRed) {
            beamred = 255;
            beamgreen = beamblue = 20;
        }
        else {
            beamred = cl_railred->value;
            beamgreen = cl_railgreen->value;
            beamblue = cl_railblue->value;
        }
    }
    else
        beamred = beamgreen = beamblue = 255;
    
    while (len > 0)
    {
        VectorCopy (move, last);
        VectorAdd (move, vec, move);
        
        len -= RAILTRAILSPACE;
        
        for (i=0;i<3;i++)
            setupParticle (
                           last[0],    last[1],    last[2],
                           move[0],    move[1],    move[2],
                           0,    0,    0,
                           0,    0,    0,
                           beamred,    beamgreen,    beamblue,
                           0,    0,    0,
                           0.75,        -0.75,
                           GL_SRC_ALPHA, GL_ONE,
                           RAILTRAILSPACE*TWOTHIRDS,    (colored)?0:-5,
                           particle_beam2,
                           PART_BEAM,
                           NULL,0);
    }
    if (cl_railtype->value == 0)
        CL_RailSprial (start, end, isRed);
}

void
CL_IonripperTrail(vec3_t start, vec3_t ent)
{
	vec3_t move;
	vec3_t vec;
    vec3_t  leftdir,up;
	int len;
	int dec;
	int left = 0;

    VectorCopy(start, move);
	VectorSubtract(ent, start, vec);
	len = (int)VectorNormalize(vec);

    MakeNormalVectors (vec, leftdir, up);
    
    dec = 3*cl_particle_scale->value;
    VectorScale (vec, dec, vec);
	VectorScale(vec, 5, vec);

	while (len > 0)
	{
		len -= dec;

		if (!free_particles)
		{
			return;
		}

        setupParticle (
                       0,    0,    0,
                       move[0],    move[1],    move[2],
                       0,    0,    0,
                       0,        0,        0,
                       255,    75,        0,
                       0,    0,    0,
                       0.75,        -1.0 / (0.3 + frand() * 0.2),
                       GL_SRC_ALPHA, GL_ONE,
                       3,            0,            // was dec
                       particle_generic,
                       0,
                       NULL,0);

		VectorAdd(move, vec, move);
	}
}

void
CL_BubbleTrail(vec3_t start, vec3_t end)
{
	vec3_t move;
	vec3_t vec;
	int len;
    int            i;
    float        dec, size;

	VectorCopy(start, move);
	VectorSubtract(end, start, vec);
	len = VectorNormalize(vec);

	dec = 32;
	VectorScale(vec, dec, vec);

	for (i = 0; i < len; i += 32)
	{
		if (!free_particles)
		{
			return;
		}

        size = (frand()>0.25)? 1 : (frand()>0.5) ? 2 : (frand()>0.75) ? 3 : 4;
        
        setupParticle (
                       0,    0,    0,
                       move[0]+crand()*2,    move[1]+crand()*2,    move[2]+crand()*2,
                       crand()*5,    crand()*5,    crand()*5+6,
                       0,        0,        0,
                       255,    255,    255,
                       0,    0,    0,
                       0.75,        -0.5 / (1 + frand() * 0.2),
                       GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                       size,    1,
                       particle_bubble,
                       PART_TRANS|PART_SHADED,
                       NULL,0);

		VectorAdd(move, vec, move);
	}
}

#define    BEAMLENGTH            16

void
CL_FlyParticles(vec3_t origin, int count)
{
	int i;
	float angle;
	float sp, sy, cp, cy;
	vec3_t forward;
	float dist = 64;
	float ltime;
	float time;

	time = (float)cl.time;

	if (count > NUMVERTEXNORMALS)
	{
		count = NUMVERTEXNORMALS;
	}

	if (!avelocities[0][0])
	{
		for (i = 0; i < NUMVERTEXNORMALS; i++)
		{
			avelocities[i][0] = (randk() & 255) * 0.01f;
			avelocities[i][1] = (randk() & 255) * 0.01f;
			avelocities[i][2] = (randk() & 255) * 0.01f;
		}
	}

	ltime = time / 1000.0f;

	for (i = 0; i < count; i += 2)
	{
		angle = ltime * avelocities[i][0];
		sy = (float)sin(angle);
		cy = (float)cos(angle);
		angle = ltime * avelocities[i][1];
		sp = (float)sin(angle);
		cp = (float)cos(angle);

		forward[0] = cp * cy;
		forward[1] = cp * sy;
		forward[2] = -sp;


        setupParticle (
                       0,    0,    0,
                       origin[0] + bytedirs[i][0]*dist + forward[0]*BEAMLENGTH,origin[1] + bytedirs[i][1]*dist + forward[1]*BEAMLENGTH,
                       origin[2] + bytedirs[i][2]*dist + forward[2]*BEAMLENGTH,
                       0,    0,    0,
                       0,    0,    0,
                       0,    0,    0,
                       0,    0,    0,
                       1,        -100,
                       GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                       1+sin(i+ltime),    1,
                       particle_generic,
                       PART_TRANS,
                       NULL,0);
	}
}

void
CL_FlyEffect(centity_t *ent, vec3_t origin)
{
	int n;
	int count;
	int starttime;

	if (ent->fly_stoptime < cl.time)
	{
		starttime = cl.time;
		ent->fly_stoptime = cl.time + 60000;
	}
	else
	{
		starttime = ent->fly_stoptime - 60000;
	}

	n = cl.time - starttime;

	if (n < 20000)
	{
		count = (int)n * 162 / 20000.0;
	}

	else
	{
		n = ent->fly_stoptime - cl.time;

		if (n < 20000)
		{
			count = (int)n * 162 / 20000.0;
		}

		else
		{
			count = 162;
		}
	}

	CL_FlyParticles(origin, count);
}

void pBFGThink (cparticle_t *p, vec3_t org, vec3_t angle, float *alpha, float *size, int *image, float *time)
{
    vec3_t len;
    VectorSubtract(p->angle, p->org, len);
    
    *size = (float)((300/VectorLength(len))*0.75);
}

void
CL_BfgParticles(entity_t *ent)
{
	int i;
	cparticle_t *p;
	float angle;
	float sp, sy, cp, cy;
	vec3_t forward;
	float dist = 64, dist2;
	vec3_t v;
	float ltime;
	float time;

	time = (float)cl.time;

	if (!avelocities[0][0])
	{
		for (i = 0; i < NUMVERTEXNORMALS; i++)
		{
			avelocities[i][0] = (randk() & 255) * 0.01f;
			avelocities[i][1] = (randk() & 255) * 0.01f;
			avelocities[i][2] = (randk() & 255) * 0.01f;
		}
	}

	ltime = time / 1000.0;

	for (i = 0; i < NUMVERTEXNORMALS; i++)
	{
		angle = ltime * avelocities[i][0];
		sy = (float)sin(angle);
		cy = (float)cos(angle);
		angle = ltime * avelocities[i][1];
		sp = (float)sin(angle);
		cp = (float)cos(angle);

		forward[0] = cp * cy;
		forward[1] = cp * sy;
		forward[2] = -sp;

        dist2 = dist;

		dist = (float)sin(ltime + i) * 64;

        p = setupParticle (
                           ent->origin[0],    ent->origin[1],    ent->origin[2],
                           ent->origin[0] + bytedirs[i][0]*dist + forward[0]*BEAMLENGTH,ent->origin[1] + bytedirs[i][1]*dist + forward[1]*BEAMLENGTH,
                           ent->origin[2] + bytedirs[i][2]*dist + forward[2]*BEAMLENGTH,
                           0,    0,        0,
                           0,    0,        0,
                           50,    200*dist2,    20,
                           0,    0,    0,
                           1,        -100,
                           GL_SRC_ALPHA, GL_ONE,
                           1,            1,
                           particle_generic,
                           0,
                           pBFGThink, true);
        
        if (!p)
            return;
        
        VectorSubtract(p->org, ent->origin, v);
		dist = VectorLength(v) / 90.0f;
	}
}

void
CL_TrapParticles(entity_t *ent)
{
	vec3_t move;
	vec3_t vec;
	vec3_t start, end;
	int len;
	int dec;
	float time;

	time = (float)cl.time;

	ent->origin[2] -= 14;
	VectorCopy(ent->origin, start);
	VectorCopy(ent->origin, end);
	end[2] += 64;

	VectorCopy(start, move);
	VectorSubtract(end, start, vec);
	len = (int)VectorNormalize(vec);

	dec = 5;
	VectorScale(vec, 5, vec);

	while (len > 0)
	{
		len -= dec;

        setupParticle (
                       0,    0,    0,
                       move[0] + crand(),    move[1] + crand(),    move[2] + crand(),
                       crand()*15,    crand()*15,    crand()*15,
                       0,    0,    PARTICLE_GRAVITY,
                       230+crand()*25,    125+crand()*25,    25+crand()*25,
                       0,        0,        0,
                       1,        -1.0 / (0.3+frand()*0.2),
                       GL_SRC_ALPHA, GL_ONE,
                       3,            -3,
                       particle_generic,
                       0,
                       NULL,0);

		VectorAdd(move, vec, move);
	}

	{
		int i, j, k;
		float vel;
		vec3_t dir;
		vec3_t org;

		ent->origin[2] += 14;
		VectorCopy(ent->origin, org);

		for (i = -2; i <= 2; i += 4)
		{
			for (j = -2; j <= 2; j += 4)
			{
				for (k = -2; k <= 4; k += 4)
				{
					dir[0] = j * 8.0f;
					dir[1] = i * 8.0f;
					dir[2] = k * 8.0f;

					VectorNormalize(dir);
					vel = (float)(50 + (randk() & 63));

                    setupParticle (
                                   0,    0,    0,
                                   org[0] + i + ((rand()&23) * crand()), org[1] + j + ((rand()&23) * crand()),    org[2] + k + ((rand()&23) * crand()),
                                   dir[0]*vel,    dir[1]*vel,    dir[2]*vel,
                                   0,        0,        0,
                                   230+crand()*25,    125+crand()*25,    25+crand()*25,
                                   0,        0,        0,
                                   1,        -1.0 / (0.3+frand()*0.2),
                                   GL_SRC_ALPHA, GL_ONE,
                                   1,            1,
                                   particle_generic,
                                   PART_GRAVITY,
                                   NULL,0);
                    
                }
			}
		}
	}
}

void
CL_BFGExplosionParticles(vec3_t org)
{
	int i;

	for (i = 0; i < 256; i++)
	{
        setupParticle (
                       0,    0,    0,
                       org[0] + ((rand()%32)-16), org[1] + ((rand()%32)-16),    org[2] + ((rand()%32)-16),
                       (rand()%150)-75,    (rand()%150)-75,    (rand()%150)-75,
                       0,    0,    0,
                       50,    100+rand()*50,    0, //Knightmare- made more green
                       0,    0,    0,
                       1,        -0.8 / (0.5 + frand()*0.3),
                       GL_SRC_ALPHA, GL_ONE,
                       10,            -10,
                       particle_generic,
                       PART_GRAVITY,
                       NULL,0);
	}
}

void
CL_TeleportParticles(vec3_t org)
{
	int i, j, k;
	float vel;
	vec3_t dir;
	float time;

	time = (float)cl.time;

	for (i = -16; i <= 16; i += 4)
	{
		for (j = -16; j <= 16; j += 4)
		{
			for (k = -16; k <= 32; k += 4)
			{
				dir[0] = j * 16.0f;
				dir[1] = i * 16.0f;
				dir[2] = k * 16.0f;

				VectorNormalize(dir);
                vel = 150 + (rand()&63);

                setupParticle (
                               0,    0,    0,
                               org[0]+i+(rand()&3), org[1]+j+(rand()&3),    org[2]+k+(rand()&3),
                               dir[0]*vel,    dir[1]*vel,    dir[2]*vel,
                               0,        0,        0,
                               200 + 55*rand(),    200 + 55*rand(),    200 + 55*rand(),
                               0,        0,        0,
                               1,        -1.0 / (0.3 + (rand()&7) * 0.02),
                               GL_SRC_ALPHA, GL_ONE,
                               1,            3,
                               particle_generic,
                               PART_GRAVITY,
                               NULL,0);
			}
		}
	}
}

/*
 * An entity has just been parsed that has an 
 * event value. the female events are there for
 * backwards compatability
 */
extern struct sfx_s *cl_sfx_footsteps[4];
//Knightmare- Lazarus footstep sounds
extern struct sfx_s    *cl_sfx_metal_footsteps[4];
extern struct sfx_s    *cl_sfx_dirt_footsteps[4];
extern struct sfx_s    *cl_sfx_vent_footsteps[4];
extern struct sfx_s    *cl_sfx_grate_footsteps[4];
extern struct sfx_s    *cl_sfx_tile_footsteps[4];
extern struct sfx_s    *cl_sfx_grass_footsteps[4];
extern struct sfx_s    *cl_sfx_snow_footsteps[4];
extern struct sfx_s    *cl_sfx_carpet_footsteps[4];
extern struct sfx_s    *cl_sfx_force_footsteps[4];
extern struct sfx_s    *cl_sfx_gravel_footsteps[4];
extern struct sfx_s    *cl_sfx_ice_footsteps[4];
extern struct sfx_s    *cl_sfx_sand_footsteps[4];
extern struct sfx_s    *cl_sfx_wood_footsteps[4];

extern struct sfx_s    *cl_sfx_slosh[4];
extern struct sfx_s    *cl_sfx_wade[4];
extern struct sfx_s    *cl_sfx_mud_wade[2];
extern struct sfx_s    *cl_sfx_ladder[4];
//end Knightmare

/*
 ===============
 ReadTextureSurfaceAssignments
 Reads in defintions for footsteps based on texture name.
 ===============
 */
#if 1
qboolean buf_gets (char *dest, int destsize, char **f)
{
    char *old = *f;
    *f = strchr (old, '\n');
    if (!*f)
    {    // no more new lines
        *f = old + strlen(old);
        if (!strlen(*f))
            return false;    // end of file, nothing else to grab
    }
    (*f)++; // advance past EOL
    strncpy (dest, old, min(destsize-1, (int)(*f-old-1)) );
    return true;
}

void ReadTextureSurfaceAssignments()
{
    char    filename[MAX_OSPATH];
    char    *footstep_data;
    char    *parsedata;
    char    line[80];
    
    num_texsurfs = 0;
    
    Com_sprintf (filename, sizeof(filename), "texsurfs.txt");
    FS_LoadFile (filename, (void **)&footstep_data);
    parsedata = footstep_data;
    if (!footstep_data) return;
    while (buf_gets(line, sizeof(line), &parsedata) && num_texsurfs < MAX_TEX_SURF)
    {
        sscanf(line,"%d %s",&tex_surf[num_texsurfs].step_id,tex_surf[num_texsurfs].tex);
        //Com_Printf("%d %s\n",tex_surf[num_texsurfs].step_id,tex_surf[num_texsurfs].tex);
        num_texsurfs++;
    }
    FS_FreeFile (footstep_data);
}
#else
void ReadTextureSurfaceAssignments()
{
    char    filename[MAX_OSPATH];
    FILE    *f;
    char    line[80];
    
    num_texsurfs = 0;
    
    Com_sprintf (filename, sizeof(filename), "texsurfs.txt");
    FS_FOpenFile (filename, &f);
    if (!f) return;
    while (fgets(line, sizeof(line), f) && num_texsurfs < MAX_TEX_SURF)
    {
        sscanf(line,"%d %s",&tex_surf[num_texsurfs].step_id,tex_surf[num_texsurfs].tex);
        //Com_Printf("%d %s\n",tex_surf[num_texsurfs].step_id,tex_surf[num_texsurfs].tex);
        num_texsurfs++;
    }
    fclose(f);
}
#endif


/*
 ===============
 CL_FootSteps
 Plays appropriate footstep sound depending on surface flags of the ground surface.
 Since this is a replacement for plain Jane EV_FOOTSTEP, we already know
 the player is definitely on the ground when this is called.
 ===============
 */
void CL_FootSteps (entity_state_t *ent, qboolean loud)
{
    trace_t    tr;
    vec3_t    end;
    int        r;
    int        surface;
    struct    sfx_s    *stepsound = NULL;
    float    volume = 0.5;
    
    r = (rand()&3);
    
    VectorCopy(ent->origin,end);
    end[2] -= 64;
    tr = CL_PMSurfaceTrace (ent->number, ent->origin,NULL,NULL,end,MASK_SOLID | MASK_WATER);
    if (!tr.surface)
        return;
    surface = tr.surface->flags & SURF_STEPMASK;
    switch (surface)
    {
        case SURF_METAL:
            stepsound = cl_sfx_metal_footsteps[r];
            break;
        case SURF_DIRT:
            stepsound = cl_sfx_dirt_footsteps[r];
            break;
        case SURF_VENT:
            stepsound = cl_sfx_vent_footsteps[r];
            break;
        case SURF_GRATE:
            stepsound = cl_sfx_grate_footsteps[r];
            break;
        case SURF_TILE:
            stepsound = cl_sfx_tile_footsteps[r];
            break;
        case SURF_GRASS:
            stepsound = cl_sfx_grass_footsteps[r];
            break;
        case SURF_SNOW:
            stepsound = cl_sfx_snow_footsteps[r];
            break;
        case SURF_CARPET:
            stepsound = cl_sfx_carpet_footsteps[r];
            break;
        case SURF_FORCE:
            stepsound = cl_sfx_force_footsteps[r];
            break;
        case SURF_GRAVEL:
            stepsound = cl_sfx_gravel_footsteps[r];
            break;
        case SURF_ICE:
            stepsound = cl_sfx_ice_footsteps[r];
            break;
        case SURF_SAND:
            stepsound = cl_sfx_sand_footsteps[r];
            break;
        case SURF_WOOD:
            stepsound = cl_sfx_wood_footsteps[r];
            break;
        case SURF_STANDARD:
            stepsound = cl_sfx_footsteps[r];
            volume = 1.0;
            break;
        default:
            if (cl_footstep_override->value && num_texsurfs)
            {
                int    i;
                for (i=0; i<num_texsurfs; i++)
                    if (strstr(tr.surface->name,tex_surf[i].tex) && tex_surf[i].step_id > 0)
                    {
                        tr.surface->flags |= (SURF_METAL << (tex_surf[i].step_id - 1));
                        CL_FootSteps (ent, loud); // start over
                        return;
                    }
            }
            tr.surface->flags |= SURF_STANDARD;
            CL_FootSteps (ent, loud); // start over
            return;
    }
    
    if (loud)
    {
        if (volume == 1.0)
            S_StartSound (NULL, ent->number, CHAN_AUTO, stepsound, 1.0, ATTN_NORM, 0);
        else
            volume = 1.0;
    }
    S_StartSound (NULL, ent->number, CHAN_BODY, stepsound, volume, ATTN_NORM, 0);
}
//end Knightmare


void
CL_EntityEvent(entity_state_t *ent)
{
	switch (ent->event)
	{
		case EV_ITEM_RESPAWN:
			S_StartSound(NULL, ent->number, CHAN_WEAPON,
				S_RegisterSound("items/respawn1.wav"), 1, ATTN_IDLE, 0);
			CL_ItemRespawnParticles(ent->origin);
			break;
		case EV_PLAYER_TELEPORT:
			S_StartSound(NULL, ent->number, CHAN_WEAPON,
				S_RegisterSound("misc/tele1.wav"), 1, ATTN_IDLE, 0);
			CL_TeleportParticles(ent->origin);
			break;
		case EV_FOOTSTEP:

			if (cl_footsteps->value)
			{
//Knightmare- Lazarus footsteps
//                S_StartSound(NULL, ent->number, CHAN_BODY,
//                        cl_sfx_footsteps[randk() & 3], 1, ATTN_NORM, 0);
                CL_FootSteps (ent, false);
			}
            
			break;
        case EV_LOUDSTEP:
            if (cl_footsteps->value)
                CL_FootSteps (ent, true);
            break;
//end Knightmare
		case EV_FALLSHORT:
			S_StartSound(NULL, ent->number, CHAN_AUTO,
				S_RegisterSound("player/land1.wav"), 1, ATTN_NORM, 0);
			break;
		case EV_FALL:
			S_StartSound(NULL, ent->number, CHAN_AUTO,
				S_RegisterSound("*fall2.wav"), 1, ATTN_NORM, 0);
			break;
		case EV_FALLFAR:
			S_StartSound(NULL, ent->number, CHAN_AUTO,
				S_RegisterSound("*fall1.wav"), 1, ATTN_NORM, 0);
			break;
//Knightmare- more Lazarus sounds
        case EV_SLOSH:
            S_StartSound (NULL, ent->number, CHAN_BODY, cl_sfx_slosh[rand()&3], 0.5, ATTN_NORM, 0);
            break;
        case EV_WADE:
            S_StartSound (NULL, ent->number, CHAN_BODY, cl_sfx_wade[rand()&3], 0.5, ATTN_NORM, 0);
            break;
        case EV_WADE_MUD:
            S_StartSound (NULL, ent->number, CHAN_BODY, cl_sfx_mud_wade[rand()&1], 0.5, ATTN_NORM, 0);
            break;
        case EV_CLIMB_LADDER:
            S_StartSound (NULL, ent->number, CHAN_BODY, cl_sfx_ladder[rand()&3], 0.5, ATTN_NORM, 0);
            break;
//end Knightmare
	}
}

void
CL_ClearEffects(void)
{
	CL_ClearParticles();
#ifdef DECALS
    CL_ClearAllDecalPolys ();
#endif
	CL_ClearDlights();
	CL_ClearLightStyles();
}

#ifdef DECALS
/*
 ==============
 CL_UnclipDecals
 Removes decal fragment pointers
 and resets decal fragment data
 Called during a vid_restart
 ==============
 */
void CL_UnclipDecals (void)
{
    cparticle_t    *p;
    
    //Com_Printf ("Unclipping decals\n");
    for (p=active_particles; p; p=p->next)
    {
        p->decalnum = 0;
        p->decal = NULL;
    }
    CL_ClearAllDecalPolys();
}

/*
 ==============
 CL_ReclipDecals
 Re-clips all decals
 Called during a vid_restart
 ==============
 */
void CL_ReclipDecals (void)
{
    cparticle_t    *p;
    vec3_t        dir;
    
    //Com_Printf ("Reclipping decals\n");
    for (p=active_particles; p; p=p->next)
    {
        p->decalnum = 0;
        p->decal = NULL;
        if (p->flags & PART_DECAL)
        {
            AngleVectors (p->angle, dir, NULL, NULL);
            VectorNegate(dir, dir);
            clipDecal(p, p->size, -p->angle[2], p->org, dir);
            
            if (!p->decalnum) // kill on viewframe
                p->alpha = 0;
        }
    }
}
#endif

void
CL_Flashlight(int ent, vec3_t pos)
{
	cdlight_t *dl;

	dl = CL_AllocDlight(ent);
	VectorCopy(pos, dl->origin);
	dl->radius = 400;
	dl->minlight = 250;
	dl->die = cl.time + 100;
	dl->color[0] = 1;
	dl->color[1] = 1;
	dl->color[2] = 1;
}

void
CL_ColorFlash(vec3_t pos, int ent, float intensity, float r, float g, float b)
{
	cdlight_t *dl;

	dl = CL_AllocDlight(ent);
	VectorCopy(pos, dl->origin);
	dl->radius = intensity;
	dl->minlight = 250;
	dl->die = cl.time + 100;
	dl->color[0] = r;
	dl->color[1] = g;
	dl->color[2] = b;
}

void
CL_DebugTrail(vec3_t start, vec3_t end)
{
	vec3_t move;
	vec3_t vec;
	float len;
	cparticle_t *p;
	float dec;
	vec3_t right, up;

	VectorCopy(start, move);
	VectorSubtract(end, start, vec);
	len = VectorNormalize(vec);

	MakeNormalVectors(vec, right, up);

	dec = 3;
	VectorScale(vec, dec, vec);
	VectorCopy(start, move);

	while (len > 0)
	{
		len -= dec;

		if (!free_particles)
		{
			return;
		}

		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;

		p->time = (float)cl.time;
		VectorClear(p->accel);
		VectorClear(p->vel);
		p->alpha = 1.0;
		p->alphavel = -0.1f;
		p->color = 0x74 + (randk() & 7);
		VectorCopy(move, p->org);
		VectorAdd(move, vec, move);
	}
}

void
CL_SmokeTrail(vec3_t start, vec3_t end, int colorStart,
		int colorRun, int spacing)
{
	vec3_t move;
	vec3_t vec;
	float len, time;
	int j;
	cparticle_t *p;

	VectorCopy(start, move);
	VectorSubtract(end, start, vec);
	len = VectorNormalize(vec);

	VectorScale(vec, spacing, vec);

	time = (float)cl.time;

	while (len > 0)
	{
		len -= spacing;

		if (!free_particles)
		{
			return;
		}

		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;
		VectorClear(p->accel);

		p->time = time;

		p->alpha = 1.0;
		p->alphavel = -1.0f / (1 + frandk() * 0.5f);
		p->color = colorStart + (float)(randk() % colorRun);

		for (j = 0; j < 3; j++)
		{
			p->org[j] = move[j] + crandk() * 3;
			p->accel[j] = 0;
		}

		p->vel[2] = 20 + crandk() * 5;

		VectorAdd(move, vec, move);
	}
}

void
CL_ForceWall(vec3_t start, vec3_t end, int color8)
{
	vec3_t move;
	vec3_t vec;
	int j;
	cparticle_t *p;

	float len, time;

	VectorCopy(start, move);
	VectorSubtract(end, start, vec);
	len = VectorNormalize(vec);

	VectorScale(vec, 4, vec);

	time = (float)cl.time;

	while (len > 0)
	{
		len -= 4;

		if (!free_particles)
		{
			return;
		}

		if (frandk() > 0.3)
		{
			p = free_particles;
			free_particles = p->next;
			p->next = active_particles;
			active_particles = p;
			VectorClear(p->accel);

			p->time = time;

			p->alpha = 1.0;
			p->alphavel = -1.0f / (3.0 + frandk() * 0.5f);
			p->color = color8;

			for (j = 0; j < 3; j++)
			{
				p->org[j] = move[j] + crandk() * 3;
				p->accel[j] = 0;
			}

			p->vel[0] = 0;
			p->vel[1] = 0;
			p->vel[2] = -40 - (crandk() * 10);
		}

		VectorAdd(move, vec, move);
	}
}

/*
 * CL_BubbleTrail2 (lets you control the # of bubbles
 * by setting the distance between the spawns)
 */
void
CL_BubbleTrail2(vec3_t start, vec3_t end, int dist)
{
	vec3_t move;
	vec3_t vec;
	float len, time;
	int i;
	int j;
	cparticle_t *p;

	time = (float)cl.time;

	VectorCopy(start, move);
	VectorSubtract(end, start, vec);
	len = VectorNormalize(vec);

	VectorScale(vec, dist, vec);

	for (i = 0; i < len; i += dist)
	{
		if (!free_particles)
		{
			return;
		}

		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;

		VectorClear(p->accel);
		p->time = time;

		p->alpha = 1.0;
		p->alphavel = -1.0f / (1 + frandk() * 0.1f);
		p->color = 4 + (randk() & 7);

		for (j = 0; j < 3; j++)
		{
			p->org[j] = move[j] + crandk() * 2;
			p->vel[j] = crandk() * 10;
		}

		p->org[2] -= 4;
		p->vel[2] += 20;
		VectorAdd(move, vec, move);
	}
}

void
CL_Heatbeam(vec3_t start, vec3_t forward)
{
	vec3_t move;
	vec3_t vec;
	float len;
	int j;
	cparticle_t *p;
	vec3_t right, up;
	float i;
	float c, s;
	vec3_t dir;
	float ltime;
	float step = 32.0, rstep;
	float start_pt;
	float rot;
	float variance;
	float time;
	vec3_t end;

	VectorMA(start, 4096, forward, end);

	VectorCopy(start, move);
	VectorSubtract(end, start, vec);
	len = VectorNormalize(vec);

	VectorCopy(cl.v_right, right);
	VectorCopy(cl.v_up, up);

	VectorMA(move, -0.5, right, move);
	VectorMA(move, -0.5, up, move);

	time = (float)cl.time;

	ltime = (float)cl.time / 1000.0f;
	start_pt = (float)fmod(ltime * 96.0f, step);
	VectorMA(move, start_pt, vec, move);

	VectorScale(vec, step, vec);

	rstep = M_PI / 10.0f;

	for (i = start_pt; i < len; i += step)
	{
		if (i > step * 5) /* don't bother after the 5th ring */
		{
			break;
		}

		for (rot = 0; rot < M_PI * 2; rot += rstep)
		{
			if (!free_particles)
			{
				return;
			}

			p = free_particles;
			free_particles = p->next;
			p->next = active_particles;
			active_particles = p;

			p->time = time;
			VectorClear(p->accel);
			variance = 0.5;
			c = (float)cos(rot) * variance;
			s = (float)sin(rot) * variance;

			/* trim it so it looks like it's starting at the origin */
			if (i < 10)
			{
				VectorScale(right, c * (i / 10.0f), dir);
				VectorMA(dir, s * (i / 10.0f), up, dir);
			}
			else
			{
				VectorScale(right, c, dir);
				VectorMA(dir, s, up, dir);
			}

			p->alpha = 0.5;
			p->alphavel = -1000.0;
			p->color = 223 - (randk() & 7);

			for (j = 0; j < 3; j++)
			{
				p->org[j] = move[j] + dir[j] * 3;
				p->vel[j] = 0;
			}
		}

		VectorAdd(move, vec, move);
	}
}

/*
 *Puffs with velocity along direction, with some randomness thrown in
 */
void
CL_ParticleSteamEffect(vec3_t org, vec3_t dir, int color,
		int count, int magnitude)
{
	int i, j;
	cparticle_t *p;
	float d, time;
	vec3_t r, u;

	time = (float)cl.time;
	MakeNormalVectors(dir, r, u);

	for (i = 0; i < count; i++)
	{
		if (!free_particles)
		{
			return;
		}

		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;

		p->time = time;
		p->color = color + (randk() & 7);

		for (j = 0; j < 3; j++)
		{
			p->org[j] = org[j] + magnitude * 0.1f * crandk();
		}

		VectorScale(dir, magnitude, p->vel);
		d = crandk() * magnitude / 3;
		VectorMA(p->vel, d, r, p->vel);
		d = crandk() * magnitude / 3;
		VectorMA(p->vel, d, u, p->vel);

		p->accel[0] = p->accel[1] = 0;
		p->accel[2] = -PARTICLE_GRAVITY / 2;
		p->alpha = 1.0;

		p->alphavel = -1.0f / (0.5f + frandk() * 0.3f);
	}
}

void
CL_ParticleSteamEffect2(cl_sustain_t *self)
{
	int i, j;
	cparticle_t *p;
	float d;
	vec3_t r, u;
	vec3_t dir;

	VectorCopy(self->dir, dir);
	MakeNormalVectors(dir, r, u);

	for (i = 0; i < self->count; i++)
	{
		if (!free_particles)
		{
			return;
		}

		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;

		p->time = cl.time;
		p->color = self->color + (randk() & 7);

		for (j = 0; j < 3; j++)
		{
			p->org[j] = self->org[j] + self->magnitude * 0.1 * crandk();
		}

		VectorScale(dir, self->magnitude, p->vel);
		d = crandk() * self->magnitude / 3;
		VectorMA(p->vel, d, r, p->vel);
		d = crandk() * self->magnitude / 3;
		VectorMA(p->vel, d, u, p->vel);

		p->accel[0] = p->accel[1] = 0;
		p->accel[2] = -PARTICLE_GRAVITY / 2;
		p->alpha = 1.0;

		p->alphavel = -1.0 / (0.5 + frandk() * 0.3);
	}

	self->nextthink += self->thinkinterval;
}

void
CL_TrackerTrail(vec3_t start, vec3_t end, int particleColor)
{
	vec3_t move;
	vec3_t vec;
	vec3_t forward, right, up, angle_dir;
	float len;
	int j;
	cparticle_t *p;
	int dec;
	float dist;
	float time;

	time = (float)cl.time;
	VectorCopy(start, move);
	VectorSubtract(end, start, vec);
	len = VectorNormalize(vec);

	VectorCopy(vec, forward);
	AngleVectors2(forward, angle_dir);
	AngleVectors(angle_dir, forward, right, up);

	dec = 3;
	VectorScale(vec, 3, vec);

	while (len > 0)
	{
		len -= dec;

		if (!free_particles)
		{
			return;
		}

		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;
		VectorClear(p->accel);

		p->time = time;

		p->alpha = 1.0;
		p->alphavel = -2.0;
		p->color = particleColor;
		dist = DotProduct(move, forward);
		VectorMA(move, 8 * cos(dist), up, p->org);

		for (j = 0; j < 3; j++)
		{
			p->vel[j] = 0;
			p->accel[j] = 0;
		}

		p->vel[2] = 5;

		VectorAdd(move, vec, move);
	}
}

void
CL_Tracker_Shell(vec3_t origin)
{
	vec3_t dir;
	int i;
	cparticle_t *p;
	float time;

	time = (float)cl.time;

	for (i = 0; i < 300; i++)
	{
		if (!free_particles)
		{
			return;
		}

		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;
		VectorClear(p->accel);

		p->time = time;

		p->alpha = 1.0;
		p->alphavel = INSTANT_PARTICLE;
		p->color = 0;
		dir[0] = crandk();
		dir[1] = crandk();
		dir[2] = crandk();
		VectorNormalize(dir);

		VectorMA(origin, 40, dir, p->org);
	}
}

void
CL_MonsterPlasma_Shell(vec3_t origin)
{
	vec3_t dir;
	int i;
	cparticle_t *p;
	float time;

	time = (float)cl.time;

	for (i = 0; i < 40; i++)
	{
		if (!free_particles)
		{
			return;
		}

		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;
		VectorClear(p->accel);

		p->time = time;

		p->alpha = 1.0;
		p->alphavel = INSTANT_PARTICLE;
		p->color = 0xe0;
		dir[0] = crandk();
		dir[1] = crandk();
		dir[2] = crandk();
		VectorNormalize(dir);

		VectorMA(origin, 10, dir, p->org);
	}
}

void
CL_Widowbeamout(cl_sustain_t *self)
{
	vec3_t dir;
	int i;
	cparticle_t *p;
	static int colortable[4] = {2 * 8, 13 * 8, 21 * 8, 18 * 8};
	float ratio;
	float time;

	ratio = 1.0f - (((float)self->endtime - (float)cl.time) / 2100.0f);
	time = (float)cl.time;

	for (i = 0; i < 300; i++)
	{
		if (!free_particles)
		{
			return;
		}

		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;
		VectorClear(p->accel);

		p->time = time;

		p->alpha = 1.0;
		p->alphavel = INSTANT_PARTICLE;
		p->color = colortable[randk() & 3];
		dir[0] = crandk();
		dir[1] = crandk();
		dir[2] = crandk();
		VectorNormalize(dir);

		VectorMA(self->org, (45.0 * ratio), dir, p->org);
	}
}

void
CL_Nukeblast(cl_sustain_t *self)
{
	vec3_t dir;
	int i;
	cparticle_t *p;
	static int colortable[4] = {110, 112, 114, 116};
	float ratio;
	float time;

	ratio = 1.0f - (((float)self->endtime - (float)cl.time) / 1000.0f);
	time = (float)cl.time;

	for (i = 0; i < 700; i++)
	{
		if (!free_particles)
		{
			return;
		}

		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;
		VectorClear(p->accel);

		p->time = time;

		p->alpha = 1.0;
		p->alphavel = INSTANT_PARTICLE;
		p->color = colortable[randk() & 3];
		dir[0] = crandk();
		dir[1] = crandk();
		dir[2] = crandk();
		VectorNormalize(dir);

		VectorMA(self->org, (200.0 * ratio), dir, p->org);
	}
}

void
CL_WidowSplash(vec3_t org)
{
	static int colortable[4] = {2 * 8, 13 * 8, 21 * 8, 18 * 8};
	int i;
	cparticle_t *p;
	vec3_t dir;
	float time;

	time = (float)cl.time;

	for (i = 0; i < 256; i++)
	{
		if (!free_particles)
		{
			return;
		}

		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;

		p->time = time;
		p->color = colortable[randk() & 3];
		dir[0] = crandk();
		dir[1] = crandk();
		dir[2] = crandk();
		VectorNormalize(dir);
		VectorMA(org, 45.0, dir, p->org);
		VectorMA(vec3_origin, 40.0, dir, p->vel);

		p->accel[0] = p->accel[1] = 0;
		p->alpha = 1.0;

		p->alphavel = -0.8f / (0.5f + frandk() * 0.3f);
	}
}

void
CL_Tracker_Explode(vec3_t origin)
{
	vec3_t dir, backdir;
	int i;
	cparticle_t *p;
	float time;

	time = (float)cl.time;

	for (i = 0; i < 300; i++)
	{
		if (!free_particles)
		{
			return;
		}

		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;
		VectorClear(p->accel);

		p->time = time;

		p->alpha = 1.0;
		p->alphavel = -1.0;
		p->color = 0;
		dir[0] = crandk();
		dir[1] = crandk();
		dir[2] = crandk();
		VectorNormalize(dir);
		VectorScale(dir, -1, backdir);

		VectorMA(origin, 64, dir, p->org);
		VectorScale(backdir, 64, p->vel);
	}
}

void
CL_TagTrail(vec3_t start, vec3_t end, int color)
{
	vec3_t move;
	vec3_t vec;
	float len;
	int j;
	cparticle_t *p;
	int dec;
	float time;

	time = (float)cl.time;

	VectorCopy(start, move);
	VectorSubtract(end, start, vec);
	len = VectorNormalize(vec);

	dec = 5;
	VectorScale(vec, 5, vec);

	while (len >= 0)
	{
		len -= dec;

		if (!free_particles)
		{
			return;
		}

		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;
		VectorClear(p->accel);

		p->time = time;

		p->alpha = 1.0;
		p->alphavel = -1.0f / (0.8f + frandk() * 0.2f);
		p->color = color;

		for (j = 0; j < 3; j++)
		{
			p->org[j] = move[j] + crandk() * 16;
			p->vel[j] = crandk() * 5;
			p->accel[j] = 0;
		}

		VectorAdd(move, vec, move);
	}
}

void
CL_ColorExplosionParticles(vec3_t org, int color, int run)
{
	int i;
	int j;
	cparticle_t *p;
	float time;

	time = (float)cl.time;

	for (i = 0; i < 128; i++)
	{
		if (!free_particles)
		{
			return;
		}

		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;

		p->time = time;
		p->color = color + (randk() % run);

		for (j = 0; j < 3; j++)
		{
			p->org[j] = org[j] + ((randk() % 32) - 16);
			p->vel[j] = (randk() % 256) - 128;
		}

		p->accel[0] = p->accel[1] = 0;
		p->accel[2] = -PARTICLE_GRAVITY;
		p->alpha = 1.0;

		p->alphavel = -0.4f / (0.6f + frandk() * 0.2f);
	}
}

/*
 * Like the steam effect, but unaffected by gravity
 */
void
CL_ParticleSmokeEffect(vec3_t org, vec3_t dir, int color,
		int count, int magnitude)
{
	int i, j;
	cparticle_t *p;
	float d;
	vec3_t r, u;
	float time;

	time = (float)cl.time;

	MakeNormalVectors(dir, r, u);

	for (i = 0; i < count; i++)
	{
		if (!free_particles)
		{
			return;
		}

		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;

		p->time = time;
		p->color = color + (randk() & 7);

		for (j = 0; j < 3; j++)
		{
			p->org[j] = org[j] + magnitude * 0.1f * crandk();
		}

		VectorScale(dir, magnitude, p->vel);
		d = crandk() * magnitude / 3;
		VectorMA(p->vel, d, r, p->vel);
		d = crandk() * magnitude / 3;
		VectorMA(p->vel, d, u, p->vel);

		p->accel[0] = p->accel[1] = p->accel[2] = 0;
		p->alpha = 1.0;

		p->alphavel = -1.0f / (0.5f + frandk() * 0.3f);
	}
}

/*
 * Wall impact puffs (Green)
 */
void
CL_BlasterParticles2(vec3_t org, vec3_t dir, unsigned int color)
{
	int i, j;
	cparticle_t *p;
	float d;
	int count;
	float time;

	time = (float)cl.time;

	count = 40;

	for (i = 0; i < count; i++)
	{
		if (!free_particles)
		{
			return;
		}

		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;

		p->time = time;
		p->color = color + (randk() & 7);
		d = (float)(randk() & 15);

		for (j = 0; j < 3; j++)
		{
			p->org[j] = org[j] + ((randk() & 7) - 4) + d * dir[j];
			p->vel[j] = dir[j] * 30 + crandk() * 40;
		}

		p->accel[0] = p->accel[1] = 0;
		p->accel[2] = -PARTICLE_GRAVITY;
		p->alpha = 1.0;

		p->alphavel = -1.0f / (0.5f + frandk() * 0.3f);
	}
}

/*
 * Green!
 */
void
CL_BlasterTrail2(vec3_t start, vec3_t end)
{
	vec3_t move;
	vec3_t vec;
	float len;
	int j;
	cparticle_t *p;
	int dec;
	float time;

	time = (float)cl.time;

	VectorCopy(start, move);
	VectorSubtract(end, start, vec);
	len = VectorNormalize(vec);

	dec = 5;
	VectorScale(vec, 5, vec);

	while (len > 0)
	{
		len -= dec;

		if (!free_particles)
		{
			return;
		}

		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;
		VectorClear(p->accel);

		p->time = time;

		p->alpha = 1.0;
		p->alphavel = -1.0f / (float)(0.3f + frandk() * 0.2f);

		for (j = 0; j < 3; j++)
		{
			p->org[j] = move[j] + crandk();
			p->vel[j] = crandk() * 5;
			p->accel[j] = 0;
		}

		VectorAdd(move, vec, move);
	}
}

void CL_LightningBeam (vec3_t start, vec3_t end, int srcEnt, int dstEnt, float size)
{
    cparticle_t *list;
    cparticle_t *p=NULL;
    
    for (list=active_particles ; list ; list=list->next)
        if (list->src_ent == srcEnt && list->dst_ent == dstEnt && list->image == particle_lightning)
        {
            p=list;
            /*p->start =*/ p->time = cl.time;
            VectorCopy(start, p->angle);
            VectorCopy(end, p->org);
            
            return;
        }
    
    p = setupParticle (
                       start[0],    start[1],    start[2],
                       end[0],        end[1],        end[2],
                       0,    0,    0,
                       0,        0,        0,
                       255,    255,    255,
                       0,    0,    0,
                       1,        -2,
                       GL_SRC_ALPHA, GL_ONE,
                       size,        0,
                       particle_lightning,
                       PART_LIGHTNING,
                       0, false);
    
    if (!p)
        return;
    
    p->src_ent=srcEnt;
    p->dst_ent=dstEnt;
}

/*
 ===============
 CL_Explosion_Decal
 ===============
 */
#ifdef DECALS
void CL_Explosion_Decal (vec3_t org, float size)
{
    if (r_decals->value)
    {
        int i, j;
        cparticle_t *p;
        vec3_t angle[6], ang;
        trace_t    trace1, trace2;
        vec3_t end1, end2, normal, sorg, dorg;
        vec3_t    planenormals[6];
        
        VectorSet(angle[0], -1, 0, 0);
        VectorSet(angle[1], 1, 0, 0);
        VectorSet(angle[2], 0, 1, 0);
        VectorSet(angle[3], 0, -1, 0);
        VectorSet(angle[4], 0, 0, 1);
        VectorSet(angle[5], 0, 0, -1);
        
        for (i=0;i<6;i++)
        {
            VectorMA(org, -2, angle[i], sorg); // move origin 2 units back
            VectorMA(sorg, size/2, angle[i], end1);
            trace1 = CL_Trace (sorg, end1, 0, CONTENTS_SOLID);
            if (trace1.fraction < 1) // hit a surface
            {    // make sure we haven't hit this plane before
                VectorCopy(trace1.plane.normal, planenormals[i]);
                for (j=0; j<i; j++)
                    if (VectorCompare(planenormals[j],planenormals[i])) continue;
                // try tracing directly to hit plane
                VectorNegate(trace1.plane.normal, normal);
                VectorMA(sorg, size/2, normal, end2);
                trace2 = CL_Trace (sorg, end2, 0, CONTENTS_SOLID);
                // if seond trace hit same plane
                if (trace2.fraction < 1 && VectorCompare(trace2.plane.normal, trace1.plane.normal))
                    VectorCopy(trace2.endpos, dorg);
                else
                    VectorCopy(trace1.endpos, dorg);
                //if (CM_PointContents(dorg,0) & MASK_WATER) // no scorch marks underwater
                //    continue;
                vectoanglerolled(normal, rand()%360, ang);
                p = setupParticle (
                                   ang[0],    ang[1],    ang[2],
                                   dorg[0],dorg[1],dorg[2],
                                   0,        0,        0,
                                   0,        0,        0,
                                   255,    255,    255,
                                   0,        0,        0,
                                   1,        -1/r_decal_life->value,
                                   GL_ZERO, GL_ONE_MINUS_SRC_ALPHA,
                                   size,        0,
                                   particle_burnmark,
                                   PART_SHADED|PART_DECAL|PART_ALPHACOLOR,
                                   thinkDecalAlpha, true);
            }
            /*vectoanglerolled(angle[i], rand()%360, ang);
             p = setupParticle (
             ang[0],    ang[1],    ang[2],
             org[0],    org[1],    org[2],
             0,        0,        0,
             0,        0,        0,
             255,    255,    255,
             0,        0,        0,
             1,        -0.001,
             GL_ZERO, GL_ONE_MINUS_SRC_ALPHA,
             size,        0,
             particle_burnmark,
             PART_SHADED|PART_DECAL|PART_ALPHACOLOR,
             thinkDecalAlpha, true);*/
        }
    }
}
#endif

/*
 ===============
 CL_Explosion_Particle
 
 Explosion effect
 ===============
 */
void pExplosionThink (cparticle_t *p, vec3_t org, vec3_t angle, float *alpha, float *size, int *image, float *time)
{
    if (*alpha>.85)
        *image = particle_rexplosion1;
    else if (*alpha>.7)
        *image = particle_rexplosion2;
    else if (*alpha>.5)
        *image = particle_rexplosion3;
    else if (*alpha>.4)
        *image = particle_rexplosion4;
    else if (*alpha>.25)
        *image = particle_rexplosion5;
    else if (*alpha>.1)
        *image = particle_rexplosion6;
    else
        *image = particle_rexplosion7;
    
    *alpha *= 3.0;
    
    if (*alpha > 1.0)
        *alpha = 1;
    
    p->thinknext = true;
}

void pExplosionBubbleThink (cparticle_t *p, vec3_t org, vec3_t angle, float *alpha, float *size, int *image, float *time)
{
    
    if (CM_PointContents(org,0) & MASK_WATER)
        p->thinknext = true;
    else
    {
        p->think = NULL;
        p->alpha = 0;
    }
}

void CL_Explosion_Particle (vec3_t org, float size, qboolean rocket)
{
    cparticle_t *p;
    
    p = setupParticle (
                       0,        0,        0,
                       org[0],    org[1],    org[2],
                       0,        0,        0,
                       0,        0,        0,
                       255,    255,    255,
                       0,        0,        0,
                       1,        (rocket)? -2 : -1.5,
                       GL_SRC_ALPHA, GL_ONE,
                       //GL_ONE, GL_ONE,
                       (size!=0)?size:(150-(!rocket)?75:0),    0,
                       particle_rexplosion1,
                       PART_DEPTHHACK_SHORT,
                       pExplosionThink, true);
    
    if (p)
    {    // smooth color blend :D
        /*    addParticleLight (p, 225, 0, 1, 0, 0);
         addParticleLight (p, 250, 0, 1, 0.3, 0);
         addParticleLight (p, 275, 0, 1, 0.6, 0);
         addParticleLight (p, 300, 0, 1, 1, 0);*/
        // use just one, 4 lights kills the framerate
        addParticleLight (p, 300, 0, 1, 0.514, 0);
    }
}

/*
 ===============
 CL_Explosion_FlashParticle
 
 Explosion fash
 ===============
 */
void CL_Explosion_FlashParticle (vec3_t org, float size, qboolean large)
{
    if (large)
    {
        setupParticle (
                       0,        0,        0,
                       org[0],    org[1],    org[2],
                       0,        0,        0,
                       0,        0,        0,
                       255,    175,    100,
                       0,        0,        0,
                       1,        -1.75,
                       GL_SRC_ALPHA, GL_ONE,
                       //GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                       (size!=0)?size:50,    -10,
                       //100-(!rocket)?50:0,    -10,
                       particle_rflash,
                       PART_DEPTHHACK_SHORT,
                       NULL,0);
    }
    else
    {
        setupParticle (
                       0,        0,        0,
                       org[0],    org[1],    org[2],
                       0,        0,        0,
                       0,        0,        0,
                       255,    175,    100,
                       0,        0,        0,
                       1,        -1.75,
                       GL_SRC_ALPHA, GL_ONE,
                       (size!=0)?size:50,    -10,
                       //100-(!rocket)?50:0,    -10,
                       particle_blaster,
                       0,
                       NULL,0);
    }
}


/*
 ===============
 CL_Explosion_Sparks
 ===============
 */
void pExplosionSparksThink (cparticle_t *p, vec3_t org, vec3_t angle, float *alpha, float *size, int *image, float *time)
{
    int i;
    
    //setting up angle for sparks
    {
        float time1, time2;
        
        time1 = *time;
        time2 = time1*time1;
        
        for (i=0;i<2;i++)
            angle[i] = 0.25*(p->vel[i]*time1 + (p->accel[i])*time2);
        angle[2] = 0.25*(p->vel[2]*time1 + (p->accel[2]-PARTICLE_GRAVITY)*time2);
    }
    
    p->thinknext = true;
}

void CL_Explosion_Sparks (vec3_t org, int size)
{
    int    i;
    
    for (i=0; i < (64/cl_particle_scale->value); i++) // was 256
    {
        setupParticle (
                       0,    0,    0,
                       org[0] + ((rand()%size)-16),    org[1] + ((rand()%size)-16),    org[2] + ((rand()%size)-16),
                       (rand()%150)-75,    (rand()%150)-75,    (rand()%150)-75,
                       0,        0,        0,
                       255,    100,    25,
                       0,    0,    0,
                       1,        -0.8 / (0.5 + frand()*0.3),
                       GL_SRC_ALPHA, GL_ONE,
                       6,        -9,        // was 4, -6
                       particle_blaster,
                       PART_GRAVITY|PART_SPARK,
                       pExplosionSparksThink, true);
    }
}


/*
 ===============
 
 Blood effects
 
 ===============
 */
void pBloodThink (cparticle_t *p, vec3_t org, vec3_t angle, float *alpha, float *size, int *image, float *time);
void CL_BloodPuff (vec3_t org, vec3_t dir, int count);

#define MAXBLEEDSIZE 5
#define TIMEBLOODGROW 2.5f
#define BLOOD_DECAL_CHANCE 0.5F

void pBloodDecalThink (cparticle_t *p, vec3_t org, vec3_t angle, float *alpha, float *size, int *image, float *time)
{    // This REALLY slows things down
    /*if (*time<TIMEBLOODGROW)
     {
     vec3_t dir;
     
     *size *= sqrt(0.5 + 0.5*(*time/TIMEBLOODGROW));
     
     AngleVectors (angle, dir, NULL, NULL);
     VectorNegate(dir, dir);
     clipDecal(p, *size, angle[2], org, dir);
     }*/
    
    //now calc alpha
    thinkDecalAlpha (p, org, angle, alpha, size, image, time);
}

void pBloodDropThink (cparticle_t *p, vec3_t org, vec3_t angle, float *alpha, float *size, int *image, float *time)
{
    float length;
    vec3_t len;
    
    VectorSubtract(p->angle, org, len);
    {
        calcPartVelocity(p, 0.2, time, angle);
        
        length = VectorNormalize(angle);
        if (length>MAXBLEEDSIZE) length = MAXBLEEDSIZE;
        VectorScale(angle, -length, angle);
    }
    
    //now to trace for impact...
    pBloodThink (p, org, angle, alpha, size, image, time);
}

void pBloodPuffThink (cparticle_t *p, vec3_t org, vec3_t angle, float *alpha, float *size, int *image, float *time)
{
    angle[2] =    angle[0] + *time*angle[1] + *time**time*angle[2];
    
    //now to trace for impact...
    pBloodThink (p, org, angle, alpha, size, image, time);
}

/*
 ===============
 pBloodThink
 ===============
 */
void pBloodThink (cparticle_t *p, vec3_t org, vec3_t angle, float *alpha, float *size, int *image, float *time)
{
    trace_t    trace = CL_Trace (p->oldorg, org, 0, CONTENTS_SOLID); // was 0.1
    qboolean became_decal = false;
    
    if (trace.fraction < 1.0) // delete and stain...
    {
#ifdef DECALS
        if (r_decals->value && (p->flags & PART_LEAVEMARK)
            && !VectorCompare(trace.plane.normal, vec3_origin)
            && !(CM_PointContents(p->oldorg,0) & MASK_WATER)) // no blood splatters underwater...
        {
            vec3_t    normal, dir;
            int        i;
            qboolean greenblood = false;
            qboolean timedout = false;
            if (p->color[1] > 0 && p->color[2] > 0)
                greenblood = true;
            // time cutoff for gib trails
            if (p->flags & PART_GRAVITY && !(p->flags & PART_DIRECTION))
            {    // gekk gibs go flyin faster...
                if ((greenblood) && (cl.time - p->time)*0.001 > 1.0F)
                    timedout = true;
                if ((!greenblood) && (cl.time - p->time)*0.001 > 0.5F)
                    timedout = true;
            }
            
            if (!timedout)
            {
                VectorNegate(trace.plane.normal, normal);
                vectoanglerolled(normal, rand()%360, p->angle);
                
                VectorCopy(trace.endpos, p->org);
                VectorClear(p->vel);
                VectorClear(p->accel);
                p->image = particleBlood();
                p->blendfunc_src = GL_SRC_ALPHA; //GL_ZERO
                p->blendfunc_dst = GL_ONE_MINUS_SRC_ALPHA; //GL_ONE_MINUS_SRC_COLOR
                p->flags = PART_DECAL|PART_SHADED|PART_ALPHACOLOR;
                p->alpha = *alpha;
                p->alphavel = -1/r_decal_life->value;
                if (greenblood)
                    p->color[1] = 210;
                else
                    for (i=0; i<3; i++)
                        p->color[i] *= 0.5;
                p->start = newParticleTime();
                p->think = pBloodDecalThink;
                p->thinknext = true;
                p->size = MAXBLEEDSIZE*0.5*(random()*5.0+5);
                //p->size = *size*(random()*5.0+5);
                p->sizevel = 0;
                
                p->decalnum = 0;
                p->decal = NULL;
                AngleVectors (p->angle, dir, NULL, NULL);
                VectorNegate(dir, dir);
                clipDecal(p, p->size, -p->angle[2], p->org, dir);
                if (p->decalnum)
                    became_decal = true;
                //else
                //    Com_Printf(S_COLOR_YELLOW"Blood decal not clipped!\n");
            }
        }
        if (!became_decal)
#endif
        {
            *alpha = 0;
            *size = 0;
            p->alpha = 0;
        }
    }
    VectorCopy(org, p->oldorg);
    
    p->thinknext = true;
}

/*
 ===============
 CL_BloodSmack
 ===============
 */
void CL_BloodSmack (vec3_t org, vec3_t dir)
{
    cparticle_t *p;
    
    p = setupParticle (
                       crand()*180, crand()*100, 0,
                       org[0],    org[1],    org[2],
                       dir[0],    dir[1],    dir[2],
                       0,        0,        0,
                       255,    0,        0,
                       0,        0,        0,
                       1.0,        -1 / (0.5 + frand()*0.3), //was -0.75
                       GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                       10,            0,
                       particle_redblood,
                       PART_SHADED|PART_OVERBRIGHT,
                       pRotateThink,true);
    
    CL_BloodPuff(org, dir, 1);
}


/*
 ===============
 CL_BloodBleed
 ===============
 */
void CL_BloodBleed (vec3_t org, vec3_t dir, int count)
{
    cparticle_t *p;
    vec3_t    pos;
    int        i;
    
    VectorScale(dir, 10, pos);
    for (i=0; i<count; i++)
    {
        VectorSet(pos,
                  dir[0]+random()*(cl_blood->value-2)*0.01,
                  dir[1]+random()*(cl_blood->value-2)*0.01,
                  dir[2]+random()*(cl_blood->value-2)*0.01);
        VectorScale(pos, 10 + (cl_blood->value-2)*0.0001*random(), pos);
        
        p = setupParticle (
                           org[0], org[1], org[2],
                           org[0] + ((rand()&7)-4) + dir[0],    org[1] + ((rand()&7)-4) + dir[1],    org[2] + ((rand()&7)-4) + dir[2],
                           pos[0]*(random()*3+5),    pos[1]*(random()*3+5),    pos[2]*(random()*3+5),
                           0,        0,        0,
                           255,    0,        0,
                           0,        0,        0,
                           0.7,        -0.25 / (0.5 + frand()*0.3),
                           GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                           MAXBLEEDSIZE*0.5,        0,
                           particle_blooddrip,
                           PART_SHADED|PART_DIRECTION|PART_GRAVITY|PART_OVERBRIGHT,
                           pBloodDropThink,true);
        
        if (i == 0 && random() < BLOOD_DECAL_CHANCE)
            p->flags |= PART_LEAVEMARK;
    }
    
}

/*
 ===============
 CL_BloodPuff
 ===============
 */
void CL_BloodPuff (vec3_t org, vec3_t dir, int count)
{
    cparticle_t *p;
    int        i;
    float    d;
    
    for (i=0; i<count; i++)
    {
        d = rand()&31;
        p = setupParticle (
                           crand()*180, crand()*100, 0,
                           org[0] + ((rand()&7)-4) + d*dir[0],    org[1] + ((rand()&7)-4) + d*dir[1],    org[2] + ((rand()&7)-4) + d*dir[2],
                           dir[0]*(crand()*3+5),    dir[1]*(crand()*3+5),    dir[2]*(crand()*3+5),
                           0,            0,            -100,
                           255,        0,            0,
                           0,            0,            0,
                           1.0,        -1.0,
                           GL_SRC_ALPHA, GL_ONE,
                           10,            0,
                           particle_blood,
                           PART_SHADED,
                           pBloodPuffThink,true);
        
        if (i == 0 && random() < BLOOD_DECAL_CHANCE)
            p->flags |= PART_LEAVEMARK;
    }
}

/*
 ===============
 CL_BloodHit
 ===============
 */
void CL_BloodHit (vec3_t org, vec3_t dir)
{
    if (cl_blood->value < 1) // disable blood option
        return;
    if (cl_blood->value == 2) // splat
        CL_BloodSmack(org, dir);
    else if (cl_blood->value == 3) // bleed
        CL_BloodBleed (org, dir, 6);
    else if (cl_blood->value == 4) // gore
        CL_BloodBleed (org, dir, 16);
    else // 1 = puff
        CL_BloodPuff(org, dir, 5);
}

/*
 ==================
 CL_GreenBloodHit
 
 green blood spray
 ==================
 */
void CL_GreenBloodHit (vec3_t org, vec3_t dir)
{
    cparticle_t *p;
    int        i;
    float    d;
    
    if (cl_blood->value < 1) // disable blood option
        return;
    
    for (i=0;i<5;i++)
    {
        d = rand()&31;
        p = setupParticle (
                           crand()*180, crand()*100, 0,
                           org[0] + ((rand()&7)-4) + d*dir[0],    org[1] + ((rand()&7)-4) + d*dir[1],    org[2] + ((rand()&7)-4) + d*dir[2],
                           dir[0]*(crand()*3+5),    dir[1]*(crand()*3+5),    dir[2]*(crand()*3+5),
                           0,        0,        -100,
                           255,    180,    50,
                           0,        0,        0,
                           1,        -1.0,
                           GL_SRC_ALPHA, GL_ONE,
                           10,            0,
                           particle_blood,
                           PART_SHADED|PART_OVERBRIGHT,
                           pBloodPuffThink,true);
        
        if (i == 0 && random() < BLOOD_DECAL_CHANCE)
            p->flags |= PART_LEAVEMARK;
    }
    
}

/*
 ===============
 CL_ParticleEffectSplash
 
 Water Splashing
 ===============
 */
#define SplashSize 7.5
void pSplashThink (cparticle_t *p, vec3_t org, vec3_t angle, float *alpha, float *size, int *image, float *time)
{
    int i;
    vec3_t len;
    VectorSubtract(p->angle, org, len);
    
    //    *size *= (float)(SplashSize/VectorLength(len)) * 0.5/((4-*size));
    //    if (*size > SplashSize)
    //        *size = SplashSize;
    
    //setting up angle for sparks
    {
        float time1, time2;
        
        time1 = *time;
        time2 = time1*time1;
        
        for (i=0;i<2;i++)
            angle[i] = 0.5*(p->vel[i]*time1 + (p->accel[i])*time2);
        angle[2] = 0.5*(p->vel[2]*time1 + (p->accel[2]-PARTICLE_GRAVITY)*time2);
    }
    
    p->thinknext = true;
}

void CL_ParticleEffectSplash (vec3_t org, vec3_t dir, int color8, int count)
{
    int            i;
    float        d;
    vec3_t color = {color8red(color8), color8green(color8), color8blue(color8)};
    
    for (i=0 ; i<count ; i++)
    {
        d = rand()&5;
        setupParticle (
                       org[0],    org[1],    org[2],
                       org[0]+d*dir[0],    org[1]+d*dir[1],    org[2]+d*dir[2],
                       dir[0]*40 + crand()*10,    dir[1]*40 + crand()*10,    dir[2]*40 + crand()*10,
                       0,        0,        0,
                       color[0],    color[1],    color[2],
                       0,    0,    0,
                       1,        -0.75 / (0.5 + frand()*0.3),
                       GL_SRC_ALPHA, GL_ONE,
                       5,            -7,
                       particle_smoke,
                       PART_GRAVITY|PART_DIRECTION   /*|PART_TRANS|PART_SHADED*/,
                       pSplashThink,true);
    }
}

/*
 ===============
 CL_ParticleEffectSparks
 ===============
 */
void pSparksThink (cparticle_t *p, vec3_t org, vec3_t angle, float *alpha, float *size, int *image, float *time)
{
    //vec3_t dir;
    int i;
    
    //setting up angle for sparks
    {
        float time1, time2;
        
        time1 = *time;
        time2 = time1*time1;
        
        for (i=0;i<2;i++)
            angle[i] = 0.25*(p->vel[i]*time1 + (p->accel[i])*time2);
        angle[2] = 0.25*(p->vel[2]*time1 + (p->accel[2]-PARTICLE_GRAVITY)*time2);
    }
    
    p->thinknext = true;
}

void CL_ParticleEffectSparks (vec3_t org, vec3_t dir, vec3_t color, int count)
{
    int            i;
    float        d;
    cparticle_t *p;
    
    for (i=0 ; i<count ; i++)
    {
        d = rand()&7;
        p = setupParticle (
                           0,    0,    0,
                           org[0]+((rand()&3)-2),    org[1]+((rand()&3)-2),    org[2]+((rand()&3)-2),
                           crand()*20 + dir[0]*40,            crand()*20 + dir[1]*40,            crand()*20 + dir[2]*40,
                           0,        0,        0,
                           color[0],        color[1],        color[2],
                           0,    0,    0,
                           0.75,        -1.0 / (0.5 + frand()*0.3),
                           GL_SRC_ALPHA, GL_ONE,
                           4,            0, //Knightmare- increase size
                           particle_generic,
                           PART_GRAVITY|PART_SPARK,
                           pSparksThink,true);
    }
    if (p) // added light effect
        addParticleLight (p, (count>8)?130:65, 0, color[0]/255, color[1]/255, color[2]/255);
}

#ifdef DECALS
/*
 ===============
 thinkDecalAlpha
 ===============
 */
void thinkDecalAlpha (cparticle_t *p, vec3_t org, vec3_t angle, float *alpha, float *size, int *image, float *time)
{
    *alpha = pow(*alpha, 0.1);
    p->thinknext = true;
}

void vectoanglerolled (vec3_t value1, float angleyaw, vec3_t angles)
{
    float    forward, yaw, pitch;
    
    yaw = (int) (atan2(value1[1], value1[0]) * 180 / M_PI);
    forward = sqrt (value1[0]*value1[0] + value1[1]*value1[1]);
    pitch = (int) (atan2(value1[2], forward) * 180 / M_PI);
    
    if (pitch < 0)
        pitch += 360;
    
    angles[PITCH] = -pitch;
    angles[YAW] =  yaw;
    angles[ROLL] = - angleyaw;
}

/*
 ===============
 CL_ParticleBulletDecal
 ===============
 */
#define DECAL_OFFSET 0.5f
void CL_ParticleBulletDecal (vec3_t org, vec3_t dir, float size)
{
    cparticle_t    *p;
    vec3_t        ang, angle, end, origin;
    trace_t        tr;
    
    if (!r_decals->value)
        return;
    
    VectorMA(org, DECAL_OFFSET, dir, origin);
    VectorMA(org, -DECAL_OFFSET, dir, end);
    tr = CL_Trace (origin, end, 0, CONTENTS_SOLID);
    //tr = CL_Trace (origin, end, 1, 1);
    
    if (tr.fraction == 1)
        //if (!tr.allsolid)
        return;
    
    VectorNegate(tr.plane.normal, angle);
    //VectorNegate(dir, angle);
    vectoanglerolled(angle, rand()%360, ang);
    VectorCopy(tr.endpos, origin);
    
    p = setupParticle (
                       ang[0],    ang[1],    ang[2],
                       origin[0],    origin[1],    origin[2],
                       0,        0,        0,
                       0,        0,        0,
                       255,    255,    255,
                       0,        0,        0,
                       1,        -1/r_decal_life->value,
                       GL_ZERO, GL_ONE_MINUS_SRC_ALPHA,
                       size,            0,
                       particle_bulletmark,
                       PART_SHADED|PART_DECAL|PART_ALPHACOLOR, // was part_saturate
                       thinkDecalAlpha, true);
}


/*
 ===============
 CL_ParticleRailDecal
 ===============
 */
#define RAIL_DECAL_OFFSET 2.0f
void CL_ParticleRailDecal (vec3_t org, vec3_t dir, float size, qboolean isRed)
{
    vec3_t        ang, angle, end, origin;
    trace_t        tr;
    
    if (!r_decals->value)
        return;
    
    VectorMA(org, -RAIL_DECAL_OFFSET, dir, origin);
    VectorMA(org, 2*RAIL_DECAL_OFFSET, dir, end);
    tr = CL_Trace (origin, end, 0, CONTENTS_SOLID);
    
    if (tr.fraction==1)
        return;
    if (VectorCompare(tr.plane.normal, vec3_origin))
        return;
    
    VectorNegate(tr.plane.normal, angle);
    vectoanglerolled(angle, rand()%360, ang);
    VectorCopy(tr.endpos, origin);
    
    setupParticle (
                   ang[0],    ang[1],    ang[2],
                   origin[0],    origin[1],    origin[2],
                   0,        0,        0,
                   0,        0,        0,
                   255,    255,    255,
                   0,        0,        0,
                   1,        -1/r_decal_life->value,
                   GL_ZERO, GL_ONE_MINUS_SRC_ALPHA,
                   size,            0,
                   particle_bulletmark,
                   PART_SHADED|PART_DECAL|PART_ALPHACOLOR,
                   thinkDecalAlpha, true);
    
    setupParticle (
                   ang[0],    ang[1],    ang[2],
                   origin[0],    origin[1],    origin[2],
                   0,        0,        0,
                   0,        0,        0,
                   (isRed)?255:cl_railred->value,    (isRed)?20:cl_railgreen->value,    (isRed)?20:cl_railblue->value,
                   0,        0,        0,
                   1,        -0.25,
                   GL_SRC_ALPHA, GL_ONE,
                   size,            0,
                   particle_generic,
                   PART_DECAL,
                   NULL, false);
    
    setupParticle (
                   ang[0],    ang[1],    ang[2],
                   origin[0],    origin[1],    origin[2],
                   0,        0,        0,
                   0,        0,        0,
                   255,    255,    255,
                   0,        0,        0,
                   1,        -0.25,
                   GL_SRC_ALPHA, GL_ONE,
                   size*0.67,        0,
                   particle_generic,
                   PART_DECAL,
                   NULL, false);
}


/*
 ===============
 CL_ParticleBlasterDecal
 ===============
 */
void CL_ParticleBlasterDecal (vec3_t org, vec3_t dir, float size, int red, int green, int blue)
{
    cparticle_t    *p;
    vec3_t        ang, angle, end, origin;
    trace_t        tr;
    
    if (!r_decals->value)
        return;
    
    VectorMA(org, DECAL_OFFSET, dir, origin);
    VectorMA(org, -DECAL_OFFSET, dir, end);
    tr = CL_Trace (origin, end, 0, CONTENTS_SOLID);
    
    if (tr.fraction==1)
        return;
    if (VectorCompare(tr.plane.normal, vec3_origin))
        return;
    
    VectorNegate(tr.plane.normal, angle);
    vectoanglerolled(angle, rand()%360, ang);
    VectorCopy(tr.endpos, origin);
    
    p = setupParticle (
                       ang[0],    ang[1],    ang[2],
                       origin[0],    origin[1],    origin[2],
                       0,        0,        0,
                       0,        0,        0,
                       255,    255,    255,
                       0,        0,        0,
                       0.7,    -1/r_decal_life->value,
                       GL_ZERO, GL_ONE_MINUS_SRC_ALPHA,
                       size,        0,
                       particle_shadow,
                       PART_SHADED|PART_DECAL,
                       NULL, false);
    
    p = setupParticle (
                       ang[0],    ang[1],    ang[2],
                       origin[0],    origin[1],    origin[2],
                       0,        0,        0,
                       0,        0,        0,
                       red,    green,    blue,
                       0,        0,        0,
                       1,        -0.3,
                       GL_SRC_ALPHA, GL_ONE,
                       size*0.4,    0,
                       particle_generic,
                       PART_SHADED|PART_DECAL,
                       NULL, false);
    
    p = setupParticle (
                       ang[0],    ang[1],    ang[2],
                       origin[0],    origin[1],    origin[2],
                       0,        0,        0,
                       0,        0,        0,
                       red,    green,    blue,
                       0,        0,        0,
                       1,        -0.6,
                       GL_SRC_ALPHA, GL_ONE,
                       size*0.3,    0,
                       particle_generic,
                       PART_SHADED|PART_DECAL,
                       NULL, false);
}


/*
 ===============
 CL_ParticlePlasmaBeamDecal
 ===============
 */
void CL_ParticlePlasmaBeamDecal (vec3_t org, vec3_t dir, float size)
{
    cparticle_t    *p;
    vec3_t        ang, angle, end, origin;
    trace_t        tr;
    
    if (!r_decals->value)
        return;
    
    VectorMA(org, DECAL_OFFSET, dir, origin);
    VectorMA(org, -DECAL_OFFSET, dir, end);
    tr = CL_Trace (origin, end, 0, CONTENTS_SOLID);
    
    if (tr.fraction==1)
        return;
    if (VectorCompare(tr.plane.normal, vec3_origin))
        return;
    
    VectorNegate(tr.plane.normal, angle);
    vectoanglerolled(angle, rand()%360, ang);
    VectorCopy(tr.endpos, origin);
    
    p = setupParticle (
                       ang[0],    ang[1],    ang[2],
                       origin[0],    origin[1],    origin[2],
                       0,        0,        0,
                       0,        0,        0,
                       255,    255,    255,
                       0,        0,        0,
                       0.85,    -1/r_decal_life->value,
                       GL_ZERO, GL_ONE_MINUS_SRC_ALPHA,
                       size,        0,
                       particle_shadow,
                       PART_SHADED|PART_DECAL,
                       NULL, false);
}
#endif // DECALS

/*
 ===============
 CL_HyperBlasterTrail
 
 Hyperblaster particle glow effect
 ===============
 */
void CL_HyperBlasterTrail (vec3_t start, vec3_t end, int red, int green, int blue,
                           int reddelta, int greendelta, int bluedelta)
{
    vec3_t        move;
    vec3_t        vec;
    float        len;
    int            dec;
    int            i;
    
    VectorCopy (start, move);
    VectorSubtract (end, start, vec);
    VectorMA (move, 0.5, vec, move);
    len = VectorNormalize (vec);
    
    dec = 1 * cl_particle_scale->value;
    VectorScale (vec, dec, vec);
    
    for (i = 0; i < (18/cl_particle_scale->value); i++)
    {
        len -= dec;
        
        setupParticle (
                       0,        0,        0,
                       move[0] + crand(),    move[1] + crand(),    move[2] + crand(),
                       crand()*5,    crand()*5,    crand()*5,
                       0,        0,        0,
                       red,        green,        blue,
                       reddelta,    greendelta,    bluedelta,
                       1,        -16.0 / (0.5 + frand()*0.3),
                       GL_SRC_ALPHA, GL_ONE,
                       3,        -36,
                       particle_generic,
                       0,
                       NULL,0);
        
        VectorAdd (move, vec, move);
    }
}

/*
 ===============
 CL_BlasterTracer
 ===============
 */
void CL_BlasterTracer (vec3_t origin, vec3_t angle, int red, int green, int blue, float len, float size)
{
    int i;
    vec3_t        dir;
    
    AngleVectors (angle, dir, NULL, NULL);
    VectorScale(dir, len,dir);
    
    for (i=0;i<3;i++)
        setupParticle (
                       dir[0],    dir[1],    dir[2],
                       origin[0],    origin[1],    origin[2],
                       0,    0,    0,
                       0,        0,        0,
                       red, green, blue,
                       0,    0,    0,
                       1,        INSTANT_PARTICLE,
                       GL_SRC_ALPHA, GL_ONE,
                       size,        0,
                       particle_generic,
                       PART_DIRECTION|PART_INSTANT,
                       NULL,0);
}

