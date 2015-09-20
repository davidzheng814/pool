//
//  Scanner.h
//  PocketPool
//
//  Created by David Zheng on 9/19/15.
//  Copyright (c) 2015 TeamAbs. All rights reserved.
//
#import <Foundation/Foundation.h>

@interface PhysicsEngine : NSObject {}

+(NSArray *) findAllStates:(NSArray *)balls withFingerPosition:(CGPoint)fingerPosition;

@end