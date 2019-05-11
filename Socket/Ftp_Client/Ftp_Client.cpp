// Ftp_Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Ftp_Client.h"
/*
Hàm để kết nối Client tới server
Tham số đầu vào: CSocket, địa chỉ IP, port, biến isConnected để lưu đã kết nối chưa
Đầu ra kết nối thành công: true/ thất bại: false
*/
bool connect(CSocket &socket, string ip, int port, bool &isConected)
{
	if (isConected == false)
	{
		CString ipServer(ip.c_str());
		if (!socket.Connect(ipServer, port))
		{
			if (!response(socket))
			{
				socket.ShutDown(1);
				isConected = false;
				return false;
			}
			socket.ShutDown(1);
			isConected = false;
			return false;
		}
		char *m_response;
		if (!response(socket))
		{
			socket.ShutDown(1);
			isConected = false;
			return false;
		}
		if (!request(socket, "OPTS UTF8 ON\r\n"))
		{
			isConected = false;
			return false;
		}
		if (!response(socket))
		{
			isConected = false;
			return false;
		}
		cout << "User (" + ip + ":(none)): ";
		string user;
		getline(cin, user);
		user = "USER " + user + "\r\n";
		if (!request(socket, user))
		{
			isConected = false;
			return false;
		}
		m_response = getResponse(socket);
		if (!m_response)
		{
			delete[]m_response;
			isConected = false;
			return false;
		}
		if (strcmp(m_response, "331") == 0)//331 Password required for user\r\n
		{
			cout << "Password: ";
			string pass = getPassWord();
			cout << endl;
			pass = "PASS " + pass + "\r\n";
			if (!request(socket, pass))
			{
				isConected = false;
				delete[]m_response;
				return false;
			}
			m_response = getResponse(socket);
			if (strcmp(m_response, "230") != 0)
			{
				delete[] m_response;
				isConected = false;
				cout << "Login fail\n";
				return false;
			}
			delete[] m_response;
		}
		else
		{
			delete[] m_response;
			cout << "Login fail\n";
			isConected = false;
			return false;
		}
		isConected = true;
		return true;
	}
	else {
		return false;
	}
}

/*
Chức năng: Liệt kê các tệp tin
đầu vào CSocket Client, biến nhận biết ở chế độ Passive hay Active
Hàm trả về kết quả thực hiện lệnh thành công: true/thất bại: false
*/
bool ls(CSocket &client,bool isPasv)
{
	Sleep(1000);
	int pos = -1, check = -1;
	unsigned int port;
	string port_str;
	string ip;
	int portsv = 0;
	string ipsv;
	randomPort(client, port, ip);
	if (!isPasv)
	{
		if (!request(client, ip.c_str()))
			return false;
		string resp = getResponseString(client);
		if (!request(client, "NLST\r\n"))
			return false;
		pos = resp.find("226");
		if (strcmp(getResponse(client),"150")==0)
		{
		CSocket connector;
		CSocket clientData;
		clientData.Create(port);
		clientData.Listen(1);
		if (clientData.Accept(connector))
		{
			int len;
			char*buffer = new char[BUFSIZ];
			do
			{
				memset(buffer, 0, BUFSIZ);
				len = connector.Receive(buffer, BUFSIZ, 0);
				cout << buffer;
			} while (len > 0);
			delete[]buffer;
			if (pos == -1)
			{
				response(client);
			}
			connector.ShutDown(0);
			return true;
		}
		else
			return false;
	}
	}
	else
	{
		int pos226;
		if (!request(client, "PASV\r\n"))
			return false;
		getResponePort(client, portsv, ipsv, pos226,check);
		CSocket socConnect;
		if (!request(client, "NLST\r\n"))
			return false;
		if (strcmp(getResponse(client), "150") == 0)
		{
			if (!socConnect.Create(port, 1))
			{
				cout << "error";
				return false;
			}
			CString ipsv_(ipsv.c_str());
			if (!socConnect.Connect(ipsv_, portsv))
			{
				cout << "Connect sever fail!!!" << endl;
				return false;
			}
			int len;
			char*buffer = new char[BUFSIZ];
			do
			{
				memset(buffer, 0, BUFSIZ);
				len = socConnect.Receive(buffer, BUFSIZ, 0);
				cout << buffer;
			} while (len > 0);
			delete[]buffer;
			if (pos226 == -1)
				response(client);
			socConnect.ShutDown(0);
			return true;
		}
	}
}
/*
Chức năng: Liệt kê các thư mục và tệp tin
đầu vào CSocket Client, biến nhận biết ở chế độ Passive hay Active
Hàm trả về kết quả thực hiện lệnh thành công: true/thất bại: false
*/

