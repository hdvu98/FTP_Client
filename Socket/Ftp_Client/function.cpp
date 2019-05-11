#include"stdafx.h"
#include"Ftp_Client.h"
/*
Hàm in ra màn hình tin đã Response
đồng thời trả về 3 số đầu
*/
char * getResponse(CSocket &socket)
{
	char *number = new char[4];
	if (socket.Receive(number, 3, 0) == -1)
		return NULL;
	number[3] = '\0';
	char* str = new char[BUFSIZ + 1];
	if (socket.Receive(str, BUFSIZ, 0) == -1)
		return NULL;
	for (int i = 0; i < BUFSIZ; i++)
		if (str[i] == '\r')
		{
			str[i] = '\0';
			break;
		}
	//int i = BUFSIZ;
	//while (str[i] != '\n')
	//	i--;
	//str[++i] = '\0';
	cout << number << str << endl;
	delete[] str;
	return number;
}
/*
Hàm kiểm tra response thành công hay thất bại, đồng thời in ra những gì response
*/
bool response(CSocket &Socket)
{
	char * m_response = getResponseString(Socket);
	if (!m_response)
	{
		delete[]m_response;
		return false;
	}
	else
	{
		cout << m_response;
		delete[]m_response;
		return true;
	}

}
/*
Thực hiện gửi yêu cầu
đầu vào CSocket Client, và chuỗi yêu cầu cần gửi
trả về kết quả true/false
*/
bool request(CSocket &socket, string m_request)
{
	int n = m_request.length();
	if (socket.Send(m_request.c_str(), m_request.length(), 0) == -1)
		return false;
	return true;
}
/*Hàm xử lý nhập mật khẩu
trả về chuỗi mật khẩu đã nhập
*/
string getPassWord()
{

	char temp=1;
	string pass;
	fflush(stdin);
	while (temp != '\r')
	{
		if (kbhit())
		{
			temp = getch();
			if(kbhit())
			_getch();
			fflush(stdin);
			if (temp == 8 && pass.size() > 0)
				pass.erase(pass.size() - 1);
			else
				pass.insert(pass.end(), temp);
		}
	} 
	return pass;
}
/*
Hàm chuyển chuỗi đầu vào kiểu CString trả về chuỗi tương tự dưới kiểu dữ liệu string
*/
string CStringToString(CString ip)
{
	CT2CA pszConvertedAnsiString(ip);
	return string(pszConvertedAnsiString);
}
/*
Lấy địa chỉ IP
*/
string getIP(CSocket &soc)
{
	unsigned int port;
	CString ip;
	soc.GetSockName(ip, port);
	ip.Replace(L'.', L',');
	return CStringToString(ip);
}
/*
Lấy port
*/
unsigned int getPort(CSocket &soc)
{
	unsigned int port;
	CString ip;
	soc.GetSockName(ip, port);
	return port;
}
/*
Xử lý random tạo port, đồng thời cho ra chuỗi resquest
*/
void randomPort(CSocket &socket, unsigned int &port,string &port_str)
{
	srand(time(NULL));
	port = rand() % (60000 -50000 + 1) + 50000;
	port_str = "PORT " + getIP(socket) + "," + to_string(port / 256) + "," + to_string(port % 256)+"\r\n";
}
/*
Lấy chuỗi nhận được
*/
char * getResponseString(CSocket &socket)
{
	char *noti = new char[BUFSIZ+1];
	if (socket.Receive(noti, BUFSIZ, 0) == -1)
		return NULL;
	int i = 0;
	while (noti[i] != '\n')
		i++;
	noti[++i] = '\0';
	return noti;
}
/*
Lấy size của data
*/
unsigned int getNListSize(CString noti)//ls
{
	string temp;
	temp = CStringToString(noti);
	int begin = temp.find('(', sizeof(char));
	int end = temp.find(')', sizeof(char));
	temp=temp.substr(begin + 1, end - begin-1);
	end = temp.find(' ', sizeof(char));
	temp = temp.substr(0, end);
	return atoi(temp.c_str());
}
/*
Lấy nội dung data nhận được
*/
char* receiveData(CSocket &client21, string cmd, unsigned int &size, char*& resp,string filename)
{
	CSocket clientData;
	srand(time(0));
	int port = rand() % (60000 - 50000) + 50000;
	if (clientData.Create(port) == 0)
	{
		cout << "Failed...\n";
		cout << clientData.GetLastError();
		return NULL;
	}
	if (clientData.Listen(1) == 0)
	{
		cout << "Can't Listen, port: " << port << endl;
		clientData.Close();
		return NULL;
	}
	string r = "PORT " + getIP(client21) + "," + to_string(port / 256) + "," + to_string(port % 256);
	r += "\r\n";
	request(client21, r.c_str());
	response(client21);
	// gửi lệnh port để thông báo cổng
	string c = string(cmd)+" " +filename + "\r\n";
	request(client21, c.c_str());
	CString noti;
	noti = getResponseString(client21);
	cout << CStringToString(noti);
	 size = getNListSize(noti);
	if (noti.GetLength()==0)
	{
		clientData.Close();
		return NULL;
	}

	char *data = NULL;

	// bắt đầu nhận gói tin
	if (size != 0)
	{
		Sleep(100);
		data = new char[size + 1];
		CSocket connector;
		if (clientData.Accept(connector))
		{
			// mỗi lần server gửi tối đa 1460 bytes
			// tính số lần server gửi: n = size/1460 + 1

			int n;
			if (size % 1460 == 0) {
				n = size / 1460;
			}
			else {
				n = size / 1460 + 1;
			}

			int ce = 1; // kiểm tra việc nhận dữ liệu có xảy ra lỗi hay không
			if (n == 1) // 1 gói tin
			{
				ce = connector.Receive(data, size);
			}
			else
			{
				int byteRece = 0, count = size;
				while (byteRece < size) {
					ce = connector.Receive(data + byteRece, count);
					Sleep(10);
					if (ce == SOCKET_ERROR)
						break;
					byteRece += ce;
					count -= ce;
				}
			}

			if (ce == 0)
			{
				delete[] data;
				throw 0; // socket disconnect
			}
			else if (ce == SOCKET_ERROR)
			{
				delete[] data;
				throw 1; // recv error
			}
		}

		connector.ShutDown(2);
	}
	//clientData.Close();
	if (size > 0)
		data[size] = 0;
	return data;
}
/*
Lấy thông tin port để tạo chế độ Pasv
*/
bool getResponePort(CSocket &client, int &portsv, string &ipsv,int &pos226,int &pos550)
{
	pos226 = -1;
	Sleep(200);
	string resp = getResponseString(client);
	cout << resp;
	pos226 = resp.find("226");
	pos550 = resp.find("550");
	if (resp.find("227") != -1)
	{
		string temp = resp;
	int begin = temp.find('(', sizeof(char));
	int end = temp.find(')', sizeof(char));
	temp = temp.substr(begin + 1, end - begin - 1);
	int count = 0;
	int pos = 0;
	while (count < 4)
	{
		ipsv += temp[pos];
		if (temp[pos] == ',')
			count++;
		pos++;
	}
	ipsv.erase(ipsv.length() - 1);
	pos = 0;
	while (ipsv[pos] != '\0')
	{
		if (ipsv[pos] == ',')
			ipsv[pos] = '.';
		pos++;
	}
	pos = temp.length() - 1;
	string a1, a2;
	while (temp[pos] != ',')
	{
		a1.insert(a1.begin(), temp[pos]);
		pos--;
	}
	pos--;
	while (temp[pos] != ',')
	{
		a2.insert(a2.begin(), temp[pos]);
		pos--;
	}
	ipsv += "\r\n";
	portsv = atoi(a1.c_str()) + atoi(a2.c_str()) * 256;
	return true;
	}
	else
	{
	return false;
	}
}