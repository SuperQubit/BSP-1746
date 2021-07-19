//============================================================================//
//
//  File:   Motor.h
//  Rev:    1.0
//  Date:	01/08/2017
//
//  Description: 
//
//  Revision History:
//
//
//============================================================================//
  
//============================================================================//
/* Define to prevent recursive inclusion -------------------------------------*/
//============================================================================//
#ifndef __MOTOR_H
#define __MOTOR_H


//============================================================================//
/* Includes ------------------------------------------------------------------*/
//============================================================================//
#include "mbed.h"
#include "rpc.h"

/*    
#define NTC_SENSOR B57620C104J62
#define NTC_SENSOR_R25	(100000.0f)
#define NTC_SENSOR_B2550 (4120.0f)
#define NTC_SENSOR_B2585 (4190.0f)
#define NTC_SENSOR_B25100 (4300.0f)
*/
#define NTC_SENSOR B57471V2104J62
#define NTC_SENSOR_R25	(100000.0f)
#define NTC_SENSOR_B2550 (4386.0f)
#define NTC_SENSOR_B2585 (4455.0f)
#define NTC_SENSOR_B25100 (4480.0f)



//============================================================================//
/* Exported Types ------------------------------------------------------------*/
//============================================================================//
class Motor : public RPC
{ 
	public:
		Motor(PinName DIR, PinName SPD, PinName THR, PinName Temp, PinName U, PinName V, PinName W , const char *name=NULL);
		Motor();

		void Clock(void);	
		
		int On(float Throttle);
		int Off();
		float MeasureT();
		DigitalIn  ZSpeed;
		int16_t Tmosfet;
		bool RUN_GOOD;
		struct cThermalModel
		{
			float W;
			float Rca;
			float Rjc;
			float Req;
			float Cth;
			float Td;
			float Tc;
			float Tj;
			uint16_t iTd;
			uint16_t iTc;
			uint16_t iTj;
			uint16_t iW;
			uint8_t bTd;
			uint8_t bTj;
		} TM;
	
		float ThermalModelStep(float Pc, float Ip, float dt);
		/**Defines the methods available over RPC*/
		virtual const struct rpc_method *get_rpc_methods();

	private:
			PwmOut Throttle;
			DigitalOut Direction;
			AnalogIn Temp;
			AnalogIn VU;
			AnalogIn VV;
			AnalogIn VW;
			float _vt;

};
//============================================================================//
/* Exported variables --------------------------------------------------------*/
//============================================================================//

//============================================================================//
/* Exported function prototypes ----------------------------------------------*/
//============================================================================//

  
#endif /* __SUPPOSTA_H */
//============================================================================//
/*------------------------------- END OF FILE --------------------------------*/
//============================================================================//
