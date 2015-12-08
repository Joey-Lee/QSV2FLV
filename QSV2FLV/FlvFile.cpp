#include "StdAfx.h"
#include "FlvFile.h"

static BYTE g_biFlvHead[]={0x46,0x4C,0x56,0x01,0x05,0x00,0x00,0x00,0x09,0x00,0x00,0x00,0x00};

typedef struct
{
	BYTE biType;			//Tag类型
	BYTE biDataSize[3];		//数据长度
	BYTE biTimeStamp[3];	//时间戳3字节大端格式
	BYTE biTimeHiByte;		//时间戳高字节
	BYTE biStreamID[3];		//流ID，始终为0
}TAG_DATA;

//创建FLV文件
BOOL CFlvFile::Create(LPCTSTR pzFileName)
{
	if (m_hFile.m_hFile != INVALID_HANDLE_VALUE)
	{
		m_hFile.Close();
		OutputDebugString(_T("Create Not Closed!\n"));
	}
	BOOL bRet =  m_hFile.Open(pzFileName, CFile::modeCreate|CFile::modeReadWrite);
	if (!bRet)
		return FALSE;

	m_hFile.SeekToBegin();
	m_hFile.Write(g_biFlvHead, sizeof(g_biFlvHead));
	OutputDebugString(_T("Create FLV OK:"));
	OutputDebugString(pzFileName);
	return TRUE;
}

//追加数据
void CFlvFile::Write(LPCVOID lpData, UINT nLen)
{
	m_hFile.SeekToEnd();
	m_hFile.Write(lpData, nLen);
}


void CFlvFile::WriteTag(FLV_TAG *pTag)
{
	TAG_DATA tagData;
	DWORD    dwTagSize = 0;
	ZeroMemory(&tagData, sizeof(TAG_DATA));
	tagData.biType = pTag->biType;
	Big2Littel(tagData.biDataSize, &pTag->dwDataSize, 3);
	Big2Littel(tagData.biTimeStamp, &pTag->dwTimeStamp, 3);
	tagData.biTimeHiByte = (BYTE)((pTag->dwTimeStamp >> 24)&0xFF);
	Big2Littel(&dwTagSize, &pTag->dwTagSize, 4);

	m_hFile.SeekToEnd();
	m_hFile.Write(&tagData, sizeof(TAG_DATA));
	m_hFile.Write(pTag->pData, pTag->dwDataSize);
	m_hFile.Write(&dwTagSize, 4);
}

//识别是否正确的FLV
BOOL CFlvFile::IsVaildFlvMem(LPCVOID lpMem)
{
	int nCmp = memcmp(g_biFlvHead, lpMem, sizeof(g_biFlvHead));
	if (nCmp == 0)
		return TRUE;
	return FALSE;
}


//获取第一个Tag
BOOL CFlvFile::GetFirstTagFromMem(FLV_TAG* pTag, LPCVOID lpMem, UINT nMemSize)
{
	TAG_DATA* pTagData = NULL;
	pTagData = (TAG_DATA*)((BYTE*)lpMem+13);
	memset(pTag, 0, sizeof(FLV_TAG));

	pTag->biType = pTagData->biType;	//类型
	if ((pTag->biType != 0x08)&&(pTag->biType != 0x09)&&(pTag->biType != 0x12))
		return FALSE;

	Big2Littel(&pTag->dwDataSize, pTagData->biDataSize, 3);	//数据长度
	Big2Littel(&pTag->dwTimeStamp, pTagData->biTimeStamp, 3);//时间
	pTag->dwTimeStamp |= (pTagData->biTimeHiByte<<24);
	Big2Littel(&pTag->dwStreamID, pTagData->biStreamID, 3);	//流ID
	if (pTag->dwStreamID != 0)
		return FALSE;
	pTag->pData = ((BYTE*)pTagData + sizeof(TAG_DATA));		//数据
	pTag->dwTagSize= pTag->dwDataSize + 11;					//Tag总长度
	pTag->pNextTag = ((BYTE*)pTagData + pTag->dwTagSize + 4);	//下一个Tag
	pTag->pMemEnd = (BYTE*)lpMem + nMemSize;

	if (pTag->pNextTag >= pTag->pMemEnd)	//内存超界
		return FALSE;

	//验证Tag
	DWORD dwTagSize = 0;
	Big2Littel(&dwTagSize, (pTag->pNextTag-4), 4);
	if (dwTagSize != pTag->dwTagSize)
		return FALSE;

	return TRUE;
}

