/*
Copyright (c) 2011-2013 Steven Arnow
'render.h' - This file is part of libdarnit

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	claim that you wrote the original software. If you use this software
	in a product, an acknowledgment in the product documentation would be
	appreciated but is not required.

	2. Altered source versions must be plainly marked as such, and must not be
	misrepresented as being the original software.

	3. This notice may not be removed or altered from any source
	distribution.
*/


#ifndef __RENDER_H__
#define	__RENDER_H__


#define	LOGIC_OP_AND	1
#define	LOGIC_OP_NAND	2
#define	LOGIC_OP_OR	3
#define	LOGIC_OP_NOR	4
#define	LOGIC_OP_XOR	5

#define	DARNIT_SCALE_NEAREST	1
#define	DARNIT_SCALE_LINEAR	2


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	float		r;
	float		s;
	float		u;
	float		v;
} TILE;

typedef struct {
	int		line_x1;
	int		line_y1;
	int		line_x2;
	int		line_y2;
} RENDER_PARAMS;


typedef struct {
	float		x1;
	float		y1;
	float		x2;
	float		y2;
} LINE_CACHE;


typedef struct {
	float		x;
	float		y;
} COORDINATE;


typedef struct {
	float		u;
	float		v;
} TEX_COORDINATE;


typedef struct {
	unsigned char	rgba[4];
} COL_COORDINATE;


typedef struct {
	COORDINATE	coord;
	TEX_COORDINATE	tex;
	COL_COORDINATE	col;
} COLOR_TEX_VERTEX;


typedef struct {
	COORDINATE	coord;
	TEX_COORDINATE	tex;
} TEX_VERTEX;


typedef struct {
	COORDINATE	coord;
	COL_COORDINATE	col;
} COLOR_VERTEX;


typedef struct {
	COLOR_VERTEX	vertex[3];
} TRI_COLOR_CACHE;


typedef struct {
	COLOR_VERTEX	vertex[2];
} LINE_COLOR_CACHE;


typedef struct {
	COLOR_TEX_VERTEX vertex[6];
} TILE_COLOR_TEX_CACHE;


typedef struct {
	COORDINATE	coord[6];
} RECT_CACHE;

typedef struct {
	COORDINATE	coord[1];
} POINT_CACHE;

typedef struct {
	COLOR_VERTEX	coord[1];
} POINT_COLOR_CACHE;

typedef struct {
	COLOR_VERTEX	vertex[6];

} TILE_COLOR_CACHE;


typedef struct {
	TEX_VERTEX	vertex[6];
} TILE_CACHE;


typedef struct {
	float		r;
	float		g;
	float		b;
	float		a;
	float		fadefactor;
	unsigned int	fadestart;
	int		fadeprog;
	float		fadedir;
	int		fading;
} FADE_STRUCT;


#endif
