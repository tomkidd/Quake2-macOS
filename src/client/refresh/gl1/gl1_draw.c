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
 * Drawing of all images that are not textures
 *
 * =======================================================================
 */

#include "header/local.h"

image_t *draw_chars;

extern qboolean scrap_dirty;
void Scrap_Upload(void);

extern unsigned r_rawpalette[256];

void RefreshFont (void)
{
    con_font->modified = false;
    
    draw_chars = R_FindImage (va("fonts/%s.pcx", con_font->string), it_pic);
    if (!draw_chars) // fall back on default font
        draw_chars = R_FindImage ("fonts/default.pcx", it_pic);
    if (!draw_chars) // fall back on old Q2 conchars
        draw_chars = R_FindImage ("pics/conchars.pcx", it_pic);
    if (!draw_chars) // Knightmare- prevent crash caused by missing font
        VID_Error (ERR_FATAL, "RefreshFont: couldn't load pics/conchars");
    
    GL_Bind( draw_chars->texnum );
}

void
Draw_InitLocal(void)
{
    image_t    *R_DrawFindPic (char *name);
    
    qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // load console characters (don't bilerp characters)
    RefreshFont();
    
    R_InitChars (); // init char indexes
}

/*
 ================
 R_CharMapScale
 ================
 */
float R_CharMapScale (void)
{
    return (draw_chars->width/128.0); //current width / original width
}

unsigned    char_count;
/*
 ================
 R_InitChars
 ================
 */
void R_InitChars (void)
{
    char_count = 0;
}

/*
 ================
 R_FlushChars
 ================
 */
void R_FlushChars (void)
{
    if (rb_vertex == 0 || rb_index == 0) // nothing to flush
        return;
    
    GL_Disable (GL_ALPHA_TEST);
    GL_TexEnv (GL_MODULATE);
    GL_Enable (GL_BLEND);
    GL_DepthMask (false);
    GL_Bind(draw_chars->texnum);
    
    RB_DrawArrays (GL_QUADS);
    char_count = 0;
    
    GL_DepthMask (true);
    GL_Disable (GL_BLEND);
    GL_TexEnv (GL_REPLACE);
    GL_Enable (GL_ALPHA_TEST);
}

/*
 * Draws one variable sized graphics character with 0 being transparent.
 * It can be clipped to the top of the screen to allow the console to be
 * smoothly scrolled off.
 */
void
RDraw_CharScaled(int x, int y, int num, float scale,
                 int red, int green, int blue, int alpha, qboolean italic, qboolean last)
{
    int            row, col, i;
    float        frow, fcol, size, cscale, italicAdd;
    vec2_t        texCoord[4], verts[4];
    qboolean    addChar = true;
    
    num &= 255;
    
    if (alpha > 255)
        alpha = 255;
    else if (alpha < 1)
        alpha = 1;
    
    if ((num & 127) == 32)    // space
        addChar = false;
    if (y <= -(scale * DEFAULT_FONT_SIZE))    // totally off screen
        addChar = false;
    
    row = num >> 4;
    col = num&15;
    
    frow = row*0.0625;
    fcol = col*0.0625;
    size = 0.0625;
    cscale = scale * DEFAULT_FONT_SIZE;
    
    italicAdd = (italic) ? (cscale*0.25) : 0;
    
    if (addChar)
    {
        Vector2Set(texCoord[0], fcol, frow);
        Vector2Set(texCoord[1], fcol + size, frow);
        Vector2Set(texCoord[2], fcol + size, frow + size);
        Vector2Set(texCoord[3], fcol, frow + size);
        
        Vector2Set(verts[0], x+italicAdd, y);
        Vector2Set(verts[1], x+cscale+italicAdd, y);
        Vector2Set(verts[2], x+cscale-italicAdd, y+cscale);
        Vector2Set(verts[3], x-italicAdd, y+cscale);

        if (char_count == 0)
            rb_vertex = rb_index = 0;
        if (rb_vertex + 4 >= MAX_VERTICES || rb_index + 4 >= MAX_INDICES)
            R_FlushChars ();
        for (i=0; i<4; i++) {
            VA_SetElem2(texCoordArray[0][rb_vertex], texCoord[i][0], texCoord[i][1]);
            VA_SetElem2(vertexArray[rb_vertex], verts[i][0], verts[i][1]);
            VA_SetElem4(colorArray[rb_vertex], red*DIV255, green*DIV255, blue*DIV255, alpha*DIV255);
            indexArray[rb_index++] = rb_vertex;
            rb_vertex++;
        }
        char_count++;
    }
    if (last)
        R_FlushChars ();
}

