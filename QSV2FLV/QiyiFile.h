#pragma once

class CQiyiFile
{
public:
	BOOL Open(LPCTSTR pzFileName);
	void GetFileTitle(CString& szFileTitle);
	BOOL IsQiYiFile();
	INT  GetSectionCount();

	//读取分段数据，包括前0x400的加密数据
	BOOL ReadSection(DWORD dwBase, BYTE *pData, UINT nLen);
	void Close();

private:
	CFile m_hFile;
};