bool dir(CSocket &client,bool isPasv)
{
	Sleep(1000);
	unsigned int port;
	string port_str;
	int pos;
	int check;
	string ip;
	int portsv = 0;
	string ipsv;
	randomPort(client, port, ip);
	if (!isPasv)
	{
		request(client, ip.c_str());
		string resp = getResponseString(client);
		if (!request(client, "LIST\r\n"))
			return false;
		pos = resp.find("226");
		if (strcmp(getResponse(client), "150") == 0)
		{
			CSocket connector;
			CSocket clientData;
			clientData.Create(port);
			clientData.Listen(1);
			if (clientData.Accept(connector))
			{
				int len;
				char*buffer = new char[BUFSIZ];
				do
				{
					memset(buffer, 0, BUFSIZ);
					len = connector.Receive(buffer, BUFSIZ, 0);
					cout << buffer;
				} while (len > 0);
				delete[]buffer;
				connector.ShutDown(0);
				if (pos == -1)
				{
					response(client);
				}
				return true;
			}
			else
				return false;
		}
	}
	else
	{
		if (!request(client, "PASV\r\n"))
			return false;
		getResponePort(client, portsv, ipsv, pos,check);
		if (!request(client, "LIST\r\n"))
			return false;
		if (strcmp(getResponse(client), "150") == 0)
		{
			CSocket socConnect;
			if (!socConnect.Create(port, 1))
			{
				cout << "error";
				return false;
			}
			CString ipsv_(ipsv.c_str());
			if (!socConnect.Connect(ipsv_, portsv))
			{
				cout << "Connect sever fail!!!" << endl;
				return false;
			}
			int len;
			char*buffer = new char[BUFSIZ];
			do
			{
				memset(buffer, 0, BUFSIZ);
				len = socConnect.Receive(buffer, BUFSIZ, 0);
				cout << buffer;
			} while (len > 0);
			delete[]buffer;
			if (pos == -1)
			{
				response(client);
			}
			socConnect.ShutDown(0);
			return true;
		}
	}
}
/*
Chức năng: Upload 1 file từ Client lên Server
Tham số đầu vào: CSocket client, tên file: string, biến kiểm tra chế độ Pasv hay Active
Trả về kết quả của lệnh Upload thành công hay thất bại
*/
bool put(CSocket &socket, string fileName, bool isPasv)
{
	bool flag = true;
	int pos = -1, check = -1;
	ifstream is;
	is.open(fileName.c_str(), ios::binary);
	if (is.is_open())
	{
		if (!isPasv)
		{
			CSocket clientData;
			srand(time(0));
			int port = rand() % (60000 - 50000) + 50000;
			if (clientData.Create(port) == 0)
			{
				cout << "Failed...\n";
				cout << clientData.GetLastError();
				flag = false;
			}
			if (clientData.Listen(1) == 0)
			{
				cout << "Can't Listen, port: " << port << endl;
				clientData.Close();
				flag = false;
			}

			string r = "PORT " + getIP(socket) + "," + to_string(port / 256) + "," + to_string(port % 256) + "\r\n";
			if (!request(socket, r.c_str()))
				flag = false;
			string resp = getResponseString(socket);
			cout << resp;
			string req = "STOR " + fileName + "\r\n";
			if (!request(socket, req.c_str()))
				flag = false;
			pos = resp.find("226");
			check = resp.find("550");
			if (strcmp(getResponse(socket), "150") == 0)
			{
				CSocket curent;
				if (!clientData.Listen(1))
				{
					cout << "Cannot listen on this port!" << endl;
					flag = false;
				}
				if (!clientData.Accept(curent) && flag) {
					cout << "Cannot accept this connector" << endl;
					flag = false;
				}
				else {
					if (is.is_open())
					{
						int len;
						char*buffer = new char[BUFSIZ];
						while (!is.eof())
						{
							memset(buffer, 0, BUFSIZ);
							is.read(buffer, BUFSIZ);
							len = is.gcount();
							curent.Send(buffer, len);
						}
						is.close();
						delete[]buffer;
					}
					else {
						cout << "can't open file\n";
						flag = false;
						check = 1;
					}
					curent.ShutDown(1);
					clientData.ShutDown(1);
					if (pos == -1 && check == -1)
					{
						Sleep(1000);
						socket.Listen(1);
						response(socket);
					}
				}
			}
		}
		else
		{
			unsigned int port;
			string port_str;
			string ip;
			int portsv = 0;
			string ipsv;
			randomPort(socket, port, ip);
			if (!request(socket, "PASV\r\n"))
				flag = false;
			getResponePort(socket, portsv, ipsv, pos, check);
			CSocket socConnect;
			string resp = "STOR " + fileName + "\r\n";
			if (!request(socket, resp.c_str()))
				flag = false;
			if (strcmp(getResponse(socket), "150") == 0)
			{
				if (!socConnect.Create(port, 1))
				{
					cout << "error";
					flag = false;
				}
				CString ipsv_(ipsv.c_str());
				if (!socConnect.Connect(ipsv_, portsv) && flag)
				{
					cout << "Connect sever fail!!!" << endl;
					flag = false;
				}
				if (is.is_open() && flag)
				{
					int len;
					char*buffer = new char[BUFSIZ];
					while (!is.eof())
					{
						memset(buffer, 0, BUFSIZ);
						is.read(buffer, BUFSIZ);
						len = is.gcount();
						socConnect.Send(buffer, len);
					}
					is.close();
					delete[]buffer;

				}
				else
				{
					cout << "can't open file.";
					check = 1;
					flag = false;
				}
				socConnect.ShutDown(1);
				if (pos == -1 && check == -1)
				{
					Sleep(1000);
					socket.Listen(1);
					response(socket);
				}
			}
		}
		return false;
	}
	else
	{
		cout << "can't open file!" << endl;
		return false;
	}
}
/*
Chức năng: Download 1 file từ Server về  Client
Tham số đầu vào: CSocket client, tên file: string, biến kiểm tra chế độ Pasv hay Active
Trả về kết quả của lệnh Download thành công hay thất bại
*/
/*bool get(CSocket &socket, string fileName,bool isPasv)
{
	int pos = -1, check = -1;
	Sleep(1000);
	ofstream os;
	os.open(fileName.c_str(), ios::binary);
	if (os.is_open())
	{
		if (!isPasv)
		{
			CSocket clientData;
			srand(time(0));
			int port = rand() % (60000 - 50000) + 50000;
			if (clientData.Create(port) == 0)
			{
				cout << "Failed...\n";
				cout << clientData.GetLastError();
				return false;
			}
			if (clientData.Listen(1) == 0)
			{
				cout << "Can't Listen, port: " << port << endl;
				clientData.Close();
				return false;
			}

			string r = "PORT " + getIP(socket) + "," + to_string(port / 256) + "," + to_string(port % 256) + "\r\n";
			if (!request(socket, r.c_str()))
				return false;
			string resp = getResponseString(socket);
			cout << resp;
			pos = resp.find("126");
			check = resp.find("550");
			string req = "RETR " + fileName + "\r\n";
			if (!request(socket, req.c_str()))
				return false;
			if (strcmp(getResponse(socket), "150") == 0)
			{

				if (os.is_open())
				{
					CSocket curent;
					if (!clientData.Listen(1))
					{
						cout << "Cannot listen on this port!" << endl;
						return false;
					}
					if (!clientData.Accept(curent)) {
						cout << "Cannot accept this connector" << endl;
						return false;
					}

					else {
						int len;
						char*buffer = new char[BUFSIZ];

						do {
							memset(buffer, 0, BUFSIZ);
							//os.write(buffer, len);
							len = curent.Receive(buffer, BUFSIZ, 0);
							os.write(buffer, len);
						} while (len > 0);
						os.close();
						delete[]buffer;
						curent.ShutDown(1);
						clientData.ShutDown(1);
						if (pos == -1 && check == -1)
						{
							Sleep(1000);
							socket.Listen(1);
							response(socket);
						}

					}
					return true;
				}
	
			}
		}
		else
		{
			unsigned int port;
			string port_str;
			string ip;
			int portsv = 0;
			string ipsv;
			randomPort(socket, port, ip);
			if (!request(socket, "PASV\r\n"))
				return false;
			getResponePort(socket, portsv, ipsv, pos, check);
			CSocket socConnect;
			string resp = "RETR " + fileName + "\r\n";
			if (!request(socket, resp.c_str()))
				return false;
			if (strcmp(getResponse(socket), "150") == 0)
			{
				if (!socConnect.Create(port, 1))
				{
					cout << "error";
					return false;
				}
				CString ipsv_(ipsv.c_str());
				if (!socConnect.Connect(ipsv_, portsv))
				{
					cout << "Connect sever fail!!!" << endl;
					return false;
				}
				if (os.is_open())
				{
					int len;
					char*buffer = new char[BUFSIZ];

					do {
						memset(buffer, 0, BUFSIZ);
						//os.write(buffer, len);
						len = socConnect.Receive(buffer, BUFSIZ, 0);
						os.write(buffer, len);
					} while (len > 0);
					os.close();
					delete[]buffer;
					socConnect.ShutDown(1);
					if (pos == -1 || check == -1)
					{
						Sleep(200);
						socket.Listen(1);
						response(socket);
					}

				}
				return true;
			}
		}
	}
	else
	{
		cout << "can't open file!" << endl;
		return false;
	}
}
*/
bool get(CSocket &socket, string fileName, bool isPasv)
{
	Sleep(100); bool flag = true;
	int pos = -1, check = -1;
	ofstream os;
	os.open(fileName.c_str(), ios::binary);
	if (os.is_open())
	{
		if (!isPasv)
		{
			CSocket clientData;
			srand(time(0));
			int port = rand() % (60000 - 50000) + 50000;
			if (clientData.Create(port) == 0)
			{
				cout << "Failed...\n";
				cout << clientData.GetLastError();
				flag = false;
			}
			if (clientData.Listen(1) == 0)
			{
				cout << "Can't Listen, port: " << port << endl;
				clientData.Close();
				flag = false;
			}

			string r = "PORT " + getIP(socket) + "," + to_string(port / 256) + "," + to_string(port % 256) + "\r\n";
			if (!request(socket, r.c_str()))
				flag = false;
			string resp = getResponseString(socket);
			cout << resp;
			string req = "RETR " + fileName + "\r\n";
			if (!request(socket, req.c_str()))
				flag = false;
			pos = resp.find("226");
			check = resp.find("550");
			if (strcmp(getResponse(socket), "150") == 0)
			{
				CSocket curent;
				if (!clientData.Listen(1))
				{
					cout << "Cannot listen on this port!" << endl;
					flag = false;
				}
				if (!clientData.Accept(curent) && flag) {
					cout << "Cannot accept this connector" << endl;
					flag = false;
				}
				else {
					if (os.is_open())
					{
						int len;
						char*buffer = new char[BUFSIZ];

						do {
							memset(buffer, 0, BUFSIZ);
							//os.write(buffer, len);
							len = curent.Receive(buffer, BUFSIZ, 0);
							os.write(buffer, len);
						} while (len > 0);
					}
					else {
						cout << "can't open file\n";
						flag = false;
						check = 1;
					}
					curent.ShutDown(1);
					clientData.ShutDown(1);
					if (pos == -1 && check == -1)
					{
						Sleep(1000);
						socket.Listen(1);
						response(socket);
					}
				}
			}
		}
		else
		{
			unsigned int port;
			string port_str;
			string ip;
			int portsv = 0;
			string ipsv;
			randomPort(socket, port, ip);
			if (!request(socket, "PASV\r\n"))
				flag = false;
			getResponePort(socket, portsv, ipsv, pos, check);
			CSocket socConnect;
			string resp = "RETR " + fileName + "\r\n";
			if (!request(socket, resp.c_str()))
				flag = false;
			if (strcmp(getResponse(socket), "150") == 0)
			{
				if (!socConnect.Create(port, 1))
				{
					cout << "error";
					flag = false;
				}
				CString ipsv_(ipsv.c_str());
				if (!socConnect.Connect(ipsv_, portsv) && flag)
				{
					cout << "Connect sever fail!!!" << endl;
					flag = false;
				}
				if (os.is_open() && flag)
				{
					int len;
					char*buffer = new char[BUFSIZ];

					do {
						memset(buffer, 0, BUFSIZ);
						//os.write(buffer, len);
						len = socConnect.Receive(buffer, BUFSIZ, 0);
						os.write(buffer, len);
					} while (len > 0);
					os.close();
					delete[]buffer;

				}
				else
				{
					cout << "can't open file.";
					check = 1;
					flag = false;
				}
				socConnect.ShutDown(1);
				if (pos == -1 && check == -1)
				{
					Sleep(1000);
					socket.Listen(1);
					response(socket);
				}
			}
		}
		return false;
	}
	else
	{
		return false;
	}
}
/*
Chức năng: Upload NHIỀU file từ Client lên Server
Tham số đầu vào: CSocket client, tên file: string, biến kiểm tra chế độ Pasv hay Active
Trả về kết quả của lệnh Upload thành công hay thất bại
*/
bool mput(CSocket &socket, vector<string> list,bool isPasv)
{
	/*vector<string> list;
	list = List(fileName);*/
	int size = list.size();
	for (int i = 0; i < size; i++)
	{
		cout << "mput " << list[i] << " ?" << endl;
		cout << "Press Y - Yes" << endl;
		cout << "Press another  key - No" << endl;
		char ch;
		fflush(stdin);
		ch = getch();
		if (kbhit())
			_getch();
		fflush(stdin);
		if (ch == 'y' || ch == 'Y')
		{

			Sleep(1000);
			if (isPasv)
				put(socket, list[i], true);
			else
				put(socket, list[i], false);

		}
	}
	return true;
}
/*
Chức năng: Download NHIỀU file từ Server về  Client
Tham số đầu vào: CSocket client, tên file: string, biến kiểm tra chế độ Pasv hay Active
Trả về kết quả của lệnh Download thành công hay thất bại
*/
bool mget(CSocket &socket, vector<string> list,bool isPasv)
{
	//vector<string> list;
	//list = List(fileName);
	int size = list.size();
	for (int i = 0; i < size; i++)
	{
		cout << "mget " << list[i] << " ?" << endl;
		cout << "Press Y - Yes" << endl;
		cout << "Press orther key - No" << endl;
		fflush(stdin);
		char ch = getch();
		if (kbhit())
			_getch();
		fflush(stdin);
		if (ch == 'Y' || ch == 'y')
		{
			Sleep(1000);
			if (!isPasv)
				get(socket, list[i], false);
			else
				get(socket, list[i], true);
		}
	}
	return true;
}

