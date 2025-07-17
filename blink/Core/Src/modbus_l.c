#include "modbus_l.h"
#include "main.h"



enum mbMode_tag 
{
  mbm_WaitMessage = 0,
  mbm_ReceiveAlien,
  mbm_ReceiveMessage
} mbMode = mbm_WaitMessage;

static uint8_t mbAddr=0;

#define MB_RX_MAXSIZE 260

static uint8_t rx_buf[MB_RX_MAXSIZE] /*ATTRIBUTE_FAR*/;
static uint8_t *rx_ptr = rx_buf;
static uint8_t *rx_endbuf = rx_buf + MB_RX_MAXSIZE;
 
uint16_t modbus_err_longpkt = 0;
uint16_t modbus_err_badpkt = 0;
uint16_t modbus_err_alienpkt = 0;
uint16_t modbus_err_badcrc = 0;
uint16_t modbus_cnt_chars = 0;
uint16_t modbus_cnt_okchars = 0;
uint16_t modbus_cnt_fn3 = 0;
uint16_t modbus_cnt_fn4 = 0;
uint16_t modbus_cnt_fn6 = 0;
uint16_t modbus_cnt_fn16 = 0;
uint16_t modbus_cnt_if = 0;
uint16_t modbus_cnt_ex = 0;
uint16_t modbus_rxcrc = 0;
uint16_t modbus_LastChar = 0x55;

uint16_t *mb_registers_buf = 0;


static void dummyResetTimer(void)
{
//  TMR9  = 0;
//  _T9IF = 0;
  return;
};

static void dummySend(uint8_t *buf, int len)
{
//  buf = buf; len = len;
};

static void dummyNeedReg(uint16_t Addr)
{
//  Addr = Addr;
};

reset_modbus_timer_t *modbus_resetTimer = &dummyResetTimer;
modbus_senddata_t    *modbus_sendData   = &dummySend;
modbus_needReg_t     *modbus_NeedReg    = &dummyNeedReg;

