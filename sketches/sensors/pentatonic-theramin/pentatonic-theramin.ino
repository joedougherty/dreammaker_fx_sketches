/******************************************************************************
 * DreamMaker FX / www.dreammakerfx.com
 *****************************************************************************/
/*
Effect name: Pentatonic Theramin 
Effect description:  Creates a Theramin using a Sparkfun distance sensor.
When the measured distance from the sensor is less than about 2.5 feet, the tones 
are mixed in.  And when there is nothing in front of the sensor, the tones turn off.
This sketch relies on a Sparkfun distance sensor that can be obtained 
here: https://www.sparkfun.com/products/14722

Left pot label: Phase shift depth
Left pot function: Depth of the phase shifter

Center pot label: Tone
Center pot function: Sets frequency of output filter

Right pot label: Delay time
Right pot function: Length of echoes

Left footswitch label: 
Left footswitch function: Not used

Right footswitch label:
Right footswitch function: Not used

Youtube Url: https://www.youtube.com/watch?v=Pbnx5vn5iOc
Soundcloud Url: 

Created by: DreamMaker
DreamMakerFx package version: 1.5.3
Sketch version: 1.0
*/
#include <dreammakerfx.h>


// These are the include files that comes wiht the distance sensor
#include <ComponentObject.h>
#include <RangeSensor.h>
#include <SparkFun_VL53L1X.h>
#include <vl53l1x_class.h>
#include <vl53l1_error_codes.h>

// Two oscillators 
fx_oscillator     therm1(OSC_TRI, 200.0, 0.5);
fx_oscillator     therm2(OSC_RAMP_POS, 200.0, 0.5);

// Delays / echoes for each sensor
fx_delay         therm1_delay(1000.0, 0.6);
fx_delay         therm2_delay(2000.0, 0.6);

// Tone filter 
fx_biquad_filter  filt_out(800, 1, BIQUAD_TYPE_LPF, BIQUAD_ORDER_6);

// Gain control to switch oscillators on and off when sensor detects something
fx_gain     therm1_gain(2.0), therm2_gain(2.0);   

// Mixer to add two oscillators together
fx_mixer_2   mix2;                             

// Phase shifter 
fx_phase_shifter out_phasey(0.5, 0.5, 0.5);

// Output limiter to prevent clipping if resonance is dialed up or delay feedback is 
fx_compressor out_limiter(-10.0, 1000.0, 10.0, 1000.0, 1.0);

// Instance for the distnace sensor (from Sparkfun's exanple code)
SFEVL53L1X distanceSensor;


// An array of note frequencies 
#define TOT_NOTES (60)
float notes[TOT_NOTES];

void setup() {

  // Generate note frequencies starting at a very low A
  int i=0;
  while (i<TOT_NOTES) {
    notes[i] = 55.0 * pow(2.0, (float) i * (1.0/12.0));
    i++;
  }

  // Initialize the sensors
  bool sensor_working = false;
  Wire.begin();
  if (distanceSensor.begin() == 0) //Begin returns 0 on a good init
  {
    sensor_working = true;
  }
  
  // Route audio through our pedal
  pedal.init();

  pedal.route_audio(therm1.output, therm1_gain.input);
  pedal.route_audio(therm2.output, therm2_gain.input);
  
  pedal.route_audio(therm1_gain.output, therm1_delay.input);
  pedal.route_audio(therm2_gain.output, therm2_delay.input);

  pedal.route_audio(therm1_delay.output, mix2.input_1);
  pedal.route_audio(therm2_delay.output, mix2.input_2);
  
  pedal.route_audio(mix2.output, filt_out.input);
  pedal.route_audio(filt_out.output, out_phasey.input);
  pedal.route_audio(out_phasey.output, out_limiter.input);
  pedal.route_audio(out_limiter.output, pedal.amp_out);

  pedal.run();

  // Let the the world know that the sensor is working
  if (sensor_working) {
    Serial.println("Sensor online!");
  }


}

int now = millis();
float distanceInches, distanceFeet;
int incs_pentatonic[]= {3,2,2,3,2};
int note_delta = 0;
int indx_quant = 0;
int l_indx = 2;
void loop() {
  
  // Service DreamMaker FX 
  pedal.service();
  
  // Take a reading from distance sensor every 100 ms.  If we do this too frequently,
  // the sensor stops responding so 50-100ms intervals is the right amount of time
  if (millis() > now + 100) {
    Serial.print("Sensor distance: ");
    now = millis();
    distanceSensor.startRanging();
    int distance = distanceSensor.getDistance(); 
    distanceSensor.stopRanging();
    distanceInches = distance * 0.0393701;
    distanceFeet = distanceInches / 12.0;    
    // Print measured distance - comment this out if you want
    Serial.println(distanceFeet);
  }

  
  float last_5[5];
  if (distanceFeet > 3.5) {
    // If there is nothing in close range of the sensors, turn lights off 
    // and turn volume to zero on oscillators
    therm1_gain.set_gain(0.0);
    therm2_gain.set_gain(0.0);
    digitalWrite(PIN_FOOTSW_LED_2, LOW); 
    digitalWrite(PIN_FOOTSW_LED_1, LOW); 
  } else {

    // Otherwise, if something is close, translate distance to frequency and 
    // find the closest pentatonic note (and harmony)
    float freq_raw = 110 + 110 * distanceFeet;
    float freq_quant;
    
    float dist = 1000.0;
    int i=0, j=0;
    while (i < TOT_NOTES) {
      float d = abs(freq_raw - notes[i]);
      bool update = false;
      if (d< dist) {
        dist = d;
        freq_quant = notes[i] * 0.5;
        indx_quant = i;
        update = true;
      }
      i+= incs_pentatonic[j];
      j++;
      if (j >= 5) {
        j = 0;
      }
      if (update) {
        last_5[4] = last_5[3];
        last_5[3] = last_5[2];
        last_5[2] = last_5[1];
        last_5[1] = last_5[0];
        last_5[0] = freq_quant;
      }

    }

    // Set oscillator gains and frequencies 
    therm1_gain.set_gain(0.5);
    therm2_gain.set_gain(0.25);
    therm2.set_frequency(last_5[l_indx]*2.0);
    therm1.set_frequency(freq_quant);

    // Turn on the footswitch LEDs
    digitalWrite(PIN_FOOTSW_LED_2, HIGH); 
    digitalWrite(PIN_FOOTSW_LED_1, HIGH); 
  }

  // Right pot controls the length of the echo
  if (pedal.pot_right.has_changed()) {
     therm1_delay.set_length_ms(pedal.pot_right.val * 1000.0);
     therm2_delay.set_length_ms(pedal.pot_right.val * 2000.0);
  }

  // Center pot controls low-pass filter
  if (pedal.pot_center.has_changed()) {
    filt_out.set_freq(200.0 + pedal.pot_center.val * 1000);
  }    

  // Right pot controls depth of phase shifter
  if (pedal.pot_left.has_changed()) {
    out_phasey.set_depth(pedal.pot_left.val);
  }     
}