/*
Chức năng: Xóa nhiều file
Đầu vào: CSocket client, danh sách các file cần xóa dạng vector
Trả về: kết quả phép xóa true/false
*/
bool mdelete(CSocket &socket, vector<string> list)
{
	/*request(socket, "TYPE A");
	response(socket);*/
	/*vector<string>list = List(deleteFile);*/
	int len = list.size();
	for (int i = 0; i < len; i++)
	{
		unsigned int size;
		char * resp;
		char* data = receiveData(socket, "NLST", size, resp, list[i]);
		if (data)
			Delete(socket, list[i]);
	}
	return true;
}
bool Quit(CSocket &socket, string command)
{
	string message = command + "\r\n";
	request(socket, message);
	char *m_response = getResponse(socket);
	return true;
}
bool PWD(CSocket &socket, string command)
{
	string message = "X" + command + "\r\n";
	request(socket, message);
	char *m_response = getResponse(socket);
	return true;
}
bool CD(CSocket &socket)
{
	string remote;
	cout << "Remote directory "; getline(cin, remote);
	string message = "CWD " + remote + "\r\n";
	request(socket, message);
	char *m_response = getResponse(socket);
	return true;
}
bool LCD(CSocket &socket, string path)
{
	_chdir(path.c_str());
	cout << "The local directory ";
	system("cd");
	return true;
}
bool Mkdir(CSocket &socket, string command)
{
	string message = "XMKD " + command + "\r\n";
	request(socket, message);
	char *m_response = getResponse(socket);
	return true;
}
bool Rmdir(CSocket &socket, string command)
{
	string message = "XRMD " + command + "\r\n";
	request(socket, message);
	char *m_response = getResponse(socket);
	return true;
}
bool Delete(CSocket &socket, string command)
{
	string message = "DELE " + command + "\r\n";
	request(socket, message);
	char *m_response = getResponse(socket);
	return true;
}
/*
Hàm in ra các lệnh chương trình hỗ trợ
*/
void help()
{
	cout << "open\tls\tdir\tput\tget\n";
	cout << "mput\tmget\tcd\tlcd\tdelete\n";
	cout << "mdelete\tmkdir\trmdir\tpwd\tpasv\n";
	cout << "quit\thelp\n";
}
/*Chuyển sang Passive mode*/
bool pasv(CSocket &socket)
{
	if (!request(socket, "PASV\r\n"))
		return false;
	if (!response(socket))
		return false;
	return true;
}
