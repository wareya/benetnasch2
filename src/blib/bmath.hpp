#ifndef INCLUDED_BMATH
// This file is ISC licensed
#define INCLUDED_BMATH

#include <utility>
#include <cmath>

#define maximum( a, b ) ((a > b) ? a : b)
#define minimum( a, b ) ((a < b) ? a : b)
#define PI (atan( 1 ) * 4)
#define rad2deg( a ) (a*180/PI)
#define deg2rad( a ) (a*PI/180)

template <typename CType>
CType lerp(CType a, CType b, CType t) // a to b, t from 0-1
{
    return b*t + a*(1.0-t);
}

bool aabb_overlap (float x1, float y1, float x2, float y2, // coordinate pair for first box
                   float x3, float y3, float x4, float y4);// second box
bool line_aabb_overlap (float x1, float y1, float x2, float y2, // coordinate pair of line
                        float x3, float y3, float x4, float y4);// box

void HSBtoRGB( float hue, float saturation, float brightness, int rgb[3] );

void RGBtoHSB( int r, int g, int b, float hsb[3] );

double crop1( double a );

int sign( double a );


//"safe" sign (returns positive for 0)
int ssign (double a);

double absolute( double a );

double point_distance( double x1, double y1, double x2, double y2 );
double vector_length( float x, float y );

double sqdist( float x, float y );

double point_direction( double x1, double y1, double x2, double y2 );
double vector_direction( double x1, double y1 );

#endif
