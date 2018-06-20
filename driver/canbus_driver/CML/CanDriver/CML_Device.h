#ifndef _DEVICE_
#define _DEVICE_

#include "CML_MDNode.h"
#include "CML_Error.h"
#include "CML_ErrorCodes.h"
#include "CML_Threads.h"
#include "CML_Reference.h"
#include "CML_CiA301.h"
CML_NAMESPACE_START()

enum PDO_EVENT
{
	PDO_0 = 0x00000001,
	PDO_1 = 0x00000002,
	PDO_2 = 0x00000004,
	PDO_3 = 0x00000008
};

class DeviceError: public Error
{
public:
   /// The network this node is connected to has been deleted
   static const DeviceError NodeUnavailable;
   static const DeviceError WrongDevice;

protected:
   /// Standard protected constructor
   DeviceError( uint16 id, const char *desc ): Error( id, desc ){}
};

class Device: public RefObj
{
	uint8 devID_;
	uint32 nodeRef_;

public:
	Device();
	Device(MDNode &node, uint8 devID);
	virtual ~Device();

	virtual const Error *Init( MDNode &node, uint8 devID );
	virtual const Error *UnInit( void );

	virtual uint32 GetNodeRef( void );
	virtual const Error *GetDeviceType( uint32 &devType );
	virtual uint8 GetDeviceID(void){ return devID_; }
	virtual void PostPDOEvent( PDO_EVENT event ){}

	bool IsInitialized( void ){ return nodeRef_ != 0; }

private:
	Device( const Device& );
	Device& operator=( const Device& );
};

CML_NAMESPACE_END()
#endif //_DEVICE_
