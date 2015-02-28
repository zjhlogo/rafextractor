#pragma once

#include "RAFError.h"

using namespace std;

struct RAFFile
{
	// A hash of this file's path. 
	int fileHash;

	// The offset from the beginning of the associated .raf.dat file. 
	int dataOffset;

	// The size of the data stored in the associated.raf.dat file.
	int dataSize;

	//The index into the path list. This value should be an integer between 0 and the path list count - 1 (inclusive).
	int pathListIndex;

	//char* pathString;
	string pathString;
};

struct RAFPath
{
	// The number of bytes the path string is offset from the path list (not the beginning of the file) 
	int pathOffset;

	// The length of the path string in bytes.
	int pathLength;
};

class RAFReader
{
private:
	const wchar_t* filePath;

	// A fixed value indicating this is a valid RAF file. The value is 0x18be0ef0.
	int magicNumber;

	// The version of the RAF format used by this file. 
	int fileVersion;

	// A value used internally by Riot. DO NOT MODIFY.
	int managerIndex;

	// The offset from the start of the .raf file to the File List. 
	int fileListOffset;

	// The offset from the start of the .raf file to the Path List. 
	int pathListOffset;

	// A count of file entries. 
	int numFileEntries;

	// The number of bytes contained in the path list. 
	int pathListSize;

	// The number of entries in the path list. 
	int pathListCount;

	vector<RAFPath> pathEntries;

	bool readHeader();
	bool getFiles();
	bool getPaths();
	bool getPathStrings();

public:
	vector<RAFFile> fileEntries;

	bool readFile(const wchar_t* path);
	RAFReader();
	~RAFReader();
};

