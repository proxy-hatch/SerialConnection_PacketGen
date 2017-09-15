/*
 * PeerX.h
 *
 *      Author: wcs
 */

#ifndef PEERX_H_
#define PEERX_H_

#include <stdint.h> // for uint8_t

#define CHUNK_SZ	 128
#define SOH_OH  	 1			//SOH Byte Overhead
#define BLK_NUM_AND_COMP_OH  2	//Overhead for blkNum and its complement
#define DATA_POS  	 (SOH_OH + BLK_NUM_AND_COMP_OH)	//Position of data in buffer
#define PAST_CHUNK 	 (DATA_POS + CHUNK_SZ)		//Position of checksum in buffer

#define CHECKSUM_OH  1			//Overhead for checksum
#define REST_BLK_OH  (BLK_NUM_AND_COMP_OH + CHECKSUM_OH)	//Overhead in rest of block
#define REST_BLK_SZ  (CHUNK_SZ + REST_BLK_OH)
#define BLK_SZ  	 (SOH_OH + REST_BLK_SZ)

#define CRC_OH  2			//Overhead for checksum
#define REST_BLK_OH_CRC  (BLK_NUM_AND_COMP_OH + CRC_OH)	//Overhead in rest of block
#define REST_BLK_SZ_CRC  (CHUNK_SZ + REST_BLK_OH_CRC)
#define BLK_SZ_CRC  	 (SOH_OH + REST_BLK_SZ_CRC)

#define SOH 0x01
#define EOT 0x04
#define ACK 0x06
#define NAK 0x15
#define	CTRL_Z	26

typedef uint8_t blkT[BLK_SZ_CRC];

void ErrorPrinter (const char* functionCall, const char* file, int line, int error);

class PeerX {
public:
	PeerX(int d, const char *fname);
	const char* result;  // result of the file transfer

protected:
	int mediumD; // descriptor for serial port or delegate
	const char* fileName;
	int transferringFileD;	// descriptor for file being read from or written to.
	bool Crcflg; // use CRC if true (or else checksum if false)

	void
	crc16ns (uint16_t* crc16nsP, uint8_t* buf);
	void
	chksum8ns (uint8_t* xModChksum8, uint8_t* buf);
	void
	sendByte(uint8_t byte);
};

#endif /* PEERX_H_ */
