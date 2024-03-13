////////////////////////////////////////////////////////////////////////////////
// /****************************************************************************
// **
// ** Copyright (C) 2015-2022 M-Way Solutions GmbH
// ** Contact: https://www.blureange.io/licensing
// **
// ** This file is part of the Bluerange/FruityMesh implementation
// **
// ** $BR_BEGIN_LICENSE:GPL-EXCEPT$
// ** Commercial License Usage
// ** Licensees holding valid commercial Bluerange licenses may use this file in
// ** accordance with the commercial license agreement provided with the
// ** Software or, alternatively, in accordance with the terms contained in
// ** a written agreement between them and M-Way Solutions GmbH.
// ** For licensing terms and conditions see https://www.bluerange.io/terms-conditions. For further
// ** information use the contact form at https://www.bluerange.io/contact.
// **
// ** GNU General Public License Usage
// ** Alternatively, this file may be used under the terms of the GNU
// ** General Public License version 3 as published by the Free Software
// ** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
// ** included in the packaging of this file. Please review the following
// ** information to ensure the GNU General Public License requirements will
// ** be met: https://www.gnu.org/licenses/gpl-3.0.html.
// **
// ** $BR_END_LICENSE$
// **
// ****************************************************************************/
////////////////////////////////////////////////////////////////////////////////

/*
 * This file contains the type definitions for the core FruityMesh packets that can
 * be sent over e.g. MeshConnections and MeshAccessConnections.
 * 
 * *** ATTENTION ***
 * This is not something that should be changed if you want to keep your nodes
 * interoperable with other FruityMesh nodes. Functionality should be implemented
 * using module messages.
 * *** ATTENTION ***
 */

#pragma once

#include <FmTypes.h>

/* All our packet structures are packed so that they can be safely transmitted over the
 * air between different nodes that might have been compiled using different compilers.
 * Additionally static_asserts were used to make sure that the size is as expected.
*/
#pragma pack(push)
#pragma pack(1)


//########## Message types ###############################################

#ifdef __cplusplus

//The MessageTypes are solely defined by the standard and MUST NOT be used for
//other purpose, except the range specified as "User space" can be used for experimenting
//without any guarantee that this will not change in future versions.
//Defining other MessageTypes in this section will clash as soon as nodes from other vendors
//are used in the same network
enum class MessageType : u8
{
    INVALID = 0,

    //=> First 15 types may be taken by advertising message types in the future, so they are reserved

    SPLIT_WRITE_CMD = 16, //Used if a WRITE_CMD message is split
    SPLIT_WRITE_CMD_END = 17, //Used if a WRITE_CMD message is split

    //Mesh clustering and handshake: Protocol defined
    CLUSTER_WELCOME = 20, //The initial message after a connection setup (Sent between two nodes)
    CLUSTER_ACK_1 = 21, //Both sides must acknowledge the handshake (Sent between two nodes)
    CLUSTER_ACK_2 = 22, //Second ack (Sent between two nodes)
    CLUSTER_INFO_UPDATE = 23, //When the cluster size changes, this message is used (Sent to all nodes)
    RECONNECT = 24, //Sent while trying to reestablish a connection

    //Custom Connection encryption handshake
    ENCRYPT_CUSTOM_START = 25,
    ENCRYPT_CUSTOM_ANONCE = 26,
    ENCRYPT_CUSTOM_SNONCE = 27,
    ENCRYPT_CUSTOM_DONE = 28,

    //Others
    UPDATE_TIMESTAMP = 30, //This is used to set the time of a node directly, e.g. from a Smartphone and will trigger time synchronization
    UPDATE_CONNECTION_INTERVAL = 31, //Instructs a node to use a different connection interval
    ASSET_LEGACY = 32,
    CAPABILITY = 33,
    ASSET_GENERIC = 34, // Deprecated as of 14.04.2021 (sent as ModuleMessage in AssetScanningModule)
    SIG_MESH_SIMPLE = 35, //A lightweight wrapper for SIG mesh access layer messages

    //Module messages all use the same ConnPacketModule header
    MODULE_MESSAGES_START = 50,
    
