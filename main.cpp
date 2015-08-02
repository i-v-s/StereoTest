#include <iostream>
#include "opencv2/opencv.hpp"


using namespace std;

void med(unsigned int * data, uint8_t * img)
{
    unsigned int * dp = data, min = UINT_MAX, max = 0;
    uint8_t * bp = img;

    for(int x = 640 * 480 * 3; x--;)
    {
        unsigned int d = *dp;
        d += *(bp++) - (d >> 4);
        *(dp++) = d;
        if(max < d) max = d;
        if(min > d) min = d;
    }
    dp = data;
    bp = img;
    if(max == min) max++;
    for(int x = 640 * 480 * 3; x--;)
    {
        unsigned int d = *(dp++);
        *(bp++) = ((d - min) * 255) / (max - min);
    }
}

void testCams()
{
    cv::VideoCapture cap1(0), cap2(1);
    cv::Mat img1, img2;
    unsigned int * data1 = new unsigned int[640 * 480 * 3];
    unsigned int * data2 = new unsigned int[640 * 480 * 3];
    while(cap1.isOpened() && cap2.isOpened())
    {
        cap1.read(img1);
        if(!img1.empty())
        {
            med(data1, img1.data);
            cv::imshow("left", img1);

        }
        cap2.read(img2);
        if(!img2.empty())
        {
            med(data2, img2.data);
            cv::imshow("right", img2);
        }
        cv::waitKey(35);
    }
    delete[] data1;
    delete[] data2;
}

void fillStereoOut(uint8_t * po, int lastx, int lastd, int x, int d)
{
    if(!lastx)
    {
        while(x--)
        {
            *(po++) = 0;
            *(po++) = 0;
            *(po++) = 0;
        }
        return;
    }
    po += lastx * 3;
    x -= lastx;
    for(int t = 0; t < x; t++)
    {
        int r = int(lastd + float(d - lastd) * t / x);
        if(r > 255) r = 0;
        *(po++) = r;
        *(po++) = r;
        *(po++) = r;
    }
}

void calcStereo(cv::Mat &il, cv::Mat &ir, cv::Mat &io, int maxd)
{
    int w = il.cols, h = il.rows;
    if(w > ir.cols) w = ir.cols;
    if(h > ir.rows) h = ir.rows;
    for(int y = 0; y < h; y++)
    {
        uint8_t * pl = il.data + il.cols * y * 3;
        uint8_t * pr = ir.data + ir.cols * y * 3;
        uint8_t * po = io.data + io.cols * y * 3;
        int lastd = 0, lastx = 0;
        for(int x = 0; x < w - maxd; x++)
        {
            int max = 0;
            int min = maxd;
            int r1 = *(pr++), r2 = *(pr++), r3 = *(pr++);
            uint8_t * pl1 = pl;
            pl += 3;
            for(int d = 0; d < maxd; d++)
            {
                int dif = abs(r1 - *(pl1++));
                dif += abs(r2 - *(pl1++));
                dif += abs(r3 - *(pl1++));
                if(dif < 40)
                {
                    max = d;
                    if(min > d) min = d;
                }
            }
            if(min < max)
            {
                if(min > lastd)
                {
                    if(max - min > 30)
                    {
                        lastd = 0;
                        min = 0;
                    }
                    fillStereoOut(po, lastx, lastd, x, min);
                    lastd = min;
                    lastx = x;
                }
                else if(max < lastd)
                {
                    fillStereoOut(po, lastx, lastd, x, max);
                    lastd = max;
                    lastx = x;
                }
            }
        }


    }

}

void analizeStereo(cv::Mat &il, cv::Mat &ir, cv::Mat &io, int y)
{
    int w = il.cols;
    if(w > ir.cols) w = ir.cols;
    // pl > pr
    for(int d = 0; d < 100; d++)
    {
        uint8_t * pl = il.data + (il.cols * y + d) * 3;
        uint8_t * pr = ir.data + ir.cols * y * 3;
        uint8_t * po = io.data + io.cols * (y + 1 + d) * 3;
        for(int x = 0; x < w - d; x++)
        {
            int d = abs(*(pl++) - *(pr++));
            d += abs(*(pl++) - *(pr++));
            d += abs(*(pl++) - *(pr++));
            if(d > 40) d = 255;
            d = 255 - (d << 2);
            if(d < 0) d = 0;
            *(po++) = d;
            *(po++) = d;
            *(po++) = d;
        }
    }
}

int main()
{
    cv::Mat il, ir;
    il = cv::imread("../pics/test1l.png", CV_LOAD_IMAGE_COLOR);   // Read the file
    ir = cv::imread("../pics/test1r.png", CV_LOAD_IMAGE_COLOR);   // Read the file

    if(!il.data || !ir.data)
    {
        cout <<  "Could not open or find the image" << std::endl;
        return -1;
    }

    //calcStereo(il, ir, ir, 70);
    analizeStereo(il, ir, ir, 700);

    cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );// Create a window for display.
    cv::imshow( "Display window", ir );                   // Show our image inside it.

    cv::waitKey(0);                                          // Wait for a keystroke in the window
    return 0;
}

