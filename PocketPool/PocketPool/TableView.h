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
- (void)renderImageWithFingerPosition:(CGPoint)fingerPosition
                    withBallPositions:(NSArray *)ballPositions;

@end