    MODULE_CONFIG = 50, //Used for many different messages that set and get the module config
    MODULE_TRIGGER_ACTION = 51, //Trigger some custom module action
    MODULE_ACTION_RESPONSE = 52, //Response on a triggered action
    MODULE_GENERAL = 53, //A message, generated by the module not as a response to an action, e.g. an event
    MODULE_RAW_DATA = 54,
    MODULE_RAW_DATA_LIGHT = 55,
    //MODULES_GET_LIST = 56, //Deprecated as of 27.08.2020 as it was never used
    //MODULES_LIST = 57, //Deprecated as of 27.08.2020 as it was never used
    COMPONENT_ACT = 58, //Actuator messages
    COMPONENT_SENSE = 59, //Sensor messages

    MODULE_MESSAGES_END = 59,

    //Others
    TIME_SYNC = 60,
    DEAD_DATA = 61, //Used by the MeshAccessConnection when malformed data was received.

    //Reserved for other packets (User Space IDs 80 - 110)
    //Are currently free to be used by any vendor in networks that do not mix nodes from
    //different vendors. No guarantee is made that these IDs might not be needed in the future.
    DATA_1 = 80,
    DATA_1_VITAL = 81,

    CLC_DATA = 83,

    // The most significant bit of the MessageType is reserved for future use.
    // Such a use could be (but is not limited to) to extend the ConnPacketHeader
    // if the bit is set. This way an extended MessageType could be implemented
    // that uses 7 bit of the first byte and 8 bit of the second byte to have a
    // maximum possible amount of 32768 different message types. Of course the
    // most significant bit of the second byte could also be used to further
    // extend the range.
    RESERVED_BIT_START = 128,
    RESERVED_BIT_END = 255,
};
#endif

//################################################################################
//########## Basic message headers ###############################################
//################################################################################

//CONN_PACKET_HEADER is the most basic header that is used for all FruityMesh packets
constexpr size_t SIZEOF_CONN_PACKET_HEADER = 5;
typedef struct
{
    MessageType messageType;
    NodeId sender;
    NodeId receiver;
}ConnPacketHeader;
STATIC_ASSERT_SIZE(ConnPacketHeader, SIZEOF_CONN_PACKET_HEADER);

//CONN_PACKET_SPLIT_HEADER is used for new message splitting
//Each split packet uses this header (first one, subsequent ones)
constexpr size_t SIZEOF_CONN_PACKET_SPLIT_HEADER = 2;
typedef struct
{
    MessageType splitMessageType;
    u8 splitCounter;
}ConnPacketSplitHeader;
STATIC_ASSERT_SIZE(ConnPacketSplitHeader, SIZEOF_CONN_PACKET_SPLIT_HEADER);

//################################################################################
//########### Packets relevant for clustering and cluster handshaking ############
//################################################################################

//CLUSTER_WELCOME is the first handshake packet being sent over a mesh connection after two
//potential partners set up a connection
constexpr size_t SIZEOF_CONN_PACKET_PAYLOAD_CLUSTER_WELCOME = 11;
constexpr size_t SIZEOF_CONN_PACKET_PAYLOAD_CLUSTER_WELCOME_WITH_NETWORK_ID = 13;
typedef struct
{
    ClusterId clusterId;
    ClusterSize clusterSize;
    u16 meshWriteHandle;
    ClusterSize hopsToSink;
    u8 preferredConnectionInterval;
    NetworkId networkId;
}ConnPacketPayloadClusterWelcome;
STATIC_ASSERT_SIZE(ConnPacketPayloadClusterWelcome, SIZEOF_CONN_PACKET_PAYLOAD_CLUSTER_WELCOME_WITH_NETWORK_ID);

constexpr size_t SIZEOF_CONN_PACKET_CLUSTER_WELCOME = (SIZEOF_CONN_PACKET_HEADER + SIZEOF_CONN_PACKET_PAYLOAD_CLUSTER_WELCOME);
constexpr size_t SIZEOF_CONN_PACKET_CLUSTER_WELCOME_WITH_NETWORK_ID = (SIZEOF_CONN_PACKET_HEADER + SIZEOF_CONN_PACKET_PAYLOAD_CLUSTER_WELCOME_WITH_NETWORK_ID);
typedef struct
{
    ConnPacketHeader header;
    ConnPacketPayloadClusterWelcome payload;
}ConnPacketClusterWelcome;
STATIC_ASSERT_SIZE(ConnPacketClusterWelcome, SIZEOF_CONN_PACKET_CLUSTER_WELCOME_WITH_NETWORK_ID);

