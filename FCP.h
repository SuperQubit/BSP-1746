//============================================================================//
//
//  File:   FPC.h
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
#ifndef __FCP_H
#define __FCP_H


//============================================================================//
/* Includes ------------------------------------------------------------------*/
//============================================================================//
#include "mbed.h"
	 
//============================================================================//
/* Exported Types ------------------------------------------------------------*/
//============================================================================//
class FrameProtocol
{
	public:
		FrameProtocol(){FrameData.Size=0;};
	
	#define RET_BUFFER 0x02
	#define ACK_NOERROR 0xF0
	#define ACK_ERROR   0xFF
	#define ATR_FRAME_START 0xE0
	
	#define FRAME_ERROR_NONE              0x00
	#define FRAME_ERROR_TRANSFER_ONGOING  0x01
	#define FRAME_ERROR_WAITING_TRANSFER  0x02 
	#define FRAME_ERROR_INVALID_PARAMETER 0x03 
	#define FRAME_ERROR_TIME_OUT          0x04
	#define FRAME_ERROR_INVALID_FRAME     0x05

	#define FRAME_MAX_BUFFER_LENGTH  0x80
	#define FRAME_HEADER_SIZE        0x02
	#define FRAME_CRC_SIZE           0x01
	#define FRAME_MAX_SIZE           (FRAME_HEADER_SIZE + FRAME_MAX_BUFFER_LENGTH + FRAME_CRC_SIZE)

	#define FRAME_ACK_SIZE           0x01
	#define FRAME_ACK_CODE           0xFF

		//-------------- MOTOR PROTOCOL
		typedef enum
		{
			MC_PROTOCOL_CODE_NONE = 0,
			MC_PROTOCOL_CODE_SET_REG,			/* 0x01 */
			MC_PROTOCOL_CODE_GET_REG,   		/* 0x02 */ 
			MC_PROTOCOL_CODE_EXECUTE_CMD,  		/* 0x03 */
			MC_PROTOCOL_CODE_STORE_TOADDR, 		/* 0x04 */
			MC_PROTOCOL_CODE_LOAD_FROMADDR,		/* 0x05 */
			MC_PROTOCOL_CODE_GET_BOARD_INFO,	/* 0x06 */
			MC_PROTOCOL_CODE_SET_RAMP,       	/* 0x07 */
			MC_PROTOCOL_CODE_GET_REVUP_DATA,	/* 0x08 */
			MC_PROTOCOL_CODE_SET_REVUP_DATA,	/* 0x09 */
			MC_PROTOCOL_CODE_SET_CURRENT_REF,	/* 0x0A */
			MC_PROTOCOL_CODE_GET_MP_INFO,		/* 0x0B */
			MC_PROTOCOL_CODE_GET_REPORT,		/* 0x0C */
			MC_PROTOCOL_CODE_IDM_COMMAND		/* 0x0D */
		} MC_PROTOCOL_CODE;
		
	
		typedef enum
		{
			MC_PROTOCOL_CMD_NONE = 0,
			MC_PROTOCOL_CMD_START_MOTOR,		/* 0x01 */
			MC_PROTOCOL_CMD_STOP_MOTOR,   		/* 0x02 */ 
			MC_PROTOCOL_CMD_STOP_RAMP,  		/* 0x03 */
			MC_PROTOCOL_CMD_RESET, 				/* 0x04 */
			MC_PROTOCOL_CMD_PING,				/* 0x05 */
			MC_PROTOCOL_CMD_START_STOP,			/* 0x06 */
			MC_PROTOCOL_CMD_FAULT_ACK,       	/* 0x07 */
			MC_PROTOCOL_CMD_ENCODER_ALIGN,		/* 0x08 */
			MC_PROTOCOL_CMD_IQDREF_CLEAR,		/* 0x09 */
			MC_PROTOCOL_CMD_PFC_ENABLE,			/* 0x0A */
			MC_PROTOCOL_CMD_PFC_DISABLE,		/* 0x0B */
			MC_PROTOCOL_CMD_PFC_FAULT_ACK,		/* 0x0C */
			MC_PROTOCOL_CMD_SC_START,			/* 0x0D */
			MC_PROTOCOL_CMD_SC_STOP				/* 0x0E */
		} MC_PROTOCOL_CMD;
			
