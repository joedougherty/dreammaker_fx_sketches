/******************************************************************************
 * DreamMaker FX / www.dreammakerfx.com
 *****************************************************************************/
/*
Effect name: Delay Major Minor

Effect description: Volume swell effect that also has optional minor and major
third intervals that can be optionally mixed in when the left or right
foot swtich is held down.  The minor and major thirds are fed through their 
own delays.  Overall, this creates an interesting ambient effect.  It takes
a little practice using the minor and major third buttons but once you get
the hang of it, cool things can happen.

Left pot label: Feedback 
Left pot function: Basically how long the echo lasts before dying out

Center pot label: Attack time
Center pot function: Attack time for clean channel 

Right pot label: Delay time 
Right pot function: How far apart the echos are (0.1 to 3 seconds)

Left footswitch label: Minor 3rd 
Left footswitch function: When pressed, mixes in audio to minor third pitch shift that goes through a delay

Right footswitch label: Major 3rd
Right footswitch function: When pressed, mixes in audio to major third pitch shift that goes through a delay

Youtube Url:
Soundcloud Url:

Created by: DreamMaker
DreamMakerFx package version: 1.5.3
Sketch version: 1.0
*/    
#include <dreammakerfx.h>

// Add your fx module declarations here
fx_delay    delay_maj (500.0,  // 1 second initial
                       2000.0,  // 2 seconds total / max                     
                       0.7,      // 0.6 feedback ratio
                       0.0,      // mix dry
                       0.8,      // mix wet
                       false);
fx_delay    delay_min (500.0,  // 1 second initial
                       2000.0,  // 2 seconds total / max                     
                       0.7,      // 0.6 feedback ratio
                       0.0,      // mix dry
                       0.8,      // mix wet
                       false);                       

fx_pitch_shift       pitch_maj(1.259920786), pitch_min(1.189210644);


fx_adsr_envelope  env_pitch(125.0,  // Attack is 250ms
                      10.0,   // Decay is 10ms
                      10.0,   // Sustain is 10ms
                      125.0,  // Release is 500ms 
                      1.0,    // Sustain ratio
                      1.0);   // Full volume

fx_adsr_envelope  env(250.0,  // Attack is 250ms
                      10.0,   // Decay is 10ms
                      10.0,   // Sustain is 10ms
                      2000.0,  // Release is 500ms 
                      1.0,    // Sustain ratio
                      1.0);   // Full volume

fx_gain             gain_maj(0.0), gain_min(0.0), gain_swell(1.8);

fx_mixer_3          mix3;
                     
void setup() {
  // put your setup code here, to run once:

  // Initialize the pedal!
  pedal.init();

  // Route audio from instrument -> my_delay -> amp
  pedal.route_audio(pedal.instr_in, env_pitch.input);

  pedal.route_audio(env_pitch.output, gain_maj.input);
  pedal.route_audio(gain_maj.output, pitch_maj.input);
  pedal.route_audio(pitch_maj.output, delay_maj.input);
  pedal.route_audio(delay_maj.output, mix3.input_1);

  pedal.route_audio(env_pitch.output, gain_min.input);
  pedal.route_audio(gain_min.output, pitch_min.input);
  pedal.route_audio(pitch_min.output, delay_min.input);
  pedal.route_audio(delay_min.output, mix3.input_2);

  pedal.route_audio(pedal.instr_in, env.input);
  pedal.route_audio(env.output, gain_swell.input);
  pedal.route_audio(gain_swell.output, mix3.input_3);
  pedal.route_audio(mix3.output, pedal.amp_out);

  // Route new note event from pedal to kick of ADSR envelope
  pedal.route_control(pedal.new_note, env.start);
  pedal.route_control(pedal.new_note, env_pitch.start);

  pedal.run();
}

void loop() {


 if (pedal.button_pressed(FOOTSWITCH_RIGHT, true)) {
    gain_maj.set_gain(1.0);
  }
  if (pedal.button_released(FOOTSWITCH_RIGHT, true)) {
    gain_maj.set_gain(0.0);
  }

 if (pedal.button_pressed(FOOTSWITCH_LEFT, true)) {
    gain_min.set_gain(1.0);
  }
  if (pedal.button_released(FOOTSWITCH_LEFT, true)) {
    gain_min.set_gain(0.0);
  }

  // Left pot changes the feedback of the delay (determining how long the echoes last)
  if (pedal.pot_left.has_changed()) {
    delay_maj.set_feedback(pedal.pot_left.val);
    delay_min.set_feedback(pedal.pot_left.val);
  }
  
  // Right pot changes the wet / dry mix
  if (pedal.pot_right.has_changed()) {
    delay_maj.set_length_ms(100 + pedal.pot_right.val_log_inv*2000);
    delay_min.set_length_ms(100 + pedal.pot_right.val_log_inv*2000);
  }  
  
  // Center pot can also be used to change the delay length 
  // from 100ms to 3000ms
  if (pedal.pot_center.has_changed()) {
    env.set_attack_ms(50.0 * pedal.pot_center.val * 200.0);
  }    
  
  // Service pedal
  pedal.service();
}