//CLUSTER_ACK_1 will be sent as a response to CLUSTER_WELCOME
constexpr size_t SIZEOF_CONN_PACKET_PAYLOAD_CLUSTER_ACK_1 = 3;
typedef struct
{
    ClusterSize hopsToSink;
    u8 preferredConnectionInterval;
}ConnPacketPayloadClusterAck1;
STATIC_ASSERT_SIZE(ConnPacketPayloadClusterAck1, SIZEOF_CONN_PACKET_PAYLOAD_CLUSTER_ACK_1);

constexpr size_t SIZEOF_CONN_PACKET_CLUSTER_ACK_1 = (SIZEOF_CONN_PACKET_HEADER + SIZEOF_CONN_PACKET_PAYLOAD_CLUSTER_ACK_1);
typedef struct
{
    ConnPacketHeader header;
    ConnPacketPayloadClusterAck1 payload;
}ConnPacketClusterAck1;
STATIC_ASSERT_SIZE(ConnPacketClusterAck1, SIZEOF_CONN_PACKET_CLUSTER_ACK_1);

//CLUSTER_ACK_2 marks the final step of the clustering handshake
constexpr size_t SIZEOF_CONN_PACKET_PAYLOAD_CLUSTER_ACK_2 = 8;
typedef struct
{
    ClusterId clusterId;
    ClusterSize clusterSize;
    ClusterSize hopsToSink;
}ConnPacketPayloadClusterAck2;
STATIC_ASSERT_SIZE(ConnPacketPayloadClusterAck2, SIZEOF_CONN_PACKET_PAYLOAD_CLUSTER_ACK_2);

constexpr size_t SIZEOF_CONN_PACKET_CLUSTER_ACK_2 = (SIZEOF_CONN_PACKET_HEADER + SIZEOF_CONN_PACKET_PAYLOAD_CLUSTER_ACK_2);
typedef struct
{
    ConnPacketHeader header;
    ConnPacketPayloadClusterAck2 payload;
}ConnPacketClusterAck2;
STATIC_ASSERT_SIZE(ConnPacketClusterAck2, SIZEOF_CONN_PACKET_CLUSTER_ACK_2);

//CLUSTER_INFO_UPDATE informs all nodes in the mesh about cluster changes
constexpr size_t SIZEOF_CONN_PACKET_PAYLOAD_CLUSTER_INFO_UPDATE = 9;
typedef struct
{
    ClusterId newClusterId_deprecated;
    ClusterSize clusterSizeChange;
    ClusterSize hopsToSink;
    u8 connectionMasterBitHandover : 1; //Used to hand over the connection master bit
    u8 counter : 1; //A very small counter to protect against duplicate clusterUpdates
    u8 reserved : 6;
    
}ConnPacketPayloadClusterInfoUpdate;
STATIC_ASSERT_SIZE(ConnPacketPayloadClusterInfoUpdate, SIZEOF_CONN_PACKET_PAYLOAD_CLUSTER_INFO_UPDATE);

constexpr size_t SIZEOF_CONN_PACKET_CLUSTER_INFO_UPDATE = (SIZEOF_CONN_PACKET_HEADER + SIZEOF_CONN_PACKET_PAYLOAD_CLUSTER_INFO_UPDATE);
typedef struct
{
    ConnPacketHeader header;
    ConnPacketPayloadClusterInfoUpdate payload;
}ConnPacketClusterInfoUpdate;
STATIC_ASSERT_SIZE(ConnPacketClusterInfoUpdate, SIZEOF_CONN_PACKET_CLUSTER_INFO_UPDATE);

//CONN_PACKET_RECONNECT is send as a handshake message after two nodes that had a direct
//connection reconnected to each other through the reestablishing procedure after a connection loss
constexpr size_t SIZEOF_CONN_PACKET_RECONNECT = (SIZEOF_CONN_PACKET_HEADER);
typedef struct
{
    ConnPacketHeader header;
    //No Payload
}ConnPacketReconnect;
STATIC_ASSERT_SIZE(ConnPacketReconnect, SIZEOF_CONN_PACKET_RECONNECT);

//################################################################################
//############ Packets for our MeshAccessConnection encryption handshake #########
//################################################################################

//ENCRYPT_CUSTOM_START is sent unencrypted as the first message that will specify
//for example which encryption method and which key to use
constexpr size_t SIZEOF_CONN_PACKET_ENCRYPT_CUSTOM_START = (SIZEOF_CONN_PACKET_HEADER + 6);
typedef struct
{
    ConnPacketHeader header;
    u8 version;
    FmKeyId fmKeyId;
    u8 tunnelType : 2;
    u8 reserved : 6;

}ConnPacketEncryptCustomStart;
STATIC_ASSERT_SIZE(ConnPacketEncryptCustomStart, SIZEOF_CONN_PACKET_ENCRYPT_CUSTOM_START);

