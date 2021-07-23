//============================================================================//
//
//  File:   SerialBL_Host.cpp
//  Rev:    1.0
//  Date:	12/03/2018
//
//  Description: SERIAL BOOTLOADER HOST(STM32 AN3155)
//
//  Revision History:
//
//
//============================================================================//

//============================================================================//
/* Includes ------------------------------------------------------------------*/
//============================================================================//
#include "SerialBL_Host.h"

#include "ComodoDriveFW.h"

const unsigned char ComodoDriveBinaryImage[1024]={0};
const unsigned long ComodoDriveBinaryImage_length=32768;
const char ComodoDriveImageManifest[32]= "533Comodo HFOC 2.13 SDK";

#define msec(x)  ( (x) * 1000)
#define sec(x)  ( (x) * 1000000)


//namespace mbed {
	
SerialBLHost::SerialBLHost(Serial *seriale){
	pcom=seriale;
	_ser=pcom->GetSerialInstance();
	pCDFWImage=ComodoDriveBinaryImage;
	CDFWImageLenght=ComodoDriveBinaryImage_length;
	pCDFWImageManifest=ComodoDriveImageManifest;
	CDFWImageCrc= 0;
	crono.start();
}


bool SerialBLHost::InitChip(){
	int cnt;
	char c;
	
	pcom->putc(INIT);
	for	(cnt=0;cnt<10;cnt++)
	{
		wait_us(10000);
		if(pcom->readable())
		{
			c=pcom->getc();
			if(c == ACK || c== NACK)
					return true;
		}
		pcom->putc(INIT);
		
	}
	return false;
}

bool SerialBLHost::GenericQuery(char  cmd, char *buf, uint8_t *len,int timeout)
{
	uint8_t paylen;
	uint8_t lenmax=*len;
	uint8_t paycount=0;
	char c;
	
	enum { WLEN=0,PAYLOAD,WEND} status=WLEN;
	
	char *ptr=buf;
	crono.reset();
	if(GenericCMD(cmd))
	{
		
		while(crono.read_us() < timeout)
		{
			wait_us(100);
			while(pcom->readable())
			{	
				c=pcom->getc();
				switch(status){
					case WLEN:
						paylen=c;
						status=PAYLOAD;
						break;
					case PAYLOAD:
						if(paycount<= paylen && paycount <= lenmax)
						{
							*ptr++= c;
						}
						else
						{
							status=WEND;
							*len=paycount;
							return true;
						}
						break;
						
					case WEND:
						*len=paycount;
						return true;
					default:
						status=WLEN;
						break;
				}
			}
		}
	}
	return false;
}


int SerialBLHost::GetChar(bool init)
{
	static int pos=0;
	char * buffer=(char *)_ser->rx_buff.buffer;
	int cpos=_ser->rx_buff.pos;
	int ans=-1;
	if(init) pos=0;
	else
	{
		if(pos >= _ser->rx_buff.length) 
			ans=-1;
		else
			if(pos < cpos)
			{
				ans=buffer[pos++];
			}
			else
			{
				ans=-1;
			}
	}
	return ans;	
}	

bool SerialBLHost::GenericCMD(char  cmd)
{
	pcom->putc(cmd);
	pcom->putc(cmd ^ 0xFF);
	return true;
}


int SerialBLHost::StartRead(int timeout,int len, char *buffer,int lenfix, bool NoWaitAck)
{
	int ans=0;
	enum { ACK=0,WLEN,PAYLOAD,WEND} status=ACK;
	int value=0;
	uint16_t paylen;
	uint16_t paycount=0;
	
	int stoptime=crono.read_us()+ timeout;
	_FillSignal=false;
	GetChar(true);
	pcom->read((uint8_t *)buffer,len, event_callback_t(this,&SerialBLHost::_TransferCompleted_cb),SERIAL_EVENT_RX_COMPLETE);			
	while( !_FillSignal && (stoptime > crono.read_us()))
	{
		if((value=GetChar(false))>=0)
		{
			switch(status){
			case ACK:
				status=WLEN;
				if(lenfix!=0)
				{
					paylen=lenfix;
					status=PAYLOAD;
				}
				break;
			case WLEN:
				paylen=(char)value;
				status=PAYLOAD;
				break;
			case PAYLOAD:
				if(paycount< paylen)
				{
					paycount++;
					
				}
				else
				{
					status=WEND;
					if(NoWaitAck)
					{
							_FillSignal=true;
							ans=paycount;
					}
					
				}
				break;
				
			case WEND:
				_FillSignal=true;
				ans=paycount;
				break;
			default:
				status=WLEN;
				break;
			}
		}
	}
	pcom->abort_read();
	return ans;
}

bool SerialBLHost::WaitAck(int timeout)
{
	bool ans=false;
	char buff[3];
	volatile int value;
	int stoptime=crono.read_us()+ timeout;
	_FillSignal=false;
	GetChar(true);
	pcom->read((uint8_t *)buff,3, event_callback_t(this,&SerialBLHost::_TransferCompleted_cb),SERIAL_EVENT_RX_COMPLETE);			
	while( !_FillSignal && (stoptime > crono.read_us()))
	{
		if((value=GetChar(false))== SerialBLHost::ACK)
		{
				_FillSignal=true;
				ans=true;
			 
		}
	}
	pcom->abort_read();
	return ans;
}


bool SerialBLHost::WaitFixedPayload(int timeout,int lenfix,char *buffer,int *len)
{
	*len=StartRead(timeout,lenfix+1, buffer,lenfix,true);
	if(*len!=lenfix) return false;
	if(buffer[0]!=ACK) return false;
	memcpy(buffer,buffer+1,lenfix);
	return true;
}

