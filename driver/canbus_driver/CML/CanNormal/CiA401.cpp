/********************************************************/
/*                                                      */
/*  Copley Motion Libraries                             */
/*                                                      */
/*  Copyright (c) 2002 Copley Controls Corp.            */
/*                     http://www.copleycontrols.com    */
/*                                                      */
/********************************************************/

/** \file
  I/O module object support.  This file holds the code used to implement
  a standard DS401 I/O module.
 */

#include "CML.h"

CML_NAMESPACE_USE();



/***************************************************************************/
/**
  Default constructor for an I/O module.
  Any object created using this constructor must be initialized by a call
  to CiA401::Init before it is used.
 */
/***************************************************************************/
CiA401::CiA401( void )
{
	SetRefName( "CiA401" );
}

/***************************************************************************/
/**
  Construct an CiA401 object and initialize it using default settings.
  @param net The Network object that this module is associated with.
  @param nodeID The node ID of the module on the network.
 */
/***************************************************************************/
CiA401::CiA401( MDNode &node, uint8 devID )
{
	SetRefName( "CiA401" );
	Init( node, devID );
}

/***************************************************************************/
/**
  Construct an CiA401 object and initialize it using custom settings.
  @param net The Network object that this module is associated with.
  @param nodeID The node ID of the module on the network.
  @param settings The settings to use when configuring the module
 */
/***************************************************************************/
CiA401::CiA401( MDNode &node, uint8 devID, CiA401Settings &settings )
{
	SetRefName( "CiA401" );
	Init( node, devID, settings );
}

/***************************************************************************/
/**
  Virtual destructor for the CiA401 object.
 */
/***************************************************************************/
CiA401::~CiA401()
{
}

/***************************************************************************/
/**
  Initialize an I/O module using default settings.  This function associates the
  object with the CANopen network it will be used on.

  @param net The Network object that this module is associated with.
  @param nodeID The node ID of the module on the network.
  @return A pointer to an error object, or NULL on success
 */
/***************************************************************************/
const Error *CiA401::Init( MDNode &node, uint8 devID )
{
	CiA401Settings settings;
	return Init( node, devID, settings );
}

/***************************************************************************/
/**
  Initialize an I/O module using custom settings.  This function associates the
  object with the CANopen network it will be used on.

  @param net The Network object that this module is associated with.
  @param nodeID The node ID of the module on the network.
  @param settings The settings to use when configuring the module
  @return A pointer to an error object, or NULL on success
 */
/***************************************************************************/
const Error *CiA401::Init( MDNode &node, uint8 devID, CiA401Settings &settings )
{
	const Error *err;
	err = Device::Init( node, devID );
	if( err ) return err;

	if(node.GetDevProfile(devID) != 401)
		return &DeviceError::WrongDevice;

	// Check to see if analog & digital interrupts are enabled.
	// On error, just assume they are
	if( DinGetIntEna( dinIntEna ) ) dinIntEna = true;
	if( AinGetIntEna( ainIntEna ) ) ainIntEna = true;

	// Check the number of 8-bit digital output blocks.  If this is
	// more then zero then I'll map them (up to 8) to the first
	// receive PDO.  Note that I don't check errors here.  If this
	// fails the count will come back as zero.
	uint8 ct;
	Dout8GetCt( ct );

	if( ct && settings.useStdDoutPDO )
	{
		if( ct > 8 ) ct = 8;

		uint8 ids[8];
		for( int i=0; i<ct; i++ ) ids[i] = i;

		err = doutPDO.Init( this, 0, settings.stdDoutPDOType, ct, ids );
		if( err ) return err;
	}

	// Map up to 12 16-bit analog outputs to PDOs.  These are the
	// standard analog output PDOs defined by the spec.
	if( settings.useStdAoutPDO )
	{
		Aout16GetCt( ct );

		for( int n=0; n<3 && ct>0; n++ )
		{
			uint8 map = ct;
			if( map > 4 ) map = 4;
			ct -= map;

			uint8 ids[4];
			for( int i=0; i<map; i++ ) ids[i] = 4*n+i;

			err = aoutPDO[n].Init( this, n+1, settings.stdAoutPDOType, map, ids );
			if( err ) return err;
		}
	}

	// Check the number of 8-bit digital input blocks.  If this is
	// more then zero then I'll map them (up to 8) to the first
	// transmit PDO.  Note that I don't check errors here.  If this
	// fails the count will come back as zero.
	Din8GetCt( ct );
	if( ct && settings.useStdDinPDO )
	{
		if( ct > 8 ) ct = 8;

		uint8 ids[8];
		for( int i=0; i<ct; i++ ) ids[i] = i;

		err = dinPDO.Init( this, 0, settings.stdDinPDOType, ct, ids, CIA401_DIN_PDO0 );
		if( err ) return err;
	}

	// Map up to 12 16-bit analog inputs to PDOs.  These are the
	// standard analog input PDOs defined by the spec.
	if( settings.useStdAinPDO )
	{
		Ain16GetCt( ct );

		for( int n=0; n<3 && ct>0; n++ )
		{
			uint8 map = ct;
			if( map > 4 ) map = 4;
			ct -= map;

			uint8 ids[4];
			for( int i=0; i<map; i++ ) ids[i] = 4*n+i;

			err = ainPDO[n].Init( this, n+1, settings.stdAinPDOType, map, ids, (CIA401_EVENTS)(CIA401_AIN_PDO0<<n) );
			if( err ) return err;
		}
	}


	// Start the node at the end of the last dev's initialization.
	if(devID == node.GetDevCnt())
		err = node.StartNode();

	return err;
}

