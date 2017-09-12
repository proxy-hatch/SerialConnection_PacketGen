//============================================================================
// Name        : Ensc351Part1.cpp
// Version     : September 5th
// Copyright   : Copyright 2017, Craig Scratchley
// Description : Starting point for Part 1 of Multipart Project
//============================================================================

#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <iostream>

#include "SenderX.h"
#include "myIO.h"

using namespace std;

int testSenderX() {
#ifdef __MINGW32__
	_fmode = _O_BINARY;  // needed for MinGW compiler which runs on MS Windows
#endif



	const char* oFileName = "xmodem_sender_data.dat";
	mode_t mode = S_IRUSR | S_IWUSR; // | S_IRGRP | S_IROTH;
	int mediumD = myCreat(oFileName, mode);
	if(mediumD == -1) {
		cout /* cerr */ << "Error opening medium file named: " << oFileName << endl;
		ErrorPrinter("creat(oFileName, mode)", __FILE__, __LINE__, errno);
	}

	SenderX xSender("inputTextFile.txt", mediumD);
	xSender.Crcflg = false; // test sending with checksum
	cout << "test sending with checksum" << endl;
	xSender.sendFile();
	cout << "Sender finished with result: " << xSender.result << endl << endl;

	SenderX xSender2("inputTextFile.txt", mediumD);
	xSender2.Crcflg = true; // test sending with CRC
	cout << "test sending with CRC" << endl;
	xSender2.sendFile();
	cout << "Sender finished with result: " << xSender2.result << endl;

	if (-1 == myClose(mediumD))
		ErrorPrinter("close(mediumD)", __FILE__, __LINE__, errno);

	return 0;
}

int main() {
	return testSenderX();
}