image_t *
RDraw_FindPic(char *name)
{
	image_t *gl;
	char fullname[MAX_QPATH];

	if ((name[0] != '/') && (name[0] != '\\'))
	{
		Com_sprintf(fullname, sizeof(fullname), "pics/%s.pcx", name);
		gl = R_FindImage(fullname, it_pic);
	}
	else
	{
		gl = R_FindImage(name + 1, it_pic);
	}

	return gl;
}

void
RDraw_GetPicSize(int *w, int *h, char *pic)
{
	image_t *gl;

	gl = RDraw_FindPic(pic);

	if (!gl)
	{
		*w = *h = -1;
		return;
	}

	*w = gl->width;
	*h = gl->height;
}

void
RDraw_StretchPic(int x, int y, int w, int h, char *pic, float alpha)
{
	image_t *gl;
    int            i;
    vec2_t        texCoord[4], verts[4];

	gl = RDraw_FindPic(pic);

	if (!gl)
	{
		R_Printf(PRINT_ALL, "Can't find pic: %s\n", pic);
		return;
	}

	if (scrap_dirty)
	{
		Scrap_Upload();
	}
    
    // Psychospaz's transparent console support
    if (gl->has_alpha || alpha < 1.0)
    {
        GL_Disable (GL_ALPHA_TEST);
        GL_TexEnv (GL_MODULATE);
        //qglColor4f (1,1,1,alpha);
        GL_Enable (GL_BLEND);
        GL_DepthMask (false);
    }

	R_Bind(gl->texnum);

//    GLfloat vtx[] = {
//        x, y,
//        x + w, y,
//        x + w, y + h,
//        x, y + h
//    };
//
//    GLfloat tex[] = {
//        gl->sl, gl->tl,
//        gl->sh, gl->tl,
//        gl->sh, gl->th,
//        gl->sl, gl->th
//    };
//
//    glEnableClientState( GL_VERTEX_ARRAY );
//    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
//
//    glVertexPointer( 2, GL_FLOAT, 0, vtx );
//    glTexCoordPointer( 2, GL_FLOAT, 0, tex );
//    glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
//
//    glDisableClientState( GL_VERTEX_ARRAY );
//    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
    
    Vector2Set(texCoord[0], gl->sl, gl->tl);
    Vector2Set(texCoord[1], gl->sh, gl->tl);
    Vector2Set(texCoord[2], gl->sh, gl->th);
    Vector2Set(texCoord[3], gl->sl, gl->th);
    
    Vector2Set(verts[0], x, y);
    Vector2Set(verts[1], x+w, y);
    Vector2Set(verts[2], x+w, y+h);
    Vector2Set(verts[3], x, y+h);
    
    rb_vertex = rb_index = 0;
    for (i=0; i<4; i++) {
        VA_SetElem2(texCoordArray[0][rb_vertex], texCoord[i][0], texCoord[i][1]);
        VA_SetElem2(vertexArray[rb_vertex], verts[i][0], verts[i][1]);
        VA_SetElem4(colorArray[rb_vertex], 1.0, 1.0, 1.0, alpha);
        indexArray[rb_index++] = rb_vertex;
        rb_vertex++;
    }
    RB_DrawArrays (GL_QUADS);
    
    // Psychospaz's transparent console support
    if (gl->has_alpha || alpha < 1.0)
    {
        GL_DepthMask (true);
        GL_TexEnv (GL_REPLACE);
        GL_Disable (GL_BLEND);
        //qglColor4f (1,1,1,1);
        GL_Enable (GL_ALPHA_TEST);
    }
}

