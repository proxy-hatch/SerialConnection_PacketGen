#ifndef SENDER_H
#define SENDER_H

#include <unistd.h>

#include "PeerX.h"

class SenderX : public PeerX
{
	friend int testSenderX();
public:
	SenderX(const char *fname, int d);
	void sendFile();
	ssize_t bytesRd;  // The number of bytes last read from the input file.

private:
	blkT blkBuf;    // A block
	uint8_t blkNum;	// number of the current block to be acknowledged
	void genBlk(blkT blkBuf);
};

#endif