		typedef enum
		{
			ERROR_NONE = 0,
			ERROR_BAD_FRAME_ID,         /* 0x01 */
			ERROR_CODE_SET_READ_ONLY,   /* 0x02 */ 
			ERROR_CODE_GET_WRITE_ONLY,  /* 0x03 */
			ERROR_CODE_NO_TARGET_DRIVE, /* 0x04 */
			ERROR_CODE_WRONG_SET,       /* 0x05 */
			ERROR_CODE_CMD_ID,          /* 0x06 */
			ERROR_CODE_WRONG_CMD,       /* 0x07 */
			ERROR_CODE_OVERRUN,         /* 0x08 */
			ERROR_CODE_TIMEOUT,         /* 0x09 */
			ERROR_CODE_BAD_CRC,         /* 0x0A */
			ERROR_BAD_MOTOR_SELECTED,   /* 0x0B */
			ERROR_MP_NOT_ENABLED        /* 0x0C */
		} MC_PROTOCOL_ERROR_CODE;
		
		typedef enum {
		  MC_PROTOCOL_REG_TARGET_MOTOR,          /* 0   */
		  MC_PROTOCOL_REG_FLAGS,                 /* 1   */
		  MC_PROTOCOL_REG_STATUS,                /* 2   */
		  MC_PROTOCOL_REG_CONTROL_MODE,          /* 3   */
		  MC_PROTOCOL_REG_SPEED_REF,             /* 4   */
		  MC_PROTOCOL_REG_SPEED_KP,              /* 5   */
		  MC_PROTOCOL_REG_SPEED_KI,              /* 6   */
		  MC_PROTOCOL_REG_SPEED_KD,              /* 7   */
		  MC_PROTOCOL_REG_TORQUE_REF,            /* 8   */
		  MC_PROTOCOL_REG_TORQUE_KP,             /* 9   */
		  MC_PROTOCOL_REG_TORQUE_KI,             /* 10  */
		  MC_PROTOCOL_REG_TORQUE_KD,             /* 11  */
		  MC_PROTOCOL_REG_FLUX_REF,              /* 12  */
		  MC_PROTOCOL_REG_FLUX_KP,               /* 13  */
		  MC_PROTOCOL_REG_FLUX_KI,               /* 14  */
		  MC_PROTOCOL_REG_FLUX_KD,               /* 15  */
		  MC_PROTOCOL_REG_OBSERVER_C1,           /* 16  */
		  MC_PROTOCOL_REG_OBSERVER_C2,           /* 17  */
		  MC_PROTOCOL_REG_OBSERVER_CR_C1,        /* 18  */
		  MC_PROTOCOL_REG_OBSERVER_CR_C2,        /* 19  */
		  MC_PROTOCOL_REG_PLL_KI,                /* 20  */
		  MC_PROTOCOL_REG_PLL_KP,                /* 21  */
		  MC_PROTOCOL_REG_FLUXWK_KP,             /* 22  */
		  MC_PROTOCOL_REG_FLUXWK_KI,             /* 23  */
		  MC_PROTOCOL_REG_FLUXWK_BUS,            /* 24  */
		  MC_PROTOCOL_REG_BUS_VOLTAGE,           /* 25  */
		  MC_PROTOCOL_REG_HEATS_TEMP,            /* 26  */
		  MC_PROTOCOL_REG_MOTOR_POWER,           /* 27  */
		  MC_PROTOCOL_REG_DAC_OUT1,              /* 28  */
		  MC_PROTOCOL_REG_DAC_OUT2,              /* 29  */
		  MC_PROTOCOL_REG_SPEED_MEAS,            /* 30  */
		  MC_PROTOCOL_REG_TORQUE_MEAS,           /* 31  */
		  MC_PROTOCOL_REG_FLUX_MEAS,             /* 32  */
		  MC_PROTOCOL_REG_FLUXWK_BUS_MEAS,       /* 33  */
		  MC_PROTOCOL_REG_RUC_STAGE_NBR,         /* 34  */
		  MC_PROTOCOL_REG_I_A,                   /* 35  */
		  MC_PROTOCOL_REG_I_B,                   /* 36  */
		  MC_PROTOCOL_REG_I_ALPHA,               /* 37  */
		  MC_PROTOCOL_REG_I_BETA,                /* 38  */
		  MC_PROTOCOL_REG_I_Q,                   /* 39  */
		  MC_PROTOCOL_REG_I_D,                   /* 40  */
		  MC_PROTOCOL_REG_I_Q_REF,               /* 41  */
		  MC_PROTOCOL_REG_I_D_REF,               /* 42  */
		  MC_PROTOCOL_REG_V_Q,                   /* 43  */
		  MC_PROTOCOL_REG_V_D,                   /* 44  */
		  MC_PROTOCOL_REG_V_ALPHA,               /* 45  */
		  MC_PROTOCOL_REG_V_BETA,                /* 46  */
		  MC_PROTOCOL_REG_MEAS_EL_ANGLE,         /* 47  */
		  MC_PROTOCOL_REG_MEAS_ROT_SPEED,        /* 48  */
		  MC_PROTOCOL_REG_OBS_EL_ANGLE,          /* 49  */
		  MC_PROTOCOL_REG_OBS_ROT_SPEED,         /* 50  */
		  MC_PROTOCOL_REG_OBS_I_ALPHA,           /* 51  */
		  MC_PROTOCOL_REG_OBS_I_BETA,            /* 52  */
		  MC_PROTOCOL_REG_OBS_BEMF_ALPHA,        /* 53  */
		  MC_PROTOCOL_REG_OBS_BEMF_BETA,         /* 54  */
		  MC_PROTOCOL_REG_OBS_CR_EL_ANGLE,       /* 55  */
		  MC_PROTOCOL_REG_OBS_CR_ROT_SPEED,      /* 56  */
		  MC_PROTOCOL_REG_OBS_CR_I_ALPHA,        /* 57  */
		  MC_PROTOCOL_REG_OBS_CR_I_BETA,         /* 58  */
		  MC_PROTOCOL_REG_OBS_CR_BEMF_ALPHA,     /* 59  */
		  MC_PROTOCOL_REG_OBS_CR_BEMF_BETA,      /* 60  */
		  MC_PROTOCOL_REG_DAC_USER1,             /* 61  */
		  MC_PROTOCOL_REG_DAC_USER2,             /* 62  */
		  MC_PROTOCOL_REG_MAX_APP_SPEED,         /* 63  */
		  MC_PROTOCOL_REG_MIN_APP_SPEED,         /* 64  */
		  MC_PROTOCOL_REG_IQ_SPEEDMODE,          /* 65  */
		  MC_PROTOCOL_REG_EST_BEMF_LEVEL,        /* 66  */
		  MC_PROTOCOL_REG_OBS_BEMF_LEVEL,        /* 67  */
		  MC_PROTOCOL_REG_EST_CR_BEMF_LEVEL,     /* 68  */
		  MC_PROTOCOL_REG_OBS_CR_BEMF_LEVEL,     /* 69  */
		  MC_PROTOCOL_REG_FF_1Q,                 /* 70  */
		  MC_PROTOCOL_REG_FF_1D,                 /* 71  */
		  MC_PROTOCOL_REG_FF_2,                  /* 72  */
		  MC_PROTOCOL_REG_FF_VQ,                 /* 73  */
		  MC_PROTOCOL_REG_FF_VD,                 /* 74  */
		  MC_PROTOCOL_REG_FF_VQ_PIOUT,           /* 75  */
		  MC_PROTOCOL_REG_FF_VD_PIOUT,           /* 76  */
		  MC_PROTOCOL_REG_PFC_STATUS,            /* 77  */
		  MC_PROTOCOL_REG_PFC_FAULTS,            /* 78  */
		  MC_PROTOCOL_REG_PFC_DCBUS_REF,         /* 79  */
		  MC_PROTOCOL_REG_PFC_DCBUS_MEAS,        /* 80  */
		  MC_PROTOCOL_REG_PFC_ACBUS_FREQ,        /* 81  */
		  MC_PROTOCOL_REG_PFC_ACBUS_RMS,         /* 82  */
		  MC_PROTOCOL_REG_PFC_I_KP,              /* 83  */
		  MC_PROTOCOL_REG_PFC_I_KI,              /* 84  */
		  MC_PROTOCOL_REG_PFC_I_KD,              /* 85  */
		  MC_PROTOCOL_REG_PFC_V_KP,              /* 86  */
		  MC_PROTOCOL_REG_PFC_V_KI,              /* 87  */
		  MC_PROTOCOL_REG_PFC_V_KD,              /* 88  */
		  MC_PROTOCOL_REG_PFC_STARTUP_DURATION,  /* 89  */
		  MC_PROTOCOL_REG_PFC_ENABLED,           /* 90  */
		  MC_PROTOCOL_REG_RAMP_FINAL_SPEED,      /* 91  */
		  MC_PROTOCOL_REG_RAMP_DURATION,         /* 92  */
		  MC_PROTOCOL_REG_HFI_EL_ANGLE,          /* 93  */
		  MC_PROTOCOL_REG_HFI_ROT_SPEED,         /* 94  */
		  MC_PROTOCOL_REG_HFI_CURRENT,           /* 95  */
		  MC_PROTOCOL_REG_HFI_INIT_ANG_PLL,      /* 96  */
		  MC_PROTOCOL_REG_HFI_INIT_ANG_SAT_DIFF, /* 97  */
		  MC_PROTOCOL_REG_HFI_PI_PLL_KP,         /* 98  */
		  MC_PROTOCOL_REG_HFI_PI_PLL_KI,         /* 99  */
		  MC_PROTOCOL_REG_HFI_PI_TRACK_KP,       /* 100 */
		  MC_PROTOCOL_REG_HFI_PI_TRACK_KI,       /* 101 */
		  MC_PROTOCOL_REG_SC_CHECK,              /* 102 */
		  MC_PROTOCOL_REG_SC_STATE,              /* 103 */
		  MC_PROTOCOL_REG_SC_RS,                 /* 104 */
		  MC_PROTOCOL_REG_SC_LS,                 /* 105 */
		  MC_PROTOCOL_REG_SC_KE,                 /* 106 */
		  MC_PROTOCOL_REG_SC_VBUS,               /* 107 */
		  MC_PROTOCOL_REG_SC_MEAS_NOMINALSPEED,  /* 108 */
		  MC_PROTOCOL_REG_SC_STEPS,              /* 109 */
		  MC_PROTOCOL_REG_SPEED_KP_DIV,          /* 110 */
		  MC_PROTOCOL_REG_SPEED_KI_DIV,          /* 111 */
		  MC_PROTOCOL_REG_UID,                   /* 112 */
		  MC_PROTOCOL_REG_HWTYPE,                /* 113 */
		  MC_PROTOCOL_REG_CTRBDID,               /* 114 */
		  MC_PROTOCOL_REG_PWBDID,                /* 115 */
		  MC_PROTOCOL_REG_SC_PP,                 /* 116 */
		  MC_PROTOCOL_REG_SC_CURRENT,            /* 117 */
		  MC_PROTOCOL_REG_SC_SPDBANDWIDTH,       /* 118 */
		  MC_PROTOCOL_REG_SC_LDLQRATIO,          /* 119 */
		  MC_PROTOCOL_REG_SC_NOMINAL_SPEED,      /* 120 */
		  MC_PROTOCOL_REG_SC_CURRBANDWIDTH,      /* 121 */
		  MC_PROTOCOL_REG_SC_J,                  /* 122 */
		  MC_PROTOCOL_REG_SC_F,                  /* 123 */
		  MC_PROTOCOL_REG_SC_MAX_CURRENT,        /* 124 */
		  MC_PROTOCOL_REG_SC_STARTUP_SPEED,      /* 125 */
		  MC_PROTOCOL_REG_SC_STARTUP_ACC,        /* 126 */
		  MC_PROTOCOL_REG_SC_PWM_FREQUENCY,      /* 127 */
		  MC_PROTOCOL_REG_SC_FOC_REP_RATE,       /* 128 */
		  MC_PROTOCOL_REG_PWBDID2,               /* 129 */
		  MC_PROTOCOL_REG_SC_COMPLETED,          /* 130 */
		  MC_PROTOCOL_REG_UNDEFINED
		} Protocol_REG;
		
