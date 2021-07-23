//============================================================================//
//
//  File:   Comodo.h
//  Rev:    1.0 -a9
//  Date:	01/08/2017
//
//  Description: 
//
//  Revision History:
//		1.00 -a00	BSP base board 1719+SPIN32F0
//			 -a01	BSP	base board 1746+SPIN32F0A
//			 -a02	New SAP
//			 -a03	BootLoading
//			 -a04	BSP	LED
//			 -a05   SerialCommander/Diagnostic
//			 -a06   Accelerometer/SAP
//			 -a07 
//			 -a08 
//			 -a09  Supervisor Reset Ext 
//
//============================================================================//
 //---------------------------
//--  PINOUT
//
// MOTOR1:
// 	PB0		O	DIRECTION
// 	PC5		I	ZSPEED  (RUN_GOOD)
// 	PB1		O	THROTTLE	(PWM3.4)
// 	PA6		AI	U
// 	PA5		AI	V
// 	PA4		AI	W
// 	PA7		AI	T
//  PC10	O	URX		(UART4_TX)
//	PC11	I	UTX 	(UART4_RX)
//
// MOTOR2:
// 	PC13	O	DIRECTION
// 	PB6		I	ZSPEED (RUN_GOOD)
// 	PB4		O	THROTTLE	(PWM3.1)
// 	PC3		AI	U
// 	PC2		AI	V
// 	PC1		AI	W
// 	PC0		AI	T
//  PC12	O	URX		(USART5_TX)
//	PD2		I	UTX 	(USART5_RX)
//
// PWCTRL:
//  PA15	O	BRAKE	(PWM2.1)
//	PC4		AI	VBUS	(ADC2.5)
//	PC6		O	RELE	
//
// ACCELLEROMETER:
//	PB12		CS
//	PB13-PA8	SCL
//	PB14		SA0
//	PB15-PC9	SDA	
//	PB2			INT1
//	PB10		INT2
//
// LED:
//	PC14   LED1(RED)
//	PC15   LED2(GREEN)
//
// ACCELEROMETER
//	SPI					I2C
//	PB12      CS
//	PB13      SCL       PA8
//	PB15      SDA       PC9
//	PB14      SA0
//	PB2       INT1
//	PB10      INT2

//============================================================================//
/* Define to prevent recursive inclusion -------------------------------------*/
//============================================================================//
#ifndef __COMODO_H
#define __COMODO_H

//#pragma diag_suppress 1299



//============================================================================//
/* Includes ------------------------------------------------------------------*/
//============================================================================//
#include "mbed.h"
#include "SerialOw.h"

#include <vector>
#include "rpc.h"
#include "SerialCommander.h"
#include "RpcClasses.h"
#include "Motor.h"
#include "MotorCom.h"
#include "LIS3DH.h"
#define COMODO_BSP_SAP_VERSION		"1.00a19"
#define COMODO_BSP_MANIFEST		"COMODO Swatch 1746"

//============================================================================//
/* EventRecorder--------------------------------------------------------------*/
//============================================================================//
#ifdef TRACEALYZER_ENABLE
extern "C" {
#include "EventRecorder.h"
}
#define ID_OUTPUT         EventID(EventLevelAPI,0xB1, 0x00)
#define EvtRecord_OutputPwm(p)     EventRecord2(ID_OUTPUT, (p), 0)
#else
#define EvtRecord_OutputPwm(p)  
#endif

//============================================================================//
/* Exported Types ------------------------------------------------------------*/
//============================================================================//
class ComodoUI : public RPC {
public:
	ComodoUI():RPC("UI"),led1(PC_14),led2(PC_15)
	{ 
		led1=0;
		led2=0;
		_TickerPeriod=0.05;
		ledHertBeat.attach(  callback(this, &ComodoUI::ledHertBeatHandler) ,  _TickerPeriod);
	}
	
	void Red(uint16_t m){ RedLed=m;};
	void Green(uint16_t m){ GreenLed=m;};
	void Period(float p){
		if(p!= _TickerPeriod){	
		_TickerPeriod=p;
		ledHertBeat.detach();
		ledHertBeat.attach( callback(this, &ComodoUI::ledHertBeatHandler) ,_TickerPeriod);
		}
	};
	void Disable(void){ ledHertBeat.detach(); };
	void Enable(void){ ledHertBeat.attach( callback(this, &ComodoUI::ledHertBeatHandler) , _TickerPeriod);};
	
    virtual const struct rpc_method *get_rpc_methods() {
        static const rpc_method rpc_methods[] = {
            {"Red", rpc_method_caller<ComodoUI,uint16_t,&ComodoUI::Red>},
            {"Green", rpc_method_caller<ComodoUI, uint16_t, &ComodoUI::Green>},
			{"Period", rpc_method_caller<ComodoUI, float, &ComodoUI::Period>},
            RPC_METHOD_END
        };
        return rpc_methods;
    }

	uint16_t GreenLed;
	uint16_t RedLed;
	DigitalOut led1;
	DigitalOut led2;
		
private:
	float _TickerPeriod;
	Ticker ledHertBeat;
	void ledHertBeatHandler(void)
	{
		static uint16_t cLedMask=1;
	
		if( cLedMask<<=1 );else cLedMask=1;
		if( GreenLed & cLedMask ) led2=1; else led2=0;
		if( RedLed & cLedMask ) led1=1; else led1=0;
	}

};



