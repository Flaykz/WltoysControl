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
uint8_t gear = 1;
uint8_t steering_step = 2;
uint8_t throttle_step = 2;
uint8_t gear_step = 26;
uint8_t max_gear = 127 / gear_step;
uint8_t steering_trim = 124;
uint8_t state[3] = {0, 0};

void get_next_steer() {
  if (state[1] < 128) {
    state[1] = (state[1] + steering_step) < 128 ? state[1] + steering_step : 127;
  } else {
    state[1] = (state[1] + steering_step) < 256 ? state[1] + steering_step : 255;
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
      state[0] = (gear_step * gear) > 127 ? 127 : gear_step * gear;
    }
    else if (data == 's')
    {
      state[0] = (128 + (gear_step * gear)) > 255 ? 255 : 128 + (gear_step * gear);
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
    }
    else if (data == 'g')
    {
      state[0] = 0;
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
    else if (data == 'j')
    {
      if (steering_trim >= 10)
      {
        steering_trim -= 2;
      }
    }
    else if (data == 'l')
    {
      if (steering_trim <= 245)
      {
        steering_trim += 2;
      }
    }
  }

  if (do_steer)
  {
    get_next_steer();
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