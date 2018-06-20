#include "CML.h"
#include "stdio.h"

CML_NAMESPACE_USE();

CML_NEW_ERROR( DeviceError, NodeUnavailable, "The node this device is connected to has been deleted." );
CML_NEW_ERROR( DeviceError, WrongDevice, "You are initializing the wrong device!" );

Device::Device(): RefObj( "Device" )
{
	nodeRef_ = 0;
}

Device::Device(MDNode &node, uint8 devID): RefObj( "Device" )
{
	nodeRef_ = 0;
	Init(node, devID);
}

Device::~Device()
{
	if( IsInitialized() )
	{
		cml.Debug( "Device: %d destroyed\n", devID_ );
		KillRef();
		UnInit();
	}
}

const Error *Device::Init( MDNode &node, uint8 devID )
{
	// Un-initialize first if necessary
	int16 offset;
	const Error *err = 0;

	// Init some local parameters
	devID_ = devID;
	offset = (int16)(0x800*(devID_ - 1));

	// Grab a reference to the passed network object
	nodeRef_ = node.GrabRef();

//	err = UnInit();
//	if( err ) return err;

	if(devID != 1)
	{
		// FIXME: 3m timeout
		// Check if the device is actually existed?
		uint32 info;
		node.sdo.SetTimeout(5); // Low the sdo timeout to 3ms just for this.
		err = node.sdo.Upld32(0x67ff + offset, 0, info);
		if(err)
		{
			UnInit();
			return &CanOpenError::BadNodeID;
		}
		node.sdo.SetTimeout(2000); // Reset the timeout to default value.
	}

	// Attach this node to the network
	if( !err )
		err = node.AttachDev( this );

	// If I fail to attach to the network, the
	// discard my network reference
	if( err )
	{
		RefObj::ReleaseRef( nodeRef_ );
		nodeRef_ = 0;
	}

	return err;
}

const Error *Device::UnInit( void )
{
	// Detach this node from the network
	if( nodeRef_ )
	{
		cml.Debug( "NDeviceode: %d uninitializing\n", devID_ );

		MDNode *node = (MDNode*)RefObj::LockRef( nodeRef_ );
		if( node )
		{
			node->DetachDev( this );
			node->UnlockRef();
		}
		RefObj::ReleaseRef( nodeRef_ );
		nodeRef_ = 0;
	}

	return 0;
}

uint32 Device::GetNodeRef( void )
{
	return nodeRef_;
}

const Error *Device::GetDeviceType( uint32 &devType )
{
	RefObjLocker<MDNode> node( nodeRef_ );
	if( !node ) return &DeviceError::NodeUnavailable;
	return node->sdo.Upld32( 0x67ff+(devID_-1)*0x800, 0, devType );
}
