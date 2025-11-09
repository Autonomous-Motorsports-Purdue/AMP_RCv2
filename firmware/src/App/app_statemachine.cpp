#include "App/Inc/app_statemachine.h"
#include "App/Inc/controller_packet.h"
#include "WInterrupts.h"
#include "wiring_constants.h"
#include "wiring_digital.h"
#include <Arduino.h>
#include <HardwareTimer.h>
#include <LoRa.h>
#include <SPI.h>
#include <U8g2lib.h>
#include "App/Inc/oled_image.h"

// Pin definitions
#define OLED_NSS PA11 // CS
#define OLED_DC PB5   // DC
#define OLED_RST PB4  // Reset
#define BUTTON_EBRAKE PA3
#define BUTTON_SLOW PB6
#define BUTTON_FAST PF1
#define BUTTON_AUTO PB7

#define JOYSTICK_DEADZONE_X 150
#define JOYSTICK_DEADZONE_Y 150
#define JOYSTICK_CENTER_X 2047
#define JOYSTICK_CENTER_Y 2047

// Hardware SPI OLED object
U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, OLED_NSS, OLED_DC,
                                            OLED_RST);

// Pin definitions (adjust for your board)
#define LORA_CS_PIN PA10
#define LORA_RST_PIN PA9
#define LORA_DIO0_PIN PA12
#define THROTTLE_Y PA1
#define STEERING_X PA5

#define TICKS_PER_SEC 10
#define EBRAKE_MIN_SEC 5

#define LORA_START_BYTE 0xFE
#define LORA_STOP_BYTE 0xFF

#define SPEED_FWD_FAST 100
#define SPEED_REV_FAST -80
#define SPEED_FWD_SLOW 50
#define SPEED_REV_SLOW -30

#define BTN_EBRAKE 0b00000001
#define BTN_AUTO 0b00000010
#define BTN_SLOW 0b00000100



// Global variables
State_T current_state;
volatile uint32_t ticks_in_state;
uint32_t temp_data;

bool lora_ok = false;
int32_t thrust = 0;
uint8_t steering = 128;
char buttons = 0;
char thrust_str[13];


//Initialize packet
Controller_Packet_T data = {.state = KART_STATE_RC, .throttle = 0, .steering = 128};


volatile char state_flag = 0;  //Represents current state (s = slow, e = ebrake, f = fast)

void App_StateMachine_Init() {

  SPI.setMOSI(PA7);
  SPI.setSCLK(PB3);
  SPI.setMISO(PA6);

  //Serial.println("A");
  ticks_in_state = 0;
  temp_data = 0;
  thrust = 0;
  steering = 128;
  buttons = 0;

  analogReadResolution(12);

  // Initialize OLED
  //Serial.println("A");
  u8g2.begin();
  u8g2.clearBuffer();

  // Draw startup logo (if you have AMP image data)
  //Serial.println("A");
  u8g2.setFont(u8g2_font_7x13B_mf);
  
  u8g2.drawXBMP(21, 4, 83, 23, amp_logo); //Draw AMP logo
  u8g2.drawStr(30, 42, "Controller");
  u8g2.drawStr(38, 57, "Startup");
  u8g2.sendBuffer();
  delay(1500);

  // Initialize LoRa
  //Serial.println("A");
  LoRa.setPins(LORA_CS_PIN, LORA_RST_PIN, LORA_DIO0_PIN);


  

  //Serial.println("A");
  if (LoRa.begin(915E6)) {
    //Serial.println("LoRa OK");
    lora_ok = true;
  } else {
    //Serial.println("LoRa FAILED");
    lora_ok = false;
  }

  pinMode(BUTTON_EBRAKE, INPUT_PULLUP);
  pinMode(BUTTON_SLOW, INPUT_PULLUP);
  pinMode(BUTTON_FAST, INPUT_PULLUP);
  pinMode(BUTTON_AUTO, INPUT_PULLUP);


  attachInterrupt(
      BUTTON_EBRAKE, []() { state_flag = 'e'; /* App_StateMachine_ChangeState(STATE_EBRAKE); */ },
      FALLING);

  attachInterrupt(
      BUTTON_SLOW, []() { state_flag = 's'; /* App_StateMachine_ChangeState(STATE_SLOW); */ }, FALLING);

  attachInterrupt(
      BUTTON_FAST, []() { state_flag = 'f'; /* App_StateMachine_ChangeState(STATE_FAST); */ }, FALLING);

  attachInterrupt(
      BUTTON_AUTO, []() { state_flag = 'a'; /*  App_StateMachine_ChangeState(STATE_IDLE); */ }, FALLING);


  // Set initial state
  App_StateMachine_ChangeState(STATE_IDLE);

}



