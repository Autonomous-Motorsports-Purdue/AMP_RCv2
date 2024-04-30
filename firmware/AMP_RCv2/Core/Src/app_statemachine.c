#include "app_statemachine.h"

#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

void App_StateMachine_Init() {
}

Cont_Info newController() {
	Cont_Info new_controller;

	new_controller.state = STATE_IDLE;
	new_controller.speed_max = 0;
	new_controller.speed_min = 0;
	new_controller.thrust = 0;
	new_controller.steer = 0;
	new_controller.e_brake = 0;
	new_controller.thrust_input = 0;
	new_controller.steer_input = 0;

	return new_controller;

}
// Super-loop that is called every TIM7 tick
Cont_Info App_StateMachine_Update(Cont_Info controller, int mode) {

	switch (mode) {
	case (1):
		controller.state = STATE_IDLE;
		break;

	case (2):
		controller.state = STATE_SLOW;
		break;
	case (3):
		controller.state = STATE_FAST;
		break;
	case (0):
		controller.state = STATE_EMERGENCY_STOP;
		break;
	default:
		break;
	}
	return controller;
}

Cont_Info Controller_setting(Cont_Info controller) {
	switch (controller.state) {
	case (STATE_IDLE):
		controller.speed_max = 0;
		controller.speed_min = 0;
		break;

	case (STATE_SLOW):
		controller.speed_max = 30;
		controller.speed_min = -30;
		break;
	case (STATE_FAST):
		controller.speed_max = 100;
		controller.speed_min = -100;
		break;
	case (STATE_EMERGENCY_STOP):
		controller.speed_max = 0;
		controller.speed_min = 0;
		break;
	default:
		break;
	}
	return controller;
}
//void Controller_data(){}

Cont_Info Controller_steer_thrust(Cont_Info controller) {
	if (controller.thrust_input < -200)
		controller.thrust = controller.thrust_input * controller.speed_max;

	else if (controller.thrust_input > 200)
		controller.thrust = controller.thrust * controller.speed_max;

	else
		controller.thrust = 0;

	if (controller.steer_input < -200)
		controller.steer = controller.steer_input;

	else if (controller.steer_input > 200)
		controller.steer = controller.steer_input;

	else {
		controller.steer = 0;
	}
	return controller;
//	return controller;
}

// Set desired state
//char* Controller_data(Cont_Info controller, LoRa lora_1){
////	int length = snprintf(NULL, 0, "A%c%c%c%c", controller.thrust, controller.steer, controller.e_brake, controller.state);
//	char* message;
////	controller.info = (char*)malloc(length + 1); // +1 for null terminator
////	  // Format the string
////	snprintf(controller.info, length + 1, "A%c%c%c%c", controller.thrust, controller.steer, controller.e_brake, controller.state);
////	 message = Controller_send_data(controller, lora_1);
////	free(controller.info);
//	return message;
////	char uart2_msg[UART2_MSG_LENGTH];	// buffer for messages to send over UART2
//
//}
//
//// Set desired state
//char* Controller_send_data(Cont_Info controller, LoRa lora_1)
//{
//	uint8_t pass = 0;
//	uint8_t pass_1 = 0;
//	char message[40];
////	pass =  LoRa_transmit(lora_1, &controller.info , size(controller.info), 50);
////	pass_1 += LoRa_receive(lora_1, &message , 40);
////	if (pass != 2){
////		message = ("Lora did not recieve sigal");
////	}
////	if (pass_1 != 1){
////		message = ("Lora did not recieve signal");
////	}
//	return message;
//
//}
