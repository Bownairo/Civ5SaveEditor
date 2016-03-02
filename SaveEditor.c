#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main (int argc, char *argv[]) {

	//Set up file stuff
	int len = 255;
	char fileName[len];
	char fileNameOut[len];

	//Chech args
	if(argc != 3) {
		printf("Usage: SaveEditor inFile outFile\n");
		return EXIT_FAILURE;
	}

	//Grab params
	if(strlen(argv[1]) < len)
		strcpy(fileName, argv[1]);
	if(strlen(argv[2]) < len)
		strcpy(fileNameOut, argv[2]);

	//Open file and get size
	FILE *fp;
	fp = fopen(fileName, "rb");
	FILE *ofp;
	ofp = fopen(fileNameOut, "wb");
	fseek(fp, 0L, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	//Read in the file to a buffer
	unsigned char buff[size];
	fread(buff, sizeof(unsigned char), size, fp);

	//Move the pointer to the beginning of the settings
	int findPointer = 0;
	while(!(buff[findPointer] == 'G' && buff[findPointer + 1] == 'A'
				&& buff[findPointer + 2] == 'M' && buff[findPointer + 3] == 'E'
				&& buff[findPointer + 4] == 'O' && buff[findPointer + 5] == 'P'
				&& buff[findPointer + 6] == 'T' && buff[findPointer + 7] == 'I'
				&& buff[findPointer + 8] == 'O' && buff[findPointer + 9] == 'N'))
		findPointer++;

	//Move the pointer to the size indicator
	findPointer -= 8;

	//If pitboss is already a setting
	int exists = 0;
	for(int i = 0; i < size - 7; i++) {
		if(buff[i] == 'P' && buff[i + 1] == 'I'
					&& buff[i + 2] == 'T' && buff[i + 3] == 'B'
					&& buff[i + 4] == 'O' && buff[i + 5] == 'S'
					&& buff[i + 6] == 'S') {
			findPointer = i;
			exists = 1;
		}
	}

	//If pitboss doesn't exist, add it to the file
	if(!exists) {
		buff[findPointer]++;

		//Find the !
		while(buff[findPointer] != '!')
			findPointer++;

		//Make a new buffer that holds everything prior to !
		int startBuffLen = findPointer;
		unsigned char startBuff[startBuffLen];
		for(int i = 0; i < startBuffLen; i++)
			startBuff[i] = buff[i];

		//Make a new buffer that holds everything with ! and after
		int endBuffLen = size - findPointer;
		unsigned char endBuff[endBuffLen];
		for(int i = findPointer; i < size; i++)
			endBuff[i - findPointer] = buff[i];

		//12 00 00 00 G A M E O P T I O N _ P I T B O S S 01 00 00 00
		int midBuffLen = 26;
		unsigned char midBuff[] = {0x12, 0x00, 0x00, 0x00,
					'G', 'A', 'M', 'E', 'O', 'P', 'T', 'I', 'O', 'N',
					'_', 'P', 'I', 'T', 'B', 'O', 'S', 'S',
					0x01, 0x00, 0x00, 0x00};

		//Combine all 3
		int finBuffLen = startBuffLen + midBuffLen + endBuffLen;
		unsigned char finBuff[finBuffLen];
		for(int i = 0; i < findPointer; i++)
			finBuff[i] = startBuff[i];
		for(int i = 0; i < 26; i++)
			finBuff[i + findPointer] = midBuff[i];
		for(int i = 0; i < size - findPointer; i++)
			finBuff[i + findPointer + 26] = endBuff[i];

		fwrite(finBuff, sizeof(unsigned char), finBuffLen, ofp);
		printf("Adding pitboss\n");
	}
	//If pitboss does exist, set it's value to 00
	else {
		while(buff[findPointer] != 0x01)
			findPointer++;
		buff[findPointer] = 0x00;

		fwrite(buff, sizeof(unsigned char), size, ofp);
		printf("Removing pitboss\n");
	}

	//Close files
	fclose(fp);
	fclose(ofp);

	printf("Everything probably worked pretty okay.\n");
	return EXIT_SUCCESS;
}
