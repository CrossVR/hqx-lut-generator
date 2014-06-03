/*
 * Copyright (C) 2003 Maxim Stepin ( maxst@hiend3d.com )
 *
 * Copyright (C) 2010 Cameron Zemek ( grom@zeminvaders.net )
 * Copyright (C) 2011 Francois Gannaz <mytskine@gmail.com>
 * Copyright (C) 2014 Jules Blok ( jules@aerix.nl )
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <stdlib.h>
#include <stdint.h>
#include "lut_generator.h"
#include "hqx/src/common.h"

inline uint32_t rgb_to_yuv(uint32_t c)
{
    // Technically it's YUV for black
    return 0;
}

/* Don't test anything */
inline int yuv_diff(uint32_t yuv1, uint32_t yuv2)
{
    return 0;
}

inline uint32_t vecToInt(uint8_t vec[4])
{
    uint32_t res = vec[0];
    res |= vec[1] << 8;
    res |= vec[2] << 16;
    res |= vec[3] << 24;
    return res;
}

#define InDiff(c1, c2) ((c1 == x && c2 == y) || (c2 == x && c1 == y))

inline int Diff(uint32_t x, uint32_t y)
{
    int ret = 0;

    ret |= g_cross & InDiff(4, 2);
    ret |= g_cross & InDiff(2, 6) << 1;
    ret |= g_cross & InDiff(8, 4) << 2;
    ret |= g_cross & InDiff(6, 8) << 3;

    return ret;
}

/* Interpolate functions */
inline uint32_t Interpolate_2(uint32_t c1, int w1, uint32_t c2, int w2, int s)
{
    uint8_t vec[4] = { 0, 0, 0, 0 };

    int i1 = g_map[s][c1], i2 = g_map[s][c2];

    vec[i1] = w1;
    vec[i2] = w2;

    return vecToInt(vec);
}

inline uint32_t Interpolate_3(uint32_t c1, int w1, uint32_t c2, int w2, uint32_t c3, int w3, int s)
{
    uint8_t vec[4] = { 0, 0, 0, 0 };

    int i1 = g_map[s][c1], i2 = g_map[s][c2], i3 = g_map[s][c3];

    vec[i1] = w1;
    vec[i2] = w2;
    vec[i3] = w3;

    return vecToInt(vec);
}

inline uint32_t* trans(uint32_t* pc)
{
    return g_dp + g_cross * g_dpL * g_dpR + ((int)pc / sizeof(pc)) * g_dpL + g_pattern;
}

inline void Interp0(uint32_t * pc, uint32_t c1)
{
    *(trans(pc)) = 1;
}

inline void Interp1(uint32_t * pc, uint32_t c1, uint32_t c2)
{
    //*(trans(pc)) = (c1*3+c2) >> 2;
    *(trans(pc)) = Interpolate_2(c1, 3, c2, 1, ((int)pc / sizeof(pc)));
}

inline void Interp2(uint32_t * pc, uint32_t c1, uint32_t c2, uint32_t c3)
{
    //*(trans(pc)) = (c1*2+c2+c3) >> 2;
    *(trans(pc)) = Interpolate_3(c1, 2, c2, 1, c3, 1, ((int)pc / sizeof(pc)));
}

inline void Interp3(uint32_t * pc, uint32_t c1, uint32_t c2)
{
    //*(trans(pc)) = (c1*7+c2)/8;
    *(trans(pc)) = Interpolate_2(c1, 7, c2, 1, ((int)pc / sizeof(pc)));
}

inline void Interp4(uint32_t * pc, uint32_t c1, uint32_t c2, uint32_t c3)
{
    //*(trans(pc)) = (c1*2+(c2+c3)*7)/16;
    *(trans(pc)) = Interpolate_3(c1, 2, c2, 7, c3, 7, ((int)pc / sizeof(pc)));
}

inline void Interp5(uint32_t * pc, uint32_t c1, uint32_t c2)
{
    //*(trans(pc)) = (c1+c2) >> 1;
    *(trans(pc)) = Interpolate_2(c1, 1, c2, 1, ((int)pc / sizeof(pc)));
}

inline void Interp6(uint32_t * pc, uint32_t c1, uint32_t c2, uint32_t c3)
{
    //*(trans(pc)) = (c1*5+c2*2+c3)/8;
    *(trans(pc)) = Interpolate_3(c1, 5, c2, 2, c3, 1, ((int)pc / sizeof(pc)));
}

inline void Interp7(uint32_t * pc, uint32_t c1, uint32_t c2, uint32_t c3)
{
    //*(trans(pc)) = (c1*6+c2+c3)/8;
    *(trans(pc)) = Interpolate_3(c1, 6, c2, 1, c3, 1, ((int)pc / sizeof(pc)));
}

inline void Interp8(uint32_t * pc, uint32_t c1, uint32_t c2)
{
    //*(trans(pc)) = (c1*5+c2*3)/8;
    *(trans(pc)) = Interpolate_2(c1, 5, c2, 3, ((int)pc / sizeof(pc)));
}

inline void Interp9(uint32_t * pc, uint32_t c1, uint32_t c2, uint32_t c3)
{
    //*(trans(pc)) = (c1*2+(c2+c3)*3)/8;
    *(trans(pc)) = Interpolate_3(c1, 2, c2, 3, c3, 3, ((int)pc / sizeof(pc)));
}

inline void Interp10(uint32_t * pc, uint32_t c1, uint32_t c2, uint32_t c3)
{
    //*(trans(pc)) = (c1*14+c2+c3)/16;
    *(trans(pc)) = Interpolate_3(c1, 14, c2, 1, c3, 1, ((int)pc / sizeof(pc)));
}