		typedef enum
		{
		ICLWAIT = 12,         /*!< Persistent state, the system is waiting for ICL 
								   deactivation. Is not possible to run the motor if 
								   ICL is active. Until the ICL is active the state is 
								   forced to ICLWAIT, when ICL become inactive the state 
								   is moved to IDLE */
		IDLE = 0,             /*!< Persistent state, following state can be IDLE_START 
								   if a start motor command has been given or 
								   IDLE_ALIGNMENT if a start alignment command has been 
								   given */
		IDLE_ALIGNMENT = 1,   /*!< "Pass-through" state containg the code to be executed 
								   only once after encoder alignment command. 
								   Next states can be ALIGN_CHARGE_BOOT_CAP or 
								   ALIGN_OFFSET_CALIB according the configuration. It 
								   can also be ANY_STOP if a stop motor command has been 
								   given. */
		ALIGN_CHARGE_BOOT_CAP = 13,/*!< Persistent state where the gate driver boot 
								   capacitors will be charged. Next states will be 
								   ALIGN_OFFSET_CALIB. It can also be ANY_STOP if a stop 
								   motor command has been given. */
		ALIGN_OFFSET_CALIB = 14,/*!< Persistent state where the offset of motor currents 
								   measurements will be calibrated. Next state will be 
								   ALIGN_CLEAR. It can also be ANY_STOP if a stop motor 
								   command has been given. */
		ALIGN_CLEAR = 15,     /*!< "Pass-through" state in which object is cleared and 
								   set for the startup.
								   Next state will be ALIGNMENT. It can also be ANY_STOP 
								   if a stop motor command has been given. */
		ALIGNMENT = 2,        /*!< Persistent state in which the encoder are properly 
								   aligned to set mechanical angle, following state can 
								   only be ANY_STOP */
		IDLE_START = 3,       /*!< "Pass-through" state containg the code to be executed
								   only once after start motor command. 
								   Next states can be CHARGE_BOOT_CAP or OFFSET_CALIB 
								   according the configuration. It can also be ANY_STOP 
								   if a stop motor command has been given. */
		CHARGE_BOOT_CAP = 16, /*!< Persistent state where the gate driver boot 
								   capacitors will be charged. Next states will be 
								   OFFSET_CALIB. It can also be ANY_STOP if a stop motor 
								   command has been given. */
		OFFSET_CALIB = 17,    /*!< Persistent state where the offset of motor currents 
								   measurements will be calibrated. Next state will be 
								   CLEAR. It can also be ANY_STOP if a stop motor 
								   command has been given. */
		CLEAR = 18,           /*!< "Pass-through" state in which object is cleared and 
								   set for the startup.
								   Next state will be START. It can also be ANY_STOP if 
								   a stop motor command has been given. */
		START = 4,            /*!< Persistent state where the motor start-up is intended 
								   to be executed. The following state is normally 
								   START_RUN as soon as first validated speed is 
								   detected. Another possible following state is 
								   ANY_STOP if a stop motor command has been executed */
		START_RUN = 5,        /*!< "Pass-through" state, the code to be executed only 
								   once between START and RUN states it’s intended to be 
								   here executed. Following state is normally  RUN but 
								   it can also be ANY_STOP  if a stop motor command has 
								   been given */
		RUN = 6,              /*!< Persistent state with running motor. The following 
								   state is normally ANY_STOP when a stop motor command 
								   has been executed */
		ANY_STOP = 7,         /*!< "Pass-through" state, the code to be executed only 
								   once between any state and STOP it’s intended to be 
								   here executed. Following state is normally STOP */
		STOP = 8,             /*!< Persistent state. Following state is normally 
								   STOP_IDLE as soon as conditions for moving state 
								   machine are detected */
		STOP_IDLE = 9,        /*!< "Pass-through" state, the code to be executed only
								   once between STOP and IDLE it’s intended to be here 
								   executed. Following state is normally IDLE */
		FAULT_NOW = 10,       /*!< Persistent state, the state machine can be moved from
								   any condition directly to this state by 
								   STM_FaultProcessing method. This method also manage 
								   the passage to the only allowed following state that 
								   is FAULT_OVER */
		FAULT_OVER = 11       /*!< Persistent state where the application is intended to
								  stay when the fault conditions disappeared. Following 
								  state is normally STOP_IDLE, state machine is moved as 
								  soon as the user has acknowledged the fault condition. 
							  */
		} State_t;
		
