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
#include "header/particles.h"

cparticle_t *active_particles, *free_particles;
cparticle_t particles[MAX_PARTICLES];
extern int cl_numparticles = MAX_PARTICLES;

void
CL_ClearParticles(void)
{
	int i;

	free_particles = &particles[0];
	active_particles = NULL;

	for (i = 0; i < cl_numparticles; i++)
	{
		particles[i].next = &particles[i + 1];
	}

	particles[cl_numparticles - 1].next = NULL;
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

#define colorAdd 25
void
CL_ParticleEffect2(vec3_t org, vec3_t dir, int color8, int count)
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
	float alpha;
	float time, time2;
	vec3_t org;
	int color;
	cparticle_t *active, *tail;

	active = NULL;
	tail = NULL;

	for (p = active_particles; p; p = next)
	{
		next = p->next;

		if (p->alphavel != INSTANT_PARTICLE)
		{
			time = (cl.time - p->time) * 0.001;
			alpha = p->alpha + time * p->alphavel;

			if (alpha <= 0)
			{
				/* faded out */
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

		color = p->color;
		time2 = time * time;

		org[0] = p->org[0] + p->vel[0] * time + p->accel[0] * time2;
		org[1] = p->org[1] + p->vel[1] * time + p->accel[1] * time2;
		org[2] = p->org[2] + p->vel[2] * time + p->accel[2] * time2;

		V_AddParticle(org, color, alpha);

		if (p->alphavel == INSTANT_PARTICLE)
		{
			p->alphavel = 0.0;
			p->alpha = 0.0;
		}
	}

	active_particles = active;
}

//void
//CL_GenericParticleEffect(vec3_t org, vec3_t dir, int color,
//        int count, int numcolors, int dirspread, float alphavel)
//{
//    int i, j;
//    cparticle_t *p;
//    float d;
//    float time;
//
//    time = (float)cl.time;
//
//    for (i = 0; i < count; i++)
//    {
//        if (!free_particles)
//        {
//            return;
//        }
//
//        p = free_particles;
//        free_particles = p->next;
//        p->next = active_particles;
//        active_particles = p;
//
//        p->time = time;
//
//        if (numcolors > 1)
//        {
//            p->color = color + (randk() & numcolors);
//        }
//
//        else
//        {
//            p->color = color;
//        }
//
//        d = (float)(randk() & dirspread);
//
//        for (j = 0; j < 3; j++)
//        {
//            p->org[j] = org[j] + ((randk() & 7) - 4) + d * dir[j];
//            p->vel[j] = crandk() * 20;
//        }
//
//        p->accel[0] = p->accel[1] = 0;
//        p->accel[2] = -PARTICLE_GRAVITY;
//        p->alpha = 1.0;
//
//        p->alphavel = -1.0f / (0.5f + frandk() * alphavel);
//    }
//}

