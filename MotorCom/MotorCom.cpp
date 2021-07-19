//============================================================================//
//
//  File:   MotorCom.cpp
//  Rev:    1.0
//  Date:	10/012/2017
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
#include "MotorCom.h"
#include "FCP.h"
using namespace std;
#pragma diag_suppress 1299

MotorCom::MotorCom() : RPC("MC"), scom(PC_10,PC_11,115200),BLCmd(&scom)
{
	HallPhaseShiftCorrection=0;
}

MotorCom::MotorCom(PinName tx, PinName rx, const char *name, int baudrate) : RPC(name), scom(tx,rx,baudrate),BLCmd(&scom)
{
	watch.start();
	HallPhaseShiftCorrection=0;
	//serialEventCb.attach(&_TransferCompleted_cb);
}


void MotorCom::Puts(const char *s)
{
	scom.printf(s);
}

const char* MotorCom::Gets(void)
{
	char idx=0;
	buf[idx] = 0;
	while ((scom.readable())&&idx<64)
	{
		buf[idx++] = scom.getc();
	}
	return buf;
}

const char* MotorCom::VC(void)
{
	
	buf[0]=0;
	uint8_t len=32;
	scom.format( 8, SerialBase::Even, 1);
	//_BootloadCS=true;
	if(BLCmd.InitChip()){
		
		//BLCmd.GenericCMD(BLCmd.GET);
		//if(len=BLCmd.StartRead(50000,20,buf))
		
		if(BLCmd.cmdGET(buf,&len))
		{
			
			strncpy(buf,"OK",32);
		}else
		{
			strncpy(buf,"ERR",32);
		}
	}
	else
    {
		strncpy(buf,"NACK",32);
	}	
	scom.format( 8, SerialBase::None, 1);	
	return buf;
}

vector<char> MotorCom::GetID(void)
{
	vector<char> report;
	buf[0]=0;
	uint8_t len=32;
	scom.format( 8, SerialBase::Even, 1);
	//_BootloadCS=true;
	if(BLCmd.InitChip()){		
		if(BLCmd.cmdGetID(buf,&len))
		{
			for (int i=0; i<len; i++) 
			{
				report.push_back(buf[i]);
			}
			
		}else
		{
			report.push_back(0);
		}
	}
	else
    {
		report.push_back(0);
	}	
	scom.format( 8, SerialBase::None, 1);	
	return report;
}


vector<char> MotorCom::Get(void)
{
	vector<char> report;
	buf[0]=0;
	uint8_t len=32;
	scom.format( 8, SerialBase::Even, 1);
	//_BootloadCS=true;
	if(BLCmd.InitChip()){		
		if(BLCmd.cmdGET(buf,&len))
		{
			for (int i=0; i<len; i++) 
			{
				report.push_back(buf[i]);
			}
			
		}else
		{
			report.push_back(0);
		}
	}
	else
    {
		report.push_back(0);
	}	
	scom.format( 8, SerialBase::None, 1);	
	return report;
}


vector<char> MotorCom::GetVersion(void)
{
	vector<char> report;
	buf[0]=0;
	uint8_t len=32;
	scom.format( 8, SerialBase::Even, 1);
	//_BootloadCS=true;
	if(BLCmd.InitChip()){		
		if(BLCmd.cmdGetVersion(buf,&len))
		{
			for (int i=0; i<len; i++) 
			{
				report.push_back(buf[i]);
			}
			
		}else
		{
			report.push_back(0);
		}
	}
	else
    {
		report.push_back(0);
	}	
	scom.format( 8, SerialBase::None, 1);	
	return report;
}


const char* MotorCom::GO(uint32_t address)
{
	buf[0]=0;
	scom.format( 8, SerialBase::Even, 1);
	//_BootloadCS=true;
	if(BLCmd.InitChip()){		
		if(BLCmd.cmdGO(address))
		{
			strncpy(buf,"OK",32);
		}else
		{
			strncpy(buf,"ERR",32);
		}
	}
	else
    {
		strncpy(buf,"NACK",32);
	}	
	
	scom.format( 8, SerialBase::None, 1);	
	return buf;
}

vector<char> MotorCom::ReadMem(uint32_t address, int length)
{
	vector<char> report;
	buf[0]=0;
	uint8_t len=32;
	scom.format( 8, SerialBase::Even, 1);
	//_BootloadCS=true;
	if(BLCmd.InitChip()){		
		if(BLCmd.cmdReadMemory(address,length, buf,&len))
		{
			for (int i=0; i<len; i++) 
			{
				report.push_back(buf[i]);
			}
			
		}else
		{
			report.push_back(len);
			report.push_back(buf[0]);
		}
	}
	else
    {
		report.push_back(0);
	}	
	scom.format( 8, SerialBase::None, 1);	
	return report;
}

