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
 * This file implements all client side lighting
 *
 * =======================================================================
 */

#include "header/client.h"

void CL_LogoutEffect (vec3_t org, int type);
void CL_GunSmokeEffect (vec3_t org, vec3_t dir);

typedef struct
{
	int length;
	float value[3];
	float map[MAX_QPATH];
} clightstyle_t;

clightstyle_t cl_lightstyle[MAX_LIGHTSTYLES];
int lastofs;

void
CL_ClearLightStyles(void)
{
	memset(cl_lightstyle, 0, sizeof(cl_lightstyle));
	lastofs = -1;
}

void
CL_RunLightStyles(void)
{
	int ofs;
	int i;
	clightstyle_t *ls;

	ofs = cl.time / 100;

	if (ofs == lastofs)
	{
		return;
	}

	lastofs = ofs;

	for (i = 0, ls = cl_lightstyle; i < MAX_LIGHTSTYLES; i++, ls++)
	{
		if (!ls->length)
		{
			ls->value[0] = ls->value[1] = ls->value[2] = 1.0;
			continue;
		}

		if (ls->length == 1)
		{
			ls->value[0] = ls->value[1] = ls->value[2] = ls->map[0];
		}

		else
		{
			ls->value[0] = ls->value[1] = ls->value[2] = ls->map[ofs % ls->length];
		}
	}
}

void
CL_SetLightstyle(int i)
{
	char *s;
	int j, k;

	// Knightmare- BIG UGLY HACK for old connected to server using old protocol
	// Changed config strings require different parsing
	if ( LegacyProtocol() )
		s = cl.configstrings[i+OLD_CS_LIGHTS];
	else
		s = cl.configstrings[i + CS_LIGHTS];

	j = (int)strlen(s);
	if (j >= MAX_QPATH)
		Com_Error (ERR_DROP, "svc_lightstyle length=%i", j);

	cl_lightstyle[i].length = j;

	for (k = 0; k < j; k++)
	{
		cl_lightstyle[i].map[k] = (float)(s[k] - 'a') / (float)('m' - 'a');
	}
}

void
CL_AddLightStyles(void)
{
	int i;
	clightstyle_t *ls;

	for (i = 0, ls = cl_lightstyle; i < MAX_LIGHTSTYLES; i++, ls++)
	{
		V_AddLightStyle(i, ls->value[0], ls->value[1], ls->value[2]);
	}
}

cdlight_t cl_dlights[MAX_DLIGHTS];

void
CL_ClearDlights(void)
{
	memset(cl_dlights, 0, sizeof(cl_dlights));
}

cdlight_t *
CL_AllocDlight(int key)
{
	int i;
	cdlight_t *dl;

	/* first look for an exact key match */
	if (key)
	{
		dl = cl_dlights;

		for (i = 0; i < MAX_DLIGHTS; i++, dl++)
		{
			if (dl->key == key)
			{
				memset (dl, 0, sizeof(*dl));
				dl->key = key;
				return dl;
			}
		}
	}

	/* then look for anything else */
	dl = cl_dlights;

	for (i = 0; i < MAX_DLIGHTS; i++, dl++)
	{
		if (dl->die < cl.time)
		{
			memset (dl, 0, sizeof(*dl));
			dl->key = key;
			return dl;
		}
	}

	dl = &cl_dlights[0];
	memset (dl, 0, sizeof(*dl));
	dl->key = key;
	return dl;
}

void
CL_NewDlight(int key, float x, float y, float z, float radius, float time)
{
	cdlight_t *dl;

	dl = CL_AllocDlight(key);
	dl->origin[0] = x;
	dl->origin[1] = y;
	dl->origin[2] = z;
	dl->radius = radius;
	dl->die = cl.time + time;
}

