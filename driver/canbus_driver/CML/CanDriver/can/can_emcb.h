#ifndef _DEF_INC_CAN_EMCBCAN
#define _DEF_INC_CAN_EMCBCAN

#include "CML_Settings.h"
#include "CML_Can.h"

#include "CML_Threads.h"
#include "CountTime.hpp"

CML_NAMESPACE_START()

/**
Kvaser specific CAN interface.

This class extends the generic CanInterface class into a working
interface for the Kvaser can device driver.

*/
typedef struct _CommOption
{
	BYTE Ch;
	unsigned int ID;
	BYTE EID;
	BYTE RTR;
	BYTE Len;
	unsigned char Data[8];
}CommOption,*pCommOption;

class EMCBCAN : public CanInterface
{
public:
	EMCBCAN( void );
	EMCBCAN( const char *port );
	virtual ~EMCBCAN( void );

	const Error *Open( const char *name ){
		portName = name;
		return Open();
	}
	const Error *Open( void );
	const Error *Close( void );
	const Error *SetBaud( int32 baud );

protected:
	const Error *RecvFrame( CanFrame &frame, Timeout timeout );
	const Error *XmitFrame( CanFrame &frame, Timeout timeout );

	/// tracks the state of the interface as open or closed.
	int open;

	/// Holds a copy of the last baud rate set
	int32 baud;

	/// Holds a value the Kvaser driver uses to identify bit rate
	int kvBaud;

	/// File handle used to configure and read from the CAN channel
	int Handle_Rd;

	/// File handle used to write to the CAN channel
	int Handle_Wr;

	const Error *ConvertError( int err );

	Mutex mutex;

private:
	/// Mutex used by reading threads to ensure clean exit
	Mutex readMutex;

	/// Counter used by reading threads
	int readCount;

	void IncReadCount( int ct );
	void WaitReadCount( void );
	CommOption rxData;
	CommOption txData;
	CountTime ct;
};

CML_NAMESPACE_END()

#endif