/***************************************************************************/
/**
  Write an individual digital output.

  The output may be written either by SDO or by PDO.  The PDO method
  is faster since it only requires a single message to be sent.  SDO
  transfers additionally require a response from the module.

  If a PDO transfer is requested, but is not possible because the module is
  not in an operational state, or because the output isn't mapped to
  the PDO, then an SDO transfer will be used.

  @param id Identifies which output to write.
  @param value The new value of the output line.
  @param viaSDO If true, the outputs will be written using SDO messages.
  If false (default), then a PDO will be used if possible.
  @return A pointer to an error object, or NULL on success
 */
/***************************************************************************/
const Error *CiA401::DoutWrite( uint16 id, bool value, bool viaSDO )
{
#ifdef CML_ENABLE_IOMODULE_PDOS
	// Update the bit value in the PDO to keep my local data current.
	// If this fails, then the bit isn't mapped and I'll have to use
	// an SDO to update it.
	if( !doutPDO.UpdateBit( id, value ) )
		viaSDO = true;

	// Output this using a PDO if requested and possible
	if( (!viaSDO) && (nodePtr_->GetState() == NODESTATE_OPERATIONAL) )
		return doutPDO.Transmit();

	else
#endif
		return BitDnld( CIA401_DOUT_1_VALUE, id, value );
}

/***************************************************************************/
/**
  Write a group of 8 digital outputs.

  The outputs may be written either by SDO or by PDO.  The PDO method
  is faster since it only requires a single message to be sent.  SDO
  transfers additionally require a response from the module.

  If a PDO transfer is requested, but is not possible because the module is
  not in an operational state, or because the output block isn't mapped to
  the PDO, then an SDO transfer will be used.

  @param id Identifies which group of outputs to write.
  @param value The new value of the output lines.
  @param viaSDO If true, the outputs will be written using SDO messages.
  If false (default), then a PDO will be used if possible.
  @return A pointer to an error object, or NULL on success
 */
/***************************************************************************/
const Error *CiA401::Dout8Write( uint8 id, uint8 value, bool viaSDO )
{
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

#ifdef CML_ENABLE_IOMODULE_PDOS
	// Update the PDO even if we are using an SDO.  This keeps
	// my PDO data up to date.
	if( !doutPDO.Update( id, value ) )
		viaSDO = true;

	// Output this using a PDO if requested and possible
	if( (!viaSDO) && (nodePtr_->GetState() == NODESTATE_OPERATIONAL) )
		return doutPDO.Transmit();

	else
#endif
		return node->sdo.Dnld8( CIA401_DOUT_8_VALUE, id+1, value );
}

/***************************************************************************/
/**
  Write a group of 16 digital outputs.

  The outputs may be written either by SDO or by PDO.  The PDO method
  is faster since it only requires a single message to be sent.  SDO
  transfers additionally require a response from the module.

  If a PDO transfer is requested, but is not possible because the module is
  not in an operational state, or because the output block isn't mapped to
  the PDO, then an SDO transfer will be used.

  @param id Identifies which group of outputs to write.
  @param value The new value of the output lines.
  @param viaSDO If true, the outputs will be written using SDO messages.
  If false (default), then a PDO will be used if possible.
  @return A pointer to an error object, or NULL on success
 */
/***************************************************************************/
const Error *CiA401::Dout16Write( uint8 id, uint16 value, bool viaSDO )
{
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

#ifdef CML_ENABLE_IOMODULE_PDOS
	if( id >= 127 )
		viaSDO = true;

	else if( !doutPDO.Update( 2*id, (uint8)value ) )
		viaSDO = true;

	else if( !doutPDO.Update( 2*id+1, (uint8)(value>>8) ) )
		viaSDO = true;

	// Output this using a PDO if requested and possible
	if( (!viaSDO) && (nodePtr_->GetState() == NODESTATE_OPERATIONAL) )
		return doutPDO.Transmit();

	else
#endif
		return node->sdo.Dnld16( CIA401_DOUT_16_VALUE, id+1, value );
}

