#include <AMP.h>
#include <u8g2.h>
#include <u8x8.h>
#include "spi.h"
#include "usart.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "LoRa.h"


static u8g2_t u8g2;

#include "app_statemachine.h"

#define TICKS_PER_SEC	10
#define EBRAKE_MIN_SEC	5	// minimum seconds to remain in e-brake state

#define LORA_START_BYTE	0xFE	// HEX VALUE
#define LORA_STOP_BYTE	0xFF	// HEX VALUE

// maximum forward/reverse values for fast/slow state
#define SPEED_FWD_FAST	100
#define SPEED_REV_FAST	-80
#define SPEED_FWD_SLOW	50
#define SPEED_REV_SLOW	-30

// below are binary masks to send the button states in 1 byte
#define BTN_EBRAKE	0b00000001
#define BTN_AUTO	0b00000010
#define BTN_SLOW	0b00000100

State_T current_state;		// variable holding current controller state
uint32_t ticks_in_state;	// variable holding number of ticks in current state
uint32_t temp_data;			// to hold miscellaneous data within a state

uint16_t lora_debug;		// variable holding LoRa debug data
int32_t thrust;			// variable holding current thrust value
int32_t steering;		// variable holding current steering value

char *thrust_str;		// variable holding string representation of thrust
char buttons;			// variable holding activated buttons

uint8_t u8x8_stm32_gpio_and_delay(U8X8_UNUSED u8x8_t *u8x8,
    U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int,
    U8X8_UNUSED void *arg_ptr)
{
  switch (msg)
  {
  case U8X8_MSG_GPIO_AND_DELAY_INIT:
    HAL_Delay(1);
    break;
  case U8X8_MSG_DELAY_MILLI:
    HAL_Delay(arg_int);
    break;
  case U8X8_MSG_GPIO_DC:
    HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, arg_int);
    break;
  case U8X8_MSG_GPIO_RESET:
    HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, arg_int);
    break;
  }
  return 1;
}
uint8_t u8x8_byte_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,
    void *arg_ptr)
{
  switch (msg)
  {
  case U8X8_MSG_BYTE_SEND:
    HAL_SPI_Transmit(&hspi1, (uint8_t *) arg_ptr, arg_int, 10000);
    break;
  case U8X8_MSG_BYTE_INIT:
    break;
  case U8X8_MSG_BYTE_SET_DC:
    HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, arg_int);
    break;
  case U8X8_MSG_BYTE_START_TRANSFER:
    break;
  case U8X8_MSG_BYTE_END_TRANSFER:
    break;
  default:
    return 0;
  }
  return 1;
}

// function to be called before tick() function
void App_StateMachine_Init()
{
	// default variable values
	ticks_in_state = 0;
	temp_data = 0;
	thrust = 0;
	steering = 128;
	buttons = 0;
	thrust_str = (char*)malloc(13 * sizeof(char));


	// initialize OLED
	u8g2_Setup_ssd1306_128x64_noname_1(&u8g2, U8G2_R0, u8x8_byte_4wire_hw_spi, u8x8_stm32_gpio_and_delay);
	u8g2_InitDisplay(&u8g2);
	u8g2_SetPowerSave(&u8g2, 0);
	u8g2_SetDrawColor(&u8g2, 1);
	u8g2_FirstPage(&u8g2);
	do {
		u8g2_DrawXBM(&u8g2, 0, 0, AMPIMG_width, AMPIMG_height, &AMPIMG_bits);
	} while (u8g2_NextPage(&u8g2));
	u8g2_SetDrawColor(&u8g2, 2);
	HAL_Delay(1000);  // This is totally optional, i just think it helps see it and is cool




	// initialize LoRa
	LoRa lora;
	lora = newLoRa();

	lora.CS_port = LORA_NSS_GPIO_Port;
	lora.CS_pin = LORA_NSS_Pin;
	lora.reset_port = LORA_RST_GPIO_Port;
	lora.reset_pin = LORA_RST_Pin;
	lora.DIO0_port = LORA_DIO0_GPIO_Port;
	lora.DIO0_pin = LORA_DIO0_Pin;
	lora.hSPIx = &hspi1;

	lora.frequency = 915;

	LoRa_reset(&lora);
	lora_debug = LoRa_init(&lora);
	if (lora_debug == LORA_OK)
	{
		HAL_UART_Transmit(&huart2, (unsigned char *) "LoRa OK\r\n", 9, 10);
	}
	else
	{
		HAL_UART_Transmit(&huart2, (unsigned char *) "LoRa FAILED\r\n", 13, 10);
	}



	// set current state to idle
	App_StateMachine_ChangeState(STATE_IDLE);
}

