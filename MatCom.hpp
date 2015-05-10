#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui\highgui.hpp>
#include <opencv2/opencv_lib.hpp>

using namespace std;
using namespace cv;

#define CLIENT 0
#define SERVER 1
#define RECEIVESIZE 65500
#define SENDSIZE 65500

class MatCom
{
private:
	Mat receivedMat;	//recieved Mat
	int sock, numrcv;
	vector<uchar> ibuff;
	struct sockaddr_in addr;
	int role = -1;
	//int receiveSize = 65500;
	char buff[RECEIVESIZE];
	//vector<unsigned char> ibuff;
	vector<int> param = vector<int>(2);

public:
	Mat receiveMat();
	MatCom(string type);
	void init(int port);
	void init(int port, string addr);
	void sendMat(Mat &dst);
	~MatCom();
};

MatCom::MatCom(string role)
{
	if (role.compare("client") == 0)
		this->role = CLIENT;
	else if (role.compare("server") == 0)
		this->role = SERVER;
	else
	{
		cerr << "Error: Invalid argument passed in MatCom constructor."
			<< endl;
		exit(EXIT_FAILURE);
	}
	WSADATA data;
	WSAStartup(MAKEWORD(2, 0), &data);
}


//function for client
void MatCom::init(int port)
{
	if (!(role == CLIENT))
	{
		cerr << "Error: Invalid init function called." << endl;
		exit(EXIT_FAILURE);
	}
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	bind(sock, (struct sockaddr *)&addr, sizeof(addr));
	receivedMat = Mat(480, 640, CV_8UC3);
}


//function for server
void MatCom::init(int port, string ip)
{
	if (!(role == SERVER))
	{
		cerr << "Error: Invalid init function called." << endl;
		exit(EXIT_FAILURE);
	}
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(12345);
	addr.sin_addr.s_addr = inet_addr(ip.c_str());
	param[0] = CV_IMWRITE_JPEG_QUALITY;
	param[1] = 85;
}

Mat MatCom::receiveMat()
{
	numrcv = recv(sock, buff, RECEIVESIZE, 0);
	for (int i = 0; i < RECEIVESIZE; ++i)
		ibuff.push_back((uchar)buff[i]);
	if (numrcv == -1)
		exit(EXIT_FAILURE);
	receivedMat = imdecode(Mat(ibuff), CV_LOAD_IMAGE_COLOR);
	ibuff.clear();
	return receivedMat;
}

void MatCom::sendMat(Mat &dst)
{
	imencode(".jpg", dst, ibuff, param);
	if (ibuff.size() < SENDSIZE)
	{
		for (int i = 0; i < ibuff.size(); ++i)
			buff[i] = ibuff[i];
		sendto(sock, buff, SENDSIZE, 0, (struct sockaddr *)&addr, sizeof(addr));
	}
	ibuff.clear();
}

MatCom::~MatCom()
{
	delete[] buff;
	delete receivedMat.ptr();
	closesocket(sock);
	WSACleanup();
}