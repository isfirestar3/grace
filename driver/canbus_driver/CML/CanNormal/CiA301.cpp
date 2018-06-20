#include "CML.h"

CML_NAMESPACE_USE();

TPDO_Data::TPDO_Data()
{
	SetRefName( "TPDO_Data" );
	device_=0;
}

TPDO_Data::~TPDO_Data()
{
	if(GetBitCt())
		Stop();
	KillRef();
}

const Error *TPDO_Data::Init( Device &device )
{
	device_ = &device;
	i16 = 0;
	i32 = 0;
	return 0;
}

const Error *TPDO_Data::AddMap(uint32 pdoMap)
{
	const Error *err = 0;

	int16 offset = (int16)(0x800*(device_->GetDeviceID() - 1));
	//	RefObjLocker<MDNode> node( device_->GetNodeRef() );
	//	if( !node ) return &DeviceError::NodeUnavailable;

	// Let the various mapped variables know which
	// objects in the amp's object dictionary they
	// are linked to.
	uint16 index;
	uint8 subIdx;

	if((pdoMap&0xFF) == 16)
	{
		index = pdoMap>>16;
		subIdx = (pdoMap>>8)&0xFF;
		if( !err )
			err = data16_[i16].Init( index+offset, subIdx );
		if( !err )
			err = AddVar( data16_[i16] );

		i16++;
	}

	if((pdoMap&0xFF) == 32)
	{
		index = pdoMap>>16;
		subIdx = (pdoMap>>8)&0xFF;
		if( !err )
			err = data32_[i32].Init( index+offset, subIdx );
		if( !err )
			err = AddVar( data32_[i32] );
		i32++;
	}

	return err;
}

const Error *TPDO_Data::Start(uint16 slot, byte type)
{
	const Error *err = 0;
	eventMask_ = (PDO_EVENT)(1<<slot);
	slot_ = slot;

	RefObjLocker<MDNode> node( device_->GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	if( !err )
		err = SetType( type );
	if( !err )
		err = node->PdoSet( slot_, *this );
	return err;
}

const Error *TPDO_Data::Stop()
{
	const Error *err = 0;

	i16 = 0;
	i32 = 0;
	err = ClearMap();

	RefObjLocker<MDNode> node( device_->GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;
	if( !err ) err = node->PdoDisable( slot_, *this );

	return err;
}


int16 TPDO_Data::GetData16(uint32 mapCode)
{
	for(int i=0; i<4; i++)
		if(data16_[i].GetMapCode() == mapCode)
			return data16_[i].Read();
	return 0;
}

int32 TPDO_Data::GetData32(uint32 mapCode)
{
	for(int i=0; i<2; i++)
		if(data32_[i].GetMapCode() == mapCode)
			return data32_[i].Read();
	return 0;
}

void TPDO_Data::Received( void )
{

	device_->PostPDOEvent(eventMask_);
}


RPDO_Data::RPDO_Data()
{
	SetRefName( "RPDO_Data" );
	device_ = 0;
}

RPDO_Data::~RPDO_Data()
{
	if(GetBitCt())
		Stop();
	KillRef();
}

const Error *RPDO_Data::Init( Device &device, uint16 slot )
{
	device_ = &device;
	i16 = 0;
	i32 = 0;
	slot_ = slot;
	return 0;
}

const Error *RPDO_Data::AddMap(uint32 pdoMap)
{
	const Error *err = 0;
	int16 offset = (int16)(0x800*(device_->GetDeviceID() - 1));
	uint16 index;
	uint8 subIdx;

	if((pdoMap&0xFF) == 16)
	{
		index = pdoMap>>16;
		subIdx = (pdoMap>>8)&0xFF;
		if( !err )
			err = data16_[i16].Init( index+offset, subIdx );
		if( !err )
			err = AddVar( data16_[i16] );

		i16++;
	}

	if((pdoMap&0xFF) == 32)
	{
		index = pdoMap>>16;
		subIdx = (pdoMap>>8)&0xFF;
		if( !err )
			err = data32_[i32].Init( index+offset, subIdx );
		if( !err )
			err = AddVar( data32_[i32] );
		i32++;
	}

	return err;
}

const Error *RPDO_Data::Start(byte type)
{
	const Error *err = 0;

	RefObjLocker<MDNode> node( device_->GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;

	//	eventMask_ = (PDO_EVENT)(1<<slot);


	if( !err )
		err = SetType( type );
	if( !err )
		err = node->PdoSet( slot_, *this );
	return err;
}

const Error *RPDO_Data::Stop()
{
	const Error *err = 0;
	i16 = 0;
	i32 = 0;

	err = ClearMap();

	RefObjLocker<MDNode> node( device_->GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;
	if( !err ) err = node->PdoDisable( slot_, *this );
	return err;
}

void RPDO_Data::SetData16(uint32 mapCode, int16 val)
{
	for(int i=0; i<4; i++)
		if(data16_[i].GetMapCode() == mapCode)
			return data16_[i].Write(val);
}

void RPDO_Data::SetData32(uint32 mapCode, int32 val)
{
	for(int i=0; i<2; i++)
		if(data32_[i].GetMapCode() == mapCode)
			return data32_[i].Write(val);
}

const Error *RPDO_Data::Transmit( void )
{
	RefObjLocker<MDNode> node( device_->GetNodeRef() );
	if( !node ) return &DeviceError::NodeUnavailable;
	RefObjLocker<Network> net( node->GetNetworkRef() );
	if( !net ) return &NodeError::NetworkUnavailable;

	return RPDO::Transmit( *net );
}
