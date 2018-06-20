#include "CML.h"

CML_NAMESPACE_USE();

// I/O module specific error objects
CML_NEW_ERROR( CiA401Error, BadID,      "The passed digital I/O pin ID number is invalid" );
CML_NEW_ERROR( CiA401Error, BadIOCount, "The number of passed I/O ID blocks is invalid" );


CiA401Settings::CiA401Settings()
{
	//	devCnt = 1;
	useStdDinPDO  = false;
	stdDinPDOType = 1;
	useStdDoutPDO = false;
	stdDoutPDOType = 1;
	useStdAinPDO  = false;
	stdAinPDOType = 1;
	useStdAoutPDO = false;
	stdAoutPDOType = 1;
}


/***************************************************************************/
/**
  Initialize a digital output PDO object.
  @param io Pointer to the I/O module to which this PDO is assigned.
  @param ct The number of output blocks to be mapped (1 to 8)
  @param id An array of ct output block ID numbers.  These will be mapped
  (in order) to the PDO.
  @return A pointer to an error object, or NULL on success
 */
/***************************************************************************/
const Error *RPDO_Dout::Init( CiA401 *io, uint16 slot, byte type, uint8 ct, uint8 id[] )
{
	if( ct < 1 || ct > 8 )
		return &IOError::BadIOCount;

	this->io_ = io;

	const Error *err = 0; //RPDO::Init( cobID ); FIXME

	if( !err ) err = SetType( type );

	for( uint8 i=0; i<ct; i++ )
	{
		if( !err ) err = out_[i].Init( CIA401_DOUT_8_VALUE, id[i]+1 );
		if( !err ) err = AddVar( out_[i] );
	}
	RefObjLocker<MDNode> node( io->GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;
	if( !err ) err = node->PdoSet( slot, *this );

	return err;
}

/***************************************************************************/
/**
  Update the locally stored value of one of the 8-bit digital output blocks
  associated with this PDO.
  @param id The output block ID to be updatad.
  @param value The new value for the output block.
  @return true if the value was updated,
  false if the block isn't mapped to this PDO.
 */
/***************************************************************************/
bool RPDO_Dout::Update( uint8 id, uint8 value  )
{
	++id;

	for( int i=0; i<mapCt; i++ )
	{
		if( out_[i].GetSub() == id )
		{
			out_[i].Write( value );
			return true;
		}
	}

	return false;
}

/***************************************************************************/
/**
  Update the locally stored value of one bit in this PDO.
  @param id The output ID to be updatad.
  @param value The new value for the output.
  @return true if the value was updated,
  false if the output isn't mapped to this PDO.
 */
/***************************************************************************/
bool RPDO_Dout::UpdateBit( uint16 id, bool value )
{
	uint8 block = 1 + (id>>3);
	uint8 mask  = 1 << (id&3);

	for( int i=0; i<mapCt; i++ )
	{
		if( out_[i].GetSub() == block )
		{
			uint8 v = out_[i].Read();

			if( value ) v |=  mask;
			else        v &= ~mask;

			out_[i].Write( v );
			return true;
		}
	}

	return false;
}

/***************************************************************************/
/**
  Transmit this PDO.
  @return A pointer to an error object, or NULL on success
 */
/***************************************************************************/
const Error *RPDO_Dout::Transmit( void )
{
	RefObjLocker<MDNode> node( io_->GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;
	RefObjLocker<Network> net( node->GetNetworkRef() );
	if( !net ) return &NodeError::NetworkUnavailable;

	return RPDO::Transmit( *net );
}

/***************************************************************************/
/**
  Initialize an analog output PDO object.
  @param io Pointer to the I/O module to which this PDO is assigned.
  @param ct The number of outputs to be mapped (1 to 4)
  @param id An array of ct output ID numbers.  These will be mapped
  (in order) to the PDO.
  @return A pointer to an error object, or NULL on success
 */
/***************************************************************************/
const Error *RPDO_Aout::Init( class CiA401 *io, uint16 slot, byte type, uint8 ct, uint8 id[] )
{
	if( ct < 1 || ct > 4 )
		return &IOError::BadIOCount;

	this->io_ = io;

	const Error *err = 0; //RPDO::Init( cobID ); FIXME

	if( !err ) err = SetType( type );

	for( uint8 i=0; i<ct; i++ )
	{
		if( !err ) err = out_[i].Init( CIA401_AOUT_16_VALUE, id[i]+1 );
		if( !err ) err = AddVar( out_[i] );
	}
	RefObjLocker<MDNode> node( io->GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;
	if( !err ) err = node->PdoSet( slot, *this );

	return err;
}

/***************************************************************************/
/**
  Update the locally stored value of one of the 16-bit analog outputs
  associated with this PDO.

  @param id The output block ID to be updatad.
  @param value The new value for the output block.
  @return true if the value was updated,
  false if the block isn't mapped to this PDO.
 */
/***************************************************************************/
bool RPDO_Aout::Update( uint8 id, int16 value )
{
	++id;

	for( int i=0; i<mapCt; i++ )
	{
		if( out_[i].GetSub() == id )
		{
			out_[i].Write( value );
			return true;
		}
	}

	return false;
}

/***************************************************************************/
/**
  Transmit this PDO.
  @return A pointer to an error object, or NULL on success
 */
/***************************************************************************/
const Error *RPDO_Aout::Transmit( void )
{
	RefObjLocker<MDNode> node( io_->GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;
	RefObjLocker<Network> net( node->GetNetworkRef() );
	if( !net ) return &NodeError::NetworkUnavailable;
	return RPDO::Transmit( *net );
}

/***************************************************************************/
/**
  Initialize a digital input PDO object.
  @param io Pointer to the I/O module to which this PDO is assigned.
  @param ct The number of input blocks to be mapped (1 to 8)
  @param id An array of ct input block ID numbers.  These will be mapped
  (in order) to the PDO.
  @param event The event bit to post when a PDO message is received.
  @return A pointer to an error object, or NULL on success
 */
/***************************************************************************/
const Error *TPDO_Din::Init( class CiA401 *io, uint16 slot, byte type, uint8 ct, uint8 id[], CIA401_EVENTS event )
{
	if( ct < 1 || ct > 8 )
		return &IOError::BadIOCount;

	io_ = io;
	eventMask_ = event;

	const Error *err = SetType( type );

	for( uint8 i=0; i<ct; i++ )
	{
		if( !err ) err = in_[i].Init( CIA401_DIN_8_VALUE, id[i]+1 );
		if( !err ) err = AddVar( in_[i] );

		RefObjLocker<MDNode> node( io->GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		// Read the initial value of this bank of inputs
		uint8 value;
		if( !err ) err = node->sdo.Upld8( CIA401_DIN_8_VALUE, id[i]+1, value );
		in_[i].Write( value );
	}
	RefObjLocker<MDNode> node( io->GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;
	if( !err ) err = node->PdoSet( slot, *this );

	return err;
}

/***************************************************************************/
/**
  Read the specified input bank from the PDO's cached data.  The value returned
  will be the last value received via PDO for this input bank.
  @param id The input block ID to be checked.
  @param value The input value for the block will be returned here.
  @return true if the value was returned,
  false if the block isn't mapped to this PDO.
 */
/***************************************************************************/
bool TPDO_Din::GetInVal( uint8 id, uint8 &value )
{
	++id;

	for( int i=0; i<mapCt; i++ )
	{
		if( in_[i].GetSub() == id )
		{
			value = in_[i].Read();
			return true;
		}
	}

	return false;
}

/***************************************************************************/
/**
  Update the locally stored value of one bit in this PDO.
  @param id The output ID to be updatad.
  @param value The new value for the output.
  @return true if the value was updated,
  false if the output isn't mapped to this PDO.
 */
/***************************************************************************/
bool TPDO_Din::GetBitVal( uint16 id, bool &value )
{
	uint8 block = 1 + (id>>3);
	uint8 mask  = 1 << (id&3);

	for( int i=0; i<mapCt; i++ )
	{
		if( in_[i].GetSub() == block )
		{
			value = (in_[i].Read() & mask) == mask;
			return true;
		}
	}

	return false;
}

/***************************************************************************/
/**
  New transmit PDO received.  This method is called by the CANopen reader thread
  when a new PDO message is received.  It causes this PDO object to post it's
  event to the CiA401 object's event map.  This will cause any waiting threads
  to wake up.
 */
/***************************************************************************/
void TPDO_Din::Received( void )
{
	io_->PostIOEvent( eventMask_ );
}

/***************************************************************************/
/**
  Initialize a analog input PDO object.
  @param io Pointer to the I/O module to which this PDO is assigned.
  @param ct The number of inputs to be mapped (1 to 4)
  @param id An array of ct input ID numbers.  These will be mapped
  (in order) to the PDO.
  @param event The event bit to post when a PDO message is received.
  @return A pointer to an error object, or NULL on success
 */
/***************************************************************************/
const Error *TPDO_Ain::Init( class CiA401 *io, uint16 slot, byte type, uint8 ct, uint8 id[], CIA401_EVENTS event )
{
	if( ct < 1 || ct > 4 )
		return &IOError::BadIOCount;

	this->io_ = io;
	eventMask_ = event;

	const Error *err = SetType( 255 );

	for( uint8 i=0; i<ct; i++ )
	{
		if( !err ) err = in_[i].Init( CIA401_AIN_16_VALUE, id[i]+1 );
		if( !err ) err = AddVar( in_[i] );

		RefObjLocker<MDNode> node( io->GetNodeRef() );
		if( !node ) return &DeviceError::NodeUnavailable;
		// Read the initial value of this input
		int16 value;
		if( !err ) err = node->sdo.Upld16( CIA401_AIN_16_VALUE, id[i]+1, value );
		in_[i].Write( value );
	}
	RefObjLocker<MDNode> node( io->GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;
	if( !err ) err = node->PdoSet( slot, *this );

	return err;
}

/***************************************************************************/
/**
  Read the specified input from the PDO's cached data.  The value returned
  will be the last value received via PDO for this input bank.
  @param id The input ID to be checked.
  @param value The input value will be returned here.  If the input is not
  mapped to this PDO, then this will not be changed.
  @return true if the value was returned,
  false if the input isn't mapped to this PDO.
 */
/***************************************************************************/
bool TPDO_Ain::GetInVal( uint8 id, int16 &value )
{
	++id;

	for( int i=0; i<mapCt; i++ )
	{
		if( in_[i].GetSub() == id )
		{
			value = in_[i].Read();
			return true;
		}
	}

	return false;
}

/***************************************************************************/
/**
  New transmit PDO received.  This method is called by the CANopen reader thread
  when a new PDO message is received.  It causes this PDO object to post it's
  event to the CiA401 object's event map.  This will cause any waiting threads
  to wake up.
 */
/***************************************************************************/
void TPDO_Ain::Received( void )
{
	io_->PostIOEvent( eventMask_ );
}

