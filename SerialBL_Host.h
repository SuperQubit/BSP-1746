//============================================================================//
//
//  File:   SerialBL_Host.h
//  Rev:    1.0
//  Date:	12/03/2018
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
#ifndef __SBL_H
#define __SBL_H


//============================================================================//
/* Includes ------------------------------------------------------------------*/
//============================================================================//
#include "mbed.h"
#include "SerialOw.h"

//============================================================================//
/* Exported Types ------------------------------------------------------------*/
//============================================================================//
class SerialBLHost
{
	public:
		SerialBLHost(Serial *scom);

		//-------------- BOOTLOADER PROTOCOL
		enum SIGN
		{
			ACK = 0x79,
			NACK= 0x1F,	
			BUSY= 0x76
		};
		
	
		enum CMD
		{
			INIT = 0x7F,
			GET = 0x00,		
			GVR = 0x01,
			GID = 0x02,
			READMEM = 0x11,
			GO = 0x21,
			WRITEMEM = 0x31,
			WRITEMEM_NS = 0x32,
			ERASE = 0x43,
			ERASE_EX= 0x44,
			ERASE_EX_NS= 0x45,
			WRITE_PROT = 0x63,
			WRITE_PROT_NS = 0x64,
			WRITE_UNPROT = 0x73,
			WRITE_UNPROT_NS = 0x74,
			READOUT_PROT = 0x82,
			READOUT_PROT_NS = 0x83,
			READOUT_UNPROT = 0x92,
			READOUT_UNPROT_NS = 0x93,
			COMPUTE_CRC = 0xA1,
			ERROR = 0xFF
		};
			
		typedef struct  
		{
			uint8_t version;
			uint8_t get;
			uint8_t gvr;
			uint8_t gid;
			uint8_t rm;
			uint8_t go;
			uint8_t wm;
			uint8_t er; /* this may be extended erase */
			uint8_t wp;
			uint8_t uw;
			uint8_t rp;
			uint8_t ur;
			uint8_t	crc;
		} rDB_GET;
		

		void SetSerialFormat(int bits=8, SerialBase::Parity parity=SerialBase::None, int stop_bits=1)
		{
			pcom->format(bits,parity,stop_bits);
		};
		void SetSerialBaud(int baud=115200){ pcom->baud(baud);};
		
		void Reset();
		bool InitChip();
		bool cmdGET(char *buffer,uint8_t *len);
		bool cmdGetVersion(char *buffer,uint8_t *len);
		bool cmdGetID(char *buffer,uint8_t *len);
		bool cmdReadMemory(uint32_t address, uint8_t length, char *buffer,uint8_t *len);
		bool cmdGO(uint32_t address);
		bool cmdWriteMemory(uint32_t address, uint8_t length, char *buffer);
		bool cmdEraseMemory(uint8_t numpages, char *buffer,uint8_t *len);
		bool cmdEraseMemoryExtended(uint16_t numpages, char *buffer,uint8_t *len);
		bool cmdEraseMemoryExtendedAll();
		bool cmdEraseMemoryAll();
		bool cmdComputeCRC(uint32_t *crc);
		
		
		int GetChar(bool init);
		int StartRead(int timeout,int len, char *buffer,int lenfix=0, bool NoWaitAck=false);
		bool WaitAck(int timeout);
		bool WaitFixedPayload(int timeout,int lenfix,char *buffer,int *len);
		bool WaitForAck(int timeout);
		bool GenericCMD(char  cmd);
		bool GenericQuery(char  cmd, char *buf, uint8_t *len,int timeout);

		const char *GetFirmwareImageManifest(void){return pCDFWImageManifest;};
		const unsigned char *GetFirmwareImage(void){return pCDFWImage;};
		const unsigned char *GetFirmwareImageBlock(int block){
			int offset;
			int ImageLen= ((CDFWImageLenght >> 8) +1)<<8;
			offset= block*256;
			if(offset < ImageLen ) return ( offset+pCDFWImage );
			return 0;
		};
		
		

		void _TransferCompleted_cb(int events) {
			if (events & SERIAL_EVENT_RX_COMPLETE)
			{
				_FillSignal=true;
			}
		}
		
	private:
	  Serial *pcom;
		serial_t * _ser;
		Timer crono;
		volatile bool _FillSignal;
	    bool _encode_addr(uint32_t addr,char *buffer);
		const unsigned char *pCDFWImage;
		size_t CDFWImageLenght;
		const  char * pCDFWImageManifest;
		uint32_t CDFWImageCrc;
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