		typedef enum
		{
		  MC_NO_ERROR = (uint16_t)(0x0000u),      /*!<No error.*/
		  MC_NO_FAULTS = (uint16_t)(0x0000u),     /*!<No error.*/
		  MC_FOC_DURATION = (uint16_t)(0x0001u),  /*!<Error: FOC rate to high.*/
		  MC_OVER_VOLT = (uint16_t)(0x0002u),     /*!<Error: Software over voltage.*/
		  MC_UNDER_VOLT = (uint16_t)(0x0004u),    /*!<Error: Software under voltage.*/
		  MC_OVER_TEMP = (uint16_t)(0x0008u),     /*!<Error: Software over temperature.*/
		  MC_START_UP = (uint16_t)(0x0010u),      /*!<Error: Startup failed.*/
		  MC_SPEED_FDBK = (uint16_t)(0x0020u),    /*!<Error: Speed feedback.*/
		  MC_BREAK_IN = (uint16_t)(0x0040u),      /*!<Error: Emergency input (Over current).*/
		  MC_SW_ERROR = (uint16_t)(0x0080u)       /*!<Software Error.*/
		} FaultCondition_t;
		
		typedef struct
		{
		  State_t 		   State;
		  FaultCondition_t ErrorCode; 
		  int16_t Speed01Hz;
		  int16_t AvgSpeed01Hz; 
		  int16_t Iph;
		  int16_t Toq;           
		 } ReportVars_t;

