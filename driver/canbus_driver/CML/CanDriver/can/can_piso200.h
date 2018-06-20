#ifndef _SURO_PACKAGE_CAN_PISO_200_7_26_H__
#define _SURO_PACKAGE_CAN_PISO_200_7_26_H__



#include "CML_Settings.h"
#include "CML_Can.h"
#include "CML_Utils.h"

#ifdef _WIN32
#include "piso_can_h.h"
#endif

CML_NAMESPACE_START()



/**
This class extends the generic CanInterface class into a working
interface for the Copley can device driver.

*/
class PisoCan200 : public CanInterface
{
public:
	PisoCan200( void );
	PisoCan200( const char *port );
   virtual ~PisoCan200( void );
   const Error *Open( const char *name ){
      portName = name;
      return Open();
   }
   virtual const Error *Open( void );
   const Error *Close( void );
   const Error *SetBaud( int32 baud );

   bool SupportsTimestamps( void ){ return false; }

protected:
   const Error *RecvFrame( CanFrame &frame, int32 timeout );
   const Error *XmitFrame( CanFrame &frame, int32 timeout );

   /// tracks the state of the interface as open or closed.
   int open;

   /// Holds a copy of the last baud rate set
   int32 baud;

   /// This pointer is used to keep track of private data
   /// used by the driver.
   void *local;

#ifdef _WIN32
   ConfigStruct CanConfig;
   int m_port ;
#else
private:
   int m_socket;

#endif


};

CML_NAMESPACE_END()






#endif
