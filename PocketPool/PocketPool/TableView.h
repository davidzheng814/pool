//
//  TableView.h
//  PocketPool
//
//  Created by Catheryn Li on 9/19/15.
//  Copyright (c) 2015 TeamAbs. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface TableView : UIView

- (void)setUp;
- (void)renderImageWithBallPositions:(NSArray *)ballPositions;
- (void)animateImageWithBallPositions:(NSArray *)ballPositions;
- (void)animateImageWithBallFrames:(NSArray *)ballFrames // array of ballPosition arrays
                         withIndex:(int)index
                      withMaxIndex:(int)max;
+ (CGPoint)convertPointToDisplayPoint:(CGPoint)point;

@end
