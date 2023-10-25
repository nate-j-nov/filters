/*
 * Nate Novak
 * CS5330 Project 1 // Spring 2022
 * Video player code to handle key presses by the user
 */

#include <iostream>
#include <string> 
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "../include/filter.hpp"

using namespace std; 
using namespace cv; 

#define BQLEVELS 15
#define CART_MAG_THRESH 15
#define BUFFSIZE 255

enum Filter { None, GrayScale, AltGrayScale, GaussBlur, XSobel, YSobel, Magnitude, BlurQuantize, Cartoon, CleanCartoon,
              ContrastBrightness, UpsideDown, News }; 

int main(int argc, char *argv[]) {
    VideoCapture *capdev;

    // open the video device
    capdev = new VideoCapture(0);
    if( !capdev->isOpened() ) {
            printf("Unable to open video device\n");
            return(-1);
    }

    // get some properties of the image
    Size refS( (int) capdev->get(CAP_PROP_FRAME_WIDTH ),
                    (int) capdev->get(CAP_PROP_FRAME_HEIGHT));
    printf("Expected size: %d %d\n", refS.width, refS.height);

    namedWindow("Video", 1); // identifies a window
    Mat frame;

    int scCounter = 0; 

    // Define the cliff image that I place inside the screen
    string cliffPath = "src/EdgeOfcliff.jpg";
    Mat edgeOfCliff = imread(cliffPath, IMREAD_COLOR);
    Mat realCliff; 
    edgeOfCliff.copyTo(realCliff);  
    resize(edgeOfCliff, realCliff, Size(150, 150)); 

    Filter appliedFilter = None; 
    string newsString = "";
    string tickerString = "UFO sighted over Roux seeking programmers, citing labor shortages.";  

    // Set default values for brightness and contrast
    int brightness = 0; 
    float contrast = 1.0; 

    // Set 
    int counter = 0; 
    int tickerStart = -1 * getTextSize(tickerString, FONT_HERSHEY_PLAIN, 1, 1, 0).width;

    for(;;) 
    {
        *capdev >> frame; // get a new frame from the camera, treat as a stream
        if( frame.empty() ) {
            printf("frame is empty\n");
            break;
        }
            

        Mat dest; 
        frame.copyTo(dest); 

        switch(appliedFilter) {
            case GrayScale: 
                cvtColor(frame, dest, COLOR_RGB2GRAY, 0); 
                break;
            
            case AltGrayScale:
            {
                int success = greyscale(frame, dest); 
                if(success != 0)
                {
                    printf("Failure to convert to grayscale\n"); 
                }
                break;
            }
                 
            case GaussBlur: 
            {
                int success = blur5x5(frame, dest); 
                if(success != 0) 
                {
                    printf("Failure to blur\n");        
                }
                break;
            }

            case XSobel: 
            {
                Mat tmp; 
                frame.copyTo(tmp); 
                int success = sobelX3x3(frame, tmp); 
                if(success != 0){
                    printf("Failure to do sobel 3x3 filter\n"); 
                }
                success = absValOfPixels(tmp, dest); 
                break;
            }

            case YSobel: 
            {  
                Mat tmp; 
                frame.copyTo(tmp); 
                int success = sobelY3x3(frame, tmp); 
                success = absValOfPixels(tmp, dest);
                if(success != 0) {
                    printf("Failure to do sobel 3x3 filter\n"); 
                } 
                break; 
            }
            
            case Magnitude: 
            {
                Mat xsob; 
                Mat ysob; 
                frame.copyTo(xsob); 
                frame.copyTo(ysob); 
                int success = sobelX3x3(frame, xsob); 
                if(success != 0) {
                    printf("Failure to do sobel x\n"); 
                }
                success = sobelY3x3(frame, ysob); 
                if(success != 0) {
                    printf("Failure to do sobel y\n"); 
                }
                success = magnitude_nate(xsob, ysob, dest); 
                if(success != 0) {
                    printf("failure to calculate magnitude\n"); 
                }
                break;
            }
            case BlurQuantize: 
            {
                int success = blurQuantize(frame, dest, BQLEVELS); 
                if(success != 0) {
                    printf("Failure to blur quantize image\n"); 
                }
                break;
            }
            case Cartoon: 
            {
                int success = cartoon(frame, dest, BQLEVELS, CART_MAG_THRESH); 
                if(success != 0) {
                    printf("Failure to cartoonize\n"); 
                }
                break;
            }
            case CleanCartoon: 
            {
                int success = cartoonWithCleaning(frame, dest, BQLEVELS, CART_MAG_THRESH); 
                if(success != 0) {
                    printf("Failure to cartoonize w/ cleaning\n"); 
                }
                break;
            }
            case ContrastBrightness: 
            {
                int success = contrastBrightness(frame, dest, contrast, brightness); 
                if(success != 0) {
                    printf("Failure to adjust contrast and brightness\n"); 
                }
                break;
            } 
            case UpsideDown: 
            {
                int success = upsideDown(frame, dest); 
                if(success != 0) {
                    printf("Failure to turn image upside down"); 
                }
                break;
            }
            case News: 
            {
                // Increase the starting point every 40 frames
                if(counter % 40 == 0) {
                    tickerStart++; 
                    if(tickerStart == frame.cols) {
                        tickerStart = 0; // If the start is greater than the number of cols, reset to zero.
                    }
                }
                int success = news(frame, dest, newsString, tickerString, tickerStart, realCliff); 
                break;
            }
                
            case None: 
                break;
            
            default: 
                break; 
        }
    
        imshow("Video", dest);

        // see if there is a waiting keystroke
        //char key = waitKey(10); 
        int keyEx = waitKeyEx(10);
        
        if(keyEx == 'q')
        {
            break; 
        } 
        else if(keyEx == 's') 
        {
            // Handles screenshot
            string imageName = "Proj1_SC_"; 
            imageName.append(to_string(scCounter)).append(".png"); 
        
            string sPath = "./screenshots/" + imageName; 
            imwrite(sPath, dest); 
            scCounter++; 
        }
        else if(keyEx == 'g')
        {
            appliedFilter = GrayScale;  
        }
        else if(keyEx == 'h')
        {
            appliedFilter = AltGrayScale; 
        }
        else if(keyEx == 'n') 
        {   
            appliedFilter = None;
        } else if(keyEx == 'b')
        {
            appliedFilter = GaussBlur; 
        } else if(keyEx == 'x') {
            appliedFilter = XSobel; 
        } else if(keyEx == 'y') {
            appliedFilter = YSobel; 
        } else if(keyEx == 'm') {
            appliedFilter = Magnitude; 
        } else if(keyEx == 'l') {
            appliedFilter = BlurQuantize; 
        } else if(keyEx == 'c') {
            appliedFilter = Cartoon; 
        } else if(keyEx == 'z') {
            appliedFilter = CleanCartoon;
        } else if (keyEx == 'r') {
            brightness = 0; 
            appliedFilter = ContrastBrightness;
            printf("To adjust the brightness value by 5, use the up/down arrows\n"); 
            printf("To adjust the contrast by .05, use the left/right arrows\n"); 
        } else if(keyEx == 2490368 && appliedFilter == ContrastBrightness) {
            if(brightness < 200) {
                brightness += 5; 
            }
            cout << "Brightness: " << brightness << endl; 
        } else if(keyEx == 2621440 && appliedFilter == ContrastBrightness) { 
            if(brightness > -200) {
                brightness -= 5; 
            }
            cout << "Brightness: " << brightness << endl;
        } else if (keyEx == 2424832 && appliedFilter == ContrastBrightness) {
            if(contrast > 1.0) {
                contrast -= .05; 
            }
            cout << "Contrast: " << contrast << endl; 
        } else if(keyEx == 2555904 && appliedFilter == ContrastBrightness){
            if(contrast < 3.0) {
                contrast += .05; 
            }
            cout << "Contrast: " << contrast << endl;
        } else if(keyEx == 'u') {
            appliedFilter = UpsideDown;
        } else if(keyEx == 'w') {
            appliedFilter = News;
            counter = 0;  
            tickerStart = -1 * getTextSize(tickerString, FONT_HERSHEY_PLAIN, 1, 1, 0).width;
            cout << "What is the news for today?" << endl; 
            getline(cin, newsString);  
        }
        else {
            continue;
        } 
    }

    delete capdev;
    return(0);
}