		typedef struct
		{
		  char Manifest[32];           
		} ReportInfo_t;
		 
		 typedef struct
		{
		  uint8_t Cmd;
		  uint8_t Mode; 
		  int16_t SetPoint_Speed01Hz;
		  int16_t SetPoint_Iq;
		  int16_t SetPoint_Id;   
		 } CommandVars_t;

		
		//------------- END MOTOR PROTOCOL
		
		typedef enum FrameState_e
		{
		  FCP_IDLE,
		  TRANSFERING_HEADER,
		  TRANSFERING_BUFFER,
		  TRANSFERING_CRC,
		  TRANSFER_COMPLETE,
		} FrameState_t;

		typedef enum FrameRXresult_e
		{
		  NONE=0,
		  TIMEOUT_START,
		  TIMEOUT_STOP,
		} FrameRXresult_t;
		
		static const int MAX_FRAME_SIZE=64;
		typedef struct 
		{ 
			uint8_t Code;
			uint8_t Size;  
			uint8_t Buffer[MAX_FRAME_SIZE];
			uint8_t nCRC;
		} FrameData_t, *PFrameData_t;
		
		typedef void (*PFN_TIMEOUT)();
		
		typedef struct Frame_s
		{
			FrameData_t Frame;
			FrameState_t State;
			PFN_TIMEOUT OnTimeOutByte;
			uint16_t FrameTransferError;
		} Frame_t;
		
