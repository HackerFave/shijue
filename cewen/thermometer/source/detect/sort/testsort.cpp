#include <iostream>
#include <fstream>
#include <iomanip> // to format image names using setw() and setfill()
#include <io.h>    // to check file existence using POSIX function access(). On Linux include <unistd.h>.
#include <set>

#include "sort.h"

#define CNUM 20
// global variables for counting
int total_frames = 0;
double total_time = 0.0;

void TestSORT(string seqName, bool display)
{
	cout << "Processing " << seqName << "..." << endl;

	// 0. randomly generate colors, only for display
	RNG rng(0xFFFFFFFF);
	Scalar_<int> randColor[CNUM];
	for (int i = 0; i < CNUM; i++)
		rng.fill(randColor[i], RNG::UNIFORM, 0, 256);

	string imgPath = "3DMOT2015/train/" + seqName + "/img1/";
	if (display)
		if (_access(imgPath.c_str(), 0) == -1)
		{
			cerr << "Image path not found!" << endl;
			display = false;
		}

	// 1. read detection file
	ifstream detectionFile;
	string detFileName = "data/" + seqName + "/det.txt";
	detectionFile.open(detFileName);
	if (!detectionFile.is_open())
	{
		cerr << "Error: can not find file " << detFileName << endl;
		return;
	}

	string detLine;
	istringstream ss;
	vector<TrackingBox> detData;
	char ch;
	float tpx, tpy, tpw, tph;
	while ( getline(detectionFile, detLine) )
	{
		TrackingBox tb;

		ss.str(detLine);
		ss >> tb.frame >> ch >> tb.id >> ch;
		ss >> tpx >> ch >> tpy >> ch >> tpw >> ch >> tph;
		ss.str("");

		tb.box = Rect_<float>(Point_<float>(tpx, tpy), Point_<float>(tpx + tpw, tpy + tph));
		detData.push_back(tb);
	}
	detectionFile.close();

	// 2. group detData by frame
	int maxFrame = 0;
	for (auto tb : detData) // find max frame number
	{
		if (maxFrame < tb.frame)
			maxFrame = tb.frame;
	}

	vector<vector<TrackingBox>> detFrameData;
	vector<TrackingBox> tempVec;
	for (int fi = 0; fi < maxFrame; fi++)
	{
		for (auto tb : detData)
			if (tb.frame == fi + 1) // frame num starts from 1
				tempVec.push_back(tb);
		detFrameData.push_back(tempVec);
		tempVec.clear();
	}

	double cycle_time = 0.0;
	int64 start_time = 0;

	// prepare result file.
	ofstream resultsFile;
	string resFileName = "output/" + seqName + ".txt";
	resultsFile.open(resFileName);
	if (!resultsFile.is_open())
	{
		cerr << "Error: can not create file " << resFileName << endl;
		return;
	}

    class sort sortinst;
    sortinst.initialize();
	//////////////////////////////////////////////
	// main loop
	for (int fi = 0; fi < maxFrame; fi++)
	{
		total_frames++;
		cout << total_frames << endl;

		// I used to count running time using clock(), but found it seems to conflict with cv::cvWaitkey(),
		// when they both exists, clock() can not get right result. Now I use cv::getTickCount() instead.
		start_time = getTickCount();

        vector<TrackingBox> frameTrackingResult = sortinst.track(detFrameData[fi]);

		cycle_time = (double)(getTickCount() - start_time);
		total_time += cycle_time / getTickFrequency();

		for (auto tb : frameTrackingResult)
			resultsFile << tb.frame << "," << tb.id << "," << tb.box.x << "," << tb.box.y << "," << tb.box.width << "," << tb.box.height << ",1,-1,-1,-1" << endl;

		if (display) // read image, draw results and show them
		{
			ostringstream oss;
			oss << imgPath << setw(6) << setfill('0') << fi + 1;
			Mat img = imread(oss.str() + ".jpg");
			if (img.empty())
				continue;
			
			for (auto tb : frameTrackingResult)
				cv::rectangle(img, tb.box, randColor[tb.id % CNUM], 2, 8, 0);
			imshow(seqName, img);
			cv::waitKey(40);
		}
	}

	resultsFile.close();

	if (display)
		destroyAllWindows();
}

int main()
{
    total_frames = 0;
    total_time = 0;

    std::vector<string> sequences = { "PETS09-S2L1", "TUD-Campus", "TUD-Stadtmitte", "ETH-Bahnhof", "ETH-Sunnyday", "ETH-Pedcross2", "KITTI-13", "KITTI-17", "ADL-Rundle-6", "ADL-Rundle-8", "Venice-2" };
    //for (auto seq : sequences)
        //TestSORT(seq, false);
    TestSORT("PETS09-S2L1", true);

    // Note: time counted here is of tracking procedure, while the running speed bottleneck is opening and parsing detectionFile.
    cout << "Total Tracking took: " << total_time << " for " << total_frames << " frames or " << ((double)total_frames / (double)total_time) << " FPS" << endl;

    return 0;
}