void
RDraw_PicScaled(int x, int y, float scale, float alpha, char *pic)
{
    float    xoff, yoff;
    float    scale_x, scale_y;
	image_t *gl;
    int        i;
    vec2_t    texCoord[4], verts[4];

	gl = RDraw_FindPic(pic);

	if (!gl)
	{
		R_Printf(PRINT_ALL, "Can't find pic: %s\n", pic);
		return;
	}

	if (scrap_dirty)
	{
		Scrap_Upload();
	}

    // add alpha support
    if (gl->has_alpha || alpha < 1.0)
    {
        GL_Disable (GL_ALPHA_TEST);
        GL_TexEnv (GL_MODULATE);
        //qglColor4f (1,1,1, alpha);
        GL_Enable (GL_BLEND);
        GL_DepthMask (false);
    }

    R_Bind(gl->texnum);

    scale_x = scale_y = scale;
    scale_x *= gl->replace_scale_w; // scale down if replacing a pcx image
    scale_y *= gl->replace_scale_h; // scale down if replacing a pcx image
    
    Vector2Set(texCoord[0], gl->sl, gl->tl);
    Vector2Set(texCoord[1], gl->sh, gl->tl);
    Vector2Set(texCoord[2], gl->sh, gl->th);
    Vector2Set(texCoord[3], gl->sl, gl->th);
    
    xoff = gl->width*scale_x-gl->width;
    yoff = gl->height*scale_y-gl->height;
    
    Vector2Set(verts[0], x, y);
    Vector2Set(verts[1], x+gl->width+xoff, y);
    Vector2Set(verts[2], x+gl->width+xoff, y+gl->height+yoff);
    Vector2Set(verts[3], x, y+gl->height+yoff);
    
    rb_vertex = rb_index = 0;
    for (i=0; i<4; i++) {
        VA_SetElem2(texCoordArray[0][rb_vertex], texCoord[i][0], texCoord[i][1]);
        VA_SetElem2(vertexArray[rb_vertex], verts[i][0], verts[i][1]);
        VA_SetElem4(colorArray[rb_vertex], 1.0, 1.0, 1.0, alpha);
        indexArray[rb_index++] = rb_vertex;
        rb_vertex++;
    }
    RB_DrawArrays (GL_QUADS);
    
//    GLfloat vtx[] = {
//        x, y,
//        x + gl->width * factor, y,
//        x + gl->width * factor, y + gl->height * factor,
//        x, y + gl->height * factor
//    };
//
//    GLfloat tex[] = {
//        gl->sl, gl->tl,
//        gl->sh, gl->tl,
//        gl->sh, gl->th,
//        gl->sl, gl->th
//    };
//
//    glEnableClientState( GL_VERTEX_ARRAY );
//    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
//
//    glVertexPointer( 2, GL_FLOAT, 0, vtx );
//    glTexCoordPointer( 2, GL_FLOAT, 0, tex );
//    glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
//
//    glDisableClientState( GL_VERTEX_ARRAY );
//    glDisableClientState( GL_TEXTURE_COORD_ARRAY );

        if (gl->has_alpha || alpha < 1.0)
    {
        GL_DepthMask (true);
        GL_TexEnv (GL_REPLACE);
        GL_Disable (GL_BLEND);
        //qglColor4f (1,1,1,1);
        // add alpha support
        GL_Enable (GL_ALPHA_TEST);
    }
}

// RDraw_Pic / R_DrawPic would go here -tkidd

/*
 * This repeats a 64*64 tile graphic to fill
 * the screen around a sized down
 * refresh window.
 */
