/*
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
#include <stdio.h>
#include "hqx/src/hqx.h"
#include "lut_generator.h"

const static uint32_t w[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

uint32_t* g_dp;
int g_dpL;
int g_pattern;
int g_cross;

#define LUT_ENTRIES_X 256
#define LUT_ENTRIES_Y 16

void generate_lut(char* fn, int scale)
{
    int width = LUT_ENTRIES_X;
    int height = LUT_ENTRIES_Y * (scale * scale);

    g_dp = (uint32_t*)calloc(width * height, sizeof(uint32_t));
    g_dpL = width;

    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 256; j++)
        {
            g_pattern = j;
            g_cross = i;

            switch (scale)
            {
                case 2:
                    hq2x_32_lut(g_pattern, 0, 2, w);
                    break;
                default:
                    goto fail;
                    break;
            }
        }
    }

    FILE *f = fopen(fn, "wb");
    fwrite(g_dp, sizeof(uint32_t), width * height, f);
    fclose(f);

fail:
    free(g_dp);
}

int main(int argc, char argv[])
{
    generate_lut("hq2x.lut", 2);
    return 0;
}