//ENCRYPT_CUSTOM_ANONCE is also sent unencrypted as a response to ENCRYPT_CUSTOM_START
//It delivers a random number used for subsequent encryption
constexpr size_t SIZEOF_CONN_PACKET_ENCRYPT_CUSTOM_ANONCE = (SIZEOF_CONN_PACKET_HEADER + 8);
typedef struct
{
    ConnPacketHeader header;
    u32 anonce[2];

}ConnPacketEncryptCustomANonce;
STATIC_ASSERT_SIZE(ConnPacketEncryptCustomANonce, SIZEOF_CONN_PACKET_ENCRYPT_CUSTOM_ANONCE);

//ENCRYPT_CUSTOM_SNONCE is sent as a response to ENCRYPT_CUSTOM_ANONCE and is encrypted
//using the anonce that was sent by the partner, this delivers the snonce
constexpr size_t SIZEOF_CONN_PACKET_ENCRYPT_CUSTOM_SNONCE = (SIZEOF_CONN_PACKET_HEADER + 8);
typedef struct
{
    ConnPacketHeader header;
    u32 snonce[2];

}ConnPacketEncryptCustomSNonce;
STATIC_ASSERT_SIZE(ConnPacketEncryptCustomSNonce, SIZEOF_CONN_PACKET_ENCRYPT_CUSTOM_SNONCE);

//ENCRYPT_CUSTOM_DONE is the final ACK that the encrypted connection was set up and 
//is sent after ENCRYPT_CUSTOM_SNONCE
constexpr size_t SIZEOF_CONN_PACKET_ENCRYPT_CUSTOM_DONE = (SIZEOF_CONN_PACKET_HEADER + 1);
typedef struct
{
    ConnPacketHeader header;
    u8 status;

}ConnPacketEncryptCustomDone;
STATIC_ASSERT_SIZE(ConnPacketEncryptCustomDone, SIZEOF_CONN_PACKET_ENCRYPT_CUSTOM_DONE);

//################################################################################
//################################ Module Packets ################################
//################################################################################
// Packets in this section use a common module header which allows them to be
//sent by every module without clashing

//###### Module Packet Headers ####################################################

//A safeguard to check if this was not already defined
#ifdef CONN_PACKET_MODULE_ENTRIES
    static_assert(false, "Double definition of CONN_PACKET_MODULE_ENTRIES");
#endif

//We need to define this so that we can construct a ConnPacketModule struct and also a struct without the data field
#define CONN_PACKET_MODULE_ENTRIES \
    ConnPacketHeader header; \
    ModuleId moduleId; \
    u8 requestHandle; /*Set to 0 if this packet does not need to be identified for reliability (Used to implement end-to-end acknowledged requests)*/ \
    u8 actionType

//CONN_PACKET_MODULE is the basic header used by all module messages
constexpr size_t SIZEOF_CONN_PACKET_MODULE = (SIZEOF_CONN_PACKET_HEADER + 3); //This size does not include the data region which is variable, add the used data region size to this size
typedef struct
{
    CONN_PACKET_MODULE_ENTRIES;
    u8 data[MAX_DATA_SIZE_PER_WRITE - SIZEOF_CONN_PACKET_HEADER - 4]; //Data can be larger and will be transmitted in subsequent packets

}ConnPacketModule;
STATIC_ASSERT_SIZE(ConnPacketModule, SIZEOF_CONN_PACKET_MODULE + sizeof(ConnPacketModule::data));

//ConnPacketModuleStart is used to construct a ConnPacketModule without the trailing data field
typedef struct
{
    CONN_PACKET_MODULE_ENTRIES;
}ConnPacketModuleStart;
STATIC_ASSERT_SIZE(ConnPacketModuleStart, SIZEOF_CONN_PACKET_MODULE);

