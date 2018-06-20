#ifndef _CIA401STC_
#define _CIA401STC_

#include "CML_Device.h"
#include "CML_Settings.h"
#include "CML_Node.h"
#include "CML_PDO.h"
CML_NAMESPACE_START()

/***************************************************************************/
/**
  I/O module errors.  This class is used to represent errors that may be returned
  by a standard I/O module.
 */
/***************************************************************************/
class CiA401Error: public DeviceError
{
public:
	static const CiA401Error BadID;        ///< The passed digital I/O pin ID number is invalid
	static const CiA401Error BadIOCount;   ///< The number of passed I/O ID blocks is invalid

protected:
	/// Standard protected constructor
	CiA401Error( uint16 id, const char *desc ): DeviceError( id, desc ){}
};



/***************************************************************************/
/**
 Receive PDO for mapping digital output pins.  This class represents the
 standard receive PDO into which up to 64 digital output pins may be
 mapped.
 */
/***************************************************************************/
class RPDO_Dout: public RPDO
{
	/// These objects holds the raw output data
	Pmap8 out_[8];

	/// This points to the CiA401 object
	class CiA401 *io_;

public:
	RPDO_Dout(){ SetRefName( "RPDO_Dout" ); io_=0; }
	virtual ~RPDO_Dout(){ KillRef(); }
	const Error *Init( class CiA401 *io, uint16 slot, byte type, uint8 ct, uint8 id[] );
	bool Update( uint8 id, uint8 value );
	bool UpdateBit( uint16 id, bool value );
	const Error *Transmit( void );

private:
	/// Private copy constructor (not supported)
	RPDO_Dout( const RPDO_Dout& );

	/// Private assignment operator (not supported)
	RPDO_Dout& operator=( const RPDO_Dout& );
};

/***************************************************************************/
/**
 Receive PDO for mapping analog outputs.  This class represents the standard
 receive PDO which can be used to transmit up to 4 16-bit analog outputs.
 */
/***************************************************************************/
class RPDO_Aout: public RPDO
{
	/// These objects map the analog outputs
	Pmap16 out_[4];

	/// This points to the CiA401 object
	class CiA401 *io_;

public:
	RPDO_Aout(){ SetRefName( "RPDO_Aout" ); io_=0; }
	virtual ~RPDO_Aout(){ KillRef(); }
	const Error *Init( class CiA401 *io, uint16 slot, byte type, uint8 ct, uint8 id[] );
	bool Update( uint8 id, int16 value );
	const Error *Transmit( void );

private:
	/// Private copy constructor (not supported)
	RPDO_Aout( const RPDO_Aout& );

	/// Private assignment operator (not supported)
	RPDO_Aout& operator=( const RPDO_Aout& );
};

/***************************************************************************/
/**
 Transmit PDO for mapping digital inputs.  This class represents the
 standard transmit PDO into which up to 64 digital inputs may be
 mapped.
 */
/***************************************************************************/
class TPDO_Din: public TPDO
{
	/// This points to the CiA401 object
	class CiA401 *io_;

	/// These objects map the raw digital input data
	Pmap8 in_[8];

	/// This holds the event mask that will be posted to the
	/// I/O modules event map when the PDO is received.
	CIA401_EVENTS eventMask_;

public:
	TPDO_Din(){ SetRefName( "TPDO_Din" ); io_=0; }
	virtual ~TPDO_Din(){ KillRef(); }
	const Error *Init( class CiA401 *io, uint16 slot, byte type, uint8 ct, uint8 id[], CIA401_EVENTS event );
	bool GetInVal( uint8 id, uint8 &value );
	bool GetBitVal( uint16 id, bool &value );
	void Received( void );

private:
	/// Private copy constructor (not supported)
	TPDO_Din( const TPDO_Din& );

	/// Private assignment operator (not supported)
	TPDO_Din& operator=( const TPDO_Din& );
};

/***************************************************************************/
/**
 Transmit PDO for mapping analog inputs.  This class represents the standard
 transmit PDO which can be used to map up to 4 16-bit analog inputs.
 */
/***************************************************************************/
class TPDO_Ain: public TPDO
{
	/// These objects map the analog inputs
	Pmap16 in_[4];

	/// This points to the CiA401 object
	class CiA401 *io_;

	/// This holds the event mask that will be posted to the
	/// I/O modules event map when the PDO is received.
	CIA401_EVENTS eventMask_;

public:
	TPDO_Ain(){ SetRefName( "TPDO_Ain" ); io_=0; }
	virtual ~TPDO_Ain(){ KillRef(); }
	const Error *Init( class CiA401 *io, uint16 slot, byte type, uint8 ct, uint8 id[], CIA401_EVENTS event );
	bool GetInVal( uint8 id, int16 &value );
	void Received( void );

private:
	/// Private copy constructor (not supported)
	TPDO_Ain( const TPDO_Ain& );

	/// Private assignment operator (not supported)
	TPDO_Ain& operator=( const TPDO_Ain& );
};



class CiA401Settings
{
public:
	CiA401Settings();
	//	uint8 devCnt;
	bool useStdDinPDO;
	byte stdDinPDOType;
	bool useStdDoutPDO;
	byte stdDoutPDOType;
	bool useStdAinPDO;
	byte stdAinPDOType;
	bool useStdAoutPDO;
	byte stdAoutPDOType;
};

CML_NAMESPACE_END()
#endif //_CIA401STC_
