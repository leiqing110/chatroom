#include "Config.h"

// �û�ģ��
bool IsNameRepeat(char *pUserName);

int Register(SOCKET connSocket);

int Login(SOCKET connSocket, PUserOnlineNode pCurrUserNode, bool *isLogin);

int Logout(SOCKET *connSocket, PUserOnlineNode pUserOnlienNode, bool *isLogin);

int SetUserName(PUserOnlineNode pUserOnlienNode);

int SetUserPwd(PUserOnlineNode pUserOnlienNode);
