//============================================================================//
//
//  File:   MotorCom.h
//  Rev:    1.0
//  Date:	10/12/2017
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
#ifndef __MOTOR_COM_H
#define __MOTOR_COM_H


//============================================================================//
/* Includes ------------------------------------------------------------------*/
//============================================================================//
#include "mbed.h"
#include "rpc.h"
#include "FCP.h"
#include "SerialBL_Host.h"
#include <vector>

//============================================================================//
/* Exported Types ------------------------------------------------------------*/
//============================================================================//
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
#define CMD_FLAG_THERMALMODEL	 (0x40)
#define CMD_FLAG_FAULTACK	 (0x80)

#define RTCODE_LIMITERMASK  (0xF0)

class MotorCom : public RPC
{ 
	public:
		MotorCom();
		MotorCom(PinName tx, PinName rx, const char *name=NULL,int baudrate=115200);
		
		void Puts(const char *s);
		const char* Gets(void);
		const char* VC(void);
		vector<char> Get(void);
		vector<char> GetID(void);
		vector<char> GetVersion(void);
		const char* GO( uint32_t address);
		vector<char> ReadMem(uint32_t address, int length);
		const char* EraseAll(void);
		const char* WriteBlock(uint32_t address, int blocknum);
		const char* WriteAllBlock(void);
		const char* CalcCRC(void);
		const char* FWVersion(void) {return InfoReport.Manifest;}

	
		 typedef struct
		{
		  uint8_t Cmd;
		  uint8_t Mode; 
		  int16_t SetPoint_Speed01Hz;
		  int16_t SetPoint_Iq;
		  int16_t SetPoint_Id; 
		  int16_t SetPoint_TSUP;
		  int16_t SetPoint_SPUP;
		  int16_t HallPhaseShift;
		 } Command_t;	 
	
		//-------- Frame Based
		void Write(uint8_t *data, uint8_t len);
		void Read(uint8_t *data, uint8_t *len);
	
		bool WaitRead(int timeout)
		{
			int stoptime=watch.read_us()+ timeout;
			while( !_FillSignal &&  (watch.read_us() < stoptime ) );
			if(_FillSignal)
			{
				_FillSignal=false;
				return true;
			}
			return false;
		}
		
		bool Info(){
			//int len=sizeof(FrameProtocol::ReportInfo_t)+3;
			FP.BuildCommandInfo();
			scom.putc(FP.FrameData.Code);
			scom.putc(FP.FrameData.Size);
			_FillSignal=false;
			scom.read((uint8_t *)&buf,35, event_callback_t(this,&MotorCom::_TransferCompleted_cb_generic) ,SERIAL_EVENT_RX_COMPLETE);
			scom.write((const uint8_t *) &FP.FrameData.Buffer,FP.FrameData.Size+1,NULL,0);
			if(WaitRead(10000)) return true;
			scom.abort_read();
			return false;
		};
				
		void MPInfo(){
			FP.BuildCommandMPInfo();
			scom.putc(FP.FrameData.Code);
			scom.putc(FP.FrameData.Size);
			scom.read((uint8_t *)&buf,sizeof(FrameProtocol::ReportVars_t)+3, event_callback_t(this,&MotorCom::_TransferCompleted_cb) ,SERIAL_EVENT_RX_COMPLETE);
			scom.write((const uint8_t *) &FP.FrameData.Buffer,FP.FrameData.Size+1,NULL,0);
		};
		
		void MPIDMSetSpeed(int16_t speed)
		{
			Command_t Command;
			Command.Cmd=1;
			Command.Mode=0;
			Command.SetPoint_Speed01Hz=speed;
			
			FP.BuildCommandIDM((uint8_t *) &Command,sizeof(Command));
			
			//FP.BuildCommandIDM((uint8_t *) &speed,2);
			
			scom.putc(FP.FrameData.Code);
			scom.putc(FP.FrameData.Size);
			scom.read((uint8_t *)&buf,sizeof(FrameProtocol::ReportVars_t)+3, event_callback_t(this,&MotorCom::_TransferCompleted_cb) ,SERIAL_EVENT_RX_COMPLETE);
			scom.write((const uint8_t *) &FP.FrameData.Buffer,FP.FrameData.Size+1,NULL,0);
		};
		
		void MPIDMSetPoint(int16_t speed,uint8_t cmd,uint8_t mode)
		{
			Command_t Command;
			Command.Cmd=cmd;
			Command.Mode=mode;
			Command.SetPoint_Speed01Hz=speed;
			Command.SetPoint_Iq=0;
			Command.SetPoint_Id=0;
			Command.SetPoint_TSUP=0;
			Command.SetPoint_SPUP=0;
			
			FP.BuildCommandIDM((uint8_t *) &Command,sizeof(Command));
			
			//FP.BuildCommandIDM((uint8_t *) &speed,2);
			
			scom.putc(FP.FrameData.Code);
			scom.putc(FP.FrameData.Size);
			scom.read((uint8_t *)&buf,sizeof(FrameProtocol::ReportVars_t)+3, event_callback_t(this,&MotorCom::_TransferCompleted_cb) ,SERIAL_EVENT_RX_COMPLETE);
			scom.write((const uint8_t *) &FP.FrameData.Buffer,FP.FrameData.Size+1,NULL,0);
		};
		
