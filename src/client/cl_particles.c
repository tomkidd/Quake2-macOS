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
 * This file implements all generic particle stuff
 *
 * =======================================================================
 */

#include "header/client.h"

cparticle_t *active_particles, *free_particles;
cparticle_t particles[MAX_PARTICLES];
int cl_numparticles = MAX_PARTICLES;

/*
 ==============================================================
 
 PARTICLE MANAGEMENT
 
 ==============================================================
 */

float newParticleTime ()
{
    float lerpedTime;
    
    lerpedTime = cl.time;
    
    return lerpedTime;
}


#ifdef DECALS
//=================================================
decalpolys_t    *active_decals, *free_decals;
decalpolys_t    decalfrags[MAX_DECAL_FRAGS];
int                cl_numdecalfrags = MAX_DECAL_FRAGS;

/*
 ===============
 CleanDecalPolys
 Cleans up the active_particles linked list
 ===============
 */
void CleanDecalPolys (void)
{
    decalpolys_t        *d, *next;
    decalpolys_t        *active = NULL, *tail = NULL;
    
    for (d=active_decals; d; d=next)
    {
        next = d->next;
        if (d->clearflag) {
            d->clearflag = false;
            d->numpolys = 0;
            d->nextpoly = NULL;
            d->node = NULL; // vis node
            d->next = free_decals;
            free_decals = d;
            continue;
        }
        d->next = NULL;
        if (!tail)
            active = tail = d;
        else {
            tail->next = d;
            tail = d;
        }
    }
    active_decals = active;
}

/*
 ===============
 ClearDecalPoly
 Recursively flags a decal poly chain for cleaning
 ===============
 */
void ClearDecalPoly (decalpolys_t *decal)
{
    if (!decal)
        return;
    if (decal->nextpoly)
        ClearDecalPoly(decal->nextpoly);
    decal->clearflag = true; // tell cleaning loop to clean this up
}

/*
 ===============
 CL_ClearAllDecalPolys
 Clears all decal polys
 ===============
 */
void CL_ClearAllDecalPolys (void)
{
    int        i;
    free_decals = &decalfrags[0];
    active_decals = NULL;
    
    for (i=0 ;i < cl_numdecalfrags ; i++) {
        decalfrags[i].next = &decalfrags[i+1];
        decalfrags[i].clearflag = false;
        decalfrags[i].numpolys = 0;
        decalfrags[i].nextpoly = NULL;
        decalfrags[i].node = NULL; // vis node
    }
    decalfrags[cl_numdecalfrags-1].next = NULL;
}
/*
 ===============
 NumFreeDecalPolys
 Retuns number of available decalpoly_t fields
 ===============
 */
int NumFreeDecalPolys (void)
{
    int count = 0;
    decalpolys_t *d = NULL;
    for (d = free_decals; d; d = d->next)
        count++;
    return count;
}
/*
 ===============
 NewDecalPoly
 Retuns first free decal poly
 ===============
 */
decalpolys_t *NewDecalPoly(void)
{
    decalpolys_t *d = NULL;
    if (!free_decals)
        return NULL;
    
    d = free_decals;
    free_decals = d->next;
    d->next = active_decals;
    active_decals = d;
    return d;
}

/*
 ===============
 clipDecal
 ===============
 */
void clipDecal (cparticle_t *part, float radius, float orient, vec3_t origin, vec3_t dir)
{
    vec3_t    axis[3], verts[MAX_DECAL_VERTS];
    int        numfragments, j, i;
    markFragment_t *fr, fragments[MAX_FRAGMENTS_PER_DECAL];
    
    // invalid decal
    if ( radius <= 0 || VectorCompare (dir, vec3_origin) )
        return;
    
    // calculate orientation matrix
    VectorNormalize2 ( dir, axis[0] );
    PerpendicularVector ( axis[1], axis[0] );
    RotatePointAroundVector ( axis[2], axis[0], axis[1], orient );
    CrossProduct ( axis[0], axis[2], axis[1] );
    
    numfragments = R_MarkFragments (origin, axis, radius, MAX_DECAL_VERTS, verts,
                                    MAX_FRAGMENTS_PER_DECAL, fragments);
    
    if (!numfragments)
        return;
    if (numfragments > NumFreeDecalPolys()) // not enough decalpolys free
        return;
    
    VectorScale ( axis[1], 0.5f / radius, axis[1] );
    VectorScale ( axis[2], 0.5f / radius, axis[2] );
    
    part->decalnum = numfragments;
    for ( i = 0, fr = fragments; i < numfragments; i++, fr++ )
    {
        decalpolys_t *decal = NewDecalPoly();
        vec3_t v;
        
        if (!decal)
            return;
        decal->nextpoly = part->decal;
        part->decal = decal;
        //Com_Printf("Number of verts in fragment: %i\n", fr->numPoints);
        decal->node = fr->node; // vis node
        
        for ( j = 0; j < fr->numPoints && j < MAX_VERTS_PER_FRAGMENT; j++ )
        {
            VectorCopy ( verts[fr->firstPoint+j], decal->polys[j] );
            VectorSubtract ( decal->polys[j], origin, v );
            decal->coords[j][0] = DotProduct ( v, axis[1] ) + 0.5f;
            decal->coords[j][1] = DotProduct ( v, axis[2] ) + 0.5f;
            decal->numpolys = fr->numPoints;
        }
    }
}
#endif