// function to be called on timer interrupt
void App_StateMachine_Tick()
{
	// statements to be called regardless of state
	ticks_in_state += 1;




	u8g2_FirstPage(&u8g2);
	u8g2_SetDrawColor(&u8g2, 1);

	// TODO need lora driver send function here
	// run state-specific code
	switch (current_state)
	{
		case (STATE_IDLE):
		{

			App_StateMachine_ChangeState(STATE_IDLE); // @gyoder confirm

			break;
		}

		case (STATE_SLOW):
		{
			// TODO

			// Display
			do {
				Draw_LoRa_Status();
				Draw_State_Normal();
			} while (u8g2_NextPage(&u8g2));
			// End Display


			//TODO Remove, Temp testing code  for swapping states
			if (ticks_in_state > 50) {
				App_StateMachine_ChangeState(STATE_FAST);
			}
			break;
		}

		case (STATE_FAST):
		{
			// TODO

			// Display
			do {
				Draw_LoRa_Status();
				Draw_State_Normal();
				Draw_Speedometer();
				Draw_Steering();
			} while (u8g2_NextPage(&u8g2));
			// End Display

			//TODO Remove, Temp testing code  for swapping states
			if (ticks_in_state > 500) {
				App_StateMachine_ChangeState(STATE_EBRAKE);
			}
			break;
		}

		case (STATE_EBRAKE):
		{

			if (ticks_in_state > (EBRAKE_MIN_SEC * TICKS_PER_SEC))
			{
				// TODO only allow leaving after set amount of time
			}
			// Display
			do {
				if (ticks_in_state / 5 % 2 == 0) {
					u8g2_SetDrawColor(&u8g2, 1);
					u8g2_DrawBox(&u8g2, 0, 0, 128, 64);
					u8g2_SetDrawColor(&u8g2, 0);
				} else {
					u8g2_SetDrawColor(&u8g2, 1);

				}
				Draw_LoRa_Status();
				u8g2_SetFont(&u8g2, u8g2_font_helvR18_te);
				u8g2_DrawStr(&u8g2, 14, 48, "EBRAKE");
			} while (u8g2_NextPage(&u8g2));
			// End Display


			//TODO Remove, Temp testing code  for swapping states
			if (ticks_in_state > 50) {
				App_StateMachine_ChangeState(STATE_ERROR);
			}
			break;

		}

		case (STATE_ERROR):
		{
			// TODO

			// Display
			do {
				if (ticks_in_state / 5 % 2 == 0) {
					u8g2_SetDrawColor(&u8g2, 1);
					u8g2_DrawBox(&u8g2, 0, 0, 128, 64);
					u8g2_SetDrawColor(&u8g2, 0);
				} else {
					u8g2_SetDrawColor(&u8g2, 1);

				}
				Draw_LoRa_Status();
				u8g2_SetFont(&u8g2, u8g2_font_helvR18_te);
				u8g2_DrawStr(&u8g2, 20, 48, "ERROR");
			} while (u8g2_NextPage(&u8g2));
			// End Display



			break;
		}
	}


	// update oled display
}

void App_StateMachine_ChangeState(State_T new_state)
{
	// statements to be called regardless of state transition
	ticks_in_state = 0;
	temp_data = 0;
	// change state
	current_state = new_state;
}

void Controller_setting(State_T new_state, uint32_t joystick_x, uint32_t joystick_y){
	switch(new_state)
	{
			case (STATE_IDLE):
				thrust = 0;
				steering = 0;
				break;

			case (STATE_SLOW):
					if(joystick_x >=2400)
						thrust = joystick_x * SPEED_FWD_SLOW;
					else if(joystick_x <= 1600)
						thrust = joystick_x * SPEED_REV_SLOW;
					else
						thrust = 2000;
					if(joystick_y >=2400)
						steering = joystick_y * SPEED_FWD_SLOW;
					else if(joystick_y <= 1600 )
						steering = joystick_y * SPEED_REV_SLOW;
					else
						steering = 2000;
				break;
			case (STATE_FAST):
				thrust = SPEED_FWD_FAST;
				steering = SPEED_REV_FAST;

				break;
			case (STATE_EBRAKE):
				thrust = 2000;
				steering = 2000;
				break;
			default:
				break;
		}

}

