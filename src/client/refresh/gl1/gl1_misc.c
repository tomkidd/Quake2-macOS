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
 * Misc refresher functions
 *
 * =======================================================================
 */

#include "header/local.h"
#include <jpeglib.h> // Heffo - JPEG Screenshots


static byte dottexture[16][16] = {
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 1, 2, 3, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 1, 3, 3, 3, 3, 3, 3, 1, 0, 0, 0, 0, 0, 0, 0},
	{0, 2, 3, 3, 3, 3, 3, 3, 2, 0, 0, 0, 0, 0, 0, 0},
	{0, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0},
	{0, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0},
	{0, 2, 3, 3, 3, 3, 3, 3, 2, 0, 0, 0, 0, 0, 0, 0},
	{0, 1, 3, 3, 3, 3, 3, 3, 1, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 1, 2, 3, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

static byte notex[4][4] = {
	{0, 0, 0, 0},
	{0, 0, 1, 1},
	{0, 1, 1, 1},
	{0, 1, 1, 1}
};

typedef struct _TargaHeader
{
	unsigned char id_length, colormap_type, image_type;
	unsigned short colormap_index, colormap_length;
	unsigned char colormap_size;
	unsigned short x_origin, y_origin, width, height;
	unsigned char pixel_size, attributes;
} TargaHeader;

void
R_InitParticleTexture(void)
{
	int x, y;
	byte partData[16][16][4];
	byte notexData[8][8][4];

	/* particle texture */
	for (x = 0; x < 16; x++)
	{
		for (y = 0; y < 16; y++)
		{
			partData[y][x][0] = 255;
			partData[y][x][1] = 255;
			partData[y][x][2] = 255;
			partData[y][x][3] = dottexture[x][y] * 85;
		}
	}

	r_particletexture = R_LoadPic("***particle***", (byte *)partData,
	                              16, 0, 16, 0, it_sprite, 32);

	/* also use this for bad textures, but without alpha */
	for (x = 0; x < 8; x++)
	{
		for (y = 0; y < 8; y++)
		{
			notexData[y][x][0] = notex[x & 3][y & 3] * 255;
			notexData[y][x][1] = 0;
			notexData[y][x][2] = 0;
			notexData[y][x][3] = 255;
		}
	}

	r_notexture = R_LoadPic("***r_notexture***", (byte *)notexData,
	                        8, 0, 8, 0, it_wall, 32);
}

void
R_ScreenShot(void)
{
	int w=vid.width, h=vid.height;
	byte *buffer = malloc(w*h*3);

	if (!buffer)
	{
		R_Printf(PRINT_ALL, "R_ScreenShot: Couldn't malloc %d bytes\n", w*h*3);
		return;
	}

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, buffer);

	// the pixels are now row-wise left to right, bottom to top,
	// but we need them row-wise left to right, top to bottom.
	// so swap bottom rows with top rows
	{
		size_t bytesPerRow = 3*w;
		byte rowBuffer[bytesPerRow];
		byte *curRowL = buffer; // first byte of first row
		byte *curRowH = buffer + bytesPerRow*(h-1); // first byte of last row
		while(curRowL < curRowH)
		{
			memcpy(rowBuffer, curRowL, bytesPerRow);
			memcpy(curRowL, curRowH, bytesPerRow);
			memcpy(curRowH, rowBuffer, bytesPerRow);

			curRowL += bytesPerRow;
			curRowH -= bytesPerRow;
		}
	}

	ri.Vid_WriteScreenshot(w, h, 3, buffer);

	free(buffer);
}

void
R_Strings(void)
{
	R_Printf(PRINT_ALL, "GL_VENDOR: %s\n", gl_config.vendor_string);
	R_Printf(PRINT_ALL, "GL_RENDERER: %s\n", gl_config.renderer_string);
	R_Printf(PRINT_ALL, "GL_VERSION: %s\n", gl_config.version_string);
	R_Printf(PRINT_ALL, "GL_EXTENSIONS: %s\n", gl_config.extensions_string);
}