/***************************************************************************/
/**
  Write a group of 32 digital outputs.

  The outputs may be written either by SDO or by PDO.  The PDO method
  is faster since it only requires a single message to be sent.  SDO
  transfers additionally require a response from the module.

  If a PDO transfer is requested, but is not possible because the module is
  not in an operational state, or because the output block isn't mapped to
  the PDO, then an SDO transfer will be used.

  @param id Identifies which group of outputs to write.
  @param value The new value of the output lines.
  @param viaSDO If true, the outputs will be written using SDO messages.
  If false (default), then a PDO will be used if possible.
  @return A pointer to an error object, or NULL on success
 */
/***************************************************************************/
const Error *CiA401::Dout32Write( uint8 id, uint32 value, bool viaSDO )
{
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

#ifdef CML_ENABLE_IOMODULE_PDOS
	if( id >= 63 )
		viaSDO = true;

	else if( !doutPDO.Update( 4*id, (uint8)value ) )
		viaSDO = true;

	else if( !doutPDO.Update( 4*id+1, (uint8)(value>>8) ) )
		viaSDO = true;

	else if( !doutPDO.Update( 4*id+2, (uint8)(value>>16) ) )
		viaSDO = true;

	else if( !doutPDO.Update( 4*id+3, (uint8)(value>>24) ) )
		viaSDO = true;

	// Output this using a PDO if requested and possible
	if( (!viaSDO) && (nodePtr_->GetState() == NODESTATE_OPERATIONAL) )
		return doutPDO.Transmit();

	else
#endif
		return node->sdo.Dnld32( CIA401_DOUT_32_VALUE, id+1, value );
}

/***************************************************************************/
/**
  Write to a 16-bit analog output.  Since 16-bit outputs are mapped to the
  default PDOs of the I/O module, these outputs may be written using either
  PDOs or SDOs.

  @param id The analog input channel ID
  @param value The value to write.
  @param viaSDO If true, the outputs will be written using SDO messages.
  If false (default), then a PDO will be used if possible.
  @return A pointer to an error object, or NULL on success
 */
/***************************************************************************/
const Error *CiA401::Aout16Write( uint8 id, int16 value, bool viaSDO )
{
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

#ifdef CML_ENABLE_IOMODULE_PDOS
	// Update the local copies stored by the PDOs regardless of whether
	// we are using the PDO for output.  This keeps me synchronized.
	int pdo = -1;
	for( int i=0; i<3; i++ )
	{
		if( aoutPDO[i].Update( id, value ) )
			pdo = i;
	}

	// Output this using a PDO if requested and possible
	if( (!viaSDO) && (pdo>=0) && (nodePtr_->GetState() == NODESTATE_OPERATIONAL) )
		return aoutPDO[pdo].Transmit();

	else
#endif
		return node->sdo.Dnld16( CIA401_AOUT_16_VALUE, id+1, value );
}

/***************************************************************************/
/**
  Read a single digital input.

  @param id Identifies the digital input to read.
  @param value The value of the input.
  @param viaSDO If true, an SDO will be used to read the input pin.  If false
  (default), the latest value returned via PDO will be returned, if
  available.
  @return A pointer to an error object, or NULL on success
 */
/***************************************************************************/
const Error *CiA401::DinRead( uint16 id, bool &value, bool viaSDO )
{
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

#ifdef CML_ENABLE_IOMODULE_PDOS
	if( !dinIntEna || !dinPDO.GetBitVal( id, value ) )
		viaSDO = true;

	if( (!viaSDO) && (nodePtr_->GetState()==NODESTATE_OPERATIONAL) )
		return 0;

	else
#endif
		return BitUpld(CIA401_DIN_1_VALUE, id, value );
}

/***************************************************************************/
/**
  Read a group of 8 digital inputs.
  @param id Identifies which group of 8 inputs to read.
  @param value The value of the 8 input lines is returned here.
  @param viaSDO If true, read the inputs using SDO transfers.  If false
  (default) use the most recently received PDO data if this input
  group is mapped to a transmit PDO and the PDO is active.
  @return A pointer to an error object, or NULL on success
 */
/***************************************************************************/
const Error *CiA401::Din8Read( uint8 id, uint8 &value, bool viaSDO )
{
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

#ifdef CML_ENABLE_IOMODULE_PDOS
	if( !dinIntEna || !dinPDO.GetInVal( id, value ) )
		viaSDO = true;

	if( (!viaSDO) && (nodePtr_->GetState() == NODESTATE_OPERATIONAL) )
		return 0;

	else
#endif
		return node->sdo.Upld8( CIA401_DIN_8_VALUE, id+1, value );
}

