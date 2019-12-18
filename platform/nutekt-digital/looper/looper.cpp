/*
 * File: looper.cpp
 *
 * Kaossilator-style loop recorder for revfx
 * 
 * 2019 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#include "looper.hpp"

void REVFX_INIT(uint32_t platform, uint32_t api)
{
  looper_init(platform, api);
}

void REVFX_PROCESS(float *xn, uint32_t frames)
{
  looper_process(xn, frames);
}

void REVFX_PARAM(uint8_t index, int32_t value)
{
  looper_param(index, value);
}
