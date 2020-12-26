#include <Arduino.h>
#include <v202.h>

#define CE 4
#define CSN 5

#define BAUD_RATE 115200

#define MAX_VALUE 127 // 0-127 and 128-255 on each canal
#define RESOLUTION (MAX_VALUE + 1)
#define MIN_THROTTLE_TIME 800 // min time in ms to get full throttle
#define MAX_STEERING_TIME 200 // time to get full steering
#define GEAR_STEP 9 // resolution of gear, number max of gear depend on this value (with 1, you get 127 gear)
#define MAX_GEAR (MAX_VALUE / GEAR_STEP)
#define DEFAULT_STEERING_TRIM 80
#define MIN_STEERING_TRIM 10
#define MAX_STEERING_TRIM 245
#define TRIM_STEP 2
#define STEERING_STEP 2 // how much steering getting higher over time
#define THROTTLE_STEP 2 // how much throttle getting higher over time
#define THROTTLE_TIME_STEP 100 // step to configure the full throttle time

#define IS_DEBUG true

nRF24 wireless;
v202 protocol;

uint8_t txId[3] = {0x00, 0x00, 0x00};

uint8_t steer_key = 0;
uint8_t data = 0;
uint8_t prev_data = 0;

uint8_t state[3] = {0, 0}; // {throttle_value, steer_value}

bool do_steer = false; // indicate when car is actually steering
bool do_throttle = false; // indicate when car is actually accelerating

uint8_t actual_gear = 6; // default gear

uint8_t steering_trim = DEFAULT_STEERING_TRIM; // default steering trim (without input, if car go right or left)

uint32_t prev_steering_time = 0;
uint32_t time_between_steering_step = MAX_STEERING_TIME / (MAX_VALUE / STEERING_STEP);

uint32_t throttle_time = MIN_THROTTLE_TIME; // time to get full throttle
uint32_t prev_throttle_time = 0;
uint32_t time_between_throttle_step = throttle_time / (MAX_VALUE / STEERING_STEP);

uint8_t throttle_memory = 0; // allow to interrupt throttle and get back without starting acceleration from 0

void get_next_steer() {
    if (state[1] < RESOLUTION) {
        state[1] = (state[1] + STEERING_STEP) < RESOLUTION ? state[1] + STEERING_STEP : MAX_VALUE;
    } else {
        state[1] = (state[1] + STEERING_STEP) < (RESOLUTION * 2) ? state[1] + STEERING_STEP : (MAX_VALUE*2) + 1;
    }
}

void get_next_throttle() {
    if (do_throttle) {
        if (state[0] < RESOLUTION) {
            uint8_t max_throttle = actual_gear * GEAR_STEP;
            state[0] = (state[0] + THROTTLE_STEP) < max_throttle ? state[0] + THROTTLE_STEP : max_throttle;
            throttle_memory = state[0];
        } else {
            uint8_t max_throttle = (actual_gear * GEAR_STEP) + RESOLUTION;
            state[0] = (state[0] + THROTTLE_STEP) < max_throttle ? state[0] + THROTTLE_STEP : max_throttle;
        }
    } else {
        if (throttle_memory >= THROTTLE_STEP) {
            throttle_memory -= THROTTLE_STEP;
        }
    }
}

void setup() {
    Serial.begin(BAUD_RATE);
    wireless.setPins(CE, CSN);
    protocol.setTXId(txId);
    protocol.init(&wireless, TX_MODE);
    protocol.bind();
}

void loop() {
    if (Serial.available() > 0) {
        data = 0;
        data = Serial.read();
        if (data != 'i' && data != 'k' && data != 'j' && data != 'l') {
            if (data != prev_data) {
                prev_data = data;
            }
        }
        if (data == 0 || data == steer_key) {
            return;
        }
        if (data == 'z') {
            state[0] = throttle_memory;
            do_throttle = true;
        } else if (data == 's') {
            state[0] = MAX_VALUE + 1;
            do_throttle = true;
        } else if (data == 'd') {
            steer_key = data;
            state[1] = 0;
            do_steer = true;
        } else if (data == 'q') {
            steer_key = data;
            state[1] = MAX_VALUE + 1;
            do_steer = true;
        } else if (data == 't') {
            state[0] = 0;
            do_throttle = false;
        } else if (data == 'g') {
            state[0] = 0;
            do_throttle = false;
        } else if (data == 'h') {
            if (steer_key == 'd') {
                do_steer = false;
                steer_key = 0;
                state[1] = 0;
            }
        } else if (data == 'f') {
            if (steer_key == 'q') {
                do_steer = false;
                steer_key = 0;
                state[1] = 0;
            }
        } else if (data == 'i') {
            if (actual_gear < MAX_GEAR) {
                actual_gear += 1;
                if (state[0] != 0) {
                    state[0] += GEAR_STEP;
                }
            }
        } else if (data == 'k') {
            if (actual_gear > 1) {
                actual_gear -= 1;
                if (state[0] != 0) {
                    state[0] -= GEAR_STEP;
                }
            }
        } else if (data == 'a') {
            throttle_time -= THROTTLE_TIME_STEP;
            if (throttle_time < MIN_THROTTLE_TIME)
                throttle_time = MIN_THROTTLE_TIME;
            time_between_throttle_step = throttle_time / (MAX_VALUE / STEERING_STEP);
        } else if (data == 'e') {
            throttle_time += THROTTLE_TIME_STEP;
            time_between_throttle_step = throttle_time / (MAX_VALUE / STEERING_STEP);
        } else if (data == 'l') {
            if (steering_trim >= MIN_STEERING_TRIM) {
                steering_trim -= TRIM_STEP;
            }
        } else if (data == 'j') {
            if (steering_trim <= MAX_STEERING_TRIM) {
                steering_trim += TRIM_STEP;
            }
        }
    }

    if (do_steer && ((millis() - prev_steering_time) >= time_between_steering_step)) {
        prev_steering_time = millis();
        get_next_steer();
    }
    if ((millis() - prev_throttle_time) >= time_between_throttle_step) {
        prev_throttle_time = millis();
        get_next_throttle();
    }
    protocol.process(state[1], state[0], steering_trim);

    if (IS_DEBUG) {
        Serial.print(state[0]);
        Serial.print(",");
        Serial.print(state[1]);
        Serial.print(",");
        Serial.print(steering_trim);
        Serial.print(",");
        Serial.print(actual_gear);
        Serial.print(",");
        Serial.println(throttle_time);
    }
    delay(4);
}