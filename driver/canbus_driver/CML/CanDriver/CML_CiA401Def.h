#ifndef _CIA401DEF_
#define _CIA401DEF_

#include "CML_Device.h"
#include "CML_Settings.h"
CML_NAMESPACE_START()

/***************************************************************************/
/**
  Object dictionary ID values used on standard I/O modules.
 */
/***************************************************************************/
enum CIA401_OBJID
{
	CIA401_DIN_8_VALUE             = 0x6000,  ///<  8-bit digital input value
	CIA401_DIN_8_POL               = 0x6002,  ///<  8-bit digital input polarity
	CIA401_DIN_8_FILT              = 0x6003,  ///<  8-bit digital input filter constant
	CIA401_DIN_INTENA              = 0x6005,  ///< Digital input interrupt enable
	CIA401_DIN_8_MASK_ANY          = 0x6006,  ///<  8-bit digital input int mask, any change
	CIA401_DIN_8_MASK_L2H          = 0x6007,  ///<  8-bit digital input int mask, low to high
	CIA401_DIN_8_MASK_H2L          = 0x6008,  ///<  8-bit digital input int mask, high to low
	CIA401_DIN_1_VALUE             = 0x6020,  ///<  1-bit digital input value
	CIA401_DIN_1_POL               = 0x6030,  ///<  1-bit digital input polarity
	CIA401_DIN_1_FILT              = 0x6038,  ///<  1-bit digital input filter constant
	CIA401_DIN_1_MASK_ANY          = 0x6050,  ///<  1-bit digital input int mask, any change
	CIA401_DIN_1_MASK_L2H          = 0x6060,  ///<  1-bit digital input int mask, low to high
	CIA401_DIN_1_MASK_H2L          = 0x6070,  ///<  1-bit digital input int mask, high to low
	CIA401_DIN_16_VALUE            = 0x6100,  ///< 16-bit digital input value
	CIA401_DIN_16_POL              = 0x6102,  ///< 16-bit digital input polarity
	CIA401_DIN_16_FILT             = 0x6103,  ///< 16-bit digital input filter constant
	CIA401_DIN_16_MASK_ANY         = 0x6106,  ///< 16-bit digital input int mask, any change
	CIA401_DIN_16_MASK_L2H         = 0x6107,  ///< 16-bit digital input int mask, low to high
	CIA401_DIN_16_MASK_H2L         = 0x6108,  ///< 16-bit digital input int mask, high to low
	CIA401_DIN_32_VALUE            = 0x6120,  ///< 32-bit digital input value
	CIA401_DIN_32_POL              = 0x6122,  ///< 32-bit digital input polarity
	CIA401_DIN_32_FILT             = 0x6123,  ///< 32-bit digital input filter constant
	CIA401_DIN_32_MASK_ANY         = 0x6126,  ///< 32-bit digital input int mask, any change
	CIA401_DIN_32_MASK_L2H         = 0x6127,  ///< 32-bit digital input int mask, low to high
	CIA401_DIN_32_MASK_H2L         = 0x6128,  ///< 32-bit digital input int mask, high to low

	CIA401_DOUT_8_VALUE            = 0x6200,  ///<  8-bit digital output value
	CIA401_DOUT_8_POL              = 0x6202,  ///<  8-bit digital output polarity
	CIA401_DOUT_8_ERRMODE          = 0x6206,  ///<  8-bit digital output error mode
	CIA401_DOUT_8_ERRVAL           = 0x6207,  ///<  8-bit digital output error value
	CIA401_DOUT_8_FILT             = 0x6208,  ///<  8-bit digital output filter mask
	CIA401_DOUT_1_VALUE            = 0x6220,  ///<  1-bit digital output value
	CIA401_DOUT_1_POL              = 0x6240,  ///<  1-bit digital output polarity
	CIA401_DOUT_1_ERRMODE          = 0x6250,  ///<  1-bit digital output error mode
	CIA401_DOUT_1_ERRVAL           = 0x6260,  ///<  1-bit digital output error value
	CIA401_DOUT_1_FILT             = 0x6270,  ///<  1-bit digital output filter mask
	CIA401_DOUT_16_VALUE           = 0x6300,  ///< 16-bit digital output value
	CIA401_DOUT_16_POL             = 0x6302,  ///< 16-bit digital output polarity
	CIA401_DOUT_16_ERRMODE         = 0x6306,  ///< 16-bit digital output error mode
	CIA401_DOUT_16_ERRVAL          = 0x6307,  ///< 16-bit digital output error value
	CIA401_DOUT_16_FILT            = 0x6308,  ///< 16-bit digital output filter mask
	CIA401_DOUT_32_VALUE           = 0x6320,  ///< 32-bit digital output value
	CIA401_DOUT_32_POL             = 0x6322,  ///< 32-bit digital output polarity
	CIA401_DOUT_32_ERRMODE         = 0x6326,  ///< 32-bit digital output error mode
	CIA401_DOUT_32_ERRVAL          = 0x6327,  ///< 32-bit digital output error value
	CIA401_DOUT_32_FILT            = 0x6328,  ///< 32-bit digital output filter mask

	CIA401_AIN_8_VALUE             = 0x6400,  ///<  8-bit analog input value
	CIA401_AIN_16_VALUE            = 0x6401,  ///< 16-bit analog input value
	CIA401_AIN_32_VALUE            = 0x6402,  ///< 32-bit analog input value
	CIA401_AIN_FLT_VALUE           = 0x6403,  ///< floating point analog input value
	CIA401_AIN_MFG_VALUE           = 0x6404,  ///< manufacturer specific analog input value

