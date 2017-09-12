//============================================================================
//
//% Student Name 1: student1
//% Student 1 #: 123456781
//% Student 1 userid (email): stu1 (stu1@sfu.ca)
//
//% Student Name 2: student2
//% Student 2 #: 123456782
//% Student 2 userid (email): stu2 (stu2@sfu.ca)
//
//% Below, edit to list any people who helped you with the code in this file,
//%      or put 'None' if nobody helped (the two of) you.
//
// Helpers: _everybody helped us/me with the assignment (list names or put 'None')__
//
// Also, list any resources beyond the course textbooks and the course pages on Piazza
// that you used in making your submission.
//
// Resources:  ___________
//
//%% Instructions:
//% * Put your name(s), student number(s), userid(s) in the above section.
//% * Also enter the above information in other files to submit.
//% * Edit the "Helpers" line and, if necessary, the "Resources" line.
//% * Your group name should be "P1_<userid1>_<userid2>" (eg. P1_stu1_stu2)
//% * Form groups as described at:  https://courses.cs.sfu.ca/docs/students
//% * Submit files to courses.cs.sfu.ca
//
// File Name   : SenderX.cc
// Version     : September 3rd, 2017
// Description : Starting point for ENSC 351 Project
// Original portions Copyright (c) 2017 Craig Scratchley  (wcs AT sfu DOT ca)
//============================================================================

#include <iostream>
#include <stdint.h> // for uint8_t
#include <string.h> // for memset()
#include <errno.h>
#include <fcntl.h>    // for O_RDWR

#include "myIO.h"
#include "SenderX.h"

using namespace std;

// DEBUG Macro, to be disabled upon release
#define DEBUG

SenderX::SenderX(const char *fname, int d)
        : PeerX(d, fname), bytesRd(-1), blkNum(255) {
}

//-----------------------------------------------------------------------------

/* tries to generate a block.  Updates the
variable bytesRd with the number of bytes that were read
from the input file in order to create the block. Sets
bytesRd to 0 and does not actually generate a block if the end
of the input file had been reached when the previously generated block was
prepared or if the input file is empty (i.e. has 0 length).
*/
void SenderX::genBlk(blkT blkBuf) {
    // ********* The next line needs to be changed ***********
    if (-1 == (bytesRd = myRead(transferringFileD, &blkBuf[DATA_POS], CHUNK_SZ)))	//DATA_POS=3
    {
        // If number is smaller than the number of bytes requested
        ErrorPrinter("myRead(transferringFileD, &blkBuf[0], CHUNK_SZ )", __FILE__, __LINE__, errno);
    }

    if (0 == bytesRd) {
        // If number of bytes read is zero indicates end of file or file is empty
        bytesRd = 0;
        return;
    }

    // ********* and additional code must be written ***********
    blkBuf[0] = SOH;
    blkBuf[SOH_OH] = (uint8_t) (blkNum + 1);        //SOH_OH=1
    blkBuf[BLK_NUM_AND_COMP_OH] = 255 - blkNum;        //BLK_NUM_AND_COMP_OH=2
    // pad the data string with CTRL_Z if its shorter than CHUNK_SZ
    if (bytesRd < CHUNK_SZ) {
        for (int i = bytesRd; i <= CHUNK_SZ; i++)
            blkBuf[DATA_POS + i] = CTRL_Z;
    }

    // Append the checksum/CRC after data and append str with NUll terminator
    if (Crcflg) {
        uint16_t CRCchecksum;
        crc16ns(&CRCchecksum, blkBuf+DATA_POS);
        // upper half
        blkBuf[PAST_CHUNK] = (uint8_t) (CRCchecksum >> 8);
        // lower half
        blkBuf[PAST_CHUNK+1] = (uint8_t) CRCchecksum;

        // does not need to append '\0' as PAST_CHUNK+1==BLK_SZ_CRC
//        if(PAST_CHUNK+1<BLK_SZ_CRC)
//            blkBuf[PAST_CHUNK+2] ='\0';
    } else {
        uint8_t checksum;
        chksum8ns(&checksum, blkBuf+DATA_POS);
        blkBuf[PAST_CHUNK] = checksum;

        blkBuf[PAST_CHUNK+1] ='\0';
    }
}

void SenderX::sendFile() {
    transferringFileD = myOpen(fileName, O_RDWR, 0);
#ifdef DEBUG
    cout /*cerr*/ << "Errorno of open() is " << errno << endl;
#endif
    if (transferringFileD == -1) {
        cout /* cerr */ << "Error opening input file named: " << fileName << endl;
        result = "OpenError";
    } else {
        cout << "Sender will send " << fileName << endl;

        blkNum = 0; // but first block sent will be block #1, not #0

        // do the protocol, and simulate a receiver that positively acknowledges every
        //	block that it receives.

        // assume 'C' or NAK received from receiver to enable sending with CRC or checksum, respectively
        genBlk(blkBuf); // prepare 1st block
        while (bytesRd) {
            blkNum++; // 1st block about to be sent or previous block was ACK'd

            // ********* fill in some code here to send a block ***********
            if (blkNum == 255) {
                blkNum = 0;
            }

            // send data
            if (-1 == myWrite(transferringFileD, &blkBuf, BLK_SZ_CRC))
                ErrorPrinter("myWrite(transferringFileD, &blkBuf, BLK_SZ_CRC)", __FILE__, __LINE__, errno);

            // assume sent block will be ACK'd
            genBlk(blkBuf); // prepare next block
            // assume sent block was ACK'd
        };
		// finish up the protocol, assuming the receiver behaves normally
        // ********* fill in some code here ***********

        /*  When sender has no more data, it sends an <eot>. It will resend the <eot>
   	   		if it receives <nak> or something invalid.
        	Note: The receiver will always send a <nak> after the first <eot>
        */

        uint8_t byte;
        //do
        {
            // Send <eot> using a parent function
            sendByte((uint8_t)EOT);

            // Listen for ACK
            if (-1 == (bytesRd = myRead(transferringFileD, &byte, 1)))
                ErrorPrinter("myRead(transferringFileD, &byte, 1 )", __FILE__, __LINE__, errno);
        }
        //while(byte != ACK); // byte is 0 at the time, loop forever

        //(myClose(transferringFileD));
        if (-1 == myClose(transferringFileD))
            ErrorPrinter("myClose(transferringFileD)", __FILE__, __LINE__, errno);
        result = "Done";
    }
}
