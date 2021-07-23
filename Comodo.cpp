//============================================================================//
//
//  File:   bsp.cpp
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
#include "mbed.h"
#include "mbed_wait_api.h"
#include "Comodo.h"
#include "Motor.h"
#include "stm32f3xx_ll_gpio.h"
#include "LIS3DH.h"
#include <math.h>
//Motor Mot1(PB_0 ,PC_5,PB_1, PA_7,  PA_6,PA_5,PA_4,"M1");
//Motor Mot2(PC_13,PB_6,PB_4, PC_0,  PC_3,PC_2,PC_1,"M2");

Comodo BSP;
template <int PRECISION>
float roundP(float f)
{
	const float POW[]={1,10,100,1000,10000,100000};
    const float temp = POW[PRECISION];
    return floorf(f*temp)/temp;
}

Comodo::Comodo():RPC("BSP"),
VBATT(PC_4),SRESET(PB_7),SBOOT0(PB_5),
Motor1(PB_0 ,PC_5,PB_1, PA_7,  PA_6,PA_5,PA_4,"M1"),
Motor2(PC_13,PB_6,PB_4, PC_0,  PC_3,PC_2,PC_1,"M2"),
SA0(PB_14),SCS(PB_12),
Acc(/*PC_9,PA_8,*/
PB_9,PB_8,
LIS3DH_G_CHIP_ADDR,LIS3DH_DR_NR_LP_50HZ,LIS3DH_FS_8G),
_thread(osPriorityBelowNormal, 1024)
{
	BRAKE.write(0); 
	SBOOT0.write(0);
	SRESET.write(0);
	SA0.write(0);
	SCS.write(1);
	_RebootCS=true;
	HardwareLostBARACK=false;
	HardwareLostBAR=false;
	SystemFlags=0;
	_thread.start(callback(Comodo::_Runner,this));
	mVBATT=VBATT.read()*(66.0f);
	Acc.initialize (LIS3DH_G_CHIP_ADDR, LIS3DH_DR_NR_LP_50HZ, LIS3DH_FS_8G);
};


void Comodo::ClearHardwareLostBar(){
HardwareLostBARACK=true;
}

uint16_t VBARsimulate=0;

void Comodo::_Runner(const void *args){
	bool first=true;
	uint32_t VBARraw;
	uint32_t VBARmean;
	int32_t VBARac;
	uint32_t VBARvar;
	uint16_t LostBar_Counter;
	
	uint8_t Counter=0;
	Comodo *instance = (Comodo*)args;
	instance->_RebootCS=true;
	instance->HardwareLostBAR=false;
	instance->SBOOT0.write(0);
	wait_us(100);
	instance->SRESET.write(1);
	Counter=0;
	LostBar_Counter= 5;
	
    while(1){
		
        //instance->_MsgProcess();
		//wait(0.02);
		wait_us(1000);
		//--- LOST BAR CHECK
		if(VBARsimulate){
			VBARraw=VBARsimulate;
		}else
			VBARraw=instance->VBATT.read_u16();
		VBARmean= (VBARmean*15 + VBARraw)>>4;
		VBARac= VBARraw-VBARmean;
		VBARvar = (VBARvar *15 + VBARac*VBARac)>>4;

		instance->Vraw=VBARraw;
		instance->Vmean=VBARmean;
		instance->Vsigma=VBARvar;
		
		if(VBARraw< 29000)
		{
			//instance->LED.led1=1;
			instance->HardwareLostBAR=true;
			LostBar_Counter=7;
		}
		//	instance->HardwareLostBAR=false;
		
		Counter++;
		if( (Counter % 20) == 0 )
		{
			if(instance->Acc.is_Sensor_ready())
			{
				instance->Acc.read_data(instance->Axyz);
				if(first)
				{
					first=false;
					instance->mAxyz[0]=instance->Axyz[0];
					instance->mAxyz[1]=instance->Axyz[1];
					instance->mAxyz[2]=instance->Axyz[2];			
				}
			}
			if(VBARsimulate){
				instance->mVBATT=VBARsimulate*(1.0f/1000.0f);
			}else
			{
				instance->mVBATT=(instance->mVBATT*(15.0f/16.0f)+instance->VBATT.read()*(66.0f/16.0f));
			}
			instance->BRAKE.SetPWM(instance->mVBATT);
			instance->Motor1.Clock();
			instance->Motor2.Clock();
			instance->mAxyz[0]=(instance->mAxyz[0]*(31.0f/32.0f)+instance->Axyz[0]*(1.0f/32.0f));
			instance->mAxyz[1]=(instance->mAxyz[1]*(31.0f/32.0f)+instance->Axyz[1]*(1.0f/32.0f));
			instance->mAxyz[2]=(instance->mAxyz[2]*(31.0f/32.0f)+instance->Axyz[2]*(1.0f/32.0f));
		}
		if((Counter % 200) == 0 )
		{
			if(instance->Acc.read_id()==I_AM_LIS3DH) instance->Acc.SetSensor_ready(); 
			else
			{
				instance->Acc.ClearSensor_ready();
			}
			
			if(LostBar_Counter)
			{
				LostBar_Counter--;
			}
			else
			{
			  if (VBARraw > 33000)  
				{	
					if(instance->HardwareLostBARACK)
					{
						instance->HardwareLostBARACK=false;
						instance->HardwareLostBAR=false;
					}
				}
			}
		}
    }
}

