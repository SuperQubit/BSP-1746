//============================================================================//
//
//  File:   sap.h
//  Rev:    1.00 -a00
//  Date:	18/12/2017
//
//  Description: 
//
//  Revision History:
//		1.00 -a00	SAP base board 1746+SPIN32F0A
//		1.00 -a01	SAP con p3,p4
//		1.00 -a02	ax,ay,az an maskflasg (SystemFlags)
//
//============================================================================//
//============================================================================//
/* Define to prevent recursive inclusion -------------------------------------*/
//============================================================================//
#ifndef __SAP_H
#define __SAP_H

//============================================================================//
/* Includes ------------------------------------------------------------------*/
//============================================================================//
//#include "stm32f3xx_hal.h"
#include "mbed.h"
//============================================================================//
/* Exported constants --------------------------------------------------------*/
//============================================================================//

//============================================================================//
/* Exported Types ------------------------------------------------------------*/
//============================================================================//

/* Public typedef -----------------------------------------------------------*/

//*--------  RT SAP Out
#define CMD_ALGO_MASK	(0x0F)
#define CMD_ALGO_NONE	(0x00)
#define CMD_ALGO_HYBRID	(0x01)
#define CMD_ALGO_TORQUE	(0x02)
#define CMD_ALGO_SPEED	(0x03)
#define CMD_ALGO_TORQUE_ADV	(0x04)
#define CMD_ALGO_HWRESET	(0x15)

#define CMD_FLAG_BRAKEONZERO (0x10)
#define CMD_FLAG_HWILIMIT	 (0x20)
#define CMD_FLAG_SYNCFAULT	 (0x40)
#define CMD_FLAG_FAULTACK	 (0x80)


#define RTCMD_NOP  (0)
#define RTCMD_BASE (1)
#define RTCODE_NONE (0)
#define RTCODE_UNBRAKE	(1)
#define RTCODE_RUN		(2)

typedef struct _RTSAP_OutData {
	uint8_t command;
	uint8_t pcode;
	uint16_t tsn;
	int16_t	p1;
	int16_t p2;
	int16_t p3;
	int16_t p4;
	int16_t p5;
} SAP_OutData_t;

//*--------  RT SAP In
#define RTSTS_IDLE				(0)
#define RTSTS_RUN				(1)
#define RTSTS_ERROR				(0x80)

#define RTMOT_STS_IDLE			(0)
#define RTMOT_STS_MASK			(0x0F)
#define RTMOT_STS_ERROR			(0x80)
#define RTMOT_STS_ERROR_MASK	(0xF0)

typedef struct _RTSAP_InData {
	uint8_t status;
	uint8_t command;
	uint8_t pcode;
	uint8_t pgr;
	uint8_t m1_status;
	uint8_t m2_status;
	uint8_t m1_error;
	uint8_t m2_error;
	uint8_t t1;
	uint8_t t2;
	int16_t	p1;
	int16_t p2;
	int16_t p3;
	int16_t p4;
	uint16_t maskflags;
	int16_t ax;
	int16_t ay;
	int16_t az;	
} SAP_InData_t;


//============================================================================//
/* Exported variables --------------------------------------------------------*/
//============================================================================//
  
#endif /* __SAP_H */
//============================================================================//
/*------------------------------- END OF FILE --------------------------------*/
//============================================================================//
