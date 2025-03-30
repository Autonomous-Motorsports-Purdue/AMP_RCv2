/*
 * controller_packet.h
 *
 *  Created on: Mar 30, 2025
 *      Author: gyoder
 */

#ifndef APP_INC_CONTROLLER_PACKET_H_
#define APP_INC_CONTROLLER_PACKET_H_

typedef struct {
	enum state {
		KART_STATE_EBRAKE,
		KART_STATE_RC,
		KART_STATE_AUTO
	};
	uint8_t throttle;
	enum throttle_direction {
		KART_DIR_FORWARD,
		KART_DIR_BACKWARD
	};
	uint8_t steering;
} Controller_Packet_T;


#endif /* APP_INC_CONTROLLER_PACKET_H_ */