void
RDraw_TileClear(int x, int y, int w, int h, char *pic)
{
	image_t *image;
    int        i;
    vec2_t    texCoord[4], verts[4];

	image = RDraw_FindPic(pic);

	if (!image)
	{
		R_Printf(PRINT_ALL, "Can't find pic: %s\n", pic);
		return;
	}

	R_Bind(image->texnum);
    
    Vector2Set(texCoord[0], x/64.0, y/64.0);
    Vector2Set(texCoord[1], (x+w)/64.0, y/64.0);
    Vector2Set(texCoord[2], (x+w)/64.0, (y+h)/64.0);
    Vector2Set(texCoord[3], x/64.0, (y+h)/64.0);
    
    Vector2Set(verts[0], x, y);
    Vector2Set(verts[1], x+w, y);
    Vector2Set(verts[2], x+w, y+h);
    Vector2Set(verts[3], x, y+h);
    
    rb_vertex = rb_index = 0;
    for (i=0; i<4; i++) {
        VA_SetElem2(texCoordArray[0][rb_vertex], texCoord[i][0], texCoord[i][1]);
        VA_SetElem2(vertexArray[rb_vertex], verts[i][0], verts[i][1]);
        VA_SetElem4(colorArray[rb_vertex], 1.0, 1.0, 1.0, 1.0);
        indexArray[rb_index++] = rb_vertex;
        rb_vertex++;
    }
    RB_DrawArrays (GL_QUADS);

//    GLfloat vtx[] = {
//        x, y,
//        x + w, y,
//        x + w, y + h,
//        x, y + h
//    };
//
//    GLfloat tex[] = {
//        x / 64.0, y / 64.0,
//        ( x + w ) / 64.0, y / 64.0,
//        ( x + w ) / 64.0, ( y + h ) / 64.0,
//        x / 64.0, ( y + h ) / 64.0
//    };
//
//    glEnableClientState( GL_VERTEX_ARRAY );
//    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
//
//    glVertexPointer( 2, GL_FLOAT, 0, vtx );
//    glTexCoordPointer( 2, GL_FLOAT, 0, tex );
//    glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
//
//    glDisableClientState( GL_VERTEX_ARRAY );
//    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}

/*
 * Fills a box of pixels with a single color
 */
void
RDraw_Fill(int x, int y, int w, int h, int c)
{
	union
	{
		unsigned c;
		byte v[4];
	} color;

	if ((unsigned)c > 255)
	{
		ri.Sys_Error(ERR_FATAL, "Draw_Fill: bad color");
	}

//    glDisable(GL_TEXTURE_2D);

	color.c = d_8to24table[c];
    
    R_DrawFill2 (x, y, w, h, color.v[0], color.v[1], color.v[2], 255);

//    glColor4f(color.v [ 0 ] / 255.0, color.v [ 1 ] / 255.0,
//               color.v [ 2 ] / 255.0, 1);
//
//    GLfloat vtx[] = {
//        x, y,
//        x + w, y,
//        x + w, y + h,
//        x, y + h
//    };
//
//    glEnableClientState( GL_VERTEX_ARRAY );
//
//    glVertexPointer( 2, GL_FLOAT, 0, vtx );
//    glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
//
//    glDisableClientState( GL_VERTEX_ARRAY );
//
//    glColor4f( 1, 1, 1, 1 );
//    glEnable(GL_TEXTURE_2D);
}

/*
 ======================
 R_DrawFill2
 
 Fills a box of pixels with a
 24-bit color w/ alpha
 ===========================
 */
void
RDraw_Fill2 (int x, int y, int w, int h, int red, int green, int blue, int alpha)
{
    int        i;
    vec2_t    verts[4];
    
    red = min(red, 255);
    green = min(green, 255);
    blue = min(blue, 255);
    alpha = max(min(alpha, 255), 1);
    
    //qglDisable (GL_TEXTURE_2D);
    GL_DisableTexture (0);
    GL_Disable (GL_ALPHA_TEST);
    GL_TexEnv (GL_MODULATE);
    GL_Enable (GL_BLEND);
    GL_DepthMask   (false);
    
#if 1
    Vector2Set(verts[0], x, y);
    Vector2Set(verts[1], x+w, y);
    Vector2Set(verts[2], x+w, y+h);
    Vector2Set(verts[3], x, y+h);
    
    rb_vertex = rb_index = 0;
    for (i=0; i<4; i++) {
        VA_SetElem2(vertexArray[rb_vertex], verts[i][0], verts[i][1]);
        VA_SetElem4(colorArray[rb_vertex], red*DIV255, green*DIV255, blue*DIV255, alpha*DIV255);
        indexArray[rb_index++] = rb_vertex;
        rb_vertex++;
    }
    RB_DrawArrays (GL_QUADS);
#else
    qglColor4ub ((byte)red, (byte)green, (byte)blue, (byte)alpha);
    
    qglBegin (GL_QUADS);
    
    qglVertex2f (x,y);
    qglVertex2f (x+w, y);
    qglVertex2f (x+w, y+h);
    qglVertex2f (x, y+h);
    
    qglEnd ();
#endif
    
    GL_DepthMask (true);
    GL_Disable (GL_BLEND);
    GL_TexEnv (GL_REPLACE);
    //qglColor4f   (1,1,1,1);
    GL_Enable (GL_ALPHA_TEST);
    //qglEnable (GL_TEXTURE_2D);
    GL_EnableTexture (0);
}


void
RDraw_FadeScreen(void)
{
	glEnable(GL_BLEND);
//    glDisable(GL_TEXTURE_2D);
    GL_DisableTexture (0);
	glColor4f(0, 0, 0, 0.8);
    qglBegin (GL_QUADS);
//
//    GLfloat vtx[] = {
//        0, 0,
//        vid.width, 0,
//        vid.width, vid.height,
//        0, vid.height
//    };
//
//    glEnableClientState( GL_VERTEX_ARRAY );
//
//    glVertexPointer( 2, GL_FLOAT, 0, vtx );
//    glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
//
//    glDisableClientState( GL_VERTEX_ARRAY );
//
//    glColor4f(1, 1, 1, 1);
//    glEnable(GL_TEXTURE_2D);
//    glDisable(GL_BLEND);
    
    qglVertex2f (0,0);
    qglVertex2f (vid.width, 0);
    qglVertex2f (vid.width, vid.height);
    qglVertex2f (0, vid.height);
    
    qglEnd ();
    qglColor4f (1,1,1,1);
    //qglEnable (GL_TEXTURE_2D);
    GL_EnableTexture (0);
    GL_Disable (GL_BLEND);
}
#ifdef ROQ_SUPPORT

void R_DrawStretchRaw (int x, int y, int w, int h, const byte *raw, int rawWidth, int rawHeight) //qboolean noDraw)
{
    int        width = 1, height = 1;
    
    // Make sure everything is flushed if needed
    //if (!noDraw)
    //    RB_RenderMesh();
    
    // Check the dimensions
    while (width < rawWidth)
        width <<= 1;
    while (height < rawHeight)
        height <<= 1;
    
    if (rawWidth != width || rawHeight != height)
        VID_Error(ERR_DROP, "Draw_StretchRaw2: size is not a power of two (%i x %i)", rawWidth, rawHeight);
    
    if (rawWidth > gl_config.max_texsize || rawHeight > gl_config.max_texsize)
        VID_Error(ERR_DROP, "Draw_StretchRaw2: size exceeds hardware limits (%i > %i or %i > %i)", rawWidth, gl_config.max_texsize, rawHeight, gl_config.max_texsize);
    
    // Update the texture as appropriate
    GL_Bind(r_rawtexture->texnum);
    
    if (rawWidth == r_rawtexture->upload_width && rawHeight == r_rawtexture->upload_height)
        qglTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, rawWidth, rawHeight, GL_RGBA, GL_UNSIGNED_BYTE, raw);
    else
    {
        r_rawtexture->upload_width = rawWidth;
        r_rawtexture->upload_height = rawHeight;
        qglTexImage2D(GL_TEXTURE_2D, 0, gl_tex_solid_format, rawWidth, rawHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, raw);
    }
    
    //if (noDraw)
    //    return;
    
    qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Draw it
    qglColor4ub(255, 255, 255, 255);
    
    qglBegin(GL_QUADS);
    qglTexCoord2f(0, 0);
    qglVertex2f(x, y);
    qglTexCoord2f(1, 0);
    qglVertex2f(x+w, y);
    qglTexCoord2f(1, 1);
    qglVertex2f(x+w, y+h);
    qglTexCoord2f(0, 1);
    qglVertex2f(x, y+h);
    qglEnd();
}
#else // old 8-bit, 256x256 version