class ComodoSap : public RPC {

public:
	ComodoSap():RPC("SAP"){};
	
	//*--------  RT SAP Out
#define RTHEADER_CODE_OUT	(0x5F)
//*--------  RT SAP In
#define RTHEADER_CODE_IN		(0xF0)
#define RTHEADER_CODE_IN_ERROR	(0xFF)


#include "sap.h"


typedef SAP_OutData_t SAP_SlaveInData_t;
typedef SAP_InData_t SAP_SlaveOutData_t;

typedef struct _RTSAP_In{
	uint8_t SAPSTATE;
	uint8_t len;
	uint16_t tsc;
	SAP_SlaveInData_t Data;
} SAP_In_t, *pSAP_In_t;

typedef struct _RTSAP_Out{
	SAP_SlaveOutData_t Data;
} SAP_Out_t, *pSAP_Out_t;

SAP_Out_t OUT;
SAP_In_t IN;

	vector<char>  OUTread(void);
	
	int INwrite(vector<uint8_t> in ) { 
		uint8_t *ptr= ((uint8_t *)&IN) ;
		int len= in.size();
		int i;
		for ( i = 0; i < len; ++i)
		{
			if(i>sizeof(IN))break;
			*ptr++= in[i];
		}
		return i; 
	}
	
	
    int write(float a0) { return 1;}

 virtual const struct rpc_method *get_rpc_methods() {
        static const rpc_method rpc_methods[] = {
            {"OUT", rpc_method_caller<vector<char> , ComodoSap, &ComodoSap::OUTread>},
            {"IN", rpc_method_caller<int, ComodoSap,vector<uint8_t>, &ComodoSap::INwrite>},
            RPC_METHOD_END
        };
        return rpc_methods;
		}	
		
};


#define BRAKE_PWM_PERIOD	(100l)
class ComodoBrake : public RPC {
public:
	ComodoBrake():RPC("BRAKE"),o(PA_15){ o.period_us(BRAKE_PWM_PERIOD);}
	ComodoBrake(PinName a0, const char *name=NULL) : RPC(name), o(a0) { o.period_us(BRAKE_PWM_PERIOD); }

	int On(void) { o.write(mBRAKEPWM); EvtRecord_OutputPwm((int32_t)(o.read()*1000.0f)); return 1;}
	int Off(void) { o.write(0); EvtRecord_OutputPwm(0); return 1;}
	int SetPWM(float vbatt){  
		if(vbatt > 30.0f){
			mBRAKEPWM= (30.0f)/vbatt;
		} else
			mBRAKEPWM= 1.0f;
		return 1; }
	
    float read(void) {return o.read();}
    int write(float a0) {o.write(a0); return 1; }

    virtual const struct rpc_method *get_rpc_methods() {
        static const rpc_method rpc_methods[] = {
            {"read", rpc_method_caller<float, ComodoBrake, &ComodoBrake::read>},
            {"write", rpc_method_caller<int, ComodoBrake, float, &ComodoBrake::write>},
            RPC_METHOD_END
        };
        return rpc_methods;
    }
private:
    PwmOut o;
	float mBRAKEPWM;
};

class Comodo : public RPC
{ 
	public:
		Comodo();
		
		typedef struct 
		{
			float value;
			//operator int() const { return int(value);}
			operator float() const { return float(value);}
			void operator = (const float v) { 
				value=v;
			}
		} realnumber;
		
		char * Version();
		realnumber Measure();
		float VBatt();
		vector<float> GetAcceleration();
		float IstantaneusMeasure();
		int SReset();
		int SReBoot();
		int SResetAssert();
		int SetProtocol( int protocol);
		int SResetDeassert();
		bool CheckReboot();
		void ClearReboot();
		void ClearHardwareLostBar();
	
		ComodoBrake BRAKE;
		Motor Motor1;
		Motor Motor2;
		ComodoSap SAP;
		ComodoUI LED;
		LIS3DH Acc;
		SerialCommander *Commander;
		/**Defines the methods available over RPC*/
		virtual const struct rpc_method *get_rpc_methods();
		float mVBATT;
		float Axyz[3];
		float mAxyz[3];
		bool _RebootCS;
		uint16_t SystemFlags;
		uint32_t Vraw;
		uint32_t Vmean;
		uint32_t Vsigma;
		bool HardwareLostBAR;
		
	private:
		AnalogIn	VBATT;
		DigitalOut	SRESET;
		DigitalOut	SBOOT0;
		DigitalOut	SA0;
		DigitalOut	SCS;
		Thread _thread;
	  bool HardwareLostBARACK;
		static void _Runner(const void *args);
	
};
//============================================================================//
/* Exported variables --------------------------------------------------------*/
//============================================================================//
extern Comodo BSP;
extern MotorCom MC2;
extern MotorCom MC1;
//============================================================================//
/* Exported function prototypes ----------------------------------------------*/
//============================================================================//

  
#endif /* __SUPPOSTA_H */
//============================================================================//
/*------------------------------- END OF FILE --------------------------------*/
//============================================================================//
