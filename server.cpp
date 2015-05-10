#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib, "wsock32.lib")
#include <MatCom.hpp>

int main(int argc, char ** argv)
{
	MatCom mc("server");
	mc.init(12345, "172.18.1.5");
	VideoCapture cap(10);
	Mat frame;
	namedWindow("sent");
	for (;;)
	{
		cap >> frame;
		mc.sendMat(frame);
		imshow("sent", frame);
		waitKey(30);
	}
	return 0;
}