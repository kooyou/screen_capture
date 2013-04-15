#pragma once

class CLog
{
public:
	CLog(void);
	~CLog(void);

public:
	void WriteLog(CString logInfo);
private:
	CStdioFile mv_file;
	CString mv_filePath;
	BOOL mv_IsOpen;

	SYSTEMTIME mv_sysTime;
};
