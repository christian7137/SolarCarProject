/*!
 * \class can_structs
 * \brief Defines the structs shared to create the different CAN messages
 *
 * \author Beau Roland
*/

#ifndef CAN_MSG_HH
#define CAN_MSG_HH

#pragma pack(push, 1)

/* USER CAN PAYLOAD PACKING TYPES */
typedef struct{
    int16_t  accX;
    int16_t  accY;
    int16_t  accZ;
}canPkt_acc;

typedef struct{
    uint16_t  lumens;
}canPkt_light;

typedef struct{
    float lat;
}canPkt_Gps_lat;

typedef struct{
    float lon;
}canPkt_Gps_lon;

/* CAN PAYLOAD STRUCT */
#define MAX_CAN_MSG_LEN 8
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
#pragma pack(pop)

#endif


