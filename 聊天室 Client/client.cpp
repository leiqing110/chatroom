#ifndef WIN32_LEAN_AND_MEAN 
#define WIN32_LEAN_AND_MEAN 
#endif #include <windows.h> 

#include <WinSock2.h> 
#include <WS2tcpip.h> 
#include <IPHlpApi.h> 
#include <stdio.h> 
#include <string.h>
#include <process.h>
#include <conio.h>
#include <stdlib.h>
#include "BaseData.h"
#include "ClientUserManage.h"
#include "ClientMassage.h"
#include "ClientNotice.h"
#include "ClientAdminManage.h"

#define DEFAULT_BUFLEN 1024
#define MASSAGE_BUFLEN 512 
#define IP_LEN 20

SOCKET g_connectSocket = INVALID_SOCKET;
UserData g_myUser;
bool g_isLogin = false;

int CreateConnectSocket(char *serverName, SOCKET *pConnectSocket)
{
	int nRet = SUCCESS;
	int iResult;
	sockaddr_in clientService;
	// ȷ����������ַ�Ͷ˿� 
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(serverName);
	clientService.sin_port = htons(DEFAULT_PORT);

	// �����׽������ӵ�������
	*pConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (*pConnectSocket == INVALID_SOCKET)
	{
		printf("Error at socket(): %ld\n", WSAGetLastError());
		nRet = ERROR_OTHER;
	}
	// �����ɹ�,���ӷ�����
	if (nRet == 0)
	{
		iResult = connect(*pConnectSocket, (SOCKADDR *)&clientService, sizeof(clientService));
		if (iResult == SOCKET_ERROR)
		{
			printf("Error at cnnect(): %d\n", iResult);
			nRet = ERROR_OTHER;
		}
	}

	return nRet;
}

unsigned __stdcall RecvMsgThread(void *pSocket)
{
	int iResult;
	char *recvbuf;
	DataHead dataHead;
	SOCKET *connectSocket = (SOCKET*)pSocket;
	
	do
	{
		// ��ȡ����ͷ
		iResult = RecvHead(*connectSocket, &dataHead);
		if (iResult == SOCKET_ERROR)
		{
			printf("recv failed! Error code: %d\n", GetLastError());
			break;
		}

		if (dataHead.response == RES_SUCCESS)
		{
			switch (dataHead.cmd)
			{
			case CMD_PRIVATECHAT:
				printf("˽����Ϣ���ͳɹ�!\n");
				break;
			case CMD_LOGOUT:
				printf("���Ѿ��˳���½!\n");
				g_isLogin = false;
				break;
			case CMD_SET_USER_NAME:
				printf("�����û����ɹ�\n");
				break;
			case  CMD_SET_USER_PWD:
				printf("�����û�����ɹ�\n");
				break;
			case CMD_SET_USER_OFFLINE:
				printf("ǿ���û����߳ɹ�\n");
				break;
			case  CMD_SET_USER_LEVEL:
				printf("�����û�Ȩ�޳ɹ�\n");
				break;
			default:
				break;
			}
		}
		else if (dataHead.response == RES_FAULT)
		{
			switch (dataHead.cmd)
			{
			case CMD_PRIVATECHAT:
				printf("˽����Ϣ����ʧ��!\n");
				break;
			case CMD_SET_USER_NAME:
				printf("�����û���ʧ��\n");
				break;
			case  CMD_SET_USER_PWD:
				printf("�����û�����ʧ��\n");
				break;
			default:
				break;
			}
		}
		else
		{

			recvbuf = (char *)malloc(dataHead.dataLen);
			if (recvbuf == NULL)
			{
				iResult = ERROR_MALLOC;
				printf("malloc failed!");
				continue;
			}

			iResult = RecvData(*connectSocket, recvbuf, dataHead.dataLen);
			if (iResult != SUCCESS)
			{
				break;
			}

			// �ж��������ͣ������������ݲ����
			switch (dataHead.cmd)
			{

			case CMD_GROUPCHAT:
				PrintGroupMassage(recvbuf);
				break;
			case CMD_PRIVATECHAT:
				PrintPrivateMassage(recvbuf);
				break;
			case CMD_SEND_ONLINE_USER:
				PrintOnlineUser(recvbuf);
				break;
			case CMD_SEND_USER:
				PrintUser(recvbuf);
				break;
			case CMD_SEND_CURRENT_USER:
				SetCurrentUserData(recvbuf);
				PrintOnlineUser(recvbuf);
				break;
			default:
				break;
			}
			free(recvbuf);
			recvbuf = NULL;
		}

	} while (g_isLogin);

	return iResult;
}