//CONN_PACKET_MODULE_VENDOR is the basic header used by all vendor modules
constexpr size_t SIZEOF_CONN_PACKET_MODULE_VENDOR = (SIZEOF_CONN_PACKET_HEADER + 6);
typedef struct
{
    ConnPacketHeader header;
    VendorModuleId moduleId;
    u8 requestHandle;
    u8 actionType;
    u8 data[MAX_DATA_SIZE_PER_WRITE - SIZEOF_CONN_PACKET_HEADER - 7]; //Data can be larger and will be transmitted in subsequent packets

}ConnPacketModuleVendor;
STATIC_ASSERT_SIZE(ConnPacketModuleVendor, SIZEOF_CONN_PACKET_MODULE_VENDOR + sizeof(ConnPacketModuleVendor::data));

//###### Sensor and Actuator Messages ####################################################

enum class ActorMessageActionType : u8
{
    RESERVED = 0, // Unused
    WRITE = 1, // Write without acknowledgement
    READ = 2, // Read a value
    WRITE_ACK = 3, // Write with acknowledgement
    //CMD = 4, //deprecated as of 09.09.2021, use WRITE_ACK or WRITE instead
};

enum class SensorMessageActionType : u8
{
    UNSPECIFIED = 0, // Generated by sensor itself, e.g. an event
    ERROR_RSP = 1, // Error during READ or WRITE_ACK
    READ_RSP = 2, // Response following a READ
    WRITE_RSP = 3, // Response following a WRITE_ACK that contains the data actually written
    RESULT_RSP = 4, //Response following a WRITE_ACK that contains a result code
};

//COMPONENT_MESSAGE_HEADER is used for component_act and component_sense messages
constexpr size_t SIZEOF_COMPONENT_MESSAGE_HEADER = 12;
typedef struct
{
    ConnPacketHeader header;
    ModuleId moduleId;
    u8 requestHandle;
    u8 actionType;
    u16 component;
    u16 registerAddress;
}ComponentMessageHeader;
STATIC_ASSERT_SIZE(ComponentMessageHeader, SIZEOF_COMPONENT_MESSAGE_HEADER);


//CONN_PACKET_COMPONENT_MESSAGE is used as a wrapper with a pointer to the payload for component messages
//This packet generates a sensor event or instruct device to write data into register and send it through mesh
constexpr size_t SIZEOF_CONN_PACKET_COMPONENT_MESSAGE = 12;
typedef struct
{
    ComponentMessageHeader componentHeader;
    u8 payload[1];
}ConnPacketComponentMessage;
STATIC_ASSERT_SIZE(ConnPacketComponentMessage, SIZEOF_CONN_PACKET_COMPONENT_MESSAGE + 1);

//COMPONENT_MESSAGE_HEADER_VENDOR is used for component_act and component_sense messages with a VendorModuleId
constexpr size_t SIZEOF_COMPONENT_MESSAGE_HEADER_VENDOR = 15;
typedef struct
{
    ConnPacketHeader header;
    VendorModuleId moduleId;
    u8 requestHandle;
    u8 actionType;
    u16 component;
    u16 registerAddress;
}ComponentMessageHeaderVendor;
STATIC_ASSERT_SIZE(ComponentMessageHeaderVendor, SIZEOF_COMPONENT_MESSAGE_HEADER_VENDOR);

//CONN_PACKET_COMPONENT_MESSAGE_VENDOR is used as a wrapper with a pointer to the payload for component messages
//This packet generates a sensor event or instruct device to write data into register and send it through mesh
constexpr size_t SIZEOF_CONN_PACKET_COMPONENT_MESSAGE_VENDOR = 15;
typedef struct
{
    ComponentMessageHeaderVendor componentHeader;
    u8 payload[1];
}ConnPacketComponentMessageVendor;
STATIC_ASSERT_SIZE(ConnPacketComponentMessageVendor, SIZEOF_CONN_PACKET_COMPONENT_MESSAGE_VENDOR + 1);


//###### Raw Data Packets ####################################################
enum class RawDataProtocol : u8
{
    UNSPECIFIED               = 0,
    HTTP                      = 1,
    GZIPPED_JSON              = 2,
    START_OF_USER_DEFINED_IDS = 200,
    LAST_ID                   = 255
};

enum class RawDataActionType : u8
{
    START          = 0,
    START_RECEIVED = 1,
    CHUNK          = 2,
    REPORT         = 3,
    ERROR_T        = 4,
    REPORT_DESIRED = 5,
};

// ##### Raw Data Headers #####

struct RawDataHeader
{
    ConnPacketHeader connHeader;
    ModuleId moduleId;
    u8 requestHandle;
    RawDataActionType actionType;
};
static_assert(sizeof(RawDataHeader) == SIZEOF_CONN_PACKET_MODULE, "The RawDataHeader must have the exact same structure as ConnPacketModule");