void
RDraw_StretchRaw(int x, int y, int w, int h, int cols, int rows, byte *data)
{
	GLfloat tex[8];
	byte *source;
	float hscale = 1.0f;
	int frac, fracstep;
	int i, j, trows;
	int row;

	R_Bind(0);

	if(gl_config.npottextures || rows <= 256)
	{
		// X, X
		tex[0] = 0;
		tex[1] = 0;

		// X, Y
		tex[2] = 1;
		tex[3] = 0;

		// Y, X
		tex[4] = 1;
		tex[5] = 1;

		// Y, Y
		tex[6] = 0;
		tex[7] = 1;
	}
	else
	{
		// Scale params
		hscale = rows / 256.0;
		trows = 256;

		// X, X
		tex[0] = 1.0 / 512.0;
		tex[1] = 1.0 / 512.0;

		// X, Y
		tex[2] = 511.0 / 512.0;
		tex[3] = 1.0 / 512.0;

		// Y, X
		tex[4] = 511.0 / 512.0;
		tex[5] = rows * hscale / 256 - 1.0 / 512.0;

		// Y, Y
		tex[6] = 1.0 / 512.0;
		tex[7] = rows * hscale / 256 - 1.0 / 512.0;
	}

	GLfloat vtx[] = {
			x, y,
			x + w, y,
			x + w, y + h,
			x, y + h
	};

	if (!gl_config.palettedtexture)
	{
		unsigned image32[320*240]; /* was 256 * 256, but we want a bit more space */

		/* .. because now if non-power-of-2 textures are supported, we just load
		 * the data into a texture in the original format, without skipping any
		 * pixels to fit into a 256x256 texture.
		 * This causes text in videos (which are 320x240) to not look broken anymore.
		 */
		if(gl_config.npottextures || rows <= 256)
		{
			unsigned* img = image32;

			if(cols*rows > 320*240)
			{
				/* in case there is a bigger video after all,
				 * malloc enough space to hold the frame */
				img = (unsigned*)malloc(cols*rows*4);
			}

			for(i=0; i<rows; ++i)
			{
				int rowOffset = i*cols;
				for(j=0; j<cols; ++j)
				{
					byte palIdx = data[rowOffset+j];
					img[rowOffset+j] = r_rawpalette[palIdx];
				}
			}

			glTexImage2D(GL_TEXTURE_2D, 0, gl_tex_solid_format,
								cols, rows, 0, GL_RGBA, GL_UNSIGNED_BYTE,
								img);

			if(img != image32)
			{
				free(img);
			}
		}
		else
		{
			unsigned int image32[320*240];
			unsigned *dest;

			for (i = 0; i < trows; i++)
			{
				row = (int)(i * hscale);

				if (row > rows)
				{
					break;
				}

				source = data + cols * row;
				dest = &image32[i * 256];
				fracstep = cols * 0x10000 / 256;
				frac = fracstep >> 1;

				for (j = 0; j < 256; j++)
				{
					dest[j] = r_rawpalette[source[frac >> 16]];
					frac += fracstep;
				}
			}

			glTexImage2D(GL_TEXTURE_2D, 0, gl_tex_solid_format,
					256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE,
					image32);
		}
	}
	else
	{
		unsigned char image8[256 * 256];
		unsigned char *dest;

		for (i = 0; i < trows; i++)
		{
			row = (int)(i * hscale);

			if (row > rows)
			{
				break;
			}

			source = data + cols * row;
			dest = &image8[i * 256];
			fracstep = cols * 0x10000 / 256;
			frac = fracstep >> 1;

			for (j = 0; j < 256; j++)
			{
				dest[j] = source[frac >> 16];
				frac += fracstep;
			}
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_COLOR_INDEX8_EXT, 256, 256,
				0, GL_COLOR_INDEX, GL_UNSIGNED_BYTE, image8);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	glVertexPointer( 2, GL_FLOAT, 0, vtx );
	glTexCoordPointer( 2, GL_FLOAT, 0, tex );
	glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}

#endif

int
Draw_GetPalette(void)
{
	int i;
	int r, g, b;
	unsigned v;
	byte *pic, *pal;
	int width, height;

	/* get the palette */
	LoadPCX("pics/colormap.pcx", &pic, &pal, &width, &height);

	if (!pal)
	{
		ri.Sys_Error(ERR_FATAL, "Couldn't load pics/colormap.pcx");
	}

	for (i = 0; i < 256; i++)
	{
		r = pal[i * 3 + 0];
		g = pal[i * 3 + 1];
		b = pal[i * 3 + 2];

		v = (255 << 24) + (r << 0) + (g << 8) + (b << 16);
		d_8to24table[i] = LittleLong(v);
	}

	d_8to24table[255] &= LittleLong(0xffffff); /* 255 is transparent */

	free(pic);
	free(pal);

	return 0;
}

