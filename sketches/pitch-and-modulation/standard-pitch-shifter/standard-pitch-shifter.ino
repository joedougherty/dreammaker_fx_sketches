/******************************************************************************
 * DreamMaker FX / www.dreammakerfx.com
 *****************************************************************************/
/*
Effect name: Standard Pitch Shifter
Effect description:  Standard pitch shifter pedal than can pitch shift incoming
audio by an arbitrary amount.

Left pot label: Dry mix
Left pot function: Gain of dry / unprocessed signal

Center pot label: Shift amount
Center pot function: Various presets for shift amounts

Right pot label: Pitch shift mix
Right pot function: Gain of pitch shifted signal

Left footswitch label: Bypass
Left footswitch function: Bypasses the effect

Right footswitch label:
Right footswitch function: Not used

Youtube Url: 
Soundcloud Url: 

Created by: DreamMaker
DreamMakerFx package version: 1.5.3
Sketch version: 1.0
*/
#include <dreammakerfx.h>

fx_pitch_shift    pitch_shift(1.0);     

fx_mixer_2    mix2;
fx_gain       clean_mix(1.0), pitch_shift_mix(1.0);

void setup() {
  // put your setup code here, to run once:

  // Initialize the pedal!
  pedal.init();

  // Route audio from instrument -> pitch_shift -> amp
  pedal.route_audio(pedal.instr_in, pitch_shift.input);
  pedal.route_audio(pitch_shift.output, pitch_shift_mix.input);
  pedal.route_audio(pitch_shift_mix.output, mix2.input_1);
  
  pedal.route_audio(pedal.instr_in, clean_mix.input);
  pedal.route_audio(clean_mix.output, mix2.input_2);
  
  pedal.route_audio(mix2.output, pedal.amp_out);

  // left footswitch is bypass
  pedal.add_bypass_button(FOOTSWITCH_LEFT);

   // Run this effect
  pedal.run();
}

void loop() {

  // Left pot changes dry signal gain
  if (pedal.pot_left.has_changed()) {
    clean_mix.set_gain(pedal.pot_left.val);
  }
  
  // Center pot changes pitch shift amount
  if (pedal.pot_center.has_changed()) {
    if (pedal.pot_center.val < 0.15) {
       // full octave down
       pitch_shift.set_freq_shift(0.5);
    } else if (pedal.pot_center.val < 0.3) {
      // Fifth down
      pitch_shift.set_freq_shift(0.75);
    } else if (pedal.pot_center.val < 0.5) {
      // Third down
      pitch_shift.set_freq_shift(0.84);
    } else if (pedal.pot_center.val < 0.7) {
      // Third up
      pitch_shift.set_freq_shift(1.1891);
    } else if (pedal.pot_center.val < 0.9) {
      // Fifth up
      pitch_shift.set_freq_shift(1.5);
    } else {
      // Octave up
      pitch_shift.set_freq_shift(2.0);
    }
  }  
  
  // Right pot changes mix gain
  if (pedal.pot_center.has_changed()) {
    pitch_shift_mix.set_gain(pedal.pot_left.val);   
  }    
  
  // Service pedal
  pedal.service();
}