struct RawDataHeaderVendor
{
    ConnPacketHeader connHeader;
    VendorModuleId moduleId;
    u8 requestHandle;
    RawDataActionType actionType;
};
static_assert(sizeof(RawDataHeaderVendor) == SIZEOF_CONN_PACKET_MODULE_VENDOR, "The RawDataHeaderVendor must have the exact same structure as ConnPacketModuleVendor");

constexpr size_t SIZEOF_RAW_DATA_LIGHT_PACKET = SIZEOF_CONN_PACKET_HEADER + 3;
struct RawDataLight
{
    ConnPacketHeader connHeader;
    ModuleId moduleId;
    u8 requestHandle;
    RawDataProtocol protocolId;

    u8 payload[1];
};
static_assert(sizeof(RawDataHeaderVendor) == SIZEOF_CONN_PACKET_MODULE_VENDOR, "The RawDataHeaderVendor must have the exact same structure as ConnPacketModuleVendor");
STATIC_ASSERT_SIZE(RawDataLight, SIZEOF_RAW_DATA_LIGHT_PACKET + 1);

constexpr size_t SIZEOF_RAW_DATA_LIGHT_VENDOR_PACKET = SIZEOF_CONN_PACKET_HEADER + 6;
struct RawDataLightVendor
{
    ConnPacketHeader connHeader;
    VendorModuleId moduleId;
    u8 requestHandle;
    RawDataProtocol protocolId;

    u8 payload[1];
};
STATIC_ASSERT_SIZE(RawDataLightVendor, SIZEOF_RAW_DATA_LIGHT_VENDOR_PACKET + 1);

// ##### Raw Data Payload #####

#ifndef MAX_RAW_DATA_METADATA_SIZE
#define MAX_RAW_DATA_METADATA_SIZE 40
#endif
constexpr size_t SIZEOF_RAW_DATA_START_PAYLOAD = 8;
struct RawDataStartPayload
{
    u32 numChunks : 24;
    u32 protocolId : 8; //RawDataProtocol
    u32 fmKeyId;

    u8 metadata[];
};
STATIC_ASSERT_SIZE(RawDataStartPayload, SIZEOF_RAW_DATA_START_PAYLOAD);

enum class RawDataErrorType : u8
{
    UNEXPECTED_END_OF_TRANSMISSION = 0,
    NOT_IN_A_TRANSMISSION = 1,
    MALFORMED_MESSAGE = 2,
    START_OF_USER_DEFINED_ERRORS = 200,
    LAST_ID = 255
};

enum class RawDataErrorDestination : u8
{
    SENDER   = 1,
    RECEIVER = 2,
    BOTH     = 3
};

struct RawDataErrorPayload
{
    RawDataErrorType error;
    RawDataErrorDestination destination;
};
STATIC_ASSERT_SIZE(RawDataErrorPayload, 2);

constexpr size_t SIZEOF_RAW_DATA_CHUNK_PAYLOAD = 4;
struct RawDataChunkPayload
{
    u32 chunkId : 24;
    u32 reserved : 8;
    u8 payload[1];
};
STATIC_ASSERT_SIZE(RawDataChunkPayload, SIZEOF_RAW_DATA_CHUNK_PAYLOAD + 1);
static_assert(offsetof(RawDataChunkPayload, payload) % 4 == 0, "Payload should be 4 byte aligned!");

struct RawDataReportPayload
{
    u32 missings[3];
};
STATIC_ASSERT_SIZE(RawDataReportPayload, 12);

//############### Capability Reporting Packets ###################################
enum class CapabilityActionType : u8
{
    REQUESTED = 0,
    ENTRY = 1,
    END = 2
};

enum class CapabilityEntryType : u8
{
    INVALID = 0,

    HARDWARE = 1, //capability describing some hardware aspect incl. e.g. board revision, etc.
    SOFTWARE = 2, //some software component such as a firmware or bootloader incl. version information.