float Comodo::IstantaneusMeasure()
{
	
	return VBATT.read()*(66.0f);
}

Comodo::realnumber Comodo::Measure()
{	
	Comodo::realnumber n;
	n=(mVBATT);
	return n;
}

float Comodo::VBatt()
{				
	return mVBATT;
}

vector<float> Comodo::GetAcceleration()
{
	vector<float> v;
	v.push_back(mAxyz[0]);
	v.push_back(mAxyz[1]);
	v.push_back(mAxyz[2]);
	return v;
}

int Comodo::SReset()
{
	_RebootCS=true;
	LL_GPIO_SetOutputPin((GPIO_TypeDef *) GPIOD_BASE,LL_GPIO_PIN_2);
	LL_GPIO_SetOutputPin((GPIO_TypeDef *) GPIOC_BASE,LL_GPIO_PIN_11);
	LL_GPIO_SetPinMode((GPIO_TypeDef *) GPIOD_BASE, LL_GPIO_PIN_2, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode((GPIO_TypeDef *) GPIOC_BASE, LL_GPIO_PIN_11, LL_GPIO_MODE_OUTPUT);
	SBOOT0.write(0);
	SRESET.write(0);
	wait_us(100);
	SRESET.write(1);
	wait_us(100000);
	LL_GPIO_SetPinMode((GPIO_TypeDef *) GPIOD_BASE, LL_GPIO_PIN_2, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinMode((GPIO_TypeDef *) GPIOC_BASE, LL_GPIO_PIN_11, LL_GPIO_MODE_ALTERNATE);
	_RebootCS=false;
	return 1;
}

int Comodo::SResetAssert()
{
	_RebootCS=true;
	SBOOT0.write(0);
	SRESET.write(0);
	return 1;
}

int Comodo::SReBoot()
{ 
	_RebootCS=true;
	LL_GPIO_SetOutputPin((GPIO_TypeDef *) GPIOD_BASE,LL_GPIO_PIN_2);
	LL_GPIO_SetOutputPin((GPIO_TypeDef *) GPIOC_BASE,LL_GPIO_PIN_11);
	LL_GPIO_SetPinMode((GPIO_TypeDef *) GPIOD_BASE, LL_GPIO_PIN_2, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinMode((GPIO_TypeDef *) GPIOC_BASE, LL_GPIO_PIN_11, LL_GPIO_MODE_OUTPUT);
	SBOOT0.write(0);
	SRESET.write(0);
	SBOOT0.write(1);	
	wait_us(100);
	SRESET.write(1);
	wait_us(100000);
	LL_GPIO_SetPinMode((GPIO_TypeDef *) GPIOD_BASE, LL_GPIO_PIN_2, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinMode((GPIO_TypeDef *) GPIOC_BASE, LL_GPIO_PIN_11, LL_GPIO_MODE_ALTERNATE);
	return 1;
}

int Comodo::SResetDeassert()
{ 
	SBOOT0.write(0);
	SRESET.write(0);
	wait_us(100);
	SRESET.write(1);
	return 1;
}

int Comodo::SetProtocol( int protocol)
{ 
	Commander->Disable();
	Commander->SetProtocol((SerialCommander::tProtocolMode)protocol);
	Commander->Enable();
	return 1;
}

bool Comodo::CheckReboot(){

	return _RebootCS;
}


vector<char>  ComodoSap::OUTread(void) { 
		vector<char> report;
		char *ptr= ((char *)&(ComodoSap::OUT));
		BSP.SAP.OUT.Data.command= BSP.SAP.IN.Data.command;
		BSP.SAP.OUT.Data.pcode=BSP.SAP.IN.Data.pcode;
		BSP.SAP.OUT.Data.pgr=BSP.Motor1.ZSpeed+(BSP.Motor2.ZSpeed ? 0x10:0x00); //--- TODO: Rimuovere
		BSP.SAP.OUT.Data.ax=BSP.Acc.mAxis[0];
		BSP.SAP.OUT.Data.ay=BSP.Acc.mAxis[1];
		BSP.SAP.OUT.Data.az=BSP.Acc.mAxis[2];
		BSP.SAP.OUT.Data.maskflags= BSP.SystemFlags;
		
		
		for (int i=0; i< sizeof(ComodoSap::OUT); i++) 
		{
			report.push_back( *ptr++ );
		}
		uint16_t val= MC1.MPInfoReport.Iph;
		report.push_back( val & 0xFF );
		report.push_back( val >>8);
		val= MC2.MPInfoReport.Iph;
		report.push_back( val & 0xFF );
		report.push_back( val >>8);
		val=MC1.MPInfoReport.AvgSpeed01Hz;
		report.push_back( val & 0xFF );
		report.push_back( val >>8);
		val=MC2.MPInfoReport.AvgSpeed01Hz;
		report.push_back( val & 0xFF );
		report.push_back( val >>8);
		return report;
}

void Comodo::ClearReboot(){
	if(_RebootCS)_RebootCS=false;
}

char * Comodo::Version()
{
	
	return COMODO_BSP_MANIFEST ":" COMODO_BSP_SAP_VERSION;
}
const rpc_method *Comodo::get_rpc_methods()
{
	static const rpc_method rpc_methods[] = {
		{ "VBatt", rpc_method_caller< Comodo::realnumber , Comodo, &Comodo::Measure> },
		{ "VBatt2", rpc_method_caller< float, Comodo, &Comodo::VBatt> },
		
		{ "Acc", rpc_method_caller<vector<float>, Comodo, &Comodo::GetAcceleration> },		
		{ "SReset", rpc_method_caller<int, Comodo, &Comodo::SReset> },
		{ "SReBoot", rpc_method_caller<int, Comodo, &Comodo::SReBoot> },
		{ "SetProtocol", rpc_method_caller<int, Comodo, int, &Comodo::SetProtocol> },		
		{ "Version", rpc_method_caller<char *, Comodo, &Comodo::Version> },
		RPC_METHOD_END
	};
	return rpc_methods;
}

template<> void Reply::putData< vector<float> >(vector<float> v)
{
    
    for (int i=0; i<(v.size()); i++) 
    {
		separator();
        reply += sprintf(reply, "%5.3f", v[i]);
        //if (i<(v.size()-1)) reply += sprintf(reply, ",");
    }
}

template<> void Reply::putData<Comodo::realnumber>(Comodo::realnumber f) {
    separator();
    reply += sprintf(reply, "%.2f", f.value);
}

uint8_t hex(char ch) {
    uint8_t r = (ch > 57) ? (ch - 55) : (ch - 48);
    return r & 0x0F;
}

//int to_byte_array(const char *in, size_t in_size, uint8_t *out) {
//    int count = 0;
//    if (in_size % 2) {
//        while (*in && out) {
//            *out = hex(*in++);
//            if (!*in)
//                return count;
//            *out = (*out << 4) | hex(*in++);
//            *out++;
//            count++;
//        }
//        return count;
//    } else {
//        while (*in && out) {
//            *out++ = (hex(*in++) << 4) | hex(*in++);
//            count++;
//        }
//        return count;
//    }
//}



template<> vector<uint8_t> Arguments::getArg< vector<uint8_t> >(void) {
	vector<uint8_t> tokens;
    uint8_t dato;    
	
	index++;
	char *pEnd = argv[index];
    while (*pEnd){
		dato = hex(*pEnd++);
        if (!*pEnd) break;
         dato = (dato << 4) | hex(*pEnd++);
		 tokens.push_back(dato);
	}
	return tokens;
}

   // mapping of ASCII characters to hex values
//uint8_t hextobin(const char * str, uint8_t * bytes, size_t blen)
//{
//   uint8_t  pos;
//   uint8_t  idx0;
//   uint8_t  idx1;

//   // mapping of ASCII characters to hex values
//   const uint8_t hashmap[] =
//   {
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  !"#$%&'
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ()*+,-./
//     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, // 01234567
//     0x08, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 89:;<=>?
//     0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x00, // @ABCDEFG
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // HIJKLMNO
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // PQRSTUVW
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // XYZ[\]^_
//     0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x00, // `abcdefg
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // hijklmno
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // pqrstuvw
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // xyz{|}~.
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // ........
//   };

//   bzero(bytes, blen);
//   for (pos = 0; ((pos < (blen*2)) && (pos < strlen(str))); pos += 2)
//   {
//      idx0 = (uint8_t)str[pos+0];
//      idx1 = (uint8_t)str[pos+1];
//      bytes[pos/2] = (uint8_t)(hashmap[idx0] << 4) | hashmap[idx1];
//   };

//   return(0);
//}



   

//Comodo::RpcDigitalOut BRAKE(PB_2,"BRAKE");

//============================================================================//
/*------------------------------- END OF FILE --------------------------------*/
//============================================================================//

