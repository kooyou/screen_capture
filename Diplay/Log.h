#pragma once

class CLog
{
public:
	CLog(void);
	~CLog(void);

public:
	void WriteLog(CString logInfo);
private:
	CStdioFile m_pvt_file;
	CString m_pvt_filePath;
	BOOL m_pvt_IsOpen;
};
