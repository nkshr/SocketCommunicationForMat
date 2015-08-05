#include "MatCom.hpp"

int main(int argc, char ** argv)
{
	MatCom mc("client");
	mc.init(12345);
	Mat frame;
	namedWindow("received");
	for (;;)
	{
		frame = mc.receiveMat();
		imshow("received", frame);
		waitKey(50);
	}
	return 0;
}
