#pragma once

typedef struct
{
	BYTE	biType;			//Tag类型
	DWORD	dwDataSize;		//数据长度
	DWORD	dwTimeStamp;	//时间戳前3字节大端格式，最后一字节为最高位
	DWORD	dwStreamID;		//流ID，始终为0
	BYTE*	pData;
	DWORD	dwTagSize;
	BYTE*	pNextTag;		//下一个Tag地址
	BYTE*	pMemEnd;		//内存结束地址
}FLV_TAG;


class CFlvFile
{
public:
	//创建FLV文件
	BOOL Create(LPCTSTR pzFileName);

	//追加数据
	void Write(LPCVOID lpData, UINT nLen);

	//追加Tag
	void WriteTag(FLV_TAG *pTag);

	//识别是否正确的FLV
	BOOL IsVaildFlvMem(LPCVOID lpMem);

	//获取第一个Tag
	BOOL GetFirstTagFromMem(FLV_TAG* pTag, LPCVOID lpMem, UINT nSize);

	//获取下一个Tag
	BOOL GetNextTagFromMem(FLV_TAG* pTag);

	//从内存获取除去Meta信息的数据
	BOOL GetNoScriptDataFromMem(LPCVOID lpMem, UINT nMemSize, LPVOID &lpData, UINT *nDataSize);

	//从内存获取除去第一个关键帧的数据
	BOOL GetNoKeyTagDataFromMem(LPCVOID lpMem, UINT nMemSize, LPVOID &lpData, UINT *nDataSize);

	//关闭
	void Close();


private:
	//巅倒字节顺序
	void Big2Littel(LPCVOID lpDest, LPVOID lpSrc, UINT Len);


private:
	CFile m_hFile;
};
