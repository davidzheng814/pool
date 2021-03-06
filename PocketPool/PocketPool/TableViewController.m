//
//  TableViewController.m
//  PocketPool
//
//  Created by Catheryn Li on 9/19/15.
//  Copyright (c) 2015 TeamAbs. All rights reserved.
//

#import "TableViewController.h"
#import "TableView.h"
#import "PhysicsEngine.h"
#import "AppDelegate.h"
#import "Scanner.h"

@interface TableViewController ()

@property (nonatomic) UIImageView *ringImageView;
@property (nonatomic) UIImageView *arrowImageView;

@end

@implementation TableViewController

- (void)viewDidLoad {
    // Setting up ball and finger positions
    
    // Create test/default ball positions
    NSMutableArray *defaultBallPositions = [NSMutableArray array];
    for (int i = 0; i < 3; i++) {
        CGPoint point = CGPointMake(1.5 + i * .2, .5 + i * .1);
        NSValue *value = [NSValue valueWithCGPoint:point];
        [defaultBallPositions addObject:value];
    }
    self.ballPositions = defaultBallPositions; // Remember to pull points out with [array[i] CGPointValue]
    
    // Send uploaded image to scanner
    AppDelegate *appDelegate = [[UIApplication sharedApplication] delegate];
    UIImage *uploadedImage = appDelegate.uploadedImage;
//    self.ballPositions = [Scanner find_table:uploadedImage];
    
    self.fingerPosition = CGPointMake(-1, -1);
    [self.tableView setUp];
    [self rerenderTableImage];
    
    // Set up ring and arrow images
    UIImage *ringImage = [UIImage imageNamed:@"ring.png"];
    self.ringImageView.image = ringImage;
    self.ringImageView.translatesAutoresizingMaskIntoConstraints = YES;
    [self.tableView addSubview:self.ringImageView];
//    [self.tableView bringSubviewToFront:self.ringImageView];
    self.ringImageView.hidden = YES;
    //    UIImage *arrowImage = [UIImage imageNamed:@"arrow.png"];
    //    self.arrowImageView.image = arrowImage;
    //    [self addSubview:self.arrowImageView];
    
    // Setting up gesture recognizer
    UITapGestureRecognizer *singleTap = [[UITapGestureRecognizer alloc] initWithTarget:self
                                                                                action:@selector(doSingleTap:)];
    singleTap.numberOfTapsRequired = 1;
    [self.view addGestureRecognizer:singleTap];
    UITapGestureRecognizer *doubleTap = [[UITapGestureRecognizer alloc] initWithTarget:self
                                                                                action:@selector(doDoubleTap:)];
    doubleTap.numberOfTapsRequired = 2;
    [self.view addGestureRecognizer:doubleTap];
    [singleTap requireGestureRecognizerToFail:doubleTap];
    
    // Testing for coordinate values
//    UIImage *testImage = [UIImage imageNamed:@"dot.png"];
//    CGRect cropRect = CGRectMake(0.0, 0.0, 20.0, 20.0);
//    UIImageView *testImageView = [[UIImageView alloc] initWithFrame:cropRect];
//    testImageView.image = testImage;
//    testImageView.center = CGPointMake(576, 107);
//    [self.view addSubview:testImageView];
}

- (void)rerenderTableImage { // Only cares about balls
//    NSLog(@"TableViewController rerender image. BallPositions count: %lu", (unsigned long)[self.ballPositions count]);
    [self.tableView renderImageWithBallPositions:self.ballPositions];
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

- (void)doSingleTap:(UITapGestureRecognizer *)recognizer { // Places finger there
    [self rerenderTableImage];
    self.fingerPosition = [recognizer locationInView:self.tableView];
    self.ringImageView.center = self.fingerPosition;
//    NSLog(@"single tap at %f, %f", self.fingerPosition.x, self.fingerPosition.y);
    self.ringImageView.hidden = NO;
    [self.tableView bringSubviewToFront:self.ringImageView];
}

- (void)doDoubleTap:(UITapGestureRecognizer *)recognizer { // Release tap
    [self releaseTap];
}

- (IBAction)handlePan:(UIPanGestureRecognizer *)recognizer {
    self.fingerPosition = [recognizer locationInView:self.tableView];
    if (recognizer.state == UIGestureRecognizerStateBegan && hypot((self.ringImageView.center.x - self.fingerPosition.x), (self.ringImageView.center.y - self.fingerPosition.y)) < 50) {
        CGPoint translation = [recognizer translationInView:self.tableView];
        self.fingerPosition = CGPointMake(self.fingerPosition.x + translation.x, self.fingerPosition.y + translation.y);
        [recognizer setTranslation:CGPointMake(0, 0) inView:self.tableView];
    }
    self.ringImageView.center = self.fingerPosition;
    [self rerenderTableImage];
    self.ringImageView.hidden = NO;
    [self.tableView bringSubviewToFront:self.ringImageView];
}

- (void)releaseTap { // Table will animate
    // Fetch frames from physics engine
    CGPoint fingerLocation = [TableView convertDisplayPointToPoint:self.fingerPosition];
//    NSValue *fingerLocationVal = [NSValue valueWithCGPoint:fingerLocation];
    NSArray *ballFrames = [PhysicsEngine findAllStates:self.ballPositions withFingerPosition:[TableView convertDisplayPointToPoint:self.fingerPosition]];

    // Create test frames:
//    NSMutableArray *ballFrames = [NSMutableArray array];
//    for (int j = 0; j < 20; j++) {
//        NSMutableArray *ballPositions = [NSMutableArray array];
//        for (int i = 0; i < 16; i++) {
//            CGPoint point = CGPointMake(50 + j * 5 + i * 26, 50 + j * 1 + i * 10);
//            NSValue *value = [NSValue valueWithCGPoint:point];
//            [ballPositions addObject:value];
//        }
//        [ballFrames addObject:ballPositions];
//    }
    
    // Disable gestures
    
    
    // Animate
    [self.tableView animateImageWithBallFrames:ballFrames
                                     withIndex:0
                                  withMaxIndex:(int)[ballFrames count]];
}

@end
