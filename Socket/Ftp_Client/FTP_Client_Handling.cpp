#include"stdafx.h"
#include"Ftp_Client.h"

/*
chuẩn hóa chuỗi đầu vào
*/
string standardizedStr(string input)
{
	while (input[0] == ' ')
		input.erase(0, 1);
	while (input[input.size() - 1] == ' ')
		input.erase(input.size() - 1, 1);
	int pos = 0;
	for (int i = 0; i < input.length(); i++)
	{
		pos = i;
		if (input[i] == ' ')break;
		if (isalpha(input[i]) && isupper(input[i]))
			input[i] = tolower(input[i]);
	}
	for (int i = pos; i < input.length(); i++)
	{
		if (i < input.length() - 1)
		{
			if (input[i] == ' '&&input[i + 1] == ' ')
			{
				input.erase(i, 1);
				i--;
			}
		}
	}
	return input;
}
/*chuyển các tham số đầu vào vector*/
vector<string> splitInputArgument(string input)
{
	int begin = -1;
	int end = -1;
	string res = "";
	string a;
	vector<string> inputArgument;
	if (input.find("\"") != -1)// có xuất hiện dấu '   "   '
	{
		for (int i = 0; i < input.length(); i++)
		{
			if (input[i] == '\"' && begin == -1)
			{
				begin = i;
				continue;
			}
			if (input[i] == '\"' && begin != -1)
			{
				end = i + 1;
			}

			if (begin != -1 && end != -1)
			{
				inputArgument.push_back(input.substr(begin, end - begin));
				input.erase(begin - 1, end - begin + 1);
				begin = -1;
				end = -1;
				i = begin;
			}
		}
	}
	begin = -1;
	end = 0;
	if (input.find(" ") != -1)
	{
		a =input.substr(input.find_first_of(' ') + 1);
		for (int i = 0; i < a.length(); i++)
		{
			if (a[i] == ' ')
				begin = i;
			if (i == a.length() - 1)
				begin = i + 1;
			if (begin != -1)
			{
				res = a.substr(end, begin - end);
				inputArgument.push_back(res);
				end = begin + 1;
				begin = -1;
			}
		}
	}
	string temp;
	int pos = 0;
	while (input[pos] != '\0'&&input[pos] != ' ')
	{
		temp += input[pos];
		pos++;
	}
	inputArgument.insert(inputArgument.begin(), temp);
	return inputArgument;
}
int findPosition(vector<string>vector, string cmd)
{
	int len = vector.size();
	for (int i = 0; i < len; i++)
	{
		if (cmd.compare(vector[i])==0)
			return i;
	}
	return -1;//không thấy
}
/*Chương trình*/
void ftpClient()
{
	vector<string>cmd{ "open", "ls", "dir",
	"put", "get", "mput", "mget",
	"cd", "lcd",
	"delete", "mdelete",
	"mkdir", "rmdir", "pwd", "pasv",
	"quit", "help" };
	CSocket clientSocket;
	clientSocket.Create();
	bool flag = true;
	bool isConnected = false;
	bool isPasv = false;
	while (flag)
	{
		if (!isConnected)
			cout << "Ftp client is not connected!" << endl;
		int choose;
		string arg;
		cout << "ftp> ";
		getline(cin, arg);
		if (arg == "")
			continue;
		arg = standardizedStr(arg);
		vector<string>inputArg;
		inputArg = splitInputArgument(arg);
		if (inputArg.empty())
			continue;
		else
		{
			choose = findPosition(cmd, inputArg[0]);
			switch (choose)
			{
			case 0:
				if (!isConnected&&inputArg.size() == 2)
					if (!connect(clientSocket, inputArg[1], 21, isConnected))
						cout << "Connecting is failed!" << endl;
					else
						isConnected = true;
				break;
			case 1:
				ls(clientSocket, isPasv);
				break;
			case 2:
				dir(clientSocket, isPasv);
				break;
			case 3:
			{

				if (inputArg.size() < 2)
				{
					cout << "put filename.type" << endl;
					break;
				}
				put(clientSocket, inputArg[1], isPasv);
				break;
			}
			case 4:
			{
				if (inputArg.size() < 2)
				{
					cout << "get fileName.type" << endl;
					break;
				}
				get(clientSocket, inputArg[1], isPasv);
				break;
			}
			case 5:
			{
					if (inputArg.size() <2)
					{
						cout << "Command Fail!" << endl;
						break;
					}
					vector<string> list;
					if (inputArg.size() > 1)
						list.resize(inputArg.size() - 1);
					for (int i = 1; i < inputArg.size(); i++)
						list[i - 1] = inputArg[i];
					mput(clientSocket, list, isPasv);
					break;
				
			}
			case 6:
			{
				if (inputArg.size() < 2)
				{
					cout << "Command Fail!" << endl;
					break;
				}
				vector<string> list;
				if (inputArg.size() > 1)
					list.resize(inputArg.size() - 1);
				for (int i = 1; i < inputArg.size(); i++)
					list[i - 1] = inputArg[i];
				mget(clientSocket, list, isPasv);
				break;
			}
			case 7:
				if (inputArg.size() == 0)
				{
					cout << "Command Fail!" << endl;
					break;
				}
				CD(clientSocket);
				break;
			case 8:
				if (inputArg.size() <2)
				{
					cout << "Command Fail!" << endl;
					break;
				}
				LCD(clientSocket, inputArg[1]);
				break;
			case 9:
				if (inputArg.size() <2)
				{
					cout << "Command Fail!" << endl;
					break;
				}
				Delete(clientSocket,inputArg[1]);
				break;
			case 10:
			{
				if (inputArg.size() < 2)
				{
					cout << "Command Fail!" << endl;
					break;
				}
				vector<string> list;
				if (inputArg.size() > 1)
					list.resize(inputArg.size() - 1);
				for (int i = 1; i < inputArg.size(); i++)
					list[i - 1] = inputArg[i];
				mdelete(clientSocket, list);
				break;
			}
			case 11:
				if (inputArg.size() <2)
				{
					cout << "Command Fail!" << endl;
					break;
				}
				Mkdir(clientSocket, inputArg[1]);
				break;
			case 12:
				if (inputArg.size() < 2)
				{
					cout << "Command Fail!" << endl;
					break;
				}
				Rmdir(clientSocket, inputArg[1]);
				break;
			case 13:
				PWD(clientSocket, "PWD");
				break;
			case 14:
				cout << "Changing to passive mode completed!" << endl;
				isPasv = true;
				pasv(clientSocket);
				break;
			case 15:
				Quit(clientSocket, "QUIT");
				flag = false;
				break;
			case 16:
				help();
				break;
			default:
				cout << "Error! Please try again!" << endl;
				break;
		}
		}
	}
	clientSocket.Close();

}