#include <Arduino.h>
#include <SPI.h>
#include <v202.h>

#define CE 4
#define CSN 5

const char *ssid = "Wout";
const char *password = "made in china";

nRF24 wireless;
v202 protocol;

uint8_t txid[3] = {0x00, 0x00, 0x00};

char steer_key = 0;
char data = 0;
char prev_data = 0;
bool do_steer = false;
bool do_throttle = false;
uint8_t gear = 1;
uint8_t steering_step = 2;
uint8_t throttle_step = 2;
uint8_t gear_step = 26;
uint8_t max_gear = 127 / gear_step;
uint8_t max_throttle = gear * gear_step;
uint8_t steering_trim = 80;
uint8_t state[3] = {0, 0};
uint8_t throttle_history = 0;
unsigned long prev_steering_time = 0;
unsigned long prev_throttle_time = 0;
unsigned long steering_time = 200;
unsigned long time_between_steering_step = steering_time / (127 / steering_step);
unsigned long throttle_time = 1000;
unsigned long time_between_throttle_step = throttle_time / (127 / steering_step);

void get_next_steer() {
  if (state[1] < 128) {
    state[1] = (state[1] + steering_step) < 128 ? state[1] + steering_step : 127;
  } else {
    state[1] = (state[1] + steering_step) < 256 ? state[1] + steering_step : 255;
  }
}

void get_next_throttle()
{
  if (state[0] < 128)
  {
    if (do_throttle) {
      max_throttle = gear * gear_step;
      state[0] = (state[0] + throttle_step) < max_throttle ? state[0] + throttle_step : max_throttle;
      throttle_history = state[0];
    } else {
      if (throttle_history >= throttle_step) {
        throttle_history -= throttle_step;
      }
    }
  }
  else
  {
    if (do_throttle) {
      max_throttle = (gear * gear_step) + 128;
      state[1] = (state[0] + throttle_step) < max_throttle ? state[0] + throttle_step : max_throttle;
    }
  }
}

void setup()
{
  Serial.begin(115200);
  wireless.setPins(CE, CSN);
  protocol.setTXId(txid);
  protocol.init(&wireless, TX);
  protocol.bind();
}

void loop()
{
  if (Serial.available() > 0)
  {
    data = 0;
    data = Serial.read();
    if (data != 'i' && data != 'k' && data != 'j' && data != 'l')
    {
      if (data != prev_data)
      {
        prev_data = data;
      }
    }
    if (data == 0 || data == steer_key)
    {
      return;
    }
    if (data == 'z')
    {
      state[0] = throttle_history;
      do_throttle = true;
    }
    else if (data == 's')
    {
      state[0] = 128;
      do_throttle = true;
    }
    else if (data == 'd')
    {
      steer_key = data;
      state[1] = 0;
      do_steer = true;
    }
    else if (data == 'q')
    {
      steer_key = data;
      state[1] = 128;
      do_steer = true;
    }
    else if (data == 't')
    {
      state[0] = 0;
      do_throttle = false;
    }
    else if (data == 'g')
    {
      state[0] = 0;
      do_throttle = false;
    }
    else if (data == 'h')
    {
      if (steer_key == 'd')
      {
        do_steer = false;
        steer_key = 0;
        state[1] = 0;
      }
    }
    else if (data == 'f')
    {
      if (steer_key == 'q')
      {
        do_steer = false;
        steer_key = 0;
        state[1] = 0;
      }
    }
    else if (data == 'i')
    {
      if (gear < max_gear)
      {
        gear += 1;
        if (state[0] != 0)
        {
          state[0] += gear_step;
        }
      }
    }
    else if (data == 'k')
    {
      if (gear > 1)
      {
        gear -= 1;
        if (state[0] != 0)
        {
          state[0] -= gear_step;
        }
      }
    }
    else if (data == 'l')
    {
      if (steering_trim >= 10)
      {
        steering_trim -= 2;
      }
    }
    else if (data == 'j')
    {
      if (steering_trim <= 245)
      {
        steering_trim += 2;
      }
    }
  }

  if (do_steer && ((millis() - prev_steering_time) >= time_between_steering_step))
  {
    prev_steering_time = millis();
    get_next_steer();
  }
  if ((millis() - prev_throttle_time) >= time_between_throttle_step)
  {
    prev_throttle_time = millis();
    get_next_throttle();
  }
  protocol.process(state[1], state[0], steering_trim);

  // Serial.print(time);
  // Serial.print(": ");
  // Serial.print(state[0]);
  // Serial.print(",");
  // Serial.print(state[1]);
  // Serial.print(",");
  // Serial.print(steering_trim);
  // Serial.print(",");
  // Serial.println(gear);
  delay(4);
}