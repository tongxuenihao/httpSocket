#pragma once

// ���� SDKDDKVer.h ��������õ���߰汾�� Windows ƽ̨��

// ���ҪΪ��ǰ�� Windows ƽ̨����Ӧ�ó�������� WinSDKVer.h������
// �� _WIN32_WINNT ������ΪҪ֧�ֵ�ƽ̨��Ȼ���ٰ��� SDKDDKVer.h��

#include <SDKDDKVer.h>
#include<WinSock2.h>


#define kCRLFNewLine "\r\n" 
#define kCRLFLineEnding "\r\n\r\n"

//int mChannel;
//int mFreqErr;
//string mMac;
//string mTime;
int tcpSocketCreate();

