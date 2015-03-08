#include "stdafx.h"
#include "RAFReader.h"

bool RAFReader::readFile(const wchar_t* path)
{
	filePath = path;
	readHeader();
	getFiles();
	getPaths();
	getPathStrings();
	return true;
}

bool RAFReader::readHeader()
{
	ifstream inFile(filePath, ios::in | ios::binary);
	if (!inFile.is_open())
	{
		throw RAFError("An error occurred opening RAF.");
	}

	inFile.read(reinterpret_cast<char*>(&magicNumber), 4);

	if (magicNumber != 415108848)
	{
		throw RAFError("An error occurred when reading file header.");
	}

	inFile.read(reinterpret_cast<char*>(&fileVersion), 4);

	if (fileVersion != 1)
	{
		throw RAFError("Unexpected file version, this version is not currently supported.");
	}

	inFile.read(reinterpret_cast<char*>(&managerIndex), 4);
	inFile.read(reinterpret_cast<char*>(&fileListOffset), 4);
	inFile.read(reinterpret_cast<char*>(&pathListOffset), 4);

	return true;
}

bool RAFReader::getFiles()
{
	ifstream inFile(filePath, ios::in | ios::binary);
	if (!inFile.is_open())
	{
		throw RAFError("An error occurred opening RAF.");
	}

	inFile.seekg(fileListOffset, ios_base::beg);
	inFile.read(reinterpret_cast<char*>(&numFileEntries), 4);

	fileEntries.resize(numFileEntries);

	for (int i = 0; i < numFileEntries; i++)
	{
		inFile.read(reinterpret_cast<char*>(&fileEntries.at(i).fileHash), 4);
		inFile.read(reinterpret_cast<char*>(&fileEntries.at(i).dataOffset), 4);
		inFile.read(reinterpret_cast<char*>(&fileEntries.at(i).dataSize), 4);
		inFile.read(reinterpret_cast<char*>(&fileEntries.at(i).pathListIndex), 4);
	}
	return true;
}

bool RAFReader::getPaths()
{
	ifstream inFile(filePath, ios::in | ios::binary);
	if (!inFile.is_open())
	{
		throw RAFError("An error occurred opening RAF.");
	}

	inFile.seekg(pathListOffset, ios_base::beg);
	inFile.read(reinterpret_cast<char*>(&pathListSize), 4);
	inFile.read(reinterpret_cast<char*>(&pathListCount), 4);

	pathEntries.resize(pathListCount);

	for (int i = 0; i < pathListCount; ++i)
	{
		inFile.read(reinterpret_cast<char*>(&pathEntries.at(i).pathOffset), 4);
		inFile.read(reinterpret_cast<char*>(&pathEntries.at(i).pathLength), 4);
	}

	return true;
}

bool RAFReader::getPathStrings()
{
	ifstream inFile(filePath, ios::in | ios::binary);
	if (!inFile.is_open())
	{
		throw RAFError("An error occurred opening RAF.");
	}

	vector<char*> pathStrings;
	pathStrings.resize(pathListCount);

	for (int i = 0; i < pathListCount; ++i)
	{
		pathStrings.at(i) = new char[pathEntries.at(i).pathLength];

		inFile.seekg(pathListOffset + pathEntries.at(i).pathOffset, ios_base::beg);
		inFile.read(pathStrings.at(i), pathEntries.at(i).pathLength);
	}

	for (auto& i : fileEntries)
	{
		i.pathString = pathStrings.at(i.pathListIndex);
		delete[] pathStrings.at(i.pathListIndex);
	}
	return true;
}

RAFReader::RAFReader()
{
}

RAFReader::~RAFReader()
{
}