//
//  Scanner.m
//  PocketPool
//
//  Created by David Zheng on 9/19/15.
//  Copyright (c) 2015 TeamAbs. All rights reserved.
//
#import "Scanner.h"
#import <Foundation/Foundation.h>
#import <opencv2/opencv.hpp>
#import "opencv2/highgui/ios.h"
#include "iostream"
#import <vector>

using namespace cv;

@implementation Scanner
Mat cvMatFromUIImage(UIImage *image)
{
    CGColorSpaceRef colorSpace = CGImageGetColorSpace(image.CGImage);
    CGFloat cols = image.size.width;
    CGFloat rows = image.size.height;
    
    cv::Mat cvMat(rows, cols, CV_8UC4); // 8 bits per component, 4 channels (color channels + alpha)
    
    CGContextRef contextRef = CGBitmapContextCreate(cvMat.data,                 // Pointer to  data
                                                    cols,                       // Width of bitmap
                                                    rows,                       // Height of bitmap
                                                    8,                          // Bits per component
                                                    cvMat.step[0],              // Bytes per row
                                                    colorSpace,                 // Colorspace
                                                    kCGImageAlphaNoneSkipLast |
                                                    kCGBitmapByteOrderDefault); // Bitmap info flags
    
    CGContextDrawImage(contextRef, CGRectMake(0, 0, cols, rows), image.CGImage);
    CGContextRelease(contextRef);
    
    return cvMat;
}

cv::Point2f computeIntersect(cv::Vec4i a, cv::Vec4i b)
{
    int x1 = a[0], y1 = a[1], x2 = a[2], y2 = a[3];
    int x3 = b[0], y3 = b[1], x4 = b[2], y4 = b[3];
    
    if (float d = ((float)(x1-x2) * (y3-y4)) - ((y1-y2) * (x3-x4)))
    {
        cv::Point2f pt;
        pt.x = ((x1*y2 - y1*x2) * (x3-x4) - (x1-x2) * (x3*y4 - y3*x4)) / d;
        pt.y = ((x1*y2 - y1*x2) * (y3-y4) - (y1-y2) * (x3*y4 - y3*x4)) / d;
        return pt;
    }
    else
        return cv::Point2f(-1, -1);
}

void sortCorners(std::vector<cv::Point2f>& corners, cv::Point2f center)
{
    std::vector<cv::Point2f> top, bot;
    
    for (int i = 0; i < corners.size(); i++)
    {
        if (corners[i].y < center.y)
            top.push_back(corners[i]);
        else
            bot.push_back(corners[i]);
    }
    
    cv::Point2f tl = top[0].x > top[1].x ? top[1] : top[0];
    cv::Point2f tr = top[0].x > top[1].x ? top[0] : top[1];
    cv::Point2f bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
    cv::Point2f br = bot[0].x > bot[1].x ? bot[0] : bot[1];
    
    corners.clear();
    corners.push_back(tl);
    corners.push_back(tr);
    corners.push_back(br);
    corners.push_back(bl);
}

+ (int) find_table:(UIImage *)image {
    cv::Mat img;
    UIImageToMat(image, img);
    double scale_to_width = 2000;
    
    cv::resize(img, img, cv::Size(), scale_to_width/img.cols, scale_to_width/img.cols);
    const int width = img.cols;
    const int height = img.rows;
    
    Mat tbl_color = (Mat_<float>(3, 1) << 153., 131., 102.);
    const int threshold = 75;
    /*
     tbl_color = np.asarray([153, 131, 102])
     threshold = 75
     color_dist = np.sum((img - tbl_color)**2, axis=2)
     tbl_mask = 255*(color_dist < threshold**2)
    */
    
    cv::Canny(img, img, 50, 200, 3);
    
    std::vector<cv::Vec4i> lines;
    cv::HoughLines(img, lines, 1, CV_PI/180, 100);
    
    
    
    // Code to get corners and do transform.
    std::vector<cv::Point2f> corners;
    for (int i = 0; i < lines.size(); i++)
    {
        for (int j = i+1; j < lines.size(); j++)
        {
            cv::Point2f pt = computeIntersect(lines[i], lines[j]);
            if (pt.x >= 0 && pt.y >= 0)
                corners.push_back(pt);
        }
    }
    
    std::vector<cv::Point2f> approx;
    cv::approxPolyDP(cv::Mat(corners), approx,
                     cv::arcLength(cv::Mat(corners), true) * 0.02, true);
    
    if (approx.size() != 4)
    {
        return -1;
    }
    
    // Get mass center
    cv::Point2f center(0,0);
    for (int i = 0; i < corners.size(); i++)
        center += corners[i];
    
    center *= (1. / corners.size());
    sortCorners(corners, center);
    
    // Define the destination image
    cv::Mat quad = cv::Mat::zeros(300, 220, CV_8UC3);
    
    // Corners of the destination image
    std::vector<cv::Point2f> quad_pts;
    quad_pts.push_back(cv::Point2f(0, 0));
    quad_pts.push_back(cv::Point2f(quad.cols, 0));
    quad_pts.push_back(cv::Point2f(quad.cols, quad.rows));
    quad_pts.push_back(cv::Point2f(0, quad.rows));
    
    // Get transformation matrix
    cv::Mat transmtx = cv::getPerspectiveTransform(corners, quad_pts);
    
    // Apply perspective transformation
    cv::warpPerspective(src, quad, transmtx, quad.size());
}

@end;