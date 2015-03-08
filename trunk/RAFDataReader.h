#pragma once

class RAFDataReader
{
private:
	static const int defaultOutputSize = 50000000;
	static const int maxCompressionRatio = 5;
	const RAFFile& file;
	const wstring& archive;

	bool makeSureOutputFolderExists(const wstring& outDir);
	bool createFolderTree(const wstring& basePath, const wstring& relativePath);
public:
	bool extractFile(const wstring& outDir);

	RAFDataReader(const RAFFile& inFile, const wstring& inArchive) : file(inFile), archive(inArchive){};
	~RAFDataReader();
};

