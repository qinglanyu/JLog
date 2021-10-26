#pragma once
#ifndef __JLOG_H__
#define __JLOG_H__

/********************************************************************
	Name:		JLog.h
	Author:		qinglanyu
	Version:	v1.0.0
	Copyright:	2017-2025. All Rights Reserved.
	E-mail:		qinglanyu_jun@foxmail.com
	Description: This log component may help you debug your project
quickly.
*********************************************************************/

#include <afx.h>
#include <sys/timeb.h>


// compile with: /FC
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define WC_FILE_ WIDEN(__FILE__)

#define JLOGTEST()	CJLog::GetInstance()->WriteLog(_T("CJLog Test(????) start... @%d in %s"), __LINE__, WC_FILE_);
#define JLOG(x)		CJLog::GetInstance()->WriteLog(_T("%s @%d in %s"), x, __LINE__, WC_FILE_);

#define _TIMESTAMP_		( CTime::GetCurrentTime().Format(_T("[%Y-%m-%d %H:%M:%S] ")) )


class CJLog
{
public:
	enum { MAX_STRING_LENGTH = 8192};
	enum { DEFAULT_MAX_LOGFILE_SIZE = 10 * 1024 * 1024};	// default max log file size : 10MB

	CJLog();
	~CJLog();

	static CJLog* GetInstance()
	{
		static CJLog Jlog;
		return &Jlog;
	}

	void WriteLog(LPCTSTR Format, ...);
	void DbgPrint(LPCTSTR Format, ...);

protected:
	CString	NewFileName();
	CString	GetLogPath();

	BOOL	IsFolderExists(const CString strPath);
	BOOL	MkDir(CString strPath);

	BOOL	W2C(const wchar_t* wstr, char* str);

	BOOL	IsFileValid();
private:
	CFile	m_logFile;

	int	 m_nMaxLogSize;
};

#endif // !__JLOG_H__