void App_StateMachine_Tick() {

  LoRa.beginPacket();
  LoRa.print("Test");
  LoRa.endPacket();

  ticks_in_state++; 

  
 

  int16_t throttle_y = analogRead(THROTTLE_Y);
  uint16_t steering_x = analogRead(STEERING_X);
  
  Controller_setting(steering_x, throttle_y);

  //Update packet
  //Serial.println(steering);

  data.steering = steering;
  data.throttle = throttle_y;

  


  // Clear display buffer
  u8g2.clearBuffer();

  



  // Run state-specific code
  switch (current_state) {
  case STATE_IDLE: {
    data.state = KART_STATE_AUTO;
    Draw_LoRa_Status();
    u8g2.setFont(u8g2_font_luBS19_tr);
    u8g2.drawStr(35, 50, "IDLE");

    /* Auto transition to SLOW for demo
    if (ticks_in_state > 20) {
      App_StateMachine_ChangeState(STATE_SLOW);
    }*/
    break;
  }

  case STATE_SLOW: {
    Draw_LoRa_Status();
    Draw_State_Normal();
    Draw_Speedometer();
    Draw_Steering();

    data.state = KART_STATE_RC;

    /* Demo transition
    if (ticks_in_state > 50) {
      App_StateMachine_ChangeState(STATE_FAST);
    } */
    break;
  }

  case STATE_FAST: {
    Draw_LoRa_Status();
    Draw_State_Normal();
    Draw_Speedometer();
    Draw_Steering();

    data.state = KART_STATE_RC;

    /* Demo transition
    if (ticks_in_state > 100) {
      App_StateMachine_ChangeState(STATE_EBRAKE);
    }*/
    break;
  }

  case STATE_EBRAKE: {
    //Update packet 
    data.state = KART_STATE_EBRAKE;


    // Flashing effect

    if ((ticks_in_state / 5) % 2 == 0) {
      u8g2.setDrawColor(1);
      u8g2.drawBox(0, 0, 128, 64);
      u8g2.setDrawColor(0);
    } else {
      u8g2.setDrawColor(1);
    } 
    
    

    

    if (((ticks_in_state  / 5) + 4) % 6 == 0) {

      u8g2.drawXBMP(39, 2, 49, 66, brake_sign); //Brake image
    
    } else {

      u8g2.setFont(u8g2_font_luBS19_tr);
      u8g2.drawStr(10, 54, "EBRAKE");
      
      
    }
    

    Draw_LoRa_Status();

    u8g2.setDrawColor(1); 
    
    
    

    if (ticks_in_state > (EBRAKE_MIN_SEC * TICKS_PER_SEC)) {
      // Can exit e-brake after minimum time
      if (ticks_in_state > 80) { // Demo exit
        App_StateMachine_ChangeState(STATE_ERROR);
      }
    }
    break;
  }

  case STATE_ERROR: {
    // Flashing effect
    if ((ticks_in_state / 5) % 2 == 0) {
      u8g2.setDrawColor(1);
      u8g2.drawBox(0, 0, 128, 64);
      u8g2.setDrawColor(0);
    } else {
      u8g2.setDrawColor(1);
    }


    if (((ticks_in_state  / 5) + 4) % 6 == 0) {


      u8g2.drawXBMP(33, 9, 62, 59, caution_image); //Draw caution image
      Draw_LoRa_Status();
    
    } else {

      u8g2.setFont(u8g2_font_luBS19_tr);
      u8g2.drawStr(16, 54, "ERROR");
      Draw_LoRa_Status();

      
    }
  

    break;


    
  }
  }

  // Send buffer to display
  u8g2.sendBuffer();

  //SEND CONTROLLER PACKET
  LoRa.beginPacket();
  LoRa.write((uint8_t*)&data, sizeof(data));
  LoRa.endPacket(); 
  
}