/***************************************************************************/
/**
  Read a group of 16 digital inputs.
  @param id Identifies which group of 16 inputs to read.
  @param value The value of the 16 input lines is returned here.
  @param viaSDO If true, read the inputs using SDO transfers.  If false
  (default) use the most recently received PDO data if this input
  group is mapped to a transmit PDO and the PDO is active.
  @return A pointer to an error object, or NULL on success
 */
/***************************************************************************/
const Error *CiA401::Din16Read( uint8 id, uint16 &value, bool viaSDO )
{
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

#ifdef CML_ENABLE_IOMODULE_PDOS
	uint8 v[2];

	if( id >= 127 ) viaSDO = true;
	else if( !dinIntEna ) viaSDO = true;
	else if( !dinPDO.GetInVal( 2*id,   v[0] ) ) viaSDO = true;
	else if( !dinPDO.GetInVal( 2*id+1, v[1] ) ) viaSDO = true;

	if( (!viaSDO) && (nodePtr_->GetState() == NODESTATE_OPERATIONAL) )
	{
		value = bytes_to_uint16( v );
		return 0;
	}

	else
#endif
		return node->sdo.Upld16( CIA401_DIN_16_VALUE, id+1, value );
}

/***************************************************************************/
/**
  Read a group of 32 digital inputs.
  @param id Identifies which group of 32 inputs to read.
  @param value The value of the 32 input lines is returned here.
  @param viaSDO If true, read the inputs using SDO transfers.  If false
  (default) use the most recently received PDO data if this input
  group is mapped to a transmit PDO and the PDO is active.
  @return A pointer to an error object, or NULL on success
 */
/***************************************************************************/
const Error *CiA401::Din32Read( uint8 id, uint32 &value, bool viaSDO )
{
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

#ifdef CML_ENABLE_IOMODULE_PDOS
	uint8 v[4];

	if( id >= 63 ) viaSDO = true;
	else if( !dinIntEna ) viaSDO = true;
	else if( !dinPDO.GetInVal( 4*id,   v[0] ) ) viaSDO = true;
	else if( !dinPDO.GetInVal( 4*id+1, v[1] ) ) viaSDO = true;
	else if( !dinPDO.GetInVal( 4*id+2, v[2] ) ) viaSDO = true;
	else if( !dinPDO.GetInVal( 4*id+3, v[3] ) ) viaSDO = true;

	if( (!viaSDO) && (nodePtr_->GetState() == NODESTATE_OPERATIONAL) )
	{
		value = bytes_to_uint32( v );
		return 0;
	}

	else
#endif
		return node->sdo.Upld32( CIA401_DIN_32_VALUE, id+1, value );
}

/***************************************************************************/
/**
  Read a 16-bit analog input.
  @param id The analog input channel ID
  @param value The analog input value
  @param viaSDO If true, read the input using SDO transfers.  If false
  (default) use the most recently received PDO data if this input
  is mapped to a transmit PDO and the PDO is active.
  @return A pointer to an error object, or NULL on success
 */
/***************************************************************************/
const Error *CiA401::Ain16Read( uint8 id, int16 &value, bool viaSDO )
{
	RefObjLocker<MDNode> node( GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

#ifdef CML_ENABLE_IOMODULE_PDOS
	if( !ainIntEna )
		viaSDO = true;

	else
	{
		bool ok = false;

		for( int i=0; i<3 && !ok; i++ )
			ok = ainPDO[i].GetInVal( id, value );

		if( !ok ) viaSDO = true;
	}

	if( (!viaSDO) && (nodePtr_->GetState() == NODESTATE_OPERATIONAL) )
		return 0;

	else
#endif
		return node->sdo.Upld16( CIA401_AIN_16_VALUE, id+1, value );
}

/***************************************************************************/
/**
  Wait on an event associated with this I/O module.  The standard events are
  used to indicate that a new transmit PDO has been received.  A thread may
  wait on such an event by calling this function.

  @param event The event(s) to wait on.  Multiple events may be ORed together
  and in this case this function will return when any of them occur.
  @param timeout The timeout for the wait (milliseconds).  Negative values
  indicate that no timeout should be used (wait forever).  The default
  value is -1.
  @return A pointer to an error object, or NULL on success
 */
/***************************************************************************/
const Error *CiA401::WaitIOEvent( CIA401_EVENTS event, Timeout timeout )
{
	EventAny any( (uint32)event );
	return any.Wait( eventMap, timeout );
}

/***************************************************************************/
/**
  Wait for an event associated with this I/O module.  This function can be used
  to wait on any generic event associated with the I/O module.
  @param e The event to wait on.
  @param timeout The timeout for the wait (milliseconds).  If < 0, then
  wait forever.
  @param match Returns the matching event condition.
  @return A pointer to an error object, or NULL on success.
 */
/***************************************************************************/
const Error *CiA401::WaitIOEvent( Event &e, Timeout timeout, CIA401_EVENTS &match )
{
	return e.Wait( eventMap, timeout );
}

