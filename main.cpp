#include <iostream>

#include "colour_detector.h"
#include "image_aligner.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;


//Macros for getting pixels from images
#define MpixelGrey(image, x, y) ((uchar *) (((image).data) + (y)*((image).step)))[(x)]
#define MpixelB(image, x, y) ((uchar *) (((image).data) + (y)*((image).step)))[(x)*((image).channels())]
#define MpixelG(image, x, y) ((uchar *) (((image).data) + (y)*((image).step)))[(x)*((image).channels())+1]
#define MpixelR(image, x, y) ((uchar *) (((image).data) + (y)*((image).step)))[(x)*((image).channels())+2]


const string imgPaths[17] =
{
"res/2Dempty.jpg", "res/abcde.jpg",
"res/abcde_rotated.jpg", "res/abcde_rotated_scaled.jpg",
"res/abcde_scaled.jpg", "res/congratulations.jpg",
"res/congratulations_rotated.jpg", "res/congratulations_rotated_scaled.jpg",
"res/congratulations_scaled.jpg", "res/Darwin.jpg",
"res/Darwin_rotated.jpg", "res/Darwin_rotated_scaled.jpg",
"res/Darwin_scaled.jpg", "res/farfaraway.jpg",
"res/farfaraway_rotated.jpg", "res/farfaraway_rotated_scaled.jpg",
"res/farfaraway_scaled.jpg"
};

const char encodingarray[64]={' ','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','x','y','w','z',
'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','X','Y','W','Z',
'0','1','2','3','4','5','6','7','8','9','.'};

void convertRGBtoHSV(Mat &imgBGR, Mat &imgHSV);
int main()
{
    Mat rgb;
    Mat res;
    Mat grid;
    Mat blue;
    Mat rgbFiltered;

    namedWindow("Original", 0);
    namedWindow("Filtered", 0);

    resizeWindow("Original", 500, 500);
    resizeWindow("Filtered", 500, 500);

    colour_detector colourDet;
    colourDet = colour_detector();

    rgb = imread(imgPaths[1], 1);
    if(rgb.data == NULL){
        cout << "Error: File could not be read" << endl;
        exit(1);
    }

    //creates RGB filtered image that is clearer for easy reading for decoding message
    //in the process is converted to hsv and then calculates a better RGB equivilant
    colourDet.makeRGB(rgb, rgbFiltered);

    colourDet.getCircles(rgb, grid);

    image_aligner align = image_aligner();
    align.findCircles(grid);
    align.drawCircles(rgb);

    colourDet.getGrid(rgb, grid);

    align.findGrid(grid);
    align.drawGrid(rgb);

   //----------------------------------------------------------------------
   //Simplified way of reading file

    //int gridDist = ((yMax - yMin) / ()) * 1;
    int xPix = 0;
    int yPix = 0;
    bool skip = false;
    for(int y = 1; y < 11; y++){
        int length;
        if(y <= 6){
            length = 22;
        }else{
            length = 28;
        }
        for(int i = 1; i < length; i++){
            int startX1, startX2, startY1, startY2;

            int currentX1, currentX2, currentY1, currentY2;

            xPix = i -1;
            yPix = y -1;
            if(yPix <= 6){
                startX1 = align.getxMin() + (40 * 3) + 10;
                startX2 = align.getxMin() + (40 * 3) + 10 + 20;
                startY1 = align.getyMin() + 10;
                startY2 = align.getyMin() + 10;
            }else{
                startX1 = align.getxMin() + 10;
                startX2 = align.getxMin() + 10 + 20;
                startY1 = align.getyMin() + 10;
                startY2 = align.getyMin() + 10;
            }

            if(y % 2 == 0) {
                if((i == 21 && length == 22) || (i == 27 && length == 28)){
                    skip = true;
                }else{
                    currentX1 = startX1 + (40*xPix) + 20;
                    currentX2 = startX2 + (40*xPix) + 20;

                    currentY1 = startY1 + (20*yPix);
                    currentY2 = startY2 + (20*yPix);
                    skip = false;
                }
            }else{ // is an odd line
                if((i != 21 && length == 22) || (i != 27 && length == 28)){
                    currentX1 = startX1 + (40*xPix);
                    currentX2 = startX2 + (40*xPix);

                    currentY1 = startY1 + (20*yPix);
                    currentY2 = startY2 + (20*yPix);
                    skip = false;
                }else{
                    currentX1 = startX1 + (40*20);
                    currentX2 = startX1;

                    currentY1 = startY1 + (20*yPix);
                    currentY2 = startY2 + (20*(yPix+1));
                    skip = false;
                }
            }

            if(!skip){
                unsigned char temp = 0;
                if(MpixelR(rgbFiltered, currentX1, currentY1) >= 220){
                    temp = temp | (1<<5);
                }
                if((int)MpixelG(rgbFiltered, currentX1, currentY1) >= 220){
                    temp = temp | (1<<4);
                }
                if((int)MpixelB(rgbFiltered, currentX1, currentY1) >= 220){
                    temp = temp | (1<<3);
                }
                if((int)MpixelR(rgbFiltered, currentX2, currentY2) >= 220){
                    temp = temp | (1<<2);
                }
                if((int)MpixelG(rgbFiltered, currentX2, currentY2) >= 220){
                    temp = temp | (1<<1);
                }
                if((int)MpixelB(rgbFiltered, currentX2, currentY2) >= 220){
                    temp = temp | (1);
                }
                cout << encodingarray[(unsigned int)temp];
            }
        }
    }
    cout << endl;





    imshow("Original", rgb);
    imshow("Filtered", rgbFiltered);

    waitKey(0);
    return 0;
}