void App_StateMachine_ChangeState(State_T new_state) {


  ticks_in_state = 0;
  temp_data = 0;
  current_state = new_state;

  // Print state change for debugging
  //Serial.print("State changed to: ");
  //Serial.println(new_state);
}

void Controller_setting(uint16_t joystick_x, uint16_t joystick_y) {
  // Convert joystick values to thrust and steering
  

  if(abs(joystick_y - JOYSTICK_CENTER_Y) < JOYSTICK_DEADZONE_Y){
    thrust = 0;
  } else {
    if (joystick_y < JOYSTICK_CENTER_Y) {
    thrust = map(joystick_y, 0, JOYSTICK_CENTER_Y - JOYSTICK_DEADZONE_Y, -255, 0);
    } else {
      thrust = map(joystick_y,  JOYSTICK_CENTER_Y + JOYSTICK_DEADZONE_Y, 4094, 0, 255);
    }


  }

if(abs(joystick_x - JOYSTICK_CENTER_X) < JOYSTICK_DEADZONE_X){
    steering = 128;
  } else {
    
      steering = map(joystick_x, 0, 4095, 255, 0);

  }

  thrust = constrain(thrust, -255, 255);
  
  
  

  if (current_state == STATE_SLOW) {
    thrust /= 4;
  } else if (current_state == STATE_FAST) {
    thrust /= 2;
  }

}

void Draw_LoRa_Status() {

  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(93, 12, "LoRa:");
  if (lora_ok) {
    u8g2.drawStr(105, 24, "OK!");
  } else {
    u8g2.drawStr(97, 24, "FAIL");
  }
}

void Draw_State_Normal() {

  u8g2.setDrawColor(1); 

  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(3, 12, "State:");
  if (current_state == STATE_FAST) {
    u8g2.drawStr(3, 24, "Fast!");
  } else if (current_state == STATE_SLOW) {
    u8g2.drawStr(3, 24, "Slow");
  } else {
    u8g2.drawStr(3, 24, "Other");
  }
}

void Draw_Speedometer() {
  // Draw speedometer arc (Multiple functions for more thickness)
  
  u8g2.drawCircle(65, 44, 20, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_UPPER_RIGHT);
  u8g2.drawCircle(65, 44, 21, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_UPPER_RIGHT);
  u8g2.drawCircle(65, 44, 22, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_UPPER_RIGHT);
  
  

  

  // Draw thrust value


  sprintf(thrust_str, "%02ld", thrust);

 
  //Shifts thrust value if it changes number of digits or sign
  u8g2.setFont(u8g2_font_tenthinguys_tu);
  if (thrust >= 100 && thrust < 200) {
    u8g2.drawStr(55, 43, thrust_str);

  } else if (thrust < 100 && thrust >= 0) {
    u8g2.drawStr(57, 43, thrust_str);

  } else if (thrust >= 200) {
    u8g2.drawStr(53, 43, thrust_str);
  } else if (thrust < 0 && thrust > -10) {
    u8g2.drawStr(57, 43, thrust_str);
  } else if (thrust <= -10 && thrust > -100){
    u8g2.drawStr(53, 43, thrust_str);
  } else if (thrust <= -100 && thrust > -200) {
    u8g2.drawStr(51, 43, thrust_str);
  } else if (thrust <= -200){
    u8g2.drawStr(49, 43, thrust_str);
  }

  
}

void Draw_Steering() {
  // Draw steering indicator

  u8g2.drawRFrame(4, 54, 120, 7, 3);
  u8g2.drawBox(62, 54, 4, 7);

  if (steering < 122) {
    int width =  map(steering, 0, 127, 58, 0);
    u8g2.drawRBox(62 - width, 54, width, 7, 3);
    

  } else if (steering > 132) {
    int width = map(steering, 127, 255, 0, 58);
    u8g2.drawRBox(66, 54, width, 7, 3);
  }

 

}

void handleSerialCommand(char cmd) {
  switch (cmd) {
  case '1':
    App_StateMachine_ChangeState(STATE_IDLE);
    break;
  case '2':
    App_StateMachine_ChangeState(STATE_SLOW);
    break;
  case '3':
    App_StateMachine_ChangeState(STATE_FAST);
    break;
  case '4':
    App_StateMachine_ChangeState(STATE_EBRAKE);
    break;
  case '5':
    App_StateMachine_ChangeState(STATE_ERROR);
    break;
  }
}


