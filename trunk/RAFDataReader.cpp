#include "stdafx.h"
#include "RAFError.h"
#include "RAFReader.h"
#include "RAFDataReader.h"

bool  RAFDataReader::makeSureOutputFolderExists(const wstring& outDir)
{
	unsigned int pos = 0;
	do
	{
		pos = outDir.find_first_of(_T("\\/"), pos + 1);
		CreateDirectory(outDir.substr(0, pos).c_str(), NULL);
	}

	while (pos != string::npos);
	return true;
}

bool  RAFDataReader::createFolderTree(const wstring& basePath, const wstring& relativePath)
{
	wchar_t drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_FNAME];
	_wsplitpath_s(relativePath.c_str(), drive, dir, fname, ext);

	wstring pathFolders = dir;

	unsigned int pos = 0;
	do
	{
		pos = pathFolders.find_first_of(_T("\\/"), pos + 1);
		wstring fullPath = basePath + pathFolders.substr(0, pos);
		CreateDirectory(fullPath.c_str(), NULL);
	}

	while (pos != string::npos);
	return true;
}

bool RAFDataReader::extractFile(const wstring& outDir)
{
	wstring dataPath = archive;
	dataPath += _T(".dat");

	ifstream inFile(dataPath, ios::in | ios::binary);
	if (!inFile.is_open())
	{
		throw RAFError("An error occurred opening RAF data file.");
	}

	makeSureOutputFolderExists(outDir);

	unsigned long inputSize = file.dataSize;
	unsigned long outputSize;

	inFile.seekg(file.dataOffset, ios_base::beg);
	unsigned char* inBuffer = new unsigned char[inputSize];
	inFile.read(reinterpret_cast<char*>(inBuffer), inputSize);

	wstring basePath = outDir;

	if (basePath.back() != _T('\\'))
	{
		basePath.append(_T("\\"));
	}

	wstring relativePath(file.pathString.begin(), file.pathString.end());

	replace(relativePath.begin(), relativePath.end(), '/', '\\');

	wstring fullPath = basePath + relativePath;

	createFolderTree(basePath, relativePath);
	
	ofstream outFile(fullPath, ios::out | ios::binary | ios::trunc);
	if (!outFile.is_open())
	{
		throw RAFError("An error occurred attempting to create an output file.");
	}

	unsigned char* outBuffer;

	if (inputSize <= (RAFDataReader::defaultOutputSize / RAFDataReader::maxCompressionRatio))
	{
		outputSize = RAFDataReader::defaultOutputSize;
	}

	else
	{
		outputSize = inputSize * RAFDataReader::maxCompressionRatio;
	}

	outBuffer = new (std::nothrow) unsigned char[outputSize];

	if (!outBuffer)
	{
		delete[] inBuffer;
		delete[] outBuffer;
		inFile.close();

		throw RAFError("Insufficient memory available when trying to allocate buffer.");
	}

	int zlibReturnValue = uncompress(outBuffer, &outputSize, inBuffer, inputSize);

	if (zlibReturnValue == Z_DATA_ERROR)
	{
		outFile.write(reinterpret_cast<char*>(inBuffer), inputSize);
		delete[] inBuffer;
		delete[] outBuffer;
		inFile.close();
		return true;
	}

	else if (zlibReturnValue == Z_BUF_ERROR)
	{
		delete[] inBuffer;
		delete[] outBuffer;
		inFile.close();
		throw RAFError("An error occurred while inflating file.");

	}

	outFile.write(reinterpret_cast<char*>(outBuffer), outputSize);
	inFile.close();

	delete[] inBuffer;
	delete[] outBuffer;

	return true;
}

RAFDataReader::~RAFDataReader()
{
}
