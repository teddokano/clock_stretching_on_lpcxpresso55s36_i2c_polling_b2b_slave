#define	I2C_MASTER_SLAVE_ADDR_7BIT	0x5AU

#define	WRITE_TRANSFER_DATA_SIZE	2

#if 1
#define	ADDRESS_ACK_STRETCH			100
#define	WRITE_DATA_ACK_STRETCH		50
#define	READ_DATA_ACK_STRETCH		50
#else
#define	ADDRESS_ACK_STRETCH			0
#define	WRITE_DATA_ACK_STRETCH		0
#define	READ_DATA_ACK_STRETCH		0
#endif
