//============================================================================//
//
//  File:   FCP.cpp
//  Rev:    1.0
//  Date:	10/012/2017
//
//  Description: FRAME COMMUNICATION PROTOCOL
//
//  Revision History:
//
//
//============================================================================//

//============================================================================//
/* Includes ------------------------------------------------------------------*/
//============================================================================//
#include "FCP.h"


//namespace mbed {
	
	
uint8_t FrameProtocol::CalcCRC8(PFrameData_t pFrame)
{
  uint8_t nCRC = 0;
  uint16_t nSum = 0;
  uint8_t idx;

  if( NULL != pFrame)
  { 
    nSum += pFrame->Code;
    nSum += pFrame->Size;
    for(idx = 0; idx < (pFrame->Size); idx++)
    {
      nSum += pFrame->Buffer[idx];
    }
    nCRC = (uint8_t)(nSum & 0xFF); // Low Byte of nSum
    nCRC += (uint8_t) (nSum >> 8); // High Byte of nSum
  }

  return nCRC ;
}

uint8_t FrameProtocol::IsFrameValid(PFrameData_t pFrame)
{
  if (pFrame)
	{
		 if(pFrame->Buffer[pFrame->Size]==0) return 1;
	   return FrameProtocol::CalcCRC8(pFrame) == pFrame->Buffer[pFrame->Size];
	}
  else
    return 0;
}

FrameProtocol::FrameRXresult_t FrameProtocol::ReceivingFrame(uint8_t *buffer, uint16_t size)
{
 
	FrameRXresult_t RetVal = NONE;
  
  if (size == 1)  //First Byte received
  {
	  RetVal = TIMEOUT_START;
  }
  
  switch(RXFrame.State)
  {
    case TRANSFERING_HEADER:
    {
      if (size == FRAME_HEADER_SIZE)
      {
        //Receive DATA BUFFER
        if (RXFrame.Frame.Size > 0 )
        {
          RXFrame.State = TRANSFERING_BUFFER;
        }
        else
        {
          RXFrame.State = TRANSFERING_CRC;
        }
      }
      break;
    }
    case TRANSFERING_BUFFER:
    {
      //Receive CRC
      if ((RXFrame.Frame.Size + FRAME_HEADER_SIZE) == size)
        RXFrame.State = TRANSFERING_CRC;
      break;
    }
    case TRANSFERING_CRC:
    {
        if (size == (RXFrame.Frame.Size + FRAME_HEADER_SIZE + FRAME_CRC_SIZE))
        {
          RetVal =TIMEOUT_STOP;
		  RXFrame.State = FCP_IDLE;
         
          
          if (IsFrameValid(&RXFrame.Frame))
          {
            RXFrame.FrameTransferError = FRAME_ERROR_NONE;
			//dsap_ReceivedFrameProcess((uint8_t)FCP.V.frameTransmitionRX.Frame.Code, (uint8_t *)(FCP.V.frameTransmitionRX.Frame.Buffer), (uint8_t)FCP.V.frameTransmitionRX.Frame.Size);
          }
          else
          {
            //SendBadCRCMessage(((_CFCP)this)->Vars_str.parent);
			 RXFrame.FrameTransferError = FRAME_ERROR_INVALID_FRAME;
          }
        }
        break;
    }
	default:
		break;
  }
  return RetVal;
}

uint8_t FrameProtocol::Frame_Receive(void)
{
	uint8_t nRet;
//	uint8_t i;

	 nRet = FRAME_ERROR_TRANSFER_ONGOING;
	RXFrame.FrameTransferError = FRAME_ERROR_NONE;
	if(RXFrame.State == FCP_IDLE)
	{
		RXFrame.Frame.Code = 0x00;
		RXFrame.Frame.Size = 0x00;
		//for (i=0; i < FRAME_MAX_BUFFER_LENGTH; i++)
		//  RXFrame.Frame.Buffer[i] = 0x00;
		RXFrame.State = TRANSFERING_HEADER;
		//COM_ReceiveBuffer((uint8_t*)&(FCP.V.frameTransmitionRX).Frame, FRAME_MAX_SIZE); 
		nRet = FRAME_ERROR_WAITING_TRANSFER;    
	}	
	
  return nRet;
}


//}
//============================================================================//
/*------------------------------- END OF FILE --------------------------------*/
//============================================================================//

