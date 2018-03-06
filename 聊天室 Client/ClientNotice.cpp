#include "ClientNotice.h"
#include "stdio.h"

int GetOnlineUserList()
{
	int nRet = SUCCESS;
	DataHead head;
	head.sign = DEFAULT_SIGN;
	head.cmd = CMD_GET_ONLINE_USER;
	head.response = RES_NULL;
	head.dataLen = 0;

	nRet = SendHead(g_connectSocket, &head);
	return nRet;
}

void PrintOnlineUser(char *buf)
{
	PUserData pUserData = (PUserData)buf;
	printf("%s ���� Ȩ�޼��� ", pUserData->userName);
	switch (pUserData->userLevel)
	{
	case LEVEL_SUPER_ADMIN:
		printf("��������Ա");
		break;
	case LEVEL_USER:
		printf("�û�");
		break;
	case LEVEL_AMMIN:
		printf("����Ա");
		break;
	default:
			break;
	}
	printf("\n");
}

int GetAllUserList()
{
	int nRet = SUCCESS;
	DataHead head;
	head.sign = DEFAULT_SIGN;
	head.cmd = CMD_GET_ALL_USER;
	head.response = RES_NULL;
	head.dataLen = 0;

	SendHead(g_connectSocket, &head);
	return nRet;
}

int GetCurrentUserData()
{
	int nRet = SUCCESS;
	DataHead head;
	head.sign = DEFAULT_SIGN;
	head.cmd = CMD_GET_CURRENT_USER;
	head.response = RES_NULL;
	head.dataLen = 0;
	
	nRet = SendHead(g_connectSocket, &head);
	return nRet;
}

void SetCurrentUserData(char *buf)
{
	PUserData pUserData = (PUserData)buf;
	g_myUser.userLevel = pUserData->userLevel;
	strcpy_s(g_myUser.userName, USER_NAME_LEN,pUserData->userName);
	strcpy_s(g_myUser.userPwd, USER_PWD_LEN, pUserData->userPwd);
}

void PrintUser(char *buf)
{
	PUserData pUserData = (PUserData)buf;
	printf("%s  Ȩ�޼��� ", pUserData->userName);
	switch (pUserData->userLevel)
	{
	case LEVEL_SUPER_ADMIN:
		printf("��������Ա");
		break;
	case LEVEL_USER:
		printf("�û�");
		break;
	case LEVEL_AMMIN:
		printf("����Ա");
	default:
		break;
	}
	printf("\n");
}

int RecvUserData()
{
	int nRet = SUCCESS;
	return nRet;
}
