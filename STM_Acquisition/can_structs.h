/*!
 * \class can_structs
 * \brief Defines the structs shared to create the different CAN messages
 *
 * \author Beau Roland
*/

#ifndef CAN_MSG_HH
#define CAN_MSG_HH

#pragma push
#pragma pack(1)

/* USER CAN PAYLOAD PACKING TYPES */
/// \brief can payload structure for accelerometer data
typedef struct{
    int16_t  accX;
    int16_t  accY;
    int16_t  accZ;
}canPkt_acc;

/// \brief can payload structure for light flux data
typedef struct{
    uint16_t  lumens;
}canPkt_light;

/// \brief can payload structure for gps latitude data
typedef struct{
    float lat;
}canPkt_Gps_lat;

/// \brief can payload structure for gps longitude data
typedef struct{
    float lon;
}canPkt_Gps_lon;

/* CAN PAYLOAD STRUCT */
#define MAX_CAN_MSG_LEN 8
/// \brief can message structure
/// \details provides:<br>
/// raw access to the bytes through raw_msg.<br>
/// sensor id/type value <br>
/// parsed accessable payload structure
   
typedef struct{
    union{
        char raw_msg[MAX_CAN_MSG_LEN];
        struct{
            char type;
            union{
                unsigned char pData[MAX_CAN_MSG_LEN - 1 ];
                canPkt_acc      acc;
                canPkt_light    light;
                canPkt_Gps_lat  lat;
                canPkt_Gps_lon  lon;
            }data;
        }payload;
    };
}CAN_MSG;
#pragma pop

#endif