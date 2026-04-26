/*
 * controller_packet.h
 *
 *  Created on: Mar 30, 2025
 *      Author: gyoder
 */

#ifndef APP_INC_CONTROLLER_PACKET_H_
#define APP_INC_CONTROLLER_PACKET_H_

#include <stdint.h>

typedef enum { KART_STATE_EBRAKE, KART_STATE_RC, KART_STATE_AUTO } Kart_State_T;

typedef struct {
  Kart_State_T state;
  int16_t throttle;
  uint8_t steering;
} Controller_Packet_T;

Controller_Packet_T test = {.state = KART_STATE_EBRAKE};

#endif /* APP_INC_CONTROLLER_PACKET_H_ */
