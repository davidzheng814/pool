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
#include <cmath>

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

cv::Point2f computeIntersect(cv::Vec2i a, cv::Vec2i b)
{
    double rho = a[0], theta = a[1];
    double x1, x2, y1, y2;
    if (theta == 0) {
        x1 = rho, y1 = 0;
        x2 = rho, y2 = 10;
    } else {
        x1 = rho * cos(theta), y1 = rho * sin(theta);
        x2 = 0, y2 = rho/ sin(theta);
    }
    
    rho = b[0], theta = b[1];
    double x3, x4, y3, y4;
    if (theta == 0) {
        x3 = rho, y3 = 0;
        x4 = rho, y4 = 10;
    } else {
        x3 = rho * cos(theta), y3 = rho * sin(theta);
        x4 = 0, y4 = rho/ sin(theta);
    }
//    double xx1 = a[0] * cos(a[1] + PI/2);
//    double xx2 = b[0] * cos(b[1] + PI/2);
//    double yy1 = a[0] * sin(a[1] + PI/2);
//    double yy2 = b[0] * sin(b[1] + PI/2);
//    double x1 = cos(a[1])+xx1, x2 = -cos(a[1])+xx1;
//    double x3 = cos(b[1])+xx2, x4 = -cos(b[1])+xx2;
//    double y1 = sin(a[1])+yy1, y2 = -sin(a[1])+yy1;
//    double y3 = sin(b[1])+yy2, y4 = -sin(b[1])+yy2;
    
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

const int MAXN = 100100;
/*
def distance(line, point):
rho, theta = line
if theta==0:
x1, y1 = (rho, 0)
x2, y2 = (rho, 10)
else:
x1, y1 = (rho*np.cos(theta),rho*np.sin(theta))
x2, y2 = (0, rho/np.sin(theta))
x0, y0 = point
return math.fabs((y2 - y1)*x0 - (x2 - x1)*y0 + x2*y1 - y2*x1)/math.sqrt((y2-y1)*(y2-y1) + (x2-x1)*(x2-x1))

def line_dist(line1, line2):
max_rho_dist = 80
max_th_dist = 5*math.pi/180.
rho1, th1 = line1
rho2, th2 = line2

if math.fabs(rho1-rho2) < max_rho_dist and math.fabs(th1 - th2) < max_th_dist:
return True
return False
 */

double distance(Vec2d line, double x0, double y0) {
    double rho = line[0], theta = line[1];
    double x1, x2, y1, y2;
    if (theta == 0) {
        x1 = rho, y1 = 0;
        x2 = rho, y2 = 10;
    } else {
        x1 = rho * cos(theta), y1 = rho * sin(theta);
        x2 = 0, y2 = rho/ sin(theta);
    }
    return fabs((y2 - y1)*x0 - (x2 - x1)*y0 + x2*y1 - y2*x1) / sqrt((y2-y1)*(y2-y1) + (x2-x1)*(x2-x1));
}

double touching(Vec2d line1, Vec2d line2) {
    double max_rho_dist = 80;
    double max_th_dist = 5*PI/180.;
//    return false;
    return fabs(line1[0]-line2[0]) < max_rho_dist && fabs(line1[1] - line2[1]) < max_th_dist;
    
}

int par[MAXN];
int best[MAXN];
double bestDist[MAXN];

int find(int x) {
    if (par[x] == x) return x;
    return par[x] = find(par[x]);
}

void join(int a, int b) {
    a = find(a);
    b = find(b);
    par[a] = b;
}

const double PI = acos(-1);

+ (int) find_table:(UIImage *)image {
    cv::Mat img;
    UIImageToMat(image, img);
    double scale_to_width = 2000;
    
    cv::resize(img, img, cv::Size(), scale_to_width/img.cols, scale_to_width/img.cols);
    const int width = img.cols;
    const int height = img.rows;
    
    int tbl_color[] = {102, 131, 153};
//    Mat tbl_color = (Mat_<float>(3, 1) << 153., 131., 102.);
    cv::Mat img2(img.rows, img.cols, CV_8UC1);
    img2.at<char>(0,0) = 255;
    const int thres = 75;
    double sum = 0;
    double osum = 0;
    for(int i = 0; i < img.rows; ++i) {
        for(int j = 0; j < img.cols; ++j) {
            uint32_t val = img.at<uint32_t>(i, j);
            if (i == 0 && j == 0) {
                printf("%u\n", val);
                printf("%u %u %u\n", val & 255, (val >> 8) & 255, (val >> 16) & 255);
            }
//            printf("Pixel: %d",val);
            int dist = 0;
            for(int k = 0; k < 3; ++k) {
                int dx = (val >> (8 * k)) & 255;
                dx -= tbl_color[k];
                dist += dx * dx;
            }
            img2.at<char>(i, j) = (dist < thres * thres) ? 255 : 0;
            if (img2.at<char>(i, j)) {
                sum++;
            } else {
                osum++;
            }
        }
    }
    printf("sum = %lf, osum = %lf\n", sum, osum);
    /*
     tbl_color = np.asarray([153, 131, 102])
     threshold = 75
     color_dist = np.sum((img - tbl_color)**2, axis=2)
     tbl_mask = 255*(color_dist < threshold**2)
    */
    
    Canny( img2, img, 50, 200, 3 );

//    cv::Canny(img, img, 50, 200, 3);
//    cv::cvtColor( img, img2, CV_GRAY2BGR );

    std::vector<cv::Vec2f> lines;
    
    printf("Starting hough lines.\n");
//    HoughLinesP( img, _lines, 1, CV_PI/180, 80, 30, 10 );
    cv::HoughLines(img, lines, 0.5, CV_PI/180, 100);
    
    printf("Ending hough lines.\n");
    int n = int(lines.size());
    printf("I HAVE %d LINES\n", n);
    for(int i = 0; i < n; ++i) {
        Vec2f &line = lines[i];
        par[i] = i;
        bestDist[i] = 9999999;
        line[0] = sin(line[1])*(line[0]*sin(line[1]) - height/2.);
        if (line[1] > PI) {
            line[0] = -line[0];
            line[1] = PI*2 - line[1];
        }
    }
    for(int i = 0; i < n; ++i) {
        for(int j = 0; j < i; ++j) {
            if (touching(lines[i], lines[j])) {
                join(i, j);
            }
        }
    }
    
    for(int i = 0; i < n; ++i) {
        int p = find(i);
        double d = distance(lines[i], width / 2, height / 2);
        if (d < bestDist[p]) {
            bestDist[p] = p;
            best[p] = i;
        }
    }
    
    vector<Vec2d> finLines;
    int linecnt = 0;
    for(int i = 0; i < n; ++i) {
        if (par[i] == i) {
            finLines.push_back(lines[best[i]]);
            ++linecnt;
        }
    }
    printf("I HAVE %d FINLINES\n", int(finLines.size()));
    
    for(int i = 0; i<  4; ++i) {
        std::cout << finLines[i][0] << " " << finLines[i][1] << "\n";
    }
    std::vector<Point2f> corners;
    for (int i = 0; i < finLines.size(); i++)
    {
        for (int j = i+1; j < finLines.size(); j++)
        {
            Point2f pt = computeIntersect(lines[i], lines[j]);
//            if (pt.x >= 0 && pt.y >= 0)
            if (pt.x * pt.x + pt.y * pt.y < 1E8)
                corners.push_back(pt);
        }
    }
    
    printf("I HAVE %d CORNERs!!\n", int(corners.size()));
    std::cout << corners << "\n";
    
    printf("Ending corners.\n");
    printf("CheckVector = %d, depth = %d\n, want = %d or %d\n", Mat(corners).checkVector(2), Mat(corners).depth(), CV_32F, CV_32S);
    double l = arcLength(cv::Mat(corners), true);
    printf("L = %lf\n", l);
    std::vector<Point2f> approx;
    cv::approxPolyDP(corners, approx,
                     l * 0.02, true);
    
    printf("I HAVE %d CORNERS!\n\n\n\n", int(approx.size()));
    if (approx.size() != 4)
    {
        return -1;
    }
    printf("Ending approx lines.\n");

    
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
    printf("Ending push back.\n");

    // Get transformation matrix
    cv::Mat transmtx = cv::getPerspectiveTransform(corners, quad_pts);
    
    // Apply perspective transformation
    cv::warpPerspective(img, quad, transmtx, quad.size());
    return 0;
}

@end;