static
const uint8_t CRCHi[] =
{
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

static
const uint8_t CRCLo[] =
{
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04,
    0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8,
    0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10,
    0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C,
    0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0,
    0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C,
    0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54,
    0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
    0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};


/*
uint16_t modbus_CRC16( uint8_t *buf, uint16_t Len )
{
    uint8_t hi = 0xFF;
    uint8_t lo = 0xFF;
    uint16_t index;
    unsigned i;
    for ( i = 0; i < Len; i++ )
    {
	index = lo^buf[i];
	lo = hi^CRCHi[index];
	hi = CRCLo[index];
    };
//  return hi|(((uint16_t)lo)<<8);
    return (((uint16_t)hi)<<8) | lo;
};
*/





uint16_t modbus_CRC16( uint8_t *buf, uint16_t Len )
{
    uint8_t hi = 0xFF;
    uint8_t lo = 0xFF;
    uint16_t index;
    unsigned i;
    for ( i = 0; i < Len; i++ )
    {
	index = hi^buf[i];
	hi = lo^CRCHi[index];
	lo = CRCLo[index];
    };
//  return hi|(((uint16_t)lo)<<8);
    return (((uint16_t)lo)<<8) | hi;
};


/* ������ ������� ������������� ��� ��������� ��������� ����� �� master-���������� 
 * � ���������� modbus-���������.
 * \param Data - �������� ���� ������
 */
void modbus_newchar(uint8_t Data)
{
  (*modbus_resetTimer)();

  switch(mbMode)
  {
    case mbm_ReceiveAlien:  // ������ �� ��� - �� �������� ��������
      return;  break;

    case mbm_WaitMessage:  // ��� ������
	if ( Data != mbAddr )  // ���� �� ��� ��� - ����� ����� ������
          {
             modbus_err_alienpkt++; 
             mbMode = mbm_ReceiveAlien;
             return; break;
          };
        mbMode = mbm_ReceiveMessage;
        rx_ptr = rx_buf;
        /* no break */

    case mbm_ReceiveMessage: // ��������� ������
     if (rx_ptr<rx_endbuf) 
       {
         *rx_ptr++ = Data;
         modbus_cnt_okchars++;
       }
     else 
       {
	 modbus_err_longpkt++;
         mbMode = mbm_ReceiveAlien;
         return; 
       };
     break;
  };

};

void modbus_process_pdu( uint8_t *buf, uint16_t Len );


/* ������ ������� _������_ ���� ������� ����� ���������� 
 * ������ � ������� �������, ������� ������������ �������� ���� ��������.
 * �� ����� ������ ������ ������� ������������ ��������� modbus-���������.
 */
int modbus_2ch_timeout(void)
{
  if ( mbMode != mbm_ReceiveMessage)
  {
    // mbm_WaitMessage, mbm_ReceiveAlien: 
      mbMode = mbm_WaitMessage;
      rx_ptr = rx_buf;
      return 0; 
  };   

  uint16_t Len = rx_ptr-rx_buf;
  if (Len<3) 
    {
      mbMode = mbm_WaitMessage;
      rx_ptr = rx_buf;
      modbus_err_badpkt++;
      return 0;
    };

 
  uint16_t rCRC = (((uint16_t)(*(rx_ptr-1)))<<8) | *(rx_ptr-2);

  volatile uint16_t cCRC = modbus_CRC16( rx_buf, Len-2 );
  // modbus_rxcrc = CRC;

 if ( cCRC != rCRC )
    {
      mbMode = mbm_WaitMessage;
      rx_ptr = rx_buf;
      modbus_err_badcrc++;
      return 0;  
    };


  modbus_process_pdu( rx_buf+1, Len-3 );

  mbMode = mbm_WaitMessage;
  rx_ptr = rx_buf;
  return 1;
};

#define OUT_BUF_SIZE 255

static uint8_t out_buf[OUT_BUF_SIZE];


void modbus_genException(uint8_t fn, uint8_t Code)
{
   out_buf[0] = mbAddr;
   out_buf[1] = fn | 0x80;
   out_buf[2] = Code;  
   uint16_t cCRC = modbus_CRC16( out_buf, 3 );
   out_buf[3] = cCRC;
   out_buf[4] = cCRC>>8;
   modbus_cnt_ex++;
   (*modbus_sendData)( out_buf, 5 );
};


void modbus_process_pdu( uint8_t *buf, uint16_t Len )
{
  uint8_t fn = buf[0];

  if (fn == 3)  // Read Holding Registers
    {
       uint16_t Addr = buf[1]*256+buf[2];
       uint16_t Cnt =  buf[3]*256+buf[4];
	// Check Addr Valid
       if ( (Addr > MB_REGS_LEN) )
          {
	    modbus_genException(fn, 0x02 /* ILLEGAL DATA ADDRESS */ );
            return;
          };
       int16_t StartAddr = Addr;
       int16_t ReqCnt = Cnt;
	// ..and memory owerflow
       if ( Cnt*2+1+1+2+1 > OUT_BUF_SIZE ) 
          {
	    modbus_genException(fn, 0x04 /* SLAVE DEVICE FAILURE */ );
            return;
          };
	// Assemble response
       out_buf[0] = mbAddr;
       out_buf[1] = fn;
       out_buf[2] = (Cnt*2);//>>8;
       //  out_buf[3] = (Cnt*2);

       uint8_t *pbuf = &out_buf[3];
       uint16_t OutLen = 3+Cnt*2;
       //Addr -= 0x1000;
       while ( Cnt-- )
         {
            *pbuf++ = mb_registers_buf[Addr] >> 8;
            *pbuf++ = mb_registers_buf[Addr];
            Addr++;
         };
       // Calc CRC
       uint16_t cCRC = modbus_CRC16( out_buf, OutLen );
       *pbuf++ = cCRC;
       *pbuf = cCRC>>8;
       // ...and send data
       (*modbus_sendData)( out_buf, OutLen + 2 );
       while( ReqCnt--) 
         {
           (*modbus_NeedReg)(StartAddr--);
         };
       modbus_cnt_fn3++;
       return;
    };
                  
  if (fn == 4) // Read Input Registers
    {
       uint16_t Addr = buf[1]*256+buf[2];
       uint16_t Cnt =  buf[3]*256+buf[4];
	// Check Addr Valid
       if ( (Addr > MB_REGS_LEN) )
          {
	    modbus_genException(fn, 0x02 /* ILLEGAL DATA ADDRESS */ );
            return;
          };
       int16_t StartAddr = Addr;
       int16_t ReqCnt = Cnt;
	// ..and memory owerflow
       if ( Cnt*2+1+1+2+1 > OUT_BUF_SIZE ) 
          {
	    modbus_genException(fn, 0x04 /* SLAVE DEVICE FAILURE */ );
            return;
          };
	// Assemble response
       out_buf[0] = mbAddr;
       out_buf[1] = fn;
       out_buf[2] = (Cnt*2);//>>8;
       //out_buf[3] = (Cnt*2);

       uint8_t *pbuf = &out_buf[3];
       uint16_t OutLen = 3+Cnt*2;
       // Addr -= 0x1000;
       while ( Cnt-- )
         {
            *pbuf++ = mb_registers_buf[Addr] >> 8;
            *pbuf++ = mb_registers_buf[Addr];
            Addr++;
         };
       // Calc CRC
       uint16_t cCRC = modbus_CRC16( out_buf, OutLen );
       *pbuf++ = cCRC;
       *pbuf = cCRC>>8;
       // ...and send data
       (*modbus_sendData)( out_buf, OutLen + 2 );
       while( ReqCnt--) 
         {
           (*modbus_NeedReg)(StartAddr++);
         };
       modbus_cnt_fn4++;
       return;
    };

  if (fn == 6) //
  {
      uint16_t Addr = buf[1]*256+buf[2];
      uint16_t Cnt =  0;
	// Check Addr Valid
      if ( (Addr > MB_REGS_LEN) )
         {
    	   modbus_genException(fn, 0x02 /* ILLEGAL DATA ADDRESS */ );
           return;
         };
	// ..and memory owerflow
      if ( Cnt*2+1+1+2+1 > OUT_BUF_SIZE )
         {
    	   modbus_genException(fn, 0x04 /* SLAVE DEVICE FAILURE */ );
           return;
         };
	// Assemble response
      out_buf[0] = mbAddr;
      out_buf[1] = fn;
      out_buf[2] = buf[1];
      out_buf[3] = buf[2];
      out_buf[4] = buf[3];
      out_buf[5] = buf[4];
      out_buf[6] = buf[5];
      out_buf[7] = buf[6];

      uint16_t OutLen = 6;
      // ...and send data
      (*modbus_sendData)( out_buf, OutLen + 2 );

      mb_registers_buf[buf[1]*256+buf[2]] = buf[3]*256+buf[4];

      modbus_cnt_fn6++;
      return;
  };

  if (fn == 16) //
  {
      uint16_t Addr = buf[1]*256+buf[2];
      uint16_t Cnt =  0;
	// Check Addr Valid
      if ( (Addr > MB_REGS_LEN) )
         {
    	   modbus_genException(fn, 0x02 /* ILLEGAL DATA ADDRESS */ );
           return;
         };
	// ..and memory owerflow
      if ( Cnt*2+1+1+2+1 > OUT_BUF_SIZE )
         {
    	   modbus_genException(fn, 0x04 /* SLAVE DEVICE FAILURE */ );
           return;
         };
	// Assemble response
      out_buf[0] = mbAddr;//����� ����������
      out_buf[1] = fn;//�������������� ���
      out_buf[2] = buf[1];//����� ������� �������� Hi ����
      out_buf[3] = buf[2];//����� ������� �������� Lo ����
      out_buf[4] = buf[3];//���-�� ���������� ���. Hi ����
      out_buf[5] = buf[4];//���-�� ���������� ���. Lo ����

      uint16_t OutLen = 6;
      // Calc CRC
      uint16_t cCRC = modbus_CRC16( out_buf, OutLen );
      out_buf[6] = cCRC;
      out_buf[7] = cCRC>>8;
      // ...and send data
      (*modbus_sendData)( out_buf, OutLen + 2 );

      uint16_t nreg = buf[3]*256+buf[4];

      for(uint16_t i=0; i<nreg; i++)
      {
    	  mb_registers_buf[buf[1]*256+buf[2]+i] = buf[6 + 2*i]*256+buf[7 + 2*i];
      }
      modbus_cnt_fn16++;
      return;
  };

  modbus_genException(fn, 0x01 /*ILLEGAL FUNCTION*/ );
  modbus_cnt_if++;
};



void modbus_init(uint8_t Addr)
{
   mbAddr = Addr;
   mbMode = mbm_WaitMessage;
   rx_ptr = rx_buf;
   modbus_err_longpkt = 0;
   modbus_err_badpkt = 0;
   modbus_err_badcrc = 0;
};