		void MPIDMSetPointEx(int16_t speed,uint8_t cmd,uint8_t mode,int16_t VQ,int16_t Tsup, int16_t Spup, int16_t HallPhaseShift=0)
		{
			Command_t Command;
			Command.Cmd=cmd;
			Command.Mode=mode;
			Command.SetPoint_Speed01Hz=speed;
			Command.SetPoint_Iq=VQ;
			Command.SetPoint_Id=0;
			Command.SetPoint_TSUP=Tsup;
			Command.SetPoint_SPUP=Spup;
			if( HallPhaseShift )
			{
				Command.HallPhaseShift= DefaultHallPhaseShift + HallPhaseShift;
			}
			else
			{
				Command.HallPhaseShift= DefaultHallPhaseShift + HallPhaseShiftCorrection; 
			}
			
			FP.BuildCommandIDM((uint8_t *) &Command,sizeof(Command));
			
			//FP.BuildCommandIDM((uint8_t *) &speed,2);
			
			scom.putc(FP.FrameData.Code);
			scom.putc(FP.FrameData.Size);
			scom.read((uint8_t *)&buf,sizeof(FrameProtocol::ReportVars_t)+3, event_callback_t(this,&MotorCom::_TransferCompleted_cb) ,SERIAL_EVENT_RX_COMPLETE);
			scom.write((const uint8_t *) &FP.FrameData.Buffer,FP.FrameData.Size+1,NULL,0);
		};
		
		void MPIDMSetPointEx(int16_t speed,uint8_t cmd,uint8_t mode,int16_t VQ,int16_t Tsup, int16_t Spup, int16_t HallPhaseShift,int16_t VD)
		{
			Command_t Command;
			Command.Cmd=cmd;
			Command.Mode=mode;
			Command.SetPoint_Speed01Hz=speed;
			Command.SetPoint_Iq=VQ;
			Command.SetPoint_Id=VD;
			Command.SetPoint_TSUP=Tsup;
			Command.SetPoint_SPUP=Spup;
			if( HallPhaseShift )
			{
				Command.HallPhaseShift= DefaultHallPhaseShift + HallPhaseShift;
			}
			else
			{
				Command.HallPhaseShift= DefaultHallPhaseShift + HallPhaseShiftCorrection; 
			}
			
			FP.BuildCommandIDM((uint8_t *) &Command,sizeof(Command));
			
			//FP.BuildCommandIDM((uint8_t *) &speed,2);
			
			scom.putc(FP.FrameData.Code);
			scom.putc(FP.FrameData.Size);
			scom.read((uint8_t *)&buf,sizeof(FrameProtocol::ReportVars_t)+3, event_callback_t(this,&MotorCom::_TransferCompleted_cb) ,SERIAL_EVENT_RX_COMPLETE);
			scom.write((const uint8_t *) &FP.FrameData.Buffer,FP.FrameData.Size+1,NULL,0);
		};
		
		FrameProtocol::ReportVars_t  MPInfoReport;
		FrameProtocol::ReportInfo_t  InfoReport;
		
		/**Defines the methods available over RPC*/
		virtual const struct rpc_method *get_rpc_methods();


		void _TransferCompleted_cb_generic(int events) {
			if (events & SERIAL_EVENT_RX_COMPLETE){
				FrameProtocol::PFrameData_t pFrameData;
				pFrameData= (FrameProtocol::PFrameData_t ) &buf;
				if(FP.IsFrameValid(pFrameData))
				{	
					_FillSignal=true;
					memcpy(&InfoReport,&buf[2], sizeof(InfoReport));
					Readable=true;
					ComErrorCount=0;
				}
			}
		}
		
		
		void _TransferCompleted_cb(int events) {
			if (events & SERIAL_EVENT_RX_COMPLETE){
				if(FP.IsFrameValid((FrameProtocol::PFrameData_t ) &buf))
				{	
					_FillSignal=true;
					memcpy(&MPInfoReport,&buf[2], sizeof(MPInfoReport));
					Readable=true;
					ComErrorCount=0;
				}
			}
		}
		
		void ClearCom(void)
		{
			scom.abort_read();
		}
		
		SerialBLHost  BLCmd;
		volatile bool Readable;
		volatile int ComErrorCount;
		volatile int16_t HallPhaseShiftCorrection;
		
	private:
		Serial scom;
		char buf[64];
		volatile bool _FillSignal;
		const static int16_t DefaultHallPhaseShift=21845;
			
		FrameProtocol FP;
		Timer watch;
		//int _count;
		//event_callback_t  serialEventCb;
		
 };

//============================================================================//
/* Exported variables --------------------------------------------------------*/
//============================================================================//

//============================================================================//
/* Exported function prototypes ----------------------------------------------*/
//============================================================================//

  
#endif /* __MOTOR_COM_H */
//============================================================================//
/*------------------------------- END OF FILE --------------------------------*/
//============================================================================//
