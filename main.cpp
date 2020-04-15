/***********************************************************************************************
* copyright (C)
*
*FileName: Music.cpp
*Author: dc wang
*Description: ���ֲ������ײ����
*Verstion: 1.0
*Function List:  1._tmain()  �������
                 2.
*History��
<author>     <time>       <verstion>         <desc>
dc wang    2018/02/11        1.0        build this moudle
************************************************************************************************/


// Music.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include<stdio.h>
#include<Windows.h>     /*Windowsϵͳͷ�ļ�*/
#include<mmsystem.h>    /* ��ý��ӿ��ļ�  */
#pragma comment(lib,"Winmm.lib")
#include"tchar.h"

int _tmain(int argc, _TCHAR* argv[])
{
	MMCKINFO    mmckinfoParent;
	MMCKINFO    mmckinfoSubChunk;       /* ����������Ϣ */
	DWORD       dwFmtSize;
	HMMIO       m_hmmio;                /* ��Ƶ�ļ���� */
	DWORD       m_WaveLong;             /* �ļ�����     */

	HPSTR       lpData;                 /* ��Ƶ����     */
	HANDLE      m_hData;                /*ͨ�����ݾ��  */
	HANDLE      m_hFormat;              /*ͨ�þ����ʽ  */
	WAVEFORMATEX   *lpFormat;           /*wav��ʽ       */
	DWORD       m_dwDateOffset;         /*����          */

	DWORD       m_dwDateSize;            /* ���ݴ�С                  */
	WAVEHDR     pWaveOutHdr;             /* ��Ƶ���ݿ黺��ṹ        */
	WAVEOUTCAPS pwoc;                    /* ������Ƶ����豸����      */
	HWAVEOUT    hWaveOut;                /*�����ʹ��DirectX�ӿ����  */

	int  SoundOffset = 0;
	int  SoundLong = 0;
	int  DevsNum;

	/*�򿪲����ļ�*/
	if(!(m_hmmio = mmioOpen( argv[1],NULL,MMIO_READ | MMIO_ALLOCBUF)))        //һ���Ⱥţ�����
	{
	printf("���ļ�ʧ��.\n");
	system("pause");
	return false;

	}
	//�����ļ��Ƿ��������ļ�
	mmckinfoParent.fccType =mmioFOURCC( 'W','A','V','E');//ѡ�������
	if(mmioDescend(m_hmmio,(LPMMCKINFO)&mmckinfoParent,NULL,MMIO_FINDRIFF))
	{
		printf("NOT WAVE FILE AND QUIU\n");
		system("pause");
		return 0;
	}
	//Ѱ��'fmt'���ʽ
	mmckinfoSubChunk.ckid = mmioFOURCC('f','m','t',' ');
	if(mmioDescend(m_hmmio,&mmckinfoSubChunk,&mmckinfoParent,MMIO_FINDCHUNK))
	{
		printf("Can't find 'fat' chunk.\n ");
		system("pause");
		return 0;
	}

	//���'fmt'��Ĵ�С�������ڴ�
	dwFmtSize = mmckinfoSubChunk.cksize;
	//�Ӷ��з���ָ����С���ֽ���
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
	//�뿪 fmt ��
	mmioAscend(m_hmmio,&mmckinfoSubChunk,0);

	//Ѱ��data��
	mmckinfoSubChunk.ckid = mmioFOURCC('d','a','t','a');
	if(mmioDescend(m_hmmio,&mmckinfoSubChunk,&mmckinfoParent,MMIO_FINDCHUNK))
	{
		printf("Can't find data chunk");
		system("pause");
		return 0;
	}
	//��ȡdata���С
	m_dwDateSize =mmckinfoSubChunk.cksize;
	m_dwDateOffset = mmckinfoSubChunk.dwDataOffset;
	if(m_dwDateSize==0L)
	{
		printf("NO DATA IN THE 'DATA' CHUNK");
		system("pause");
		return  0;
	}
	//Ϊ��Ƶ�����ڴ�ռ�
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


//�����Ƶ�豸��������Ƶ�豸����
	if(waveOutGetDevCaps(WAVE_MAPPER,&pwoc,sizeof(WAVEOUTCAPS))!=0)
	{
		printf("Unable to allocate or lock memory.");
		system("pause");
		return 0;
	}
//�����Ƶ����豸�Ƿ��ܲ���ָ������Ƶ�ļ�
	DevsNum=WAVE_MAPPER;
	if(waveOutOpen(&hWaveOut,DevsNum,lpFormat,NULL,NULL,CALLBACK_NULL)!=0)
	{
		printf("Failed to open the Wave out devices.");
		system("pause");
	}

	//׼�������ŵ�����
	pWaveOutHdr.lpData = (HPSTR)lpData;
	pWaveOutHdr.dwBufferLength = m_WaveLong;
	pWaveOutHdr.dwFlags = 0;
	if(waveOutPrepareHeader(hWaveOut,&pWaveOutHdr,sizeof(WAVEHDR))!=0)
	{
		printf("Failed to prepare the wave data buffer.");
		system("pause");
		return 0;
	}
	//������Ƶ�����ļ�
	if(waveOutWrite(hWaveOut,&pWaveOutHdr,sizeof(WAVEHDR))!=0)
	{
		printf("failed to write the wave data buffer.");
		system("pause");
		return 0;
	}

	//�ر���Ƶ����豸���ͷ��ڴ�
	printf("����������\n");
	printf("���������ֹ...\n");
	getchar();
	waveOutReset(hWaveOut);
	waveOutClose(hWaveOut);
	LocalUnlock(m_hFormat);
	LocalFree(m_hFormat);
	delete[] lpData;
	return 0;
}