void Run()
{
	int nRet = SUCCESS;
	HANDLE hThread;
	char msgBuf[MASSAGE_BUFLEN];
	char userName[USER_NAME_LEN];
	char userPwd[USER_PWD_LEN];
	int option;
	printf("|--------------------------�����Ҳ˵���-------------------------------|\n");
	printf("| 1.����Ⱥ��Ϣ   2.����˽����Ϣ 3.��ȡ��ǰ�����û� 4.��ȡ�����û�     |\n");
	printf("| 5.�����û���   6.���õ�½����                                       |\n");
	printf("| 7.ǿ���û��뿪(ADMIN)  8.�����û�����(SUPER ADMIN)  9.�˳���½      |\n");
	printf("|---------------------------------------------------------------------|\n");
	// ����������Ϣ���߳�
	hThread = (HANDLE)_beginthreadex(NULL, 0, &RecvMsgThread, (void*)&g_connectSocket, 0, NULL);
	CloseHandle(hThread);
	GetCurrentUserData();
	while (g_isLogin)
	{
		printf("������ָ�\n");
		scanf("%d", &option);
		while (getchar() != '\n')
		{
			continue;
		}

		if (!g_isLogin)
		{
			break;
		}
		switch (option)
		{
		case 1:
			printf("��������Ҫ���͵���Ϣ��\n");
			gets(msgBuf);
			nRet = SendGroupMassage(msgBuf);
			break;
		case 2:
			printf("��������Ҫ���͵��û�����\n");
			gets(userName);
			printf("��������Ҫ���͵���Ϣ��\n");
			gets(msgBuf);
			nRet = SendPrivateMassage(userName, msgBuf);
			break;
		case 3:
			nRet = GetOnlineUserList();
			break;
		case 4:
			nRet = GetAllUserList();
			break;
		case 5:
			printf("�������µ��û�����\n");
			gets(userName);
			nRet = SetUserName(userName);
			break;
		case 6:
			printf("�������µ��û����룺\n");
			gets(userPwd);
			nRet = SetUserPwd(userPwd);
			break;
		case 7:
			if (g_myUser.userLevel >= LEVEL_AMMIN)
			{
				printf("��������Ҫǿ���뿪���û�����\n");
				gets(userName);
				nRet = SetUserOffLine(userName);
			}
			else
			{
				printf("�����û�Ȩ�޲�����\n");
			}
			break;
		case 8:
			if (g_myUser.userLevel == LEVEL_SUPER_ADMIN)
			{
				printf("��������Ҫ���õ��û�����\n");
				gets(userName);
				printf("��������Ҫ�����Ȩ�ޣ�1.��ͨ�û�  2.����Ա����\n");
				scanf("%d", &option);
				switch (option)
				{
				case 1:
					nRet = SetUserLevel(userName, LEVEL_USER);
					break;
				case 2:
					nRet = SetUserLevel(userName, LEVEL_AMMIN);
					break;
				default:
					printf("�������ָ���ȷ\n");
					break;
				}

			}
			else
			{
				printf("�����û�Ȩ�޲�����\n");
			}
			break;
		case 9:
			Logout();
			g_isLogin = false;
			break;
		default:
			printf("��������ȷ��ָ�\n");
			break;
		}
	}
}

int main(int argc, char*argv[])
{
	int iResult;
	WSADATA wsaData;
	char userName[USER_NAME_LEN];
	char userPwd[USER_PWD_LEN];
	char msgBuf[MASSAGE_BUFLEN];
	char ServerName[IP_LEN] = "127.0.0.1";
	int option;
	bool isExit = false;
	//printf("�����������ip: ");
	//gets(ServerName);

	// ��ʼ��winsock 
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}

	iResult = CreateConnectSocket(ServerName, &g_connectSocket);
	if (iResult != SUCCESS)
	{
		printf("��������ַ����ȷ��\n");
	}
	else
	{
		while (!isExit)
		{
			printf("ע�������룺1\t��½������: 2\t�˳�����������: 3\n");
			scanf("%d", &option);
			while (getchar() != '\n')
			{
				continue;
			}

			switch (option)
			{
			case 1:
				printf("�������û���(���Ȳ�����20�����ܰ����������)��\n");
				gets(userName);
				printf("�������û����ܣ�����ĸ��������ɣ���\n");
				gets(userPwd);

				if (!CheckNameAndPwd(userName, userPwd))
				{
					printf("�������ע����Ϣ���Ϸ���\n");
					break;
				}
				iResult = Register(userName, userPwd);
				if (iResult == SUCCESS)
				{
					printf("ע��ɹ���\n");
				}
				else
				{
					printf("ע��ʧ�ܣ����������û����Ѿ�����!\n");
				}
				break;
			case 2:
				printf("�������û���(���Ȳ�����20�����ܰ����������)��\n");
				gets(userName);
				printf("�������û����ܣ�����ĸ��������ɣ���\n");
				gets(userPwd);
				if (!CheckNameAndPwd(userName, userPwd))
				{
					printf("������ĵ�½��Ϣ���Ϸ���\n");
					break;
				}
				iResult = Login(userName, userPwd);
				if (iResult != SUCCESS)
				{
					printf("��½ʧ��");
					break;
				}

				printf("��½�ɹ���\n");
				strcpy_s(g_myUser.userName, USER_NAME_LEN, userName);
				strcpy_s(g_myUser.userPwd, USER_PWD_LEN, userPwd);
				g_isLogin = true;
				Run();

				break;
			case 3:
				printf("�����˳���\n");
				isExit = true;
				break;
			default:
				printf("��������ȷ������!\n");
				break;
			}
		}
	}

	closesocket(g_connectSocket);
	WSACleanup();
	getchar();
	return 0;
}