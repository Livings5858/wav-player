/***********************************************************************************************
* copyright (C)
*
*FileName: Music.cpp
*Author: dc wang
*Description: 音乐播放器底层核心
*Verstion: 1.0
*Function List:  1._tmain()  程序入口
                 2.
*History：
<author>     <time>       <verstion>         <desc>
dc wang    2018/02/11        1.0        build this moudle
************************************************************************************************/


// Music.cpp : 定义控制台应用程序的入口点。
//

#include<stdio.h>
#include<Windows.h>     /*Windows系统头文件*/
#include<mmsystem.h>    /* 多媒体接口文件  */
#pragma comment(lib,"Winmm.lib")
#include"tchar.h"

int _tmain(int argc, _TCHAR* argv[])
{
	MMCKINFO    mmckinfoParent;
	MMCKINFO    mmckinfoSubChunk;       /* 部分数据信息 */
	DWORD       dwFmtSize;
	HMMIO       m_hmmio;                /* 音频文件句柄 */
	DWORD       m_WaveLong;             /* 文件长度     */

	HPSTR       lpData;                 /* 音频数据     */
	HANDLE      m_hData;                /*通用数据句柄  */
	HANDLE      m_hFormat;              /*通用句柄格式  */
	WAVEFORMATEX   *lpFormat;           /*wav格式       */
	DWORD       m_dwDateOffset;         /*数据          */

	DWORD       m_dwDateSize;            /* 数据大小                  */
	WAVEHDR     pWaveOutHdr;             /* 音频数据块缓存结构        */
	WAVEOUTCAPS pwoc;                    /* 返回音频输出设备性能      */
	HWAVEOUT    hWaveOut;                /*输出是使用DirectX接口输出  */

	int  SoundOffset = 0;
	int  SoundLong = 0;
	int  DevsNum;

	/*打开波形文件*/
	if(!(m_hmmio = mmioOpen( argv[1],NULL,MMIO_READ | MMIO_ALLOCBUF)))        //一个等号，待定
	{
	printf("打开文件失败.\n");
	system("pause");
	return false;

	}
	//检查打开文件是否是声音文件
	mmckinfoParent.fccType =mmioFOURCC( 'W','A','V','E');//选择编码器
	if(mmioDescend(m_hmmio,(LPMMCKINFO)&mmckinfoParent,NULL,MMIO_FINDRIFF))
	{
		printf("NOT WAVE FILE AND QUIU\n");
		system("pause");
		return 0;
	}
	//寻找'fmt'块格式
	mmckinfoSubChunk.ckid = mmioFOURCC('f','m','t',' ');
	if(mmioDescend(m_hmmio,&mmckinfoSubChunk,&mmckinfoParent,MMIO_FINDCHUNK))
	{
		printf("Can't find 'fat' chunk.\n ");
		system("pause");
		return 0;
	}

	//获得'fmt'块的大小，申请内存
	dwFmtSize = mmckinfoSubChunk.cksize;
	//从堆中分配指定大小的字节数
	m_hFormat = LocalAlloc(LMEM_DISCARDABLE,LOWORD(dwFmtSize));
	if(!m_hFormat)
	{
		printf("Faileed Alloc Memory.\n");
		system("pause");
		return 0;
	}
	lpFormat = (WAVEFORMATEX*)LocalLock(m_hFormat);
	if(!lpFormat)
	{
		printf("Failed to lock the memeory.");
		system("pause");
		return 0;
	}
	if((unsigned long)mmioRead(m_hmmio,(HPSTR)lpFormat,dwFmtSize) != dwFmtSize)
	{
		printf("Failed to read format chunk.\n");
		system("pause");
		return 0;
	}
	//离开 fmt 块
	mmioAscend(m_hmmio,&mmckinfoSubChunk,0);

	//寻找data块
	mmckinfoSubChunk.ckid = mmioFOURCC('d','a','t','a');
	if(mmioDescend(m_hmmio,&mmckinfoSubChunk,&mmckinfoParent,MMIO_FINDCHUNK))
	{
		printf("Can't find data chunk");
		system("pause");
		return 0;
	}
	//获取data块大小
	m_dwDateSize =mmckinfoSubChunk.cksize;
	m_dwDateOffset = mmckinfoSubChunk.dwDataOffset;
	if(m_dwDateSize==0L)
	{
		printf("NO DATA IN THE 'DATA' CHUNK");
		system("pause");
		return  0;
	}
	//为音频分配内存空间
	lpData = new char[m_dwDateSize];
	if(!lpData)
	{
		printf("\n Can't Alloc Memory.\n");
		system("pause");
		return 0;
	}
	if(mmioSeek(m_hmmio,m_dwDateOffset,SEEK_SET)<0)
	{
		printf("Failed to read the data chunk.");
		system("pause");
		return 0;
	}
	m_WaveLong = mmioRead(m_hmmio,lpData,m_dwDateSize);
	if(m_WaveLong<0)
	{
		printf("Failed to read the data chunk.");
		system("pause");
		return 0;
	}


//检查音频设备，返回音频设备性能
	if(waveOutGetDevCaps(WAVE_MAPPER,&pwoc,sizeof(WAVEOUTCAPS))!=0)
	{
		printf("Unable to allocate or lock memory.");
		system("pause");
		return 0;
	}
//检查音频输出设备是否能播放指定的音频文件
	DevsNum=WAVE_MAPPER;
	if(waveOutOpen(&hWaveOut,DevsNum,lpFormat,NULL,NULL,CALLBACK_NULL)!=0)
	{
		printf("Failed to open the Wave out devices.");
		system("pause");
	}

	//准备待播放的数据
	pWaveOutHdr.lpData = (HPSTR)lpData;
	pWaveOutHdr.dwBufferLength = m_WaveLong;
	pWaveOutHdr.dwFlags = 0;
	if(waveOutPrepareHeader(hWaveOut,&pWaveOutHdr,sizeof(WAVEHDR))!=0)
	{
		printf("Failed to prepare the wave data buffer.");
		system("pause");
		return 0;
	}
	//播放音频数据文件
	if(waveOutWrite(hWaveOut,&pWaveOutHdr,sizeof(WAVEHDR))!=0)
	{
		printf("failed to write the wave data buffer.");
		system("pause");
		return 0;
	}

	//关闭音频输出设备，释放内存
	printf("请欣赏音乐\n");
	printf("按任意键终止...\n");
	getchar();
	waveOutReset(hWaveOut);
	waveOutClose(hWaveOut);
	LocalUnlock(m_hFormat);
	LocalFree(m_hFormat);
	delete[] lpData;
	return 0;
}