BOOL CFlvFile::GetNextTagFromMem(FLV_TAG* pTag)
{
	TAG_DATA* pTagData = NULL;
	pTagData = (TAG_DATA*)(pTag->pNextTag);

	if (pTag->pNextTag >= pTag->pMemEnd)	//内存超界
		return FALSE;

	pTag->biType = pTagData->biType;	//类型
	if ((pTag->biType != 0x08)&&(pTag->biType != 0x09)&&(pTag->biType != 0x12))
		return FALSE;

	pTag->dwDataSize = 0;
	Big2Littel(&pTag->dwDataSize, pTagData->biDataSize, 3);	//数据长度

	pTag->dwTimeStamp = 0;
	Big2Littel(&pTag->dwTimeStamp, pTagData->biTimeStamp, 3);//时间
	pTag->dwTimeStamp |= (pTagData->biTimeHiByte<<24);

	pTag->dwStreamID = 0;
	Big2Littel(&pTag->dwStreamID, pTagData->biStreamID, 3);	//流ID
	if (pTag->dwStreamID != 0)
		return FALSE;

	pTag->pData = ((BYTE*)pTagData + sizeof(TAG_DATA));		//数据
	pTag->dwTagSize= pTag->dwDataSize + 11;					//Tag总长度
	pTag->pNextTag = ((BYTE*)pTagData + pTag->dwTagSize + 4);	//下一个Tag

	//验证Tag
	DWORD dwTagSize = 0;
	Big2Littel(&dwTagSize, (pTag->pNextTag-4), 4);
	if (dwTagSize != pTag->dwTagSize)
		return FALSE;

	return TRUE;
}

//从内存获取跳过Meta信息的数据
//Tag长度 Type+Size+Time+Stream+TagSize=1+3+4+3+4=15
//头部 13
BOOL CFlvFile::GetNoScriptDataFromMem(LPCVOID lpMem, UINT nMemSize, LPVOID &lpData, UINT *nDataSize)
{
	BOOL	bRet;
	FLV_TAG flvTag;
	bRet = GetFirstTagFromMem(&flvTag, lpMem, nMemSize);
	if (!bRet)
		return FALSE;

	lpData = flvTag.pNextTag;
	*nDataSize = (UINT)(flvTag.pMemEnd - flvTag.pNextTag);
	return TRUE;
}

//从内存获取除去第一个关键帧的数据
//实际就是跳过3帧
BOOL CFlvFile::GetNoKeyTagDataFromMem(LPCVOID lpMem, UINT nMemSize, LPVOID &lpData, UINT *nDataSize)
{
	BOOL	bRet;
	FLV_TAG flvTag;
	bRet = GetFirstTagFromMem(&flvTag, lpMem, nMemSize);	//跳过0x12
	if (!bRet)
		return FALSE;

	bRet = GetNextTagFromMem(&flvTag);	//跳过0x09
	if (!bRet)
		return FALSE;
	bRet = GetNextTagFromMem(&flvTag);	//跳过0x08
	if (!bRet)
		return FALSE;
	
	lpData = flvTag.pNextTag;
	*nDataSize =(UINT)(flvTag.pMemEnd - flvTag.pNextTag);
	return TRUE;
}

//关闭
void CFlvFile::Close()
{
	m_hFile.Close();
	OutputDebugString(_T("FLV file Closed!\n"));
}


void CFlvFile::Big2Littel(LPCVOID lpDest, LPVOID lpSrc, UINT Len)
{
	UINT i;
	for(i=0; i<Len; i++)
	{
		((BYTE*)lpDest)[i] = ((BYTE*)lpSrc)[Len-i-1];
	}
}
