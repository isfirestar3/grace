#include "CML.h"
#include <stdio.h>


CML_NAMESPACE_USE();

NodeSettings::NodeSettings(void) {
    // Default to 10 ms synch period using the standard ID (0x80)
    synchPeriod = 10000;
    synchID = 0x00000080;
    synchProducer = false;
    synchUseFirstNode = true;
    timeStampID = 0x00000100;
    heartbeatPeriod = 200;
    heartbeatTimeout = 200;
    guardTime = 0;
    lifeFactor = 3;
    pdoCnt = 4;
    enableOnInit = false;
    resetOnInit = false;
}

const Error *MDNode::Init(Network &net, int16 nodeID) {
    NodeSettings settings;
    return Init(net, nodeID, settings);
}

const Error *MDNode::Init(Network &net, int16 nodeID, NodeSettings &settings) {
    int i = 0;
    const Error *err;

    cml.Debug("Node %d, Init\n", nodeID);
    err = Node::Init(net, nodeID);
    if (err) return err;

    // FIXME: 3m timeout
    // Check if the node is actually existed?
    rpdo1.Init(0x606c, 0);
    tpdo1.SetType(1);
    tpdo1.AddVar(rpdo1);
    uint32 info;
    sdo.SetTimeout(2000); // Low the sdo timeout to 5ms just for speed up the init.
    err = sdo.Upld32(0x1000, 0, info);
    if (err) {
        Node::UnInit();
        return &CanOpenError::BadNodeID;
    } else
        profileNum_[0] = info & 0x0000ffff;

    // Check out the number of devices on this node.
    i = 1;
    for (i = 1; i < 8; i++) {
        err = sdo.Upld32(0x67ff + (0x800) * i, 0, info);
        if (err)
            break;
        else
            profileNum_[i] = info & 0x0000ffff;
    }
    devCnt_ = i;

    sdo.SetTimeout(2000); // Reset the timeout to default value.

    // Make sure we're in pre-op state.  This allows us to map
    // the PDO objects.
    //	cml.Debug( "Node %d, Pre-op\n", nodeID );
    //	err = PreOpNode();
    //	if( err ) return err;

    cml.Debug("Node %d, Initting PDOs\n", nodeID);
    // FIXME: disable pdo
    for (i = 0; i < settings.pdoCnt; i++) {
        err = TpdoDisable(i);
        if (err) return err;
        err = RpdoDisable(i);
        if (err) return err;
    }

    // Setup heartbeat or node guarding
    cml.Debug("Node %d, setting up node guarding\n", nodeID);
    if (settings.heartbeatPeriod)
        err = StartHeartbeat(settings.heartbeatPeriod, settings.heartbeatTimeout);
    else if (settings.guardTime && settings.lifeFactor)
        err = StartNodeGuard(settings.guardTime, settings.lifeFactor);
    else
        err = StopGuarding();
    if (err) return err;

    // See if we are picking our own synch producer
    // If so, pick the first initialized amplifier.
    if (settings.synchUseFirstNode) {
        RefObjLocker<CanOpen> co(GetNetworkRef());
        if (!co) return &NodeError::NetworkUnavailable;
        settings.synchProducer = (co->GetSynchProducer() == 0);
    }

    // Setup the synch message.
    if (settings.synchProducer) {
        cml.Debug("Node %d, Setting up synch\n", nodeID);
        err = SetSynchPeriod(settings.synchPeriod);
        if (err) return err;
        err = SetSynchId(settings.synchID);
        if (err) return err;
        cml.Debug("Node %d, Starting synch production\n", nodeID);
        err = SynchStart();
        if (err) return err;
    } else {
        cml.Debug("Node %d, Stopping synch production\n", nodeID);
        err = SynchStop();
        if (err) return err;
    }

    err = StartNode();

    return err;
}

const Error *MDNode::AttachDev(Device *dev) {
    uint8 id = dev->GetDeviceID();

    // Check for invalid device ID
    if (id < 1 || id > 8)
        return &CanOpenError::BadDeviceID;

    MutexLocker ml(mtx_);
    if (devices_[id - 1])
        return &CanOpenError::DeviceIDUsed;

    devices_[id - 1] = dev->GrabRef();
    return 0;
}

const Error *MDNode::DetachDev(Device *dev) {
    uint8 id = dev->GetDeviceID();

    // Check for invalid device ID
    if (id < 1 || id > 8)
        return &CanOpenError::BadDeviceID;

    MutexLocker ml(mtx_);
    RefObj::ReleaseRef(devices_[id - 1]);
    devices_[id - 1] = 0;

    return 0;
}

const Error *MDNode::GetHardwareVersion(char *data) {
    int32 l;
    return sdo.UpldString(0x1009, 0, l = COPLEY_MAX_STRING, data);
}

const Error *MDNode::GetSoftwareVersion(char *data) {
    int32 l;
    return sdo.UpldString(0x100a, 0, l = COPLEY_MAX_STRING, data);
}

const Error *MDNode::SaveConfig(void) {
    return sdo.Download(0x1010, 1, 4, (byte *) "save");
}

void MDNode::HandleEmergency(CanFrame &frame) {
    if ((frame.id == 0x80) && (frame.length == 0)) {
        eventMap_.setBits(MDNODE_SYNC);

    }
    printf("sync msg=======!\n");
}

void MDNode::HandleStateChange(NodeState from, NodeState to) {
    // On a guard error, wake up any task that's pending
    // on my semaphore (i.e. waiting for move done, etc)
    printf("guard state: %d!\n", to);
    if (to == NODESTATE_GUARDERR)
        eventMap_.setBits(MDNODE_GUARDERR);
}

const Error *MDNode::WaitNodeEvent(MDNODE_EVENT event, Timeout timeout) {
    EventAny any((uint32) event);
    return any.Wait(eventMap_, timeout);
}