/*
 ===============
 setupParticle
 ===============
 */
cparticle_t *setupParticle (
                            float angle0,        float angle1,        float angle2,
                            float org0,            float org1,            float org2,
                            float vel0,            float vel1,            float vel2,
                            float accel0,        float accel1,        float accel2,
                            float color0,        float color1,        float color2,
                            float colorvel0,    float colorvel1,    float colorvel2,
                            float alpha,        float alphavel,
                            int    blendfunc_src,    int blendfunc_dst,
                            float size,            float sizevel,
                            int    image,
                            int flags,
                            void (*think)(cparticle_t *p, vec3_t org, vec3_t angle, float *alpha, float *size, int *image, float *time),
                            qboolean thinknext)
{
    int j;
    cparticle_t    *p = NULL;
    
    if (!free_particles)
        return NULL;
    p = free_particles;
    free_particles = p->next;
    p->next = active_particles;
    active_particles = p;
    
    p->time = cl.time;
    
    p->angle[0]=angle0;
    p->angle[1]=angle1;
    p->angle[2]=angle2;
    
    p->org[0]=org0;
    p->org[1]=org1;
    p->org[2]=org2;
    p->oldorg[0]=org0;
    p->oldorg[1]=org1;
    p->oldorg[2]=org2;
    
    p->vel[0]=vel0;
    p->vel[1]=vel1;
    p->vel[2]=vel2;
    
    p->accel[0]=accel0;
    p->accel[1]=accel1;
    p->accel[2]=accel2;
    
    p->color[0]=color0;
    p->color[1]=color1;
    p->color[2]=color2;
    
    p->colorvel[0]=colorvel0;
    p->colorvel[1]=colorvel1;
    p->colorvel[2]=colorvel2;
    
    p->blendfunc_src = blendfunc_src;
    p->blendfunc_dst = blendfunc_dst;
    
    p->alpha=alpha;
    p->alphavel=alphavel;
    p->size=size;
    p->sizevel=sizevel;
    
    p->image=image;
    p->flags=flags;
    
    p->src_ent=0;
    p->dst_ent=0;
    
    if (think)
        p->think = think;
    else
        p->think = NULL;
    p->thinknext=thinknext;
    
    for (j=0;j<P_LIGHTS_MAX;j++)
    {
        cplight_t *plight = &p->lights[j];
        plight->isactive = false;
        plight->light = 0;
        plight->lightvel = 0;
        plight->lightcol[0] = 0;
        plight->lightcol[1] = 0;
        plight->lightcol[2] = 0;
    }
#ifdef DECALS
    p->decalnum = 0;
    p->decal = NULL;
    
    if (flags & PART_DECAL)
    {
        vec3_t dir;
        AngleVectors (p->angle, dir, NULL, NULL);
        VectorNegate(dir, dir);
        clipDecal(p, p->size, -p->angle[2], p->org, dir);
        
        if (!p->decalnum) // kill on viewframe
            p->alpha = 0;
    }
#endif
    return p;
}


/*
 ===============
 addParticleLight
 ===============
 */
void addParticleLight (cparticle_t *p,
                       float light, float lightvel,
                       float lcol0, float lcol1, float lcol2)
{
    int i;
    
    for (i=0; i<P_LIGHTS_MAX; i++)
    {
        cplight_t *plight = &p->lights[i];
        if (!plight->isactive)
        {
            plight->isactive = true;
            plight->light = light;
            plight->lightvel = lightvel;
            plight->lightcol[0] = lcol0;
            plight->lightcol[1] = lcol1;
            plight->lightcol[2] = lcol2;
            return;
        }
    }
}

/*
 ===============
 GENERIC PARTICLE THINKING ROUTINES
 ===============
 */
//#define SplashSize        10
#define    STOP_EPSILON    0.1

void calcPartVelocity(cparticle_t *p, float scale, float *time, vec3_t velocity)
{
    float time1 = *time;
    float time2 = time1*time1;
    
    velocity[0] = scale * (p->vel[0]*time1 + (p->accel[0])*time2);
    velocity[1] = scale * (p->vel[1]*time1 + (p->accel[1])*time2);
    
    if (p->flags & PART_GRAVITY)
        velocity[2] = scale * (p->vel[2]*time1 + (p->accel[2]-(PARTICLE_GRAVITY))*time2);
    else
        velocity[2] = scale * (p->vel[2]*time1 + (p->accel[2])*time2);
}