const char* MotorCom::CalcCRC(void)
{
	buf[0]=0;
	uint32_t crcvalue;
	scom.format( 8, SerialBase::Even, 1);
	//_BootloadCS=true;
	if(BLCmd.InitChip()){		
		if(BLCmd.cmdComputeCRC(&crcvalue))
		{
			strncpy(buf,"OK",32);
		}else
		{
			strncpy(buf,"ERR",32);
		}
	}
	else
    {
		strncpy(buf,"NACK",32);
	}	
	scom.format( 8, SerialBase::None, 1);	
	return buf;
}

	
const char* MotorCom::EraseAll()
{
	buf[0]=0;
	scom.format( 8, SerialBase::Even, 1);
	//_BootloadCS=true;
	if(BLCmd.InitChip()){		
		if(BLCmd.cmdEraseMemoryExtendedAll())
		{
			strncpy(buf,"OK",32);
		}else
		{
			strncpy(buf,"ERR",32);
		}
	}
	else
    {
		strncpy(buf,"NACK",32);
	}	
	
	scom.format( 8, SerialBase::None, 1);	
	return buf;
}

const char*  MotorCom::WriteBlock(uint32_t address, int blocknum)
{
	char *ImageBlock;

	ImageBlock=(char *)BLCmd.GetFirmwareImageBlock(blocknum);
	
	if(ImageBlock)
	{
	
			scom.format( 8, SerialBase::Even, 1);
			//_BootloadCS=true;
			if(BLCmd.InitChip()){		
				if(BLCmd.cmdWriteMemory(address+blocknum*256,255, ImageBlock))
				{
					strncpy(buf,"OK",32);
				}else
				{
					strncpy(buf,"ERR",32);
				}
			}
			else
			{
				strncpy(buf,"NACK",32);
			}	
			scom.format( 8, SerialBase::None, 1);	
	}
	else
		strncpy(buf,"UNK",32);
	
	return buf;
}

const char*  MotorCom::WriteAllBlock()
{
	char *ImageBlock;
	int blocknum;
	scom.format( 8, SerialBase::Even, 1);
	//_BootloadCS=true;
	if(BLCmd.InitChip())
	{
		for (blocknum=0;blocknum<128;blocknum++)
		{
			ImageBlock=(char *)BLCmd.GetFirmwareImageBlock(blocknum);				
			if(ImageBlock)
			{	
				if(BLCmd.cmdWriteMemory(0x8000000+blocknum*256,255, ImageBlock))
				{
					
					continue;
				}else
				{
					strncpy(buf,"ERR",32);
					break;
				}
			}
			else
			{
				strncpy(buf,"OK",32);
				break;
			}
		}
	}	
	else
	{
		strncpy(buf,"NACK",32);
	}	
	scom.format( 8, SerialBase::None, 1);		
	return buf;
}


/**Defines the methods available over RPC*/
const rpc_method *MotorCom::get_rpc_methods()
{
	static const rpc_method rpc_methods[] = {
		{ "Puts", rpc_method_caller<MotorCom, const char*, &MotorCom::Puts> },
		{ "Gets", rpc_method_caller<const char*, MotorCom, &MotorCom::Gets> },
		{ "Get", rpc_method_caller<vector<char> , MotorCom, &MotorCom::Get> },		
		{ "GetID", rpc_method_caller<vector<char> , MotorCom, &MotorCom::GetID> },
		{ "GetVersion", rpc_method_caller<vector<char> , MotorCom, &MotorCom::GetVersion> },
		{ "FWVersion", rpc_method_caller<const char* , MotorCom, &MotorCom::FWVersion> },		
		{ "ReadMem", rpc_method_caller<vector<char> , MotorCom, uint32_t, int, &MotorCom::ReadMem> },
		{ "EraseAll", rpc_method_caller<const char*, MotorCom, &MotorCom::EraseAll> },
		{ "WriteBlock", rpc_method_caller<const char*, MotorCom,uint32_t, int, &MotorCom::WriteBlock> },
		{ "WriteAllBlock", rpc_method_caller<const char*, MotorCom,&MotorCom::WriteAllBlock> },
		{ "GO", rpc_method_caller<const char*, MotorCom, uint32_t, &MotorCom::GO> },
		{ "CRC", rpc_method_caller<const char*, MotorCom, &MotorCom::CalcCRC> },
		{ "VC", rpc_method_caller<const char*, MotorCom, &MotorCom::VC> },
		RPC_METHOD_END
	};
	return rpc_methods;
}

template<> void Reply::putData< vector<char> >(vector<char> v)
{
    separator();
    for (int i=0; i<(v.size()); i++) 
    {
        reply += sprintf(reply, "%02X", v[i]);
        //if (i<(v.size()-1)) reply += sprintf(reply, ",");
    }
}

template<> uint32_t Arguments::getArg<uint32_t>(void) {
    index++;
    return (uint32_t)strtol(argv[index], NULL, 16);
}

template<> uint16_t Arguments::getArg<uint16_t>(void) {
    index++;
    return (uint16_t)strtol(argv[index], NULL, 16);
}

//============================================================================//
/*------------------------------- END OF FILE --------------------------------*/
//============================================================================//