void
CL_RunDLights(void)
{
	int i;
	cdlight_t *dl;

	dl = cl_dlights;

	for (i = 0; i < MAX_DLIGHTS; i++, dl++)
	{
		if (!dl->radius)
		{
			continue;
		}

		if (dl->die < cl.time)
		{
			dl->radius = 0;
			return;
		}

		dl->radius -= cls.rframetime * dl->decay;

		if (dl->radius < 0)
		{
			dl->radius = 0;
		}
	}
}

void
CL_AddDLights(void)
{
	int i;
	cdlight_t *dl;

	dl = cl_dlights;

	for (i = 0; i < MAX_DLIGHTS; i++, dl++)
	{
		if (!dl->radius)
		{
			continue;
		}

		V_AddLight(dl->origin, dl->radius, dl->color[0], dl->color[1], dl->color[2]);
	}
}

/*
 ==============
 CL_ParseMuzzleFlash2
 ==============
 */
void CL_ParseMuzzleFlash2 (void)
{
    int            ent;
    vec3_t        origin;
    int            flash_number;
    cdlight_t    *dl;
    vec3_t        forward, right;
    char        soundname[64];
    
    ent = MSG_ReadShort (&net_message);
    if (ent < 1 || ent >= MAX_EDICTS)
        Com_Error (ERR_DROP, "CL_ParseMuzzleFlash2: bad entity");
    
    flash_number = MSG_ReadByte (&net_message);
    
    // locate the origin
    AngleVectors (cl_entities[ent].current.angles, forward, right, NULL);
    origin[0] = cl_entities[ent].current.origin[0] + forward[0] * monster_flash_offset[flash_number][0] + right[0] * monster_flash_offset[flash_number][1];
    origin[1] = cl_entities[ent].current.origin[1] + forward[1] * monster_flash_offset[flash_number][0] + right[1] * monster_flash_offset[flash_number][1];
    origin[2] = cl_entities[ent].current.origin[2] + forward[2] * monster_flash_offset[flash_number][0] + right[2] * monster_flash_offset[flash_number][1] + monster_flash_offset[flash_number][2];
    
    dl = CL_AllocDlight (ent);
    VectorCopy (origin,  dl->origin);
    dl->radius = 200 + (rand()&31);
    dl->minlight = 32;
    dl->die = cl.time;    // + 0.1;
    
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
            dl->color[0] = 1;dl->color[1] = 1;dl->color[2] = 0;
            CL_GunSmokeEffect (origin, vec3_origin);
            S_StartSound (NULL, ent, CHAN_WEAPON, S_RegisterSound("infantry/infatck1.wav"), 1, ATTN_NORM, 0);
            break;
            
        case MZ2_SOLDIER_MACHINEGUN_1:
        case MZ2_SOLDIER_MACHINEGUN_2:
        case MZ2_SOLDIER_MACHINEGUN_3:
        case MZ2_SOLDIER_MACHINEGUN_4:
        case MZ2_SOLDIER_MACHINEGUN_5:
        case MZ2_SOLDIER_MACHINEGUN_6:
        case MZ2_SOLDIER_MACHINEGUN_7:
        case MZ2_SOLDIER_MACHINEGUN_8:
            dl->color[0] = 1;dl->color[1] = 1;dl->color[2] = 0;
            CL_GunSmokeEffect (origin, vec3_origin);
            S_StartSound (NULL, ent, CHAN_WEAPON, S_RegisterSound("soldier/solatck3.wav"), 1, ATTN_NORM, 0);
            break;
            
        case MZ2_GUNNER_MACHINEGUN_1:
        case MZ2_GUNNER_MACHINEGUN_2:
        case MZ2_GUNNER_MACHINEGUN_3:
        case MZ2_GUNNER_MACHINEGUN_4:
        case MZ2_GUNNER_MACHINEGUN_5:
        case MZ2_GUNNER_MACHINEGUN_6:
        case MZ2_GUNNER_MACHINEGUN_7:
        case MZ2_GUNNER_MACHINEGUN_8:
            dl->color[0] = 1;dl->color[1] = 1;dl->color[2] = 0;
            CL_GunSmokeEffect (origin, vec3_origin);
            S_StartSound (NULL, ent, CHAN_WEAPON, S_RegisterSound("gunner/gunatck2.wav"), 1, ATTN_NORM, 0);
            break;
            
        case MZ2_ACTOR_MACHINEGUN_1:
        case MZ2_SUPERTANK_MACHINEGUN_1:
        case MZ2_SUPERTANK_MACHINEGUN_2:
        case MZ2_SUPERTANK_MACHINEGUN_3:
        case MZ2_SUPERTANK_MACHINEGUN_4:
        case MZ2_SUPERTANK_MACHINEGUN_5:
        case MZ2_SUPERTANK_MACHINEGUN_6:
        case MZ2_TURRET_MACHINEGUN:            // PGM
            dl->color[0] = 1;dl->color[1] = 1;dl->color[2] = 0;
            CL_GunSmokeEffect (origin, vec3_origin);
            S_StartSound (NULL, ent, CHAN_WEAPON, S_RegisterSound("infantry/infatck1.wav"), 1, ATTN_NORM, 0);
            break;
            
        case MZ2_BOSS2_MACHINEGUN_L1:
        case MZ2_BOSS2_MACHINEGUN_L2:
        case MZ2_BOSS2_MACHINEGUN_L3:
        case MZ2_BOSS2_MACHINEGUN_L4:
        case MZ2_BOSS2_MACHINEGUN_L5:
        case MZ2_CARRIER_MACHINEGUN_L1:        // PMM
        case MZ2_CARRIER_MACHINEGUN_L2:        // PMM
            dl->color[0] = 1;dl->color[1] = 1;dl->color[2] = 0;
            CL_GunSmokeEffect (origin, vec3_origin);
            S_StartSound (NULL, ent, CHAN_WEAPON, S_RegisterSound("infantry/infatck1.wav"), 1, ATTN_NONE, 0);
            break;
            
        case MZ2_SOLDIER_BLASTER_1:
        case MZ2_SOLDIER_BLASTER_2:
        case MZ2_SOLDIER_BLASTER_3:
        case MZ2_SOLDIER_BLASTER_4:
        case MZ2_SOLDIER_BLASTER_5:
        case MZ2_SOLDIER_BLASTER_6:
        case MZ2_SOLDIER_BLASTER_7:
        case MZ2_SOLDIER_BLASTER_8:
        case MZ2_TURRET_BLASTER:            // PGM
            dl->color[0] = 1;dl->color[1] = 1;dl->color[2] = 0;
            S_StartSound (NULL, ent, CHAN_WEAPON, S_RegisterSound("soldier/solatck2.wav"), 1, ATTN_NORM, 0);
            break;
            
        case MZ2_FLYER_BLASTER_1:
        case MZ2_FLYER_BLASTER_2:
            dl->color[0] = 1;dl->color[1] = 1;dl->color[2] = 0;
            S_StartSound (NULL, ent, CHAN_WEAPON, S_RegisterSound("flyer/flyatck3.wav"), 1, ATTN_NORM, 0);
            break;
            
        case MZ2_MEDIC_BLASTER_1:
            dl->color[0] = 1;dl->color[1] = 1;dl->color[2] = 0;
            S_StartSound (NULL, ent, CHAN_WEAPON, S_RegisterSound("medic/medatck1.wav"), 1, ATTN_NORM, 0);
            break;
            
        case MZ2_HOVER_BLASTER_1:
            dl->color[0] = 1;dl->color[1] = 1;dl->color[2] = 0;
            S_StartSound (NULL, ent, CHAN_WEAPON, S_RegisterSound("hover/hovatck1.wav"), 1, ATTN_NORM, 0);
            break;
            
        case MZ2_FLOAT_BLASTER_1:
            dl->color[0] = 1;dl->color[1] = 1;dl->color[2] = 0;
            S_StartSound (NULL, ent, CHAN_WEAPON, S_RegisterSound("floater/fltatck1.wav"), 1, ATTN_NORM, 0);
            break;
            
        case MZ2_SOLDIER_SHOTGUN_1:
        case MZ2_SOLDIER_SHOTGUN_2:
        case MZ2_SOLDIER_SHOTGUN_3:
        case MZ2_SOLDIER_SHOTGUN_4:
        case MZ2_SOLDIER_SHOTGUN_5:
        case MZ2_SOLDIER_SHOTGUN_6:
        case MZ2_SOLDIER_SHOTGUN_7:
        case MZ2_SOLDIER_SHOTGUN_8:
            dl->color[0] = 1;dl->color[1] = 1;dl->color[2] = 0;
            CL_GunSmokeEffect (origin, vec3_origin);
            S_StartSound (NULL, ent, CHAN_WEAPON, S_RegisterSound("soldier/solatck1.wav"), 1, ATTN_NORM, 0);
            break;
            
        case MZ2_TANK_BLASTER_1:
        case MZ2_TANK_BLASTER_2:
        case MZ2_TANK_BLASTER_3:
            dl->color[0] = 1;dl->color[1] = 1;dl->color[2] = 0;
            S_StartSound (NULL, ent, CHAN_WEAPON, S_RegisterSound("tank/tnkatck3.wav"), 1, ATTN_NORM, 0);
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
            dl->color[0] = 1;dl->color[1] = 1;dl->color[2] = 0;
            CL_GunSmokeEffect (origin, vec3_origin);
            Com_sprintf(soundname, sizeof(soundname), "tank/tnkatk2%c.wav", 'a' + rand() % 5);
            S_StartSound (NULL, ent, CHAN_WEAPON, S_RegisterSound(soundname), 1, ATTN_NORM, 0);
            break;
            
        case MZ2_CHICK_ROCKET_1:
        case MZ2_TURRET_ROCKET:            // PGM
            dl->color[0] = 1;dl->color[1] = 0.5;dl->color[2] = 0.2;
            S_StartSound (NULL, ent, CHAN_WEAPON, S_RegisterSound("chick/chkatck2.wav"), 1, ATTN_NORM, 0);
            break;
            
        case MZ2_TANK_ROCKET_1:
        case MZ2_TANK_ROCKET_2:
        case MZ2_TANK_ROCKET_3:
            dl->color[0] = 1;dl->color[1] = 0.5;dl->color[2] = 0.2;
            S_StartSound (NULL, ent, CHAN_WEAPON, S_RegisterSound("tank/tnkatck1.wav"), 1, ATTN_NORM, 0);
            break;
            
        case MZ2_SUPERTANK_ROCKET_1:
        case MZ2_SUPERTANK_ROCKET_2:
        case MZ2_SUPERTANK_ROCKET_3:
        case MZ2_BOSS2_ROCKET_1:
        case MZ2_BOSS2_ROCKET_2:
        case MZ2_BOSS2_ROCKET_3:
        case MZ2_BOSS2_ROCKET_4:
        case MZ2_CARRIER_ROCKET_1:
            //    case MZ2_CARRIER_ROCKET_2:
            //    case MZ2_CARRIER_ROCKET_3:
            //    case MZ2_CARRIER_ROCKET_4:
            dl->color[0] = 1;dl->color[1] = 0.5;dl->color[2] = 0.2;
            S_StartSound (NULL, ent, CHAN_WEAPON, S_RegisterSound("tank/rocket.wav"), 1, ATTN_NORM, 0);
            break;
            
        case MZ2_GUNNER_GRENADE_1:
        case MZ2_GUNNER_GRENADE_2:
        case MZ2_GUNNER_GRENADE_3:
        case MZ2_GUNNER_GRENADE_4:
            dl->color[0] = 1;dl->color[1] = 0.5;dl->color[2] = 0;
            S_StartSound (NULL, ent, CHAN_WEAPON, S_RegisterSound("gunner/gunatck3.wav"), 1, ATTN_NORM, 0);
            break;
            
        case MZ2_GLADIATOR_RAILGUN_1:
            // PMM
        case MZ2_CARRIER_RAILGUN:
        case MZ2_WIDOW_RAIL:
            // pmm
            dl->color[0] = 0.5;dl->color[1] = 0.5;dl->color[2] = 1.0;
            break;
            
            // --- Xian's shit starts ---
        case MZ2_MAKRON_BFG:
            dl->color[0] = 0.5;dl->color[1] = 1 ;dl->color[2] = 0.5;
            //S_StartSound (NULL, ent, CHAN_WEAPON, S_RegisterSound("makron/bfg_fire.wav"), 1, ATTN_NORM, 0);
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
            dl->color[0] = 1;dl->color[1] = 1;dl->color[2] = 0;
            S_StartSound (NULL, ent, CHAN_WEAPON, S_RegisterSound("makron/blaster.wav"), 1, ATTN_NORM, 0);
            break;
            
        case MZ2_JORG_MACHINEGUN_L1:
        case MZ2_JORG_MACHINEGUN_L2:
        case MZ2_JORG_MACHINEGUN_L3:
        case MZ2_JORG_MACHINEGUN_L4:
        case MZ2_JORG_MACHINEGUN_L5:
        case MZ2_JORG_MACHINEGUN_L6:
            dl->color[0] = 1;dl->color[1] = 1;dl->color[2] = 0;
            CL_GunSmokeEffect (origin, vec3_origin);
            S_StartSound (NULL, ent, CHAN_WEAPON, S_RegisterSound("boss3/xfire.wav"), 1, ATTN_NORM, 0);
            break;
            
        case MZ2_JORG_MACHINEGUN_R1:
        case MZ2_JORG_MACHINEGUN_R2:
        case MZ2_JORG_MACHINEGUN_R3:
        case MZ2_JORG_MACHINEGUN_R4:
        case MZ2_JORG_MACHINEGUN_R5:
        case MZ2_JORG_MACHINEGUN_R6:
            dl->color[0] = 1;dl->color[1] = 1;dl->color[2] = 0;
            CL_GunSmokeEffect (origin, vec3_origin);
            break;
            
        case MZ2_JORG_BFG_1:
            dl->color[0] = 0.5;dl->color[1] = 1 ;dl->color[2] = 0.5;
            break;
            
        case MZ2_BOSS2_MACHINEGUN_R1:
        case MZ2_BOSS2_MACHINEGUN_R2:
        case MZ2_BOSS2_MACHINEGUN_R3:
        case MZ2_BOSS2_MACHINEGUN_R4:
        case MZ2_BOSS2_MACHINEGUN_R5:
        case MZ2_CARRIER_MACHINEGUN_R1:            // PMM
        case MZ2_CARRIER_MACHINEGUN_R2:            // PMM
            
            dl->color[0] = 1;dl->color[1] = 1;dl->color[2] = 0;
            CL_GunSmokeEffect (origin, vec3_origin);
            break;
            
            // ======
            // ROGUE
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
            dl->color[0] = 0;dl->color[1] = 1;dl->color[2] = 0;
            S_StartSound (NULL, ent, CHAN_WEAPON, S_RegisterSound("tank/tnkatck3.wav"), 1, ATTN_NORM, 0);
            break;
            
        case MZ2_WIDOW_DISRUPTOR:
            dl->color[0] = -1;dl->color[1] = -1;dl->color[2] = -1;
            S_StartSound (NULL, ent, CHAN_WEAPON, S_RegisterSound("weapons/disint2.wav"), 1, ATTN_NORM, 0);
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
            dl->radius = 300 + (rand()&100);
            dl->color[0] = 1;dl->color[1] = 1;dl->color[2] = 0;
            dl->die = cl.time + 200;
            break;
            // ROGUE
            // ======
            
            // --- Xian's shit ends ---
            
    }
}
