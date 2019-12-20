/*
 * File: looper.cpp
 *
 * Kaossilator-style loop recorder for Delay FX and Reverb FX
 * 
 * 2019 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#include "looper.hpp"

static dsp::DelayLine s_delay;

static __sdram float s_delay_ram[BUF_SIZE * 2];

static int s_beats;
static int s_mix_mode;
static int s_record_format;

FX_INIT
{
  s_delay.setMemory(s_delay_ram, BUF_SIZE * 2);
  s_beats = BEAT_STEPS - 1;
  s_mix_mode = mix_mode_play;
  s_record_format = record_format_stereo;
}

FX_PROCESS
{
  float * __restrict x = xn;
  const float * x_e = x + 2 * frames;

// adjust length to the buffer limit
  int beats = s_beats;
  float val = (k_samplerate * 60 / BEAT_MIN) / fx_get_bpmf();
  float len;
  do {
    len = val * (1 << (beats--));
  } while (len > BUF_SIZE);

  float valf, valf2;
  switch (s_record_format) {
  case record_format_stereo:
    len *= 2;
    for (; x < x_e;) {
      valf2 = valf = s_delay.read(len);
      if (s_mix_mode == mix_mode_overwrite)
        valf2 = *x;
      else if (s_mix_mode == mix_mode_overdub)
        valf2 += *x;
      s_delay.write(valf2);
      *(x++) = (*x + valf) * .5f;
    }
    break;
  case record_format_stereo_packed:
    for (; x < x_e;) {
      valf2 = valf = s_delay.read(len);
      if (s_mix_mode == mix_mode_overwrite)
        valf2 = f32pack(*x, x[1]);
      else if (s_mix_mode == mix_mode_overdub)
        valf2 = f32pack(*x + f32unpack1(valf), x[1] + f32unpack2(valf));
      s_delay.write(valf2);
      *(x++) = (*x + f32unpack1(valf)) * .5f;
      *(x++) = (*x + f32unpack2(valf)) * .5f;
    }
    break;
  case record_format_mono_packed: //ToDo

  case record_format_mono:
    for (; x < x_e;) {
      valf2 = valf = s_delay.read(len);
      if (s_mix_mode == mix_mode_overwrite)
        valf2 = (*x + x[1]) * .5f;
      else if (s_mix_mode == mix_mode_overdub)
        valf2 += (*x + x[1]) * .5f;
      s_delay.write(valf2);
      *(x++) = (*x + valf) * .5f;
      *(x++) = (*x + valf) * .5f;
    }
    break;
  default:
    break;
  }
}

FX_PARAM
{
  const float valf = q31_to_f32(value);
  switch (index) {
  case FX_PARAM_TIME: // beats length factor
    s_beats = clipmaxf(si_floorf(valf * BEAT_STEPS), BEAT_STEPS - 1);
    break;
  case FX_PARAM_DEPTH: // record mode
    s_mix_mode = clipmaxf(si_floorf(valf * mix_mode_count), mix_mode_count - 1);
    break;
  case FX_PARAM_SHIFT_DEPTH: // record format
    s_record_format = clipmaxf(si_floorf(valf * record_format_count), record_format_count - 1);
    break;
  default:
    break;
  }
}