		FrameRXresult_t ReceivingFrame(uint8_t *buffer, uint16_t size);
		uint8_t Frame_Receive(void);
		uint8_t IsFrameValid(PFrameData_t pFrame);
		
		void Build(uint8_t  MotorID, uint8_t FrameID, uint8_t *payload, uint8_t len)
		{
			FrameData.Code= MotorID | FrameID;
			FrameData.Size=len;
			if( payload != NULL)
			{
				memcpy(FrameData.Buffer,payload,len);
			}
			FrameData.nCRC= CalcCRC8(&FrameData);
			FrameData.Buffer[len]=FrameData.nCRC;
		};
		
		void BuildCommandInfo()
		{
			Build(0x40,MC_PROTOCOL_CODE_GET_BOARD_INFO,NULL,0);
		}; 
		
		void BuildCommandMPInfo()
		{
			Build(0x40,MC_PROTOCOL_CODE_GET_REPORT,NULL,0);
		}; 

		void BuildCommandIDM(uint8_t *payload, uint8_t len)
		{
			Build(0x40,MC_PROTOCOL_CODE_IDM_COMMAND,payload,len);
		};
		
		FrameData_t FrameData;
		Frame_t RXFrame; 
		
	private:
		uint8_t CalcCRC8(PFrameData_t pFrame);
	    
};

//============================================================================//
/* Exported variables --------------------------------------------------------*/
//============================================================================//

//============================================================================//
/* Exported function prototypes ----------------------------------------------*/
//============================================================================//

  
#endif /* __FCP_H */
//============================================================================//
/*------------------------------- END OF FILE --------------------------------*/
//============================================================================//
