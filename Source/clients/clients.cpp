// clients.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "clients.h"
#include <afxsock.h>
#include <string>
#include <sstream>
#include <vector>
using namespace std;
CSocket Client;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;
bool KTTen(char s[])
{
	if (strlen(s) == 0)
		return false;
	for (int i = 0; i < strlen(s); i++)
	{
		if (!((s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z') || (s[i] >= '0' && s[i] <= '9')))
			return false;
	}
	return true;
}
string KQ(char mess[])
{ //ten/ ten/kytudoan/ baohieu/diem/ketthucgame/
	stringstream ss;
	string tam;
	ss.str(mess);
	vector<string>kq;
	while (getline(ss, tam, '/'))
		kq.push_back(tam);
	cout <<" \n- " << kq[0] << " doan: " << kq[1];
	if (kq[2] == "0") // đoán sai
		cout << " .Doan SAI. So diem hien tai la: " << kq[3] << " .Bi mat luot choi. \n";
	else
	if (kq[2] == "1")
		cout << " .Doan SAI. So diem hien tai la: " << kq[3] <<endl;
	else
		cout << " .Doan DUNG. So diem hien tai la: " << kq[3] << endl;
	return kq[4];
	kq.clear();
}
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: code your application's behavior here.
			AfxSocketInit(NULL);
			Client.Create();
			int length;//chiều dìa chuỗi nhận, gửi
			char tam[255];
			memset(tam, 0, 255);
			srand(time(NULL));
			int id,stt; //id người chơi
			if (Client.Connect(_T("127.0.0.1"), 1006))
			{
				cout << "\t\t   <<==== GAME CHIEC NON KI DIEU ====>>\n\n";
				// nhập đến khi tên hợp lệ
				do
				{
					cout << " ==>> Nhap ten: ";
					gets(tam);
					while (KTTen(tam) == false) //kt tên có hợp lệ hay không
					{
						cout << " <> Ten khong hop le. Moi nhap lai: ";
						gets(tam);
					}
					Client.Send(tam, strlen(tam), 0);
					length = Client.Receive(tam,5);
					tam[length] = 0;
					if (strcmp(tam, "0") != 0) //tên thỏa mãn
						break;
					else
						cout << " <> Ten nguoi choi da ton tai. Moi Nhap lai!\n";
				} while (true);
				stt = atoi(tam);
				cout << " ==>> Ket noi thanh cong\n";
				Client.Receive((char*)&length,sizeof(int),0);
				tam[length] = 0;
				Client.Receive(tam, length, 0);
				cout << "\n\t\t\t ===>> THONG TIN: <<===\n\n";
				cout << "\t\t- Goi y: " << tam <<endl;
				bool isWin = false;
				Client.Receive((char*)&length, sizeof(int), 0);
				tam[length] = 0;
				Client.Receive(tam, length, 0);
				cout << "\t\t- O chu: " << tam << endl;
				cout << "\n\t\t\t ===>> GAME START <<===\n";
				int point;
				while (!isWin)
				{
					// nhận id
					Client.Receive((char*)&id, sizeof(int), 0);//nhận stt xem tới lượt ai
					cout << "\n - Luot choi thuoc ve nguoi choi thu " << id << endl;
					// kiểm tra tới lượt của mình không
					if (stt == id)
					{
						point = rand() % 10 + 1;
						cout << " - Ban quay duoc so diem la: " << point << endl;
						itoa(point, tam, 10); // gửi điểm về server
						Client.Send(tam, strlen(tam) + 1, 0);
						cout << " - Ban doan chu (hoac o chu can tim): ";
						gets(tam);
						length = strlen(tam);
						Client.Send(&length, sizeof(length), 0);
						Client.Send(tam, length, 0);
					}
					//nhận thông báo từ server
					Client.Receive((char*)&length, sizeof(int), 0);
					tam[length] = 0;
					Client.Receive(tam, length, 0);
					isWin = atoi(KQ(tam).c_str());
					//nhận cập nhật ô chữ
					Client.Receive((char*)&length, sizeof(int), 0);
					tam[length] = 0;
					Client.Receive(tam, length, 0);
					cout << " - O chu: " << tam << endl;
					cout << "------------------------------------------\n";
				}
				system("cls");
				cout << "\t\t ===>> KET THUC GAME <<===\n";
				Client.Receive((char*)&id, sizeof(int), 0); 
				//int win = atoi(tam);
					cout << "\t\t <>Nguoi choi thu: " << id << " THANG CUOC.\n";
					system("pause");
					Client.Close();
			}
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	return nRetCode;
}