bool SerialBLHost::cmdGET(char *buffer,uint8_t *len)
{
	
		GenericCMD(GET);
		*len=StartRead(50000,16,buffer);
		if(*len==0x0B){
			if(buffer[0]==ACK && buffer[(*len)+3]==ACK)
			{
				memcpy(buffer,buffer+2,*len+2);
				(*len)++;
			}
		}
		else
			*len=0;
		
		return (*len!=0);
}

bool SerialBLHost::cmdGetVersion(char *buffer,uint8_t *len)
{
	
		GenericCMD(GVR);
		*len=StartRead(50000,16,buffer,2);
		if(*len==2){
			if(buffer[0]==ACK && buffer[(*len)+2]==ACK)
			{
				memcpy(buffer,buffer+1,*len+1);
				(*len)++;
			}
		}
		else
			*len=0;
		
		return (*len!=0);
}

bool SerialBLHost::cmdGetID(char *buffer,uint8_t *len)
{
	
		GenericCMD(GID);
		*len=StartRead(50000,16,buffer);
		if(*len==0x01){
			if(buffer[0]==ACK && buffer[(*len)+3]==ACK)
			{
				memcpy(buffer,buffer+2,*len+2);
				(*len)++;
			}
		}
		else
			*len=0;
		
		return (*len!=0);
}

bool SerialBLHost::cmdReadMemory(uint32_t address, uint8_t lengthm1, char *buffer,uint8_t *lenm1)
{

		uint8_t N;
		int reslen;
		GenericCMD(READMEM);
		buffer[0]=0x01;
		if(!WaitAck(10000))return false;
		_encode_addr(address, buffer);
		pcom->putc(buffer[0]);
		pcom->putc(buffer[1]);
		pcom->putc(buffer[2]);
		pcom->putc(buffer[3]);
		pcom->putc(buffer[4]);
		if(!WaitAck(10000)) return false;
		N = lengthm1 & 0xFF;
		pcom->putc(N);
		pcom->putc(N ^ 0xFF);
		if(!WaitFixedPayload(50000, ((int)lengthm1)+1, buffer, &reslen))return false;
		*lenm1=(uint8_t)reslen;
		return true;
}

bool SerialBLHost::cmdGO(uint32_t address)
{
		char buffer[5];
		GenericCMD(GO);
		if(!WaitAck(10000))return false;
		_encode_addr(address, buffer);
		pcom->putc(buffer[0]);
		pcom->putc(buffer[1]);
		pcom->putc(buffer[2]);
		pcom->putc(buffer[3]);
		pcom->putc(buffer[4]);
		if(!WaitAck(10000))return false;
		return true;
}

bool SerialBLHost::cmdWriteMemory(uint32_t address, uint8_t lengthm1, char *buffer)
{
		char abuf[5];	
		uint8_t chk,d;
	
		GenericCMD(WRITEMEM);
		if(!WaitAck(msec(10)))return false;
		_encode_addr(address, abuf);
		pcom->putc(abuf[0]);
		pcom->putc(abuf[1]);
		pcom->putc(abuf[2]);
		pcom->putc(abuf[3]);
		pcom->putc(abuf[4]);
		if(!WaitAck(msec(10))) return false;
		pcom->putc(lengthm1);
		chk=0xFF;
		for(int i=0; i<=lengthm1; i++ )
		{
			d=buffer[i];
			chk^=d;
			pcom->putc(d);
		}	
		pcom->putc(chk);
		if(!WaitAck(msec(10))) return false;
		return true;
}

bool SerialBLHost::cmdEraseMemory(uint8_t numpages, char *buffer,uint8_t *len)
{
	
		GenericCMD(ERASE);
		return false;
}

bool SerialBLHost::cmdEraseMemoryExtended(uint16_t numpages, char *buffer,uint8_t *len)
{
	
		GenericCMD(ERASE_EX);
		return false;
}

bool SerialBLHost::cmdEraseMemoryExtendedAll()
{
	
		GenericCMD(ERASE_EX);
		if(!WaitAck(10000))return false;
		pcom->putc(0xFF);
		pcom->putc(0xFF);
		pcom->putc(0x00);
		if(!WaitAck(sec(20)))return false;
		return true;
}



bool SerialBLHost::_encode_addr(uint32_t addr,char *buffer)
{
	char chk;
	chk=buffer[0]= (addr >>24) & 0xff;
	chk^=buffer[1]= (addr >>16) & 0xff;
	chk^=buffer[2]= (addr >>8) & 0xff;
	chk^=buffer[3]= (addr ) & 0xff;
	buffer[4]=chk;
	return true;
}

bool SerialBLHost::cmdComputeCRC(uint32_t *crc)
{
		char abuf[6];	
		//uint8_t len=5;
	
		GenericCMD(COMPUTE_CRC);
		if(!WaitAck(msec(10)))return false;
		_encode_addr(0x08000000, abuf);
		pcom->putc(abuf[0]);
		pcom->putc(abuf[1]);
		pcom->putc(abuf[2]);
		pcom->putc(abuf[3]);
		pcom->putc(abuf[4]);
		if(!WaitAck(msec(10))) return false;
		_encode_addr(0x00008000, abuf);
		pcom->putc(abuf[0]);
		pcom->putc(abuf[1]);
		pcom->putc(abuf[2]);
		pcom->putc(abuf[3]);
		pcom->putc(abuf[4]);
		if(!WaitAck(msec(10))) return false;
//		if(!WaitFixedPayload(50000, 5, abuf, &len))return false;
		*crc=(abuf[0] << 24) | (abuf[1] << 16) | (abuf[2] << 8) | abuf[3];
		return true;
}

//}


//============================================================================//
/*------------------------------- END OF FILE --------------------------------*/
//============================================================================//