    /*
     * The revision information here identifies a concrete version of the metadata document that shall be processed when
     * assembling the set of controls (aka. actuators and sensors) and other features supported by the device.
     * A metadata document itself may contain further restrictions on applicability such as specific hardware and
     * software capabilities that need to be present as well.
     */
    METADATA = 3, //designates same metadata document as stored in the device catalog.
    /*
     * This may be used when the firmware reports device specific values that make no sense looking up in the device
     * catalog. An example use is the number of heads in a multi-part device. The use of this type of metadata should be
     * avoided as this is not intended as a replacement of properly modeling metadata!
     * The model field serves as a key, while the revision serves as a value.
     */
    PROPERTY = 4, //designates a metadata property reported by device firmware.

    NOT_READY = 100,    //The module is currently not ready to report the capability with the provided index but will be in the near future.
};

struct CapabilityEntry
{
    CapabilityEntryType type;
    //WARNING: The following values are not guaranteed to have a terminating zero!
    char manufacturer[32];
    char modelName[53];
    char revision[32];
};

struct CapabilityHeader
{
    ConnPacketHeader header;
    CapabilityActionType actionType;
};

struct CapabilityRequestedMessage
{
    CapabilityHeader header;
};
STATIC_ASSERT_SIZE(CapabilityRequestedMessage, 6);

struct CapabilityEntryMessage
{
    CapabilityHeader header;
    u32 index;
    CapabilityEntry entry;
};
STATIC_ASSERT_SIZE(CapabilityEntryMessage, 128);

struct CapabilityEndMessage
{
    CapabilityHeader header;
    u32 amountOfCapabilities;
};
STATIC_ASSERT_SIZE(CapabilityEndMessage, 10);


//################################################################################
//##################### Packets used for time synchronization ####################
//################################################################################

//Timestamp synchronization packet
constexpr size_t SIZEOF_CONN_PACKET_UPDATE_TIMESTAMP = (SIZEOF_CONN_PACKET_HEADER + 8);
typedef struct
{
    ConnPacketHeader header;
    u32 timestampSec;
    u16 remainderTicks;
    i16 offset;
}connPacketUpdateTimestamp;
STATIC_ASSERT_SIZE(connPacketUpdateTimestamp, SIZEOF_CONN_PACKET_UPDATE_TIMESTAMP);

#ifdef __cplusplus
enum class TimeSyncType : u8 {
    INITIAL = 0,
    INITIAL_REPLY = 1,
    CORRECTION = 2,
    CORRECTION_REPLY = 3,
    INTER_NETWORK = 4, //A special time sync packet intended for syncing time between two networks or a network and an asset.
};

struct TimeSyncHeader
{
    ConnPacketHeader header;
    TimeSyncType type;
};

struct TimeSyncInitial
{
    TimeSyncHeader header;
    u32 syncTimeStamp;
    u32 timeSincSyncTimeStamp;
    u32 additionalTicks;
    i16 offset;
    u32 counter;
};
STATIC_ASSERT_SIZE(TimeSyncInitial, 24);

struct TimeSyncInterNetwork
{
    TimeSyncHeader header;
    u32 syncTimeStamp;
    u32 timeSincSyncTimeStamp;
    u32 additionalTicks;
    i16 offset;
};
STATIC_ASSERT_SIZE(TimeSyncInterNetwork, 20);

struct TimeSyncInitialReply
{
    TimeSyncHeader header;
};
STATIC_ASSERT_SIZE(TimeSyncInitialReply, 6);

struct TimeSyncCorrection
{
    TimeSyncHeader header;
    u32 correctionTicks;
};
STATIC_ASSERT_SIZE(TimeSyncCorrection, 10);

struct TimeSyncCorrectionReply
{
    TimeSyncHeader header;
};
STATIC_ASSERT_SIZE(TimeSyncCorrectionReply, 6);
#endif

//enrolled nodes
enum class EnrolledNodesType : u8 {
  SET = 0,
  SET_REPLY = 1
};

struct EnrolledNodesHeader
{
  ConnPacketHeader header;
  EnrolledNodesType type;
};

struct EnrolledNodesMessage
{
  EnrolledNodesHeader header;
  u16 enrolledNodes;
};
STATIC_ASSERT_SIZE(EnrolledNodesMessage, 8);

//################################################################################
//########################### Other packet types #################################
//################################################################################

//A data packet only used for debugging
constexpr size_t SIZEOF_CONN_PACKET_PAYLOAD_DATA_1 = (MAX_DATA_SIZE_PER_WRITE - SIZEOF_CONN_PACKET_HEADER);
typedef struct
{
    u8 length;
    u8 data[SIZEOF_CONN_PACKET_PAYLOAD_DATA_1 - 1];
    
}ConnPacketPayloadData1;
STATIC_ASSERT_SIZE(ConnPacketPayloadData1, SIZEOF_CONN_PACKET_PAYLOAD_DATA_1);

