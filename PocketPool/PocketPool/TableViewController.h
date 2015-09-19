//
//  TableViewController.h
//  PocketPool
//
//  Created by Catheryn Li on 9/19/15.
//  Copyright (c) 2015 TeamAbs. All rights reserved.
//

#import <UIKit/UIKit.h>

@class TableView;

@interface TableViewController : UIViewController

@property (nonatomic, copy) NSArray *ballPositions; // array[0] = white ball, array[i] = ball numbered i
@property (nonatomic) CGPoint fingerPosition;
@property (weak, nonatomic) IBOutlet TableView *tableView;

- (void)rerenderImage;

@end