void ClipVelocity (vec3_t in, vec3_t normal, vec3_t out)
{
    float    backoff, change;
    int        i;
    
    backoff = VectorLength(in)*0.25 + DotProduct (in, normal) * 3.0f;
    
    for (i=0 ; i<3 ; i++)
    {
        change = normal[i]*backoff;
        out[i] = in[i] - change;
        if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
            out[i] = 0;
    }
}

void pBounceThink (cparticle_t *p, vec3_t org, vec3_t angle, float *alpha, float *size, int *image, float *time)
{
    float clipsize;
    trace_t tr;
    vec3_t velocity;
    
    clipsize = *size*0.5;
    if (clipsize<0.25) clipsize = 0.25;
    tr = CL_BrushTrace (p->oldorg, org, clipsize, MASK_SOLID); // was 1
    
    if (tr.fraction < 1)
    {
        calcPartVelocity(p, 1, time, velocity);
        ClipVelocity(velocity, tr.plane.normal, p->vel);
        
        VectorCopy(vec3_origin, p->accel);
        VectorCopy(tr.endpos, p->org);
        VectorCopy(p->org, org);
        VectorCopy(p->org, p->oldorg);
        
        p->alpha = *alpha;
        p->size = *size;
        
        p->start = p->time = newParticleTime();
        
        if (p->flags&PART_GRAVITY && VectorLength(p->vel)<2)
            p->flags &= ~PART_GRAVITY;
    }
    
    p->thinknext = true;
}

void
CL_ClearParticles(void)
{
	int i;

	free_particles = &particles[0];
	active_particles = NULL;

	for (i = 0; i < cl_numparticles; i++)
	{
		particles[i].next = &particles[i + 1];
#ifdef DECALS
        particles[i].decalnum = 0; // Knightmare added
        particles[i].decal = NULL; // Knightmare added
#endif
	}

	particles[cl_numparticles - 1].next = NULL;
}

void
CL_ParticleEffect (vec3_t org, vec3_t dir, int color8, int count)
{
    int            i;
    float        d;
    vec3_t color = { color8red(color8), color8green(color8), color8blue(color8)};
    
    for (i=0 ; i<count ; i++)
    {
        d = rand()&31;
        setupParticle (
                       0,        0,        0,
                       org[0] + ((rand()&7)-4) + d*dir[0],    org[1] + ((rand()&7)-4) + d*dir[1],    org[2] + ((rand()&7)-4) + d*dir[2],
                       crand()*20,            crand()*20,            crand()*20,
                       0,        0,        0,
                       color[0],        color[1],        color[2],
                       0,    0,    0,
                       1.0,        -1.0 / (0.5 + frand()*0.3),
                       GL_SRC_ALPHA, GL_ONE,
                       1,            0,
                       particle_generic,
                       PART_GRAVITY,
                       NULL,0);
    }
}

#define colorAdd 25
void
CL_ParticleEffect2 (vec3_t org, vec3_t dir, int color8, int count)
{
    int            i;
    float        d;
    vec3_t color = { color8red(color8), color8green(color8), color8blue(color8)};
    
    for (i=0 ; i<count ; i++)
    {
        d = rand()&7;
        setupParticle (
                       0,    0,    0,
                       org[0]+((rand()&7)-4)+d*dir[0],    org[1]+((rand()&7)-4)+d*dir[1],    org[2]+((rand()&7)-4)+d*dir[2],
                       crand()*20,            crand()*20,            crand()*20,
                       0,        0,        0,
                       color[0] + colorAdd,        color[1] + colorAdd,        color[2] + colorAdd,
                       0,    0,    0,
                       1,        -1.0 / (0.5 + frand()*0.3),
                       GL_SRC_ALPHA, GL_ONE,
                       1,            0,
                       particle_generic,
                       PART_GRAVITY,
                       NULL,0);
    }
}

void
CL_ParticleEffect3 (vec3_t org, vec3_t dir, int color8, int count)
{
    int            i;
    float        d;
    vec3_t color = { color8red(color8), color8green(color8), color8blue(color8)};
    
    for (i=0 ; i<count ; i++)
    {
        d = rand()&7;
        setupParticle (
                       0,    0,    0,
                       org[0]+((rand()&7)-4)+d*dir[0],    org[1]+((rand()&7)-4)+d*dir[1],    org[2]+((rand()&7)-4)+d*dir[2],
                       crand()*20,            crand()*20,            crand()*20,
                       0,        0,        0,
                       color[0] + colorAdd,        color[1] + colorAdd,        color[2] + colorAdd,
                       0,    0,    0,
                       1,        -0.25 / (0.5 + frand()*0.3),
                       GL_SRC_ALPHA, GL_ONE,
                       2,            -0.25,
                       particle_generic,
                       PART_GRAVITY,
                       NULL, false);
    }
}

