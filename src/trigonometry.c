/*
Copyright (c) 2015 Steven Arnow <s@rdw.se>
Modified 2017 Axel Isaksson: added coordinate system function
'trigonometry.c'

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

#include <math.h>
#include "trigonometry.h"

#ifndef M_PI
#define	M_PI 3.141592653589793  
#endif

#define	trig_delta_to_angle(type, suffix, tan_func)  		\
	type trig_delta_to_angle_##suffix(type dx, type dy) {	\
		type angle;					\
								\
		if (dx == 0)					\
			angle = (dy<0)?-HUGE_VAL:HUGE_VAL;	\
		else						\
			angle = dy / dx;			\
		angle = tan_func(angle);			\
		if (dx < 0)					\
			angle = angle + M_PI;			\
		else if (angle < 0)				\
			angle = 2. * M_PI + angle;	\
								\
		return angle;					\
	}

trig_delta_to_angle(float, f, atanf)
trig_delta_to_angle(double, d, atan)


Vector2 transform_rotated_system(int center_x, int center_y, float degrees, int x_in, int y_in) {
	int x, y;
	double dist, ang;
	Vector2 ret;
	
	x = x_in - center_x;
	y = y_in - center_y;
	
	dist = sqrt(x*x + y*y);
	ang = trig_delta_to_angle_d(x, y);
	
	ang -= degrees;
	
	ret.x = dist*cos(ang *M_PI/180.0);
	ret.y = dist*sin(ang *M_PI/180.0);
	
	return ret;
}
