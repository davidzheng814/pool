//
//  TableView.m
//  PocketPool
//
//  Created by Catheryn Li on 9/19/15.
//  Copyright (c) 2015 TeamAbs. All rights reserved.
//

#import "TableView.h"

@interface TableView ()

@property (nonatomic, copy) NSArray *ballViewArray;

@end

@implementation TableView

- (void)setUp {
    // Render background image
//    NSLog(@"TableView setUp");
    UIImage *backgroundImage = [UIImage imageNamed:@"table.png"];
    UIImageView *backgroundView = [[UIImageView alloc] initWithImage:backgroundImage];
    [self addSubview:backgroundView];
    [self sendSubviewToBack:backgroundView];
    // Set up ball views
    NSMutableArray *ballArray = [NSMutableArray array];
    for (int i = 0; i < 16; i++) {
        NSString *ballName = [NSString stringWithFormat:@"ball%d.png", i];
        UIImageView *ballImage = [[UIImageView alloc] initWithFrame:CGRectMake(50, 50, 25, 25)];
        ballImage.image = [UIImage imageNamed:ballName];
        [self addSubview:ballImage];
        ballImage.hidden = YES;
        [ballArray addObject:ballImage];
    }
    self.ballViewArray = ballArray;
}

- (void)renderImageWithBallPositions:(NSArray *)ballPositions {
//    NSLog(@"TableView renderImage called");
    for (int i = 0; i < [ballPositions count]; i++) {
//        NSLog(@"Drawing ball %d", i);
        NSValue *positionVal = ballPositions[i];
        CGPoint position = [positionVal CGPointValue];
        UIImageView *ballImage = self.ballViewArray[i];
        if (position.x > 0 && position.y > 0) {
            ballImage.hidden = NO;
            ballImage.center = position;
        } else {
            ballImage.hidden = YES;
        }
    }
}

- (void)animateImageWithBallPositions:(NSArray *)ballPositions {
    [UIView animateWithDuration:1
                     animations:^ {
                         for (int i = 0; i < [ballPositions count]; i++) {
                             NSValue *positionVal = ballPositions[i];
                             CGPoint position = [positionVal CGPointValue];
                             UIImageView *ballImage = self.ballViewArray[i];
                             if (position.x > 0 && position.y > 0) {
                                 ballImage.hidden = NO;
                                 ballImage.center = position;
                             } else {
                                 ballImage.hidden = YES;
                             }
                         }
                     }
                     completion:^(BOOL finished) {
                         if (finished) {
                             NSLog(@"Animation complete.");
                         }
                     }];
}

- (void)animateImageWithBallFrames:(NSArray *)ballFrames withIndex:(int)index withMaxIndex:(int)max {
    NSLog(@"animateImageWithballFrames called at index %d", index);
    [UIView animateWithDuration:0.04
                          delay:0
                        options:UIViewAnimationOptionCurveLinear | UIViewAnimationOptionBeginFromCurrentState
                     animations:^{
                         for (int i = 0; i < [ballFrames[0] count]; i++) {
                             NSArray *ballPositions = ballFrames[index];
                             NSValue *positionVal = ballPositions[i];
                             CGPoint position = [positionVal CGPointValue];
                             UIImageView *ballImage = self.ballViewArray[i];
                             if (position.x > 0 && position.y > 0) {
                                 ballImage.hidden = NO;
                                 ballImage.center = position;
                             } else {
                                 ballImage.hidden = YES;
                             }
                         }
                     }
                     completion:^(BOOL finished) {
                         if (index == max) {
                             NSLog(@"Animation complete.");
                         } else {
                             int newIndex = index + 1;
                             [self animateImageWithBallFrames:ballFrames withIndex:newIndex withMaxIndex:max];
                         }
                     }];
}

@end
