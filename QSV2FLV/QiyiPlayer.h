#pragma once

#define		QSV_INFO_SIZE	0x50		//描述部分长度
#define		QSV_ENC_SIZE	0x400		//加密长度

//解密内容在内存中布局
//0x50 + 0x400
//0x38 Base
//0x40 Size
typedef struct
{
	BYTE	Reserved1[0x38];
	DWORD	dwOffset;				//在QSV文件中的偏移地址
	BYTE    Reserved2[4];
	DWORD	dwSize;					//分段文件长度
	BYTE	Reserved3[0x0C];
	BYTE	biFLV[QSV_ENC_SIZE];	//解密后的FLV头部
}QIYI_INFO;					//一共0x450字节



class CQiyiPlayer
{
public:
	//用于播放QSV文件，并检测播放器是否开始播放
	//返回1:发现播放器
	//返回0:没有发现播放器
	BOOL Play(LPCTSTR pzFileName);
	void Close();

	INT  GetPid(){return m_nPidPlayer;}

private:
	BOOL WaitForPlay(INT nTimeout);

private:
	INT  m_nPidPlayer;
	HWND m_hWndClient;
	HWND m_hWndPlayer;
};
