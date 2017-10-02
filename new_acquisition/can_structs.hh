/*!
 * \class can_structs
 * \brief Defines the structs shared to create the different CAN messages
 *
 * \author Beau Roland
*/

#ifndef CAN_MSG_HH
#define CAN_MSG_HH

#pragma push()
#pragma pack(1)

/* CAN PAYLOAD STRUCT */
#define MAX_CAN_MSG_LEN 8
typedef struct{
	union{
		char type:4;
		char extraPayload:4;
		char data;
	}byte0;
    union{
        unsigned char pData[MAX_CAN_MSG_LEN - 1 ];
        canPktType1 data1;
        canPktType2 data2;
    }data;
}CAN_MSG;


/* USER CAN PAYLOAD PACKING TYPES */
typedef struct{
    signed int      sensor1Data;
    signed char     sensor2Data;
    unsigned short  sensor3Data;
}canPktType1;
#define PKT_SIZE_1 7
#define PKT_TYPE_1 1

typedef struct{
    unsigned short  sensor1Data;
    signed char     sensor2Data;
    unsigned short  sensor3Data;
}canPktType2;
#define PKT_SIZE_2 5
#define PKT_TYPE_2 2

typedef struct{
    int  sensor1Data;
}canPktType3;
#define PKT_SIZE_3 sizeof(int)
#define PKT_TYPE_3 3

#pragma pop()

#endif