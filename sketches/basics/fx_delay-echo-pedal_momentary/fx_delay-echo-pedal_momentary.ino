/**
 * This is an implementation of a typical delay / echo pedal.   
 * 
 * Left pot: "feedback" - basically how long the echo lasts before dying out
 * Center pot: delay time - how far apart the echos are (0.1 to 3 seconds)
 * Right pot: wet/dry mix - the mix of clean audio and the echo effect (set to center for an even mix of effect and clean)
 * 
 * Left footswitch: bypass - turns on and off the effect
 * Right footswitch: tap - tap it a few times at a set interval and the delay will lock on
 * 
 * This effect uses a tiny amount of the available processing power and memory.
 * It's provided as an example of how to use the various features of the fx_delay block
 * 
 * 
 */
#include <dreammakerfx.h>

fx_delay    my_delay(1000.0, 3000.0, 0.7, 0.0, 1.0, false);

fx_gain     delay_mix(0.0);
fx_mixer_2  mix2;

void setup() {
  // put your setup code here, to run once:

  // Initialize the pedal!
  pedal.init();

  // Route audio from instrument -> my_delay -> amp
  pedal.route_audio(pedal.instr_in, mix2.input_1);
  pedal.route_audio(pedal.instr_in, delay_mix.input);
  pedal.route_audio(delay_mix.output, my_delay.input);
  pedal.route_audio(my_delay.output, mix2.input_2);
  pedal.route_audio(mix2.output, pedal.amp_out);

  // Right foot switch is tap delay length
  pedal.add_tap_interval_button(FOOTSWITCH_RIGHT, true);

  pedal.run();
}

void loop() {

  if (pedal.button_pressed(FOOTSWITCH_LEFT, true)) {
    delay_mix.set_gain(1.0);
  }
  if (pedal.button_released(FOOTSWITCH_LEFT, true)) {
    delay_mix.set_gain(0.0);
  }

  // If new delay time has been tapped in, use that
  if (pedal.new_tap_interval()) { 
    my_delay.set_length_ms(pedal.get_tap_interval_ms());
  } 

  // Left pot changes the feedback of the delay (determining how long the echoes last)
  if (pedal.pot_left.has_changed()) {
    my_delay.set_feedback(pedal.pot_left.val);
  }
  
  // Right pot changes the wet / dry mix
  if (pedal.pot_right.has_changed()) {
    my_delay.set_dry_mix(1.0 - pedal.pot_right.val);
    my_delay.set_wet_mix(pedal.pot_right.val);
  }  
  
  // Center pot can also be used to change the delay length 
  // from 100ms to 3000ms
  if (pedal.pot_center.has_changed()) {
    float new_length_ms = 100.0 + pedal.pot_center.val*2900.0;
    my_delay.set_length_ms(new_length_ms);
    pedal.set_tap_blink_rate_ms(new_length_ms);
  }    
  
  // Service pedal
  pedal.service();
}
