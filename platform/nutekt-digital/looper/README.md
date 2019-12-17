## Loop recorder for revfx

This is a Kaossilator-style stereo loop recorded for revfx slot.

Parameters:
* time (A knob) : loop length linked to current BPM and sticked to 1/16, 1/8, 1/4, 1/2, 1, 2, 4, 8, 16 beats. Length reduced dynamically depending on BPM change due to available SDRAM size limit.
* depth (B knob) :
  * 0...33 : overwrite - 50/50 mixed input and delay buffer passed to the output, input rewrites the delay buffer.
  * 34...66 : play - 50/50 mixed input and delay buffer passed to the output, delay buffer untouched.
  * 67...100 : overdub - 50/50 mixed input and delay buffer passed to the output, input added to the delay buffer.