	CIA401_AOUT_8_VALUE            = 0x6410,  ///<  8-bit analog output value
	CIA401_AOUT_16_VALUE           = 0x6411,  ///< 16-bit analog output value
	CIA401_AOUT_32_VALUE           = 0x6412,  ///< 32-bit analog output value
	CIA401_AOUT_FLT_VALUE          = 0x6413,  ///< floating point analog output value
	CIA401_AOUT_MFG_VALUE          = 0x6414,  ///< manufacturer specific analog output value

	CIA401_AIN_TRIG                = 0x6421,  ///< Analog input trigger selection
	CIA401_AIN_INTSRC              = 0x6422,  ///< Analog input interrupt source
	CIA401_AIN_INTENA              = 0x6423,  ///< Analog input interrupt enable

	CIA401_AIN_32_UPLIM            = 0x6424,  ///< 32-bit analog input upper limit
	CIA401_AIN_32_LWLIM            = 0x6425,  ///< 32-bit analog input lower limit
	CIA401_AIN_32_UDELTA           = 0x6426,  ///< 32-bit analog input unsigned delta
	CIA401_AIN_32_NDELTA           = 0x6427,  ///< 32-bit analog input negative delta
	CIA401_AIN_32_PDELTA           = 0x6428,  ///< 32-bit analog input positive delta

	CIA401_AIN_FLT_UPLIM           = 0x6429,  ///< floating point analog input upper limit
	CIA401_AIN_FLT_LWLIM           = 0x642A,  ///< floating point analog input lower limit
	CIA401_AIN_FLT_UDELTA          = 0x642B,  ///< floating point analog input unsigned delta
	CIA401_AIN_FLT_NDELTA          = 0x642C,  ///< floating point analog input negative delta
	CIA401_AIN_FLT_PDELTA          = 0x642D,  ///< floating point analog input positive delta

	CIA401_AIN_FLT_OFFSET          = 0x642E,  ///< floating point analog input offset
	CIA401_AIN_FLT_SCALE           = 0x642F,  ///< floating point analog input scaling
	CIA401_AIN_UNIT                = 0x6430,  ///< analog input SI Unit
	CIA401_AIN_32_OFFSET           = 0x6431,  ///< 32-bit analog input offset
	CIA401_AIN_32_SCALE            = 0x6432,  ///< 32-bit analog input scaling

	CIA401_AOUT_FLT_OFFSET         = 0x6441,  ///< floating point analog output offset
	CIA401_AOUT_FLT_SCALE          = 0x6442,  ///< floating point analog output scaling
	CIA401_AOUT_ERRMODE            = 0x6443,  ///< analog output error mode
	CIA401_AOUT_32_ERRVAL          = 0x6444,  ///< 32-bit analog output error value
	CIA401_AOUT_FLT_ERRVAL         = 0x6445,  ///< floating point analog output error value
	CIA401_AOUT_32_OFFSET          = 0x6446,  ///< 32-bit analog output offset
	CIA401_AOUT_32_SCALE           = 0x6447,  ///< 32-bit analog output scaling
	CIA401_AOUT_UNIT               = 0x6450   ///< analog output SI Unit
};

/***************************************************************************/
/**
  This enumeration is used to define the types of events that may cause an
  analog input to generate an interrupt event.
 */
/***************************************************************************/
enum CIA401_AIN_TRIG_TYPE
{
	CIA401_UPPER_LIM             = 0x0001, ///< Input above upper limit
	CIA401_LOWER_LIM             = 0x0002, ///< Input below lower limit
	CIA401_UDELTA                = 0x0004, ///< Input changed by more then the unsigned delta amount
	CIA401_NDELTA                = 0x0008, ///< Input reduced by more then the negative delta amount
	CIA401_PDELTA                = 0x0010  ///< Input increased by more then the positive delta
};

/***************************************************************************/
/**
  This enumeration gives the various events that can be waited on.  The default
  events are simply the reception of one of the standard transmit PDO objects.
 */
/***************************************************************************/
enum CIA401_EVENTS
{
	/// Digital input PDO 0 was received.  By default, this PDO is
	/// transmitted by the module when any of the first 64 digital
	/// inputs changes state.
	CIA401_DIN_PDO0                = 0x00000001,

	/// Analog input PDO 0 was received.  By default, this PDO is
	/// transmitted by the module when any of the first 4 16-bit
	/// analog inputs generates an event.
	///
	/// There are many different types of events that are programmable
	/// for analog inputs, however not all I/O module manufacturers
	/// support all (or any) of these events.  The function
	/// IOmodule::AinSetTrigType can be used to set the type of event
	/// associated with an analog input.
	///
	/// Consult the documentation provided with the I/O module to determine
	/// what types of analog input events are available for your module.
	CIA401_AIN_PDO0                = 0x00010000,

	/// Analog input PDO 1 was received.  This PDO is similar to analog input
	/// PDO 0, however it maps the second group of 4 16-bit analog inputs.
	CIA401_AIN_PDO1                = 0x00020000,

	/// Analog input PDO 2 was received.  This PDO is similar to analog input
	/// PDO 0, however it maps the third group of 4 16-bit analog inputs.
	CIA401_AIN_PDO2                = 0x00040000
};

CML_NAMESPACE_END()
#endif //_CIA401DEF_
