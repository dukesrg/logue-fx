/*
 * File: looper.cpp
 *
 * Kaossilator-style loop recorder for revfx
 * 
 * 2019 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#include "fx_api.h"
#include "osc_api.h"
#include "userrevfx.h"

#include "delayline.hpp"

#define MIN_BPM 56.f
//#define MIN_BEATS 1.f/16
#define MAX_BEATS 4.f
//Min BPM for 16 beats stereo loop: sample_rate * sizeof(float) * 2_channels * 16_beats * 60_seconds / 2432K = 148.026 BPM
//or 74.01 for 8 beats
//or 37.01 for 4 beats
//Really, KORG? :(
#define bufsize (size_t)(k_samplerate * 60 * MAX_BEATS / MIN_BPM)

static dsp::DelayLine s_delay;

static __sdram float s_delay_ram[bufsize * 2];

static float s_len;
static bool s_record;

void REVFX_INIT(uint32_t platform, uint32_t api)
{
  s_delay.setMemory(s_delay_ram, bufsize * 2);
  s_len = bufsize;
  s_record = false;
}

void REVFX_PROCESS(float *xn, uint32_t frames)
{
  float * __restrict x = xn;
  const float * x_e = x + 2 * frames;
  for (; x != x_e; x++) {
    *x = (*x + s_delay.read(s_len)) * .5f;
    if (s_record) {
      s_delay.write(*x);
    }
  }
}

void REVFX_PARAM(uint8_t index, int32_t value)
{
  const float valf = q31_to_f32(value);
  switch (index) {
  case k_user_revfx_param_time: // time stick to BPM from 1/16 to MAX_BEATS
    s_len = k_samplerate * 60.f * (1 >> (int)(valf * 7.f)) / (fx_get_bpmf() * 4);
    break;
  case k_user_revfx_param_depth: // mix or record
    s_record = valf > 0.5f;
    break;
  case k_user_revfx_param_shift_depth:
    break;
  default:
    break;
  }
}

