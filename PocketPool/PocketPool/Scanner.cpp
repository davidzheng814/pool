//
//  Scanner.cpp
//  PocketPool
//
//  Created by David Zheng on 9/19/15.
//  Copyright (c) 2015 TeamAbs. All rights reserved.
//

#include "Scanner.h"
#ifdef __cplusplus
#import <opencv2/opencv.hpp>
#endif

using namespace cv;

Mat load_image(const string& filename) {
    return imread(filename);
}

void find_table(char *filename) {
//    double scale_to_width = 2000;
    
}