void
CL_AddParticles(void)
{
	cparticle_t *p, *next;
	float alpha, size, light;
	float time=0, time2;
	vec3_t org, color, angle;
	int i, image, flags, decals;
	cparticle_t *active, *tail;

	active = NULL;
	tail = NULL;
    decals = 0;

	for (p = active_particles; p; p = next)
	{
		next = p->next;
        flags = p->flags;

		if (p->alphavel != INSTANT_PARTICLE)
		{
			time = (cl.time - p->time) * 0.001;
			alpha = p->alpha + time * p->alphavel;

#ifdef DECALS
            if (flags&PART_DECAL)
            {
                if (decals >= r_decals->value || alpha <= 0)
                {    // faded out
                    p->alpha = 0;
                    p->flags = 0;
                    ClearDecalPoly (p->decal); // flag decal chain for cleaning
                    p->decalnum = 0;
                    p->decal = NULL;
                    p->next = free_particles;
                    free_particles = p;
                    continue;
                }
            }
            else
#endif
            if (alpha <= 0)
			{
				/* faded out */
                p->alpha = 0;
                p->flags = 0;
				p->next = free_particles;
				free_particles = p;
				continue;
			}
		}
		else
		{
			time = 0.0f;
			alpha = p->alpha;
		}

		p->next = NULL;

		if (!tail)
		{
			active = tail = p;
		}

		else
		{
			tail->next = p;
			tail = p;
		}

		if (alpha > 1.0f)
		{
			alpha = 1;
		}
        if (alpha < 0.0)
            alpha = 0;

		time2 = time * time;
        image = p->image;

        for (i=0;i<3;i++)
        {
            color[i] = p->color[i] + p->colorvel[i]*time;
            if (color[i]>255) color[i]=255;
            if (color[i]<0) color[i]=0;
            
            angle[i] = p->angle[i];
            org[i] = p->org[i] + p->vel[i]*time + p->accel[i]*time2;
        }
        
        if (p->flags&PART_GRAVITY)
            org[2]+=time2*-PARTICLE_GRAVITY;
        
        size = p->size + p->sizevel*time;
        
        for (i=0;i<P_LIGHTS_MAX;i++)
        {
            const cplight_t *plight = &p->lights[i];
            if (plight->isactive)
            {
                light = plight->light*alpha + plight->lightvel*time;
                V_AddLight (org, light, plight->lightcol[0], plight->lightcol[1], plight->lightcol[2]);
            }
        }
        
        if (p->thinknext && p->think)
        {
            p->thinknext=false;
            p->think(p, org, angle, &alpha, &size, &image, &time);
        }
        
#ifdef DECALS
        if (flags & PART_DECAL)
        {
            decalpolys_t *d;
            if (p->decalnum > 0 && p->decal)
                for (d=p->decal; d; d=d->nextpoly)
                    V_AddDecal (org, angle, color, alpha, p->blendfunc_src, p->blendfunc_dst, size, image, flags, d);
            else
                V_AddDecal (org, angle, color, alpha, p->blendfunc_src, p->blendfunc_dst, size, image, flags, NULL);
            decals++;
        }
        else
#endif
            V_AddParticle (org, angle, color, alpha, p->blendfunc_src, p->blendfunc_dst, size, image, flags);

		if (p->alphavel == INSTANT_PARTICLE)
		{
			p->alphavel = 0.0;
			p->alpha = 0.0;
		}
        VectorCopy(org, p->oldorg);
	}

	active_particles = active;
#ifdef DECALS
    CleanDecalPolys(); // clean up active_decals linked list
#endif
}

//Knightmare- removed for Psychospaz's enhanced particle code
#if 0
void
CL_GenericParticleEffect(vec3_t org, vec3_t dir, int color,
		int count, int numcolors, int dirspread, float alphavel)
{
	int i, j;
	cparticle_t *p;
	float d;
	float time;

	time = (float)cl.time;

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

		if (numcolors > 1)
		{
			p->color = color + (randk() & numcolors);
		}

		else
		{
			p->color = color;
		}

		d = (float)(randk() & dirspread);

		for (j = 0; j < 3; j++)
		{
			p->org[j] = org[j] + ((randk() & 7) - 4) + d * dir[j];
			p->vel[j] = crandk() * 20;
		}

		p->accel[0] = p->accel[1] = 0;
		p->accel[2] = -PARTICLE_GRAVITY;
		p->alpha = 1.0;

		p->alphavel = -1.0f / (0.5f + frandk() * alphavel);
	}
}
#endif
