/********************************************************************
	Name:		JLog.h
	Author:		qinglanyu
	Version:	v1.0.0
	Copyright:	2017-2025. All Rights Reserved.
	E-mail:		qinglanyu_jun@foxmail.com
	Description: This log component may help you debug your project
quickly.
*********************************************************************/

#include "stdafx.h"
#include "JLog.h"

CJLog::CJLog()
{
	m_nMaxLogSize = DEFAULT_MAX_LOGFILE_SIZE;

	JLOGTEST();
}


CJLog::~CJLog()
{
	if (m_logFile.m_hFile != CFile::hFileNull)
	{
		m_logFile.Close();
	}
}

void CJLog::WriteLog(LPCTSTR Format, ...)
{
	TCHAR	str[MAX_STRING_LENGTH];
	TCHAR	chToolStr[MAX_STRING_LENGTH + 50];
	char	szLogText[MAX_STRING_LENGTH + 50];

	va_list	vaList;
	va_start(vaList, Format);
	_vstprintf_s(str, Format, vaList);
	va_end(vaList);

	wcscpy_s(chToolStr, _TIMESTAMP_);
	wcscat_s(chToolStr, str);
	wcscat_s(chToolStr, _T("\r\n"));

#ifdef DEBUG
	OutputDebugString(chToolStr);
#endif // DEBUG

	if (IsFileValid())
	{
		W2C(chToolStr, szLogText);
		m_logFile.Write(szLogText, strlen(szLogText));
		m_logFile.Flush();
	}	
}

void CJLog::DbgPrint(LPCTSTR Format, ...)
{
#ifdef _DEBUG
	TCHAR	str[MAX_STRING_LENGTH];
	TCHAR	chToolStr[MAX_STRING_LENGTH + 50];

	va_list	vaList;

	va_start(vaList, Format);

	_vstprintf_s(str, Format, vaList);

	va_end(vaList);

	wcscpy_s(chToolStr, _TIMESTAMP_);
	wcscat_s(chToolStr, str);
	wcscat_s(chToolStr, _T("\r\n"));

	OutputDebugString(chToolStr);
#endif
}

CString	CJLog::NewFileName()
{
	CString fileName = CTime::GetCurrentTime().Format(_T("%Y-%m-%d %H-%M-%S.log"));
	fileName.Insert(0, GetLogPath());

	return fileName;
}

CString	CJLog::GetLogPath()
{
	CString	strPath;
	GetModuleFileName(NULL, strPath.GetBuffer(256), 256);
	strPath.ReleaseBuffer(256);

	int nPos = strPath.ReverseFind('\\');
	strPath = strPath.Left(nPos + 1);
	strPath += _T("\\JLog\\");

	if (!IsFolderExists(strPath))
	{
		MkDir(strPath);
	}

	return strPath;
}

BOOL CJLog::IsFolderExists(const CString strPath)
{
	DWORD	attr = GetFileAttributes(strPath);
	return ((attr != (DWORD)-1 && (attr & FILE_ATTRIBUTE_DIRECTORY)));
}

BOOL CJLog::MkDir(CString strPath)
{
	int nLen = strPath.GetLength();
	if (nLen < 2)
	{
		return FALSE;
	}

	if (_T('\\') == strPath[nLen - 1])
	{
		strPath = strPath.Left(nLen - 1);
		nLen = strPath.GetLength();
	}

	if (nLen <= 0)
	{	
		return FALSE;
	}

	if (nLen <= 3)
	{
		return IsFolderExists(strPath);
	}

	if (IsFolderExists(strPath))
	{
		return TRUE;
	}

	CString	strParent = strPath.Left(strPath.ReverseFind(_T('\\')));
	if (strParent.GetLength() <= 0)
	{
		return FALSE;
	}

	BOOL bRet = MkDir(strParent);
	if (bRet)
	{
		SECURITY_ATTRIBUTES	sa;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = 0;
		bRet = (CreateDirectory(strPath, &sa) == TRUE);
		return bRet;
	}
	else
	{
		return FALSE;
	}
}

BOOL CJLog::W2C(const wchar_t* wstr, char* str)
{
	int nLen = WideCharToMultiByte(CP_OEMCP, 0, wstr, -1, str, 0, 0, 0);
	return nLen == WideCharToMultiByte(CP_OEMCP, 0, wstr, -1, str, nLen, 0, 0);
}

BOOL CJLog::IsFileValid()
{
	if (m_logFile.m_hFile == CFile::hFileNull)
	{
		if (!m_logFile.Open(NewFileName(), CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate))
		{
			DbgPrint(_T("Failed to open log file..."));
			return FALSE;
		}
	}

	if (m_logFile.GetLength() >= m_nMaxLogSize)
	{
		m_logFile.Close();
		if (!m_logFile.Open(NewFileName(), CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate))
		{
			DbgPrint(_T("Failed to open log file..."));
			return FALSE;
		}
	}

	return TRUE;
}