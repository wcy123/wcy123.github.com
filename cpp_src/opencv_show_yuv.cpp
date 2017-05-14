
#include <iostream>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>


using namespace cv;
using namespace std;



char* readFile2(const string &fileName)
{
    ifstream ifs(fileName.c_str(), ios::in | ios::binary | ios::ate);
    if(!ifs) {
        cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
             << "cannot open file: " << fileName
             << endl;
    }
    ifstream::pos_type fileSize = ifs.tellg();
    char* ret = new char[fileSize];
    ifs.seekg(0, ios::beg);
    ifs.read(ret, fileSize);
    if(!ifs) {
        cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
             << "error"
             << "size "  << fileSize << " "
             << endl;
    }
    cerr <<  __FILE__ << ":" << __LINE__ << ": [" << __FUNCTION__<< "] "
         << "fileSize "  << fileSize << " "
         << endl;
    return ret;
}

int main( int argc, char** argv )
{
    int width;
    int height;
    if (argc != 4)
    {
        fprintf(stderr, "usage: %s file.yuv width height\n", argv[0]);
        return 1;
    }
    width = atoi(argv[2]);
    height = atoi(argv[3]);
    if (width <= 0 || height <= 0)
    {
        fprintf(stderr, "error: bad frame dimensions: %d x %d\n", width, height);
        return 1;
    }


    char * buf = readFile2(argv[1]); // , width*height*2);
    // Mat imageYUV(Size{width,height},CV_8UC3, buf);
    // Mat image(Size{width,height},CV_8UC3, buf);
    Mat imageY(Size{width,height*3},CV_8UC1, buf);
    Mat imageCrHalf(Size{width,height/2},CV_8UC1, buf + width * height);
    Mat imageCbHalf(Size{width/2,height/2},CV_8UC1, buf + width * height + width * height / 4);
    // Mat imageCr(Size{width,height},CV_8UC1);
    // Mat imageCb(Size{width,height},CV_8UC1);
    // cv::resize(imageCrHalf, imageCr, imageCr.size(), CV_INTER_CUBIC);
    // cv::resize(imageCbHalf, imageCb, imageCb.size(), CV_INTER_CUBIC);
    // cv::merge({imageY, imageCr, imageCb}, imageYUV);
    // cv::cvtColor(imageYUV, imageYUV, CV_YCrCb2BGR);
    namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "Display window Y", imageY);                   // Show our image inside it.
    //imshow( "Display window CbHalf", imageCbHalf);                   // Show our image inside it.
    //imshow( "Display window CrHalf", imageCrHalf);                   // Show our image inside it.

    waitKey(0);                                          // Wait for a keystroke in the window
    return 0;
}
