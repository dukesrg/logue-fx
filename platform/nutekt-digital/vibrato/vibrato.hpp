/*
 * File: vibrato.hpp
 *
 * Simple vibrato with pitch shifter for Delay FX and Reverb FX
 * 
 * 2019 (c) Oleg Burdaev
 * mailto: dukesrg@gmail.com
 *
 */

#pragma once

#include "buffer_ops.h"
#include "fixed_math.h"
#include "float_math.h"
#include "fx_api.h"
#include "osc_api.h"

#define k_user_module_modfx_val 1
#define k_user_module_delfx_val 2
#define k_user_module_revfx_val 3
#define k_user_module_osc_val 4

#define VAL_(a) a##_val
#define VAL(a) VAL_(a)

#if VAL(USER_TARGET_MODULE) == k_user_module_delfx_val
  #pragma message "Compiling Delay FX"
  #include "userdelfx.h"
  #define FX_INIT void DELFX_INIT(__attribute__((unused)) uint32_t platform, __attribute__((unused)) uint32_t api)
  #define FX_PROCESS void DELFX_PROCESS(float *xn, uint32_t frames)
  #define FX_PARAM void DELFX_PARAM(uint8_t index, int32_t value)
  #define FX_PARAM_TIME k_user_delfx_param_time
  #define FX_PARAM_DEPTH k_user_delfx_param_depth
  #define FX_PARAM_SHIFT_DEPTH k_user_delfx_param_shift_depth
#elif VAL(USER_TARGET_MODULE) == k_user_module_revfx_val
  #pragma message "Compiling Reverb FX"
  #include "userrevfx.h"
  #define FX_INIT void REVFX_INIT(__attribute__((unused)) uint32_t platform, __attribute__((unused)) uint32_t api)
  #define FX_PROCESS void REVFX_PROCESS(float *xn, uint32_t frames)
  #define FX_PARAM void REVFX_PARAM(uint8_t index, int32_t value)
  #define FX_PARAM_TIME k_user_revfx_param_time
  #define FX_PARAM_DEPTH k_user_revfx_param_depth
  #define FX_PARAM_SHIFT_DEPTH k_user_revfx_param_shift_depth
#else
  #error "Wrong module"
#endif