//void Controller_steer_thrust(Cont_Info controller){
//	if (controller.thrust_input < -200)
//		controller.thrust = controller.thrust_input * controller.speed_max;
//		else if(controller.thrust_input > 200 )
//			controller.thrust = controller.thrust * controller.speed_max;
//		else
//			controller.thrust = 0;
//	if (controller.steer_input < -200)
//		controller.steer = controller.steer_input;
//		else if(controller.steer_input > 200 )
//			controller.steer = controller.steer_input;
//		else{
//			controller.steer = 0;
//		}
//	//return controller;
//}

char* Controller_data(){
//	int length = snprintf(NULL, 0, "A%c%c%c%c", controller.thrust, controller.steer, controller.e_brake, controller.state);
	char* message;
//	controller.info = (char*)malloc(length + 1); // +1 for null terminator
//	  // Format the string
//	snprintf(controller.info, length + 1, "A%c%c%c%c", controller.thrust, controller.steer, controller.e_brake, controller.state);
//	 message = Controller_send_data(controller, lora_1);
//	free(controller.info);
	return message;
//	char uart2_msg[UART2_MSG_LENGTH];	// buffer for messages to send over UART2

}

// Set desired state
char* Controller_send_data()
{
	uint8_t pass = 0;
	uint8_t pass_1 = 0;
	char message[40];
//	pass =  LoRa_transmit(lora_1, &controller.info , size(controller.info), 50);
//	pass_1 += LoRa_receive(lora_1, &message , 40);
//	if (pass != 2){
//		message = ("Lora did not recieve sigal");
//	}
//	if (pass_1 != 1){
//		message = ("Lora did not recieve signal");
//	}
	return message;
}


void Draw_LoRa_Status()
{
	u8g2_SetFont(&u8g2, u8g2_font_smart_patrol_nbp_tr);
	u8g2_DrawStr(&u8g2, 86, 12, "LoRa:");
	if (lora_debug == LORA_OK)
	{
		u8g2_DrawStr(&u8g2, 104, 24, "OK!");
	}
	else
	{
		u8g2_DrawStr(&u8g2, 86, 24, "FAIL");
	}
}

void Draw_State_Normal()
{
	u8g2_SetFont(&u8g2, u8g2_font_smart_patrol_nbp_tr);
	u8g2_DrawStr(&u8g2, 3, 12, "State:");
	if (current_state == STATE_FAST)
	{
		u8g2_DrawStr(&u8g2, 3, 24, "Fast!");
	}
	else if (current_state == STATE_SLOW)
	{
		u8g2_DrawStr(&u8g2, 3, 24, "Slow");
	}
	else
	{
		u8g2_DrawStr(&u8g2, 3, 24, "NOT HANDLED");
	}
}


void Draw_Speedometer()
{
	u8g2_DrawArc(&u8g2, 73, 40, 19, 240, 144);
	u8g2_DrawArc(&u8g2, 73, 40, 25, 240, 144);
	for (int i = 19; i <= 25; i++) {

		u8g2_DrawArc(&u8g2, 73, 40, i, (int) (239 + (1.26 * (127 - thrust))) % 256, 144);
		u8g2_DrawArc(&u8g2, 73, 40, i, 239, 241);
	}
	sprintf(thrust_str, "%02ld", thrust);
	u8g2_SetFont(&u8g2, u8g2_font_smart_patrol_nbp_tr);
	u8g2_DrawStr(&u8g2, 64, 42, thrust_str);


}

void Draw_Steering() 
{

	u8g2_DrawRFrame(&u8g2, 4, 54, 120, 7, 3);
	u8g2_DrawBox(&u8g2, 62, 54, 4, 7);
	if (steering < 127) {
		u8g2_DrawRBox(&u8g2, 64 - (int) (steering / 2.03), 54, (int) (steering / 2.03), 7, 3);
	} else if (steering > 127) {
		u8g2_DrawRBox(&u8g2, 64, 54, (int) ((steering - 126) / 2.1), 7, 3);
	}


}
;