void
R_SetDefaultState(void)
{
	glClearColor(1, 0, 0.5, 0.5);
	glDisable(GL_MULTISAMPLE);
	glCullFace(GL_FRONT);
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.666);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	glColor4f(1, 1, 1, 1);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glShadeModel(GL_FLAT);

	R_TextureMode(gl_texturemode->string);
	R_TextureAlphaMode(gl1_texturealphamode->string);
	R_TextureSolidMode(gl1_texturesolidmode->string);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter_min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter_max);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	R_TexEnv(GL_REPLACE);

	if (gl_config.pointparameters)
	{
		float attenuations[3];

		attenuations[0] = gl1_particle_att_a->value;
		attenuations[1] = gl1_particle_att_b->value;
		attenuations[2] = gl1_particle_att_c->value;

		/* GL_POINT_SMOOTH is not implemented by some OpenGL
		   drivers, especially the crappy Mesa3D backends like
		   i915.so. That the points are squares and not circles
		   is not a problem by Quake II! */
		glEnable(GL_POINT_SMOOTH);
		qglPointParameterfARB(GL_POINT_SIZE_MIN_EXT, gl1_particle_min_size->value);
		qglPointParameterfARB(GL_POINT_SIZE_MAX_EXT, gl1_particle_max_size->value);
		qglPointParameterfvARB(GL_DISTANCE_ATTENUATION_EXT, attenuations);
	}

	if (gl_config.palettedtexture)
	{
		glEnable(GL_SHARED_TEXTURE_PALETTE_EXT);
		R_SetTexturePalette(d_8to24table);
	}

	if (gl_msaa_samples->value)
	{
		glEnable(GL_MULTISAMPLE);
		glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
	}
}

/*
 ================
 R_ResampleShotLerpLine
 from DarkPlaces
 ================
 */
void R_ResampleShotLerpLine (byte *in, byte *out, int inwidth, int outwidth)
{
    int j, xi, oldx = 0, f, fstep, l1, l2, endx;
    
    fstep = (int) (inwidth*65536.0f/outwidth);
    endx = (inwidth-1);
    for (j = 0,f = 0; j < outwidth; j++, f += fstep)
    {
        xi = (int) f >> 16;
        if (xi != oldx)
        {
            in += (xi - oldx) * 3;
            oldx = xi;
        }
        if (xi < endx)
        {
            l2 = f & 0xFFFF;
            l1 = 0x10000 - l2;
            *out++ = (byte) ((in[0] * l1 + in[3] * l2) >> 16);
            *out++ = (byte) ((in[1] * l1 + in[4] * l2) >> 16);
            *out++ = (byte) ((in[2] * l1 + in[5] * l2) >> 16);
        }
        else // last pixel of the line has no pixel to lerp to
        {
            *out++ = in[0];
            *out++ = in[1];
            *out++ = in[2];
        }
    }
}

/*
 ================
 R_ResampleShot
 ================
 */
void R_ResampleShot (void *indata, int inwidth, int inheight, void *outdata, int outwidth, int outheight)
{
    int i, j, yi, oldy, f, fstep, l1, l2, endy = (inheight-1);
    
    byte *inrow, *out, *row1, *row2;
    out = outdata;
    fstep = (int) (inheight*65536.0f/outheight);
    
    row1 = malloc(outwidth*3);
    row2 = malloc(outwidth*3);
    inrow = indata;
    oldy = 0;
    R_ResampleShotLerpLine (inrow, row1, inwidth, outwidth);
    R_ResampleShotLerpLine (inrow + inwidth*3, row2, inwidth, outwidth);
    for (i = 0, f = 0; i < outheight; i++,f += fstep)
    {
        yi = f >> 16;
        if (yi != oldy)
        {
            inrow = (byte *)indata + inwidth*3*yi;
            if (yi == oldy+1)
                memcpy(row1, row2, outwidth*3);
            else
                R_ResampleShotLerpLine (inrow, row1, inwidth, outwidth);
            
            if (yi < endy)
                R_ResampleShotLerpLine (inrow + inwidth*3, row2, inwidth, outwidth);
            else
                memcpy(row2, row1, outwidth*3);
            oldy = yi;
        }
        if (yi < endy)
        {
            l2 = f & 0xFFFF;
            l1 = 0x10000 - l2;
            for (j = 0;j < outwidth;j++)
            {
                *out++ = (byte) ((*row1++ * l1 + *row2++ * l2) >> 16);
                *out++ = (byte) ((*row1++ * l1 + *row2++ * l2) >> 16);
                *out++ = (byte) ((*row1++ * l1 + *row2++ * l2) >> 16);
            }
            row1 -= outwidth*3;
            row2 -= outwidth*3;
        }
        else // last line has no pixels to lerp to
        {
            for (j = 0;j < outwidth;j++)
            {
                *out++ = *row1++;
                *out++ = *row1++;
                *out++ = *row1++;
            }
            row1 -= outwidth*3;
        }
    }
    free(row1);
    free(row2);
}

