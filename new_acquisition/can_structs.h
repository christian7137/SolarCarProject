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
typedef struct{
    signed int      sensor1Data;
    signed char     sensor2Data;
    unsigned short  sensor3Data;
}canPktType1;
#define PKT_SIZE_1 7
#define PKT_TYPE_1 1

typedef struct{
    int16_t  accX;
    int16_t  accY;
    int16_t  accZ;
}canPktType2;
#define PKT_TYPE_2 2

typedef struct{
    int16_t  sensor1Data;
}canPktType3;
#define PKT_SIZE_3 sizeof(int)
#define PKT_TYPE_3 3

/* CAN PAYLOAD STRUCT */
#define MAX_CAN_MSG_LEN 8
typedef struct{
    union{
        char raw_msg[MAX_CAN_MSG_LEN];
        struct{
            char type;
            union{
                unsigned char pData[MAX_CAN_MSG_LEN - 1 ];
                canPktType1 type1;
                canPktType2 type2;
                canPktType3 type3;
            }data;
        }payload;
    };
}CAN_MSG;
#pragma pop

#endif