constexpr size_t SIZEOF_CONN_PACKET_DATA_1 = (SIZEOF_CONN_PACKET_HEADER + SIZEOF_CONN_PACKET_PAYLOAD_DATA_1);
typedef struct
{
    ConnPacketHeader header;
    ConnPacketPayloadData1 payload;
}ConnPacketData1;
STATIC_ASSERT_SIZE(ConnPacketData1, SIZEOF_CONN_PACKET_DATA_1);

//CLC_DATA_PACKET
constexpr size_t SIZEOF_CONN_PACKET_PAYLOAD_CLC_DATA = (MAX_DATA_SIZE_PER_WRITE - SIZEOF_CONN_PACKET_HEADER);
typedef struct
{
    u8 data[SIZEOF_CONN_PACKET_PAYLOAD_CLC_DATA];

}ConnPacketPayloadClcData;
STATIC_ASSERT_SIZE(ConnPacketPayloadClcData, SIZEOF_CONN_PACKET_PAYLOAD_CLC_DATA);

constexpr size_t SIZEOF_CONN_PACKET_CLC_DATA = (SIZEOF_CONN_PACKET_HEADER + SIZEOF_CONN_PACKET_PAYLOAD_CLC_DATA);
typedef struct
{
    ConnPacketHeader header;
    ConnPacketPayloadClcData payload;
}ConnPacketDataClcData;
STATIC_ASSERT_SIZE(ConnPacketDataClcData, SIZEOF_CONN_PACKET_CLC_DATA);

//UPDATE_CONNECTION_INTERVAL is used to tell nodes to update their connection interval settings
constexpr size_t SIZEOF_CONN_PACKET_UPDATE_CONNECTION_INTERVAL = (SIZEOF_CONN_PACKET_HEADER + 2);
typedef struct
{
    ConnPacketHeader header;
    u16 newInterval;
}ConnPacketUpdateConnectionInterval;
STATIC_ASSERT_SIZE(ConnPacketUpdateConnectionInterval, SIZEOF_CONN_PACKET_UPDATE_CONNECTION_INTERVAL);

enum class TrackedAssetMessageEntryType : u8
{
    BLE    = 0x00,
    INS    = 0x01
};
struct PeriodicAssetMessageEntry
{
    u8 moving : 1;
    u8 hasFreeInConnection : 1;
    u8 interestedInConnection : 1;
    u8 moveMod : 1;
    u8 reservedBits : 4;

    NodeId nodeId;
    u8 lastRssi;

    TrackedAssetMessageEntryType entryType;
    u8 payload[SIZEOF_ADV_STRUCTURE_ASSET_SERVICE_DATA_PAYLOAD];
};
constexpr size_t SIZEOF_TRACKED_ASSET_MESSAGE_ENTRY = 12;
constexpr size_t SIZEOF_TRACKED_ASSET_MESSAGE_WITH_CONN_PACKET_HEADER = SIZEOF_TRACKED_ASSET_MESSAGE_ENTRY + SIZEOF_CONN_PACKET_HEADER;

//Size must never change! Old nodes will process the message exactly like this as multiple entries are concatenated
STATIC_ASSERT_SIZE(PeriodicAssetMessageEntry, SIZEOF_TRACKED_ASSET_MESSAGE_ENTRY);

struct StandstillAssetMessageEntry
{
    u8 hasFreeInConnection : 1;
    u8 interestedInConnection : 1;
    u8 reservedBits : 1;
    u8 positionCounter : 5;
    NodeId nodeId;
    u8 avgRssi;
    u8 rssiCount;
};
constexpr size_t SIZEOF_STANDSTILL_ASSET_MESSAGE_ENTRY = 5;
constexpr size_t SIZEOF_STANDSTILL_ASSET_MESSAGE_WITH_CONN_PACKET_HEADER = SIZEOF_STANDSTILL_ASSET_MESSAGE_ENTRY + SIZEOF_CONN_PACKET_HEADER;

//Size must never change! Old nodes will process the message exactly like this as multiple entries are concatenated
STATIC_ASSERT_SIZE(StandstillAssetMessageEntry, SIZEOF_STANDSTILL_ASSET_MESSAGE_ENTRY);

//End Packing
#pragma pack(pop)

