#pragma once
#include "resource.h"
#include <afxsock.h>
#include <tchar.h>
#include <string>
#include <cstring>
#include<iostream>
#include<conio.h>
#include<time.h>
#include<stdlib.h>
#include<fstream>
#include<vector>
#include <direct.h>
using namespace std;
char * getResponse(CSocket &socket);
bool response(CSocket &Socket);
bool request(CSocket &socket, string m_request);
string getPassWord();
bool connect(CSocket &socket, string ip, int port, bool &isConected);
string CStringToString(CString ip);
string getIP(CSocket &soc);
unsigned int getPort(CSocket &soc);
void randomPort(CSocket &socket, unsigned int &port, string &port_str);
char * getResponseString(CSocket &socket);
unsigned int getNListSize(CString noti);
bool dir(CSocket &socket, bool isPasv);
char* receiveData(CSocket &client21, string cmd, unsigned int &size, char*& resp, string filename);
bool put(CSocket &socket, string fileName, bool isPasv);
bool get(CSocket &socket, string fileName, bool isPasv);
bool mput(CSocket &socket, vector<string> list, bool isPasv);
bool mget(CSocket &socket, vector<string> list, bool isPasv);
bool mdelete(CSocket &socket, vector<string> list);
bool Delete(CSocket &socket, string command);
bool ls(CSocket &client, bool isPasv);
bool getResponePort(CSocket &client, int &portsv, string &ipsv,int &pos226,int &pos550);
bool Quit(CSocket &socket, string command);
bool PWD(CSocket &socket, string command);
bool CD(CSocket &socket);
bool LCD(CSocket &socket, string path);
bool Mkdir(CSocket &socket, string command);
bool Rmdir(CSocket &socket, string command);
void help();
bool pasv(CSocket &socket);
string standardizedStr(string input);
vector<string> splitInputArgument(string input);
int findPosition(vector<string>vector, string cmd);
void ftpClient();

