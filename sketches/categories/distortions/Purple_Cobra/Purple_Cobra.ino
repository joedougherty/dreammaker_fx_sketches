
/******************************************************************************
 * DreamMaker FX / www.dreammakerfx.com
 *****************************************************************************/
/*
Effect name: Purple Cobra

Effect description:  Do not taunt the purple cobra.  This is a tightly compressed
distortion that runs through a fuzz destructor to create tight notes with lots
of colorful harmonics.  This effect also uses a pitch shifter.  Stepping on the 
right footswitch mixes in the lower octave

Left pot label: Tone
Left pot function: Tone of distortion

Center pot label: Drive
Center pot function: Amount of input drive going into destructor

Right pot label: Gain
Right pot function: Sets output gain of distortion

Left footswitch label: Octave down
Left footswitch function: While pressed, mixes in a dropped octave

Right footswitch label: Bypass
Right footswitch function: Bypasses the effect

Youtube Url: https://youtu.be/e9hNA7tGwJw
Soundcloud Url:

Created by: DreamMaker
DreamMakerFx package version: 1.4.2
Sketch version: 1.0
*/ 
#include <dm_fx.h>

fx_compressor       compressor(-30,       // Threshold in dB
                                40,       // Ratio
                                20,       // Attack time (ms)
                                1000,     // Release time (ms)
                                1.0);     // Output gain

fx_destructor       destruct(0.1,           // Clipping level (from 0 to 1.0)
                             4.0,           // Input drive 
                             SMOOTH_FUZZ);  // Distortion function = fuzz                                                                

fx_biquad_filter    tone_filter(800.0,              // Initial filter cutoff freq
                                FILTER_WIDTH_MEDIUM,// Filter width
                                BIQUAD_TYPE_BPF);   // Filter type is bandpass

fx_phase_shifter    gentle_phase(0.5,       // Rate in Hertz
                                 0.3,       // Depth (0.0 to 1.0)
                                 -0.3);     // Feedback (-1.0 to 1.0)

fx_pitch_shift      pitch_down(0.50);       // Pitch shift down by one octave

fx_gain             oct_gain(0.0);   // Gain control used to switch on/off octave
fx_gain             dist_gain(1.5);
fx_gain             pedal_gain(2.0);

fx_mixer_2          mix2;                   // Mixer utility

void setup() {
  // put your setup code here, to run once:
  pedal.init();

  pedal.route_audio(pedal.instr_in, compressor.input);
  pedal.route_audio(compressor.output, destruct.input);
  pedal.route_audio(destruct.output, tone_filter.input);
  pedal.route_audio(tone_filter.output, gentle_phase.input);
  pedal.route_audio(gentle_phase.output, dist_gain.input);
  pedal.route_audio(dist_gain.output, mix2.input_1);

  pedal.route_audio(compressor.output, pitch_down.input);
  pedal.route_audio(pitch_down.output, oct_gain.input);
  pedal.route_audio(oct_gain.output, mix2.input_2);

  pedal.route_audio(mix2.output,  pedal_gain.input);
  pedal.route_audio(pedal_gain.output, pedal.amp_out);

  pedal.add_bypass_button(FOOTSWITCH_LEFT);
  
  pedal.run();
}

void loop() {
  // put your main code here, to run repeatedly:

  if (pedal.pot_left.has_changed()) {
    tone_filter.set_freq(200 + pedal.pot_left.val*1000.0);
  }

  if (pedal.pot_center.has_changed()) {
    destruct.set_param_2(2 + pedal.pot_center.val*16.0);
  }

  if (pedal.pot_2.has_changed()) {
    gentle_phase.set_depth(pedal.pot_2.val);
  }  

 if (pedal.button_pressed(FOOTSWITCH_RIGHT, true)) {
    oct_gain.set_gain(1.0);
  }
  if (pedal.button_released(FOOTSWITCH_RIGHT, true)) {
    oct_gain.set_gain(0.0);
  }
   // Run pedal service to take care of stuff
  pedal.service();

}
