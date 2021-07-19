//============================================================================//
//
//  File:   Motor.cpp
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
/* Includes ------------------------------------------------------------------*/
//============================================================================//
#include "Motor.h"
#pragma diag_suppress 1299

Motor::Motor(PinName DIR, PinName SPD, PinName THR, PinName T, PinName U, PinName V, PinName W , const char *name): RPC(name), Direction(DIR), Throttle(THR), ZSpeed(SPD), Temp(T), VU(U) ,VV(V), VW(W)
{
	Throttle.period_us(1000l);
	Throttle.write(0);
	Direction=1;
	_vt= Temp.read(); 
	MeasureT();
	ZSpeed.mode(PullDown);
	TM.Rca=40.0;
	TM.Rjc=1.2;
	TM.Req=0.004;
	TM.Tc= 300.0f;
	TM.Tj= 300.0f;
	TM.Cth= 0.001;
	RUN_GOOD=0;
}

Motor::Motor() : RPC("M1"), Throttle(PB_7), Temp(PC_4), Direction(PB_0), ZSpeed(PC_5), VU(PA_6),VV(PA_5),VW(PA_4)
{
	Throttle.period_us(1000l);
	Throttle.write(0);
	Direction=1;
	ZSpeed.mode(PullDown);
	TM.Rca=40.0;
	TM.Rjc=1.2;
	TM.Req=0.004;
	TM.Tc= 300.0f;
	TM.Tj= 300.0f;
	TM.Cth= 0.001;
	RUN_GOOD=0;
}

int Motor::On(float Thr)
{
	if(Thr>0.0f){
		Direction=1;
	}
	else
	{
		Direction=0;
	}
		Throttle.write(fabs(Thr));
	return 1;
}

int Motor::Off()
{
	Throttle.write(0);
	Direction=0;
	return 1;
}

float Motor::MeasureT()
{
		const float Ru=20000.0;
		const float Rtn=NTC_SENSOR_R25;
		const float  beta = NTC_SENSOR_B2585;
		float temperature, resistance,a;
        a = _vt; /* Read analog value */
        
		resistance=(float) (Ru * a /(1.0f-a)); 
        
        /* Convert the resistance to temperature using Steinhart's Hart equation */
				TM.Td=(1/((log(resistance/Rtn)/beta) + (1.0f/298.15f)));
        temperature=TM.Td-273.15f; 
		Tmosfet=(int16_t) temperature;
	return temperature; //Temp.read()*(3.3f);
}

float Motor::ThermalModelStep(float Pc, float Ip, float dt)
{
	 TM.W=(TM.Req*Ip*Ip+Pc);
	 TM.Tc= TM.Tc +  (dt *(TM.W-(TM.Tc-TM.Td)/TM.Rca))/TM.Cth;
	 TM.Tj= TM.Tc + TM.Rjc * TM.W; 
	 TM.iTd= (uint16_t)(TM.Td)-223;
	 TM.iTc= (uint16_t)(TM.Tc)-223;
	 TM.iTj= (uint16_t)(TM.Tj)-223;
	 TM.iW= (uint16_t)(TM.W *1000.0f);
}

void Motor::Clock(void)
{
	static uint8_t decimator=0;
	_vt= _vt*(63.0f/64.0f)+Temp.read()/64.0f;
	RUN_GOOD= ZSpeed;
	if(decimator++==20)
	{
		MeasureT();
		decimator=0;
	}
}

/**Defines the methods available over RPC*/
const rpc_method *Motor::get_rpc_methods()
{
	static const rpc_method rpc_methods[] = {
		{ "On", rpc_method_caller<int, Motor, float, &Motor::On> },
		{ "Off", rpc_method_caller<int, Motor, &Motor::Off> },
		{ "MeasureT", rpc_method_caller<float, Motor, &Motor::MeasureT> },
		RPC_METHOD_END
	};
	return rpc_methods;
}

//============================================================================//
/*------------------------------- END OF FILE --------------------------------*/
//============================================================================//

