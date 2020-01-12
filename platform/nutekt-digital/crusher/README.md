# Crusher

This is a bit crusher and decimator effect for Modulation FX, Delay FX and Reverb FX.

## Parameters
* Sampling frequency (time / A knob) - 48000...480Hz sample rate reduce
* Bit depth (depth / B knob) - 24...1 bit resolution reduce
* Dry/wet mix (shift depth / FX button + B knob)

## Limitations
Dry/wet parameter is not available for Modulation FX.
On the Korg NTS-1 the analog input noise got aplificated by very low bit resolution setting which produces very nasty effect.
To avoid this either:
* make sure actual signal is present on audio input, not silence
* route audio input after the effect
