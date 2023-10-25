/*
 * Nate Novak
 * CS5330 Project 1 // Spring 2022
 * Image displayer to handle key presses by the user
 */

#include <iostream>
#include <string>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core.hpp>

using namespace std; 
using namespace cv; 

int main() 
{
    string bootPath = "src/Boots.jpg";
    string cliffPath = "src/EdgeOfcliff.jpg";
    Mat image = imread(bootPath, IMREAD_COLOR); 
    Mat currentImage = image; 
    bool bootsShowing;
    bool cliffShowing;  

    cout << "Welcome to my image displayer." << endl
    << "You'll start by seeing an image of my ski boots." << endl
    << "If you want to see a picture of me taking that picture, press \'c\'." << endl
    << "If you want to return to my boots, press \'b\'. " << endl
    << "If you want to see whatever picture you're looking at in gray, press \'g\'." << endl
    << "Press \'q\' to terminate the program." << endl; 

    if(image.empty()) 
    {
        cout << "Could not read the image" << endl; 
        return 0; 
    }

    imshow("Boots", image); 
    bootsShowing = true; 

    while(true) {
        // Wait for key press
        char key = waitKey(0);
        if(key == 'q') 
        {
            return 0; 
        } else if(key =='c') {   
            if(cliffShowing) continue;   
            image = imread(cliffPath, IMREAD_COLOR); 
            destroyWindow("Boots");
            bootsShowing = false;
            imshow("EdgeOfCliff", image); 
            cliffShowing = true; 
        }
        else if(key = 'g')
        {
            if(bootsShowing) 
            {
                Mat image = imread(bootPath, IMREAD_GRAYSCALE); 
                destroyWindow("Boots"); 
                imshow("Boots", image); 
            } 
            if(cliffShowing)
            {
                Mat image = imread(cliffPath, IMREAD_GRAYSCALE); 
                destroyWindow("EdgeOfCliff");
                imshow("EdgeOfCliff", image);  
            }
        }
        else if(key == 'b') 
        {   
            if(bootsShowing) continue; 
            image = imread(bootPath, IMREAD_COLOR); 
            destroyWindow("EdgeOfCliff"); 
            cliffShowing = false;
            imshow("Boots", image); 
            bootsShowing = true; 
        } 
    }

    return 0; 
}