#define    MAXPRINTMSG    4096
void VID_Printf (int print_level, char *fmt, ...)
{
    va_list        argptr;
    char        msg[MAXPRINTMSG];
    static qboolean    inupdate;
    
    va_start (argptr,fmt);
    vsprintf (msg,fmt,argptr);
    va_end (argptr);
    
    if (print_level == PRINT_ALL)
        Com_Printf ("%s", msg);
//    else
//        Com_DPrintf ("%s", msg);
}


/*
 ==================
 R_ScaledScreenshot
 by Knightmare
 ==================
 */
byte    *saveshotdata;
int        saveshotsize = 256;
void R_ScaledScreenshot (char *name)
{
    struct jpeg_compress_struct        cinfo;
    struct jpeg_error_mgr            jerr;
    JSAMPROW                        s[1];
    FILE                            *file;
    char                            shotname[MAX_OSPATH];
    int                                offset;
    
    if (!saveshotdata) return;
    
    Com_sprintf (shotname, sizeof(shotname), "%s", name);
    
    // Open the file for Binary Output
    file = fopen(shotname, "wb");
    if (!file)
    {
        VID_Printf (PRINT_ALL, "Menu_ScreenShot: Couldn't create %s\n", name);
        return;
    }
    
    // Initialise the JPEG compression object
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, file);
    
    // Setup JPEG Parameters
    cinfo.image_width = saveshotsize; //256;
    cinfo.image_height = saveshotsize; //256;
    cinfo.in_color_space = JCS_RGB;
    cinfo.input_components = 3;
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 85, TRUE); // was 100
    
    // Start Compression
    jpeg_start_compress(&cinfo, true);
    
    // Feed Scanline data
    offset = (cinfo.image_width * cinfo.image_height * 3) - (cinfo.image_width * 3);
    while(cinfo.next_scanline < cinfo.image_height)
    {
        s[0] = &saveshotdata[offset - (cinfo.next_scanline * (cinfo.image_width * 3))];
        jpeg_write_scanlines(&cinfo, s, 1);
    }
    
    // Finish Compression
    jpeg_finish_compress(&cinfo);
    
    // Destroy JPEG object
    jpeg_destroy_compress(&cinfo);
    
    // Close File
    fclose(file);
}


/*
 ==================
 R_GrabScreen
 by Knightmare
 ==================
 */
void R_GrabScreen (void)
{
    byte    *rgbdata;
    
    // Free saveshot buffer first
    if (saveshotdata)
        free(saveshotdata);
    
    // Optional hi-res saveshots
    if (r_saveshotsize->value && (vid.width >= 1024) && (vid.height >= 1024))
        saveshotsize = 1024;
    else if (r_saveshotsize->value && (vid.width >= 512) && (vid.height >= 512))
        saveshotsize = 512;
    else
        saveshotsize = 256;
    
    // Allocate room for a copy of the framebuffer
    rgbdata = malloc(vid.width * vid.height * 3);
    if (!rgbdata)
        return;
    // Allocate room for reduced screenshot
    saveshotdata = malloc(saveshotsize * saveshotsize * 3);
    if (!saveshotdata)
    {
        free(rgbdata);
        return;
    }
    
    // Read the framebuffer into our storage
    glReadPixels(0, 0, vid.width, vid.height, GL_RGB, GL_UNSIGNED_BYTE, rgbdata);
    // Resize to 256x256
    R_ResampleShot(rgbdata, vid.width, vid.height, saveshotdata, saveshotsize, saveshotsize);
    
    // Free Temp Framebuffer
    free(rgbdata);
}
