// server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "server.h"
#include <afxsock.h>
#include <string>
#include <vector>
using namespace std;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
struct Player
{
	char name[255];
	int point;
	bool turn; // mất lượt hay không
};
struct word
{
	char dapan[255];
	char hienthi[255];
	bool check[255]; // kiểm tra kí tự đã lật hay chưa;
};

// The one and only application object

CWinApp theApp;

using namespace std;
bool KTTen(char name[], vector<Player> listPlayer)
{
	for (int i = 0; i < listPlayer.size(); i++)
	{
		if (strcmp(name,listPlayer[i].name)==0)
			return false;
	}
	return true;
}
void SendMess(CSocket* listClient, int songuoichoi, char* mess)
{
	int length = strlen(mess);
	for (int i = 0; i < songuoichoi; i++)
	{
		listClient[i].Send(&length, sizeof(int), 0);
		mess[length] = 0;
		listClient[i].Send(mess, length, 0);
	}
}
int KtKytu(word& ochu, char doan[])
{
	int dem = 0;
	if (strlen(doan) == 1) //đoán 1 ký tự
	{
		for (int i = 0; i < strlen(ochu.dapan);i++)
			if (doan[0] == ochu.dapan[i] && ochu.check[i] == false)
			{
				ochu.check[i] = true; // đoán đúng thì mở ổ chữ lên
				dem++;
			}
	}
	else // đoán cả ô chữ
	{
		if (strcmp(ochu.dapan,doan)==0) //đoán đúng
		for (int i = 0; i < strlen(ochu.dapan);i++)
		if (ochu.check[i] == false)
		{
			dem++;
			ochu.check[i] = true;
		}
	}
	return dem;
}
bool KTGame(word ochu)
{
	for (int i = 0; i < strlen(ochu.dapan);i++)
	if (ochu.check[i] == false)
		return false;
	return true;
}
int tongDiem(int dem, int length, int diem)
{
	if (length == 1) // đoán 1 kí tự
		return diem * dem;
	return 20 * dem;
}
void Capnhat(word& ochu)
{
	for (int i = 0; i <strlen(ochu.dapan);i++)
	if (ochu.check[i] == true && ochu.hienthi[i] == '*')
		ochu.hienthi[i] = ochu.dapan[i];
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
			CSocket Server;
			CSocket* listClients;
			int songuoichoi;
			AfxSocketInit(NULL);
			Server.Create(1006);
			Server.Listen(10); // Chấp nhận 10 kết nối mạng chờ mà chưa được acept
			cout << "\t\t\t ---- MANG MAY TINH ----\n";
			cout << "\t\t<<===== DO AN 1 - LAP TRINH SOCKET =====>>\n";
			cout << "\t\t   <<==== GAME CHIEC NON KI DIEU ====>>\n\n";
			cout << " ===>> So luong nguoi choi: ";
			cin >> songuoichoi;
			cout << " <> Server: Khoi tao thanh cong! Cho ket noi tu cac nguoi choi!\n";
			listClients = new CSocket[songuoichoi];
			Player a;
			strcpy(a.name, ""); a.point = 0; a.turn = true; //khởi tạo 
			vector <Player> listPlayer(songuoichoi, a);
			char tam[255]; // biến tạm gửi nhận thông tin trong chương trình
			memset(tam, 0, 255);
			int length;//chiều dài chuỗi nhận gửi
			for (int i = 0; i < songuoichoi; i++)
			{
				Server.Accept(listClients[i]);
				// kiểm tra tên trùng nhau
				while (true)
				{
					length = listClients[i].Receive(tam,255);
					tam[length] = 0;
					if (KTTen(tam, listPlayer)) //kiểm tra tên có trùng nhau không
					{
						strcpy(listPlayer[i].name,tam);
						cout << " - "<< listPlayer[i].name << ": vua ket noi\n";
						itoa(i + 1, tam, 10);
						listClients[i].Send(tam, strlen(tam) + 1);
						break;
					}
					else
					{
						listClients[i].Send("0", 1); //tên trùng nhau;
						continue;
					}
				}
			}
			cout << "\n\t\t\t ===>> THONG TIN: <<===\n\n";
			word tukhoa;
			cout << "\t\t- Tu khoa: ";
			fflush(stdin);
			gets(tukhoa.dapan);
			cout << "\t\t- Goi y la: ";
			gets(tam);
			SendMess(listClients, songuoichoi, tam);
			int lengthword = strlen(tukhoa.dapan); //độ dài ô chữ cần đoán;
			//hiện thị ô chữ để người chơi dễ hình dung
			memset(tukhoa.hienthi, 0, 255);
			for (int i = 0; i < lengthword; i++)
			{
					tukhoa.hienthi[i] = '*';
				tukhoa.check[i] = false;
			}
			SendMess(listClients, songuoichoi, tukhoa.hienthi); // gửi chuỗi hiện thị cho client;
			cout << "\t\t- O chu: " << tukhoa.hienthi << endl;
			cout << "\n\t\t\t ===>> GAME START <<===\n\n";
			//Game start
			bool isWin = false; // kiể tra kết thúc game chưa
			char mess[255]; // chuỗi thông báo kết quả dạng: ten/kytudoan/ baohieu/diem/ketthucgame
			int dem,diem;
			for (int i = 0; i < songuoichoi; i++)
			{
				while (!isWin)
				{
					if (listPlayer[i].turn)
					{
						for (int j = 0; j < songuoichoi; j++)
						{
							int k = i + 1;
						listClients[j].Send((char*)&k, sizeof(int), 0); //gửi id đến lượt ai

						}
						// nhận điểm người chơi quay được
						length = listClients[i].Receive(tam, 10);
						tam[length] = 0;
						diem = atoi(tam);
						cout << " - " << listPlayer[i].name << " quay duoc so diem la: " << diem << endl;
						// nhận chuỗi người chơi đoán
						listClients[i].Receive((char*)&length, sizeof(int), 0);
						tam[length] = 0;
						listClients[i].Receive(tam, length, 0);
						cout << " - " << listPlayer[i].name << " doan: " << tam << endl;
						strcpy(mess, listPlayer[i].name); //coppy chuỗi tên người chơi
						strcat(mess, "/"); strcat(mess, tam); //coppy chuỗi người chơi đoán
						dem = KtKytu(tukhoa, tam);//kể tra chuỗi đoán. =0: sai
						if (dem == 0) //đoán sai
						{
							if (length > 1) //đoán cả ô chữ
							{
								cout << " - " << listPlayer[i].name << " doan ca o chu nhung sai! Bi mat luot choi!\n";
								listPlayer[i].turn = false;
								strcat(mess, "/0/0/"); // báo hiệu đoán cả ô nhưng sai
								listPlayer[i].point = 0;
							}
							else
							{
								strcat(mess, "/1/");//báo hệu đoán 1 kí tự nhưng sai
								listPlayer[i].point += tongDiem(dem, length, diem);
								cout << " - " << listPlayer[i].name << " doan sai! So diem la: " <<listPlayer[i].point <<endl;
								itoa(listPlayer[i].point, tam, 10);
								strcat(mess, tam); //báo điểm
								strcat(mess, "/");
							}
							i++;
						}
						else //đoán đúng
						{
							strcat(mess, "/2/");//báo hiệu đoán đúng
							listPlayer[i].point += tongDiem(dem, length, diem);
							itoa(listPlayer[i].point, tam, 10);
							cout << " - " << listPlayer[i].name << " doan dung! So diem la: " << listPlayer[i].point << endl;
							strcat(mess, tam); //báo điểm
							strcat(mess, "/");
							if (KTGame(tukhoa)) //kết thúc game;
								isWin = true;
						}
						// hết 1 lượt quay lại từ đầu
						if (i == songuoichoi)
							i = 0;
						itoa((int)isWin, tam, 10); // báo kết thúc game
						strcat(mess, tam);
						strcat(mess, "/");
						SendMess(listClients, songuoichoi, mess); //gửi thông báo cho các người chơi
						Capnhat(tukhoa); // cập nhật ô chữ hiển thị
						SendMess(listClients, songuoichoi, tukhoa.hienthi); // gửi ô chữ
					}
					else
					{
						if (isWin)
							break;
						else
						if (i == songuoichoi - 1)
							i = 0;
						else
							i++;
					}
				}
			}
			system("cls");
			int max; // tìm nguwoif chơi thắng;
			cout << "\t\t====>> KET THUC GAME <<====\n";
			max = 0;
				for (int i = 0; i < songuoichoi; i++)
				{
					cout <<"\t\t - "<< listPlayer[i].name << " : " << listPlayer[i].point << endl;
					if (listPlayer[i].point > listPlayer[max].point)
						max = i;
				}
				cout << "\t\t <> Nguoi thang cuoc: " << listPlayer[max].name << endl;
				max++;
				for (int i = 0; i < songuoichoi; i++)
					listClients[i].Send((char*)&max, sizeof(int), 0);
			system("pause");
					for (int i = 0; i < songuoichoi; i++)
					{
						listClients->Close();
					}
					delete[] listClients;
					listPlayer.clear();
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
