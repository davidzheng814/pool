//
//  TableViewController.m
//  PocketPool
//
//  Created by Catheryn Li on 9/19/15.
//  Copyright (c) 2015 TeamAbs. All rights reserved.
//

#import "TableViewController.h"
#import "TableView.h"

@implementation TableViewController

- (void)viewDidLoad {
    // Setting up default ball and finger positions
    self.ballPositions = @[[NSValue valueWithCGPoint:CGPointMake(10, 10)],
                           [NSValue valueWithCGPoint:CGPointMake(20, 20)],
                           [NSValue valueWithCGPoint:CGPointMake(30, 30)],
                           [NSValue valueWithCGPoint:CGPointMake(40, 40)],
                           [NSValue valueWithCGPoint:CGPointMake(50, 50)]
                           ]; // Remember to pull points out with [array[i] CGPointValue]
    self.fingerPosition = CGPointMake(-1, -1);
    [self.tableView setUp];
    [self rerenderImage];
}

- (void)rerenderImage {
    NSLog(@"TableViewController rerender image. BallPositions count: %lu", (unsigned long)[self.ballPositions count]);
    [self.tableView renderImageWithFingerPosition:self.fingerPosition
                                withBallPositions:self.ballPositions];
}

// MARK: Orientation lock

- (UIInterfaceOrientation)preferredInterfaceOrientationForPresentation {
//    return UIInterfaceOrientationLandscapeRight;
    return UIInterfaceOrientationLandscapeLeft;
}

- (NSUInteger)supportedInterfaceOrientations {
    return UIInterfaceOrientationMaskLandscape;
}

// MARK: Handle gestures

- (IBAction)handleTap:(UITapGestureRecognizer *)recognizer {
    if (recognizer.state == UIGestureRecognizerStateEnded) {
        self.fingerPosition = [recognizer locationInView:self.tableView];
    }
    // Double tap = erase finger
}

- (IBAction)handlePan:(UIPanGestureRecognizer *)recognizer {
    
    CGPoint translation = [recognizer translationInView:self.tableView];
    recognizer.view.center = CGPointMake(recognizer.view.center.x + translation.x,
                                         recognizer.view.center.y + translation.y);
    [recognizer setTranslation:CGPointMake(0, 0) inView:self.tableView];
}

@end
