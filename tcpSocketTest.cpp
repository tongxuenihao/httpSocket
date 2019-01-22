// tcpSocketTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "targetver.h"
#include<iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
using namespace std;

#pragma comment(lib, "ws2_32.lib")
#pragma  comment(lib, "json_vc71_libmtd.lib")

SOCKET socketFd;
int recvThreadFlag = 0;

int tcpSocketCreate()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("初始化Winsock失败");
		return 0;
	}
	SOCKADDR_IN addrSrv;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(6690);
	inet_pton(AF_INET, "192.168.0.103", &addrSrv.sin_addr);

	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);
	if (SOCKET_ERROR == sockClient) 
	{
		printf("Socket() error:%d", WSAGetLastError());
		return 0;
	}
	if (connect(sockClient, (struct  sockaddr*)&addrSrv, sizeof(addrSrv)) == INVALID_SOCKET) 
	{
		printf("连接失败:%d", WSAGetLastError());
		return 0;
	}
	printf("连接成功\n");
	return sockClient;
}

int tcpSocketSend(SOCKET socketFd, char *buff, int len)
{
	return send(socketFd, buff, len, 0);
}

#if 0
void itemCodeSet(int channel)
{
	mChannel = channel;
}

int itemCodeGet()
{
	return mChannel;
}

void macCodeSet(string mac)
{
	mMac = mac;
}

string macCodeGet()
{
	return mMac;
}

void timeCodeSet(string time)
{
	mTime = time;
}

string timeCodeGet()
{
	return mTime;
}


void freqErrCodeSet(int freqErr)
{
	mFreqErr = freqErr;
}

int freqErrCodeGet()
{
	return mFreqErr;
}
#endif

std::string jsonHardwarePacketCreate()
{
	Json::Value arrayObj, arrayObjExt;
	Json::Value jsonData;
	Json::Value testValueExt;
	jsonData["ITEM_CODE"] = "WIFI_TX_CH7_11M";
	jsonData["MAC_ADDRESS"] = "84:7C:9B:00:03:40";
	jsonData["PROCESS_STATUS"] = "Pending";
	jsonData["TEST_DATETIME"] = "2018-12-19 15:20:35";
	jsonData["TEST_RESULT"] = 1;

	testValueExt["ITEM_CODE_SUFFIX"] = "FreqErr";
	testValueExt["TEST_VALUE"] = 2;
	arrayObjExt.append(testValueExt);
	jsonData["TEST_VALUE_EXT"] = arrayObjExt;

	arrayObj.append(jsonData);
	arrayObj.toStyledString();
	std::string out = arrayObj.toStyledString();
	return out;
}

std::string jsonMacPacketCreate()
{
	Json::Value arrayObj, arrayObjExt;
	Json::Value jsonData;

	jsonData["MAC_ADDRESS"] = "84:7C:9B:00:03:40";
	jsonData["PROCESS_STATUS"] = "Pending";
	jsonData["TEST_DATETIME"] = "2018-12-19 15:20:35";
	jsonData["TEST_RESULT"] = 1;
	arrayObj.append(jsonData);
	arrayObj.toStyledString();
	std::string out = arrayObj.toStyledString();
	return out;
}

void httpHardwarePostPacketBuild()
{
	char *url = "/api/iot/testing/hardware/stduri/bulkadd";
	const char *payLoad;
	char txBuff[1024] = { 0 };
	std::string jsonStr = jsonHardwarePacketCreate();
	payLoad = jsonStr.data();
	int payLoadLenth = strlen(payLoad);
	snprintf((char *)txBuff, 1024, "%s %s %s%s%s %s%s%s %d%s%s%s%s%s",
		"POST", url, "HTTP/1.1", kCRLFNewLine,
		"Host:", "192.168.0.103", kCRLFNewLine,
		"Content-Length:", payLoadLenth, kCRLFNewLine,
		"Content-Type: application/json", kCRLFNewLine,
		kCRLFNewLine,
		payLoad
	);
	std::cout << "send :" << txBuff << std::endl;
	tcpSocketSend(socketFd, txBuff, strlen(txBuff));
}

void httpMacPostPacketBuild()
{
	char *url = "/api/iot/testing/burning/stduri/bulkadd";
	const char *payLoad;
	char txBuff[1024] = { 0 };
	std::string jsonStr = jsonMacPacketCreate();
	payLoad = jsonStr.data();
	int payLoadLenth = strlen(payLoad);
	snprintf((char *)txBuff, 1024, "%s %s %s%s%s %s%s%s %d%s%s%s%s%s",
		"POST", url, "HTTP/1.1", kCRLFNewLine,
		"Host:", "192.168.0.103", kCRLFNewLine,
		"Content-Length:", payLoadLenth, kCRLFNewLine,
		"Content-Type: application/json", kCRLFNewLine,
		kCRLFNewLine,
		payLoad
	);
	std::cout << "send :" << txBuff << std::endl;
	tcpSocketSend(socketFd, txBuff, strlen(txBuff));
}

void msgHandle(char *data)
{
	Json::Reader Reader;
	Json::Value jsonData;
	Json::Value jsonDataElement;
	Json::Value jsonDataElementObj;
	int returnval;
	char *header = NULL;
	char *body = NULL;
	header = strstr(data, kCRLFLineEnding);
	if (header)
	{
		body = header + strlen("\r\n\r\n");
	}
	else
	{
		return;
	}

	Reader.parse(body, jsonData);
	bool out = jsonData["success"].asBool();
	if (out == true)
	{
		jsonDataElement = jsonData["data"];
		if (jsonDataElement.isArray())
		{
			jsonDataElementObj = jsonDataElement[(unsigned int)0];
			if (jsonDataElementObj["ITEM_CODE"].isString())
			{
				goto HARDWARE;

			}
			else
			{
				goto MAC;
			}
		}
	}

HARDWARE:
	returnval = jsonDataElementObj["ReturnVal"].asInt();
	std::cout << "returnval:" << returnval << std::endl;
	if (returnval == 1)
	{
		httpMacPostPacketBuild();
	}
	return;

MAC:
	returnval = jsonDataElementObj["ReturnVal"].asInt();
	std::cout << "returnval:" << returnval << std::endl;
	if (returnval == 1)
	{
		std::cout << "success" << std::endl;
	}
	return;
}

DWORD WINAPI tcpSocketRecv(LPVOID lpParamter)
{
	int ret;
	int recvCount;
	fd_set fd;
	struct timeval tm;
	char rxBuff[1024];
	while (1)
	{
		tm.tv_sec = 10;
		tm.tv_usec = 0;
		memset(rxBuff, 0, 1024);
		FD_ZERO(&fd);
		FD_SET(socketFd, &fd);
		ret = select(socketFd + 1, &fd, NULL, NULL, &tm);
		if (ret <= 0)
		{
			continue;
		}
		else
		{
			recvCount = recv(socketFd, rxBuff, 1024, 0);
			std::cout << "recv :" << rxBuff << std::endl;
			msgHandle(rxBuff);
		}
		Sleep(2000);
	}
}

void projectRunLoop()
{
	HANDLE hThread;
	DWORD dwThreadId;
	httpHardwarePostPacketBuild();
	if (recvThreadFlag == 0)
	{
		hThread = CreateThread(NULL, NULL, tcpSocketRecv, NULL, 0, &dwThreadId);
		recvThreadFlag = 1;
	}
}

int main()
{
	socketFd = tcpSocketCreate();
	projectRunLoop();
	while (1)
	{
		Sleep(5000);
	}
	return 0;
}


