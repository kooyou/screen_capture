#pragma once

class CSaveBufferFile
{
public:
	CSaveBufferFile(void);
	~CSaveBufferFile(void);

public:
	int SaveBuffer(BYTE* buffer,int bufferSize);
	int SaveBufferNotLen(BYTE* buffer,int bufferSize);
	BOOL InitSaveFile(CString filePath);

private:
	CStdioFile mv_file;
	CString mv_filePath;
	BOOL mv_isOpen;
	int mv_fileSize;
	int mv_currentPos;

	int mv_saveTimes;
};
