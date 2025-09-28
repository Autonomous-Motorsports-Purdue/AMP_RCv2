#include "App/Inc/app_statemachine.h"
#include "PinNames.h"
#include "SPI.h"
#include "U8g2lib.h"
#include "WSerial.h"
#include "stm32f303x8.h"
#include "variant_NUCLEO_F303K8.h"
#include <Arduino.h>
#include <HardwareTimer.h>
#include <cstdint>

bool can_tick_state_machine = false;

void setup() {
  Serial.begin(115200);
  App_StateMachine_Init();
  HardwareTimer *statemachine_timer = new HardwareTimer(TIM2);
  statemachine_timer->setOverflow(10, HERTZ_FORMAT);
  statemachine_timer->attachInterrupt([]() { can_tick_state_machine = true; });
  statemachine_timer->resume();
}

void loop() {
  if (can_tick_state_machine) {
    App_StateMachine_Tick();
    can_tick_state_machine = false;
  }
  // Serial.println("TEST");
}
