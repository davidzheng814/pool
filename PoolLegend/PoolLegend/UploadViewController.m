//
//  ViewController.m
//  PocketPool
//
//  Created by Catheryn Li on 9/19/15.
//  Copyright (c) 2015 TeamAbs. All rights reserved.
//

#import "UploadViewController.h"

@interface UploadViewController ()

@property (weak, nonatomic) IBOutlet UIButton *libraryPhotoButton;
@property (weak, nonatomic) IBOutlet UIButton *cameraPhotoButton;
@property (nonatomic, strong) UIImage *uploadedImage;
@property (weak, nonatomic) IBOutlet UIImageView *uploadedImageView;

@end

@implementation UploadViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)uploadPhotoButtonPressed:(id)sender {
    if ([UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypePhotoLibrary]) {
        UIImagePickerController *ipc = [[UIImagePickerController alloc] init];
        ipc.sourceType = UIImagePickerControllerSourceTypePhotoLibrary;
        ipc.delegate = self;
        [self presentViewController:ipc animated:YES completion:nil];
    } else {
        UIAlertView *noPhotoLibraryAlert = [[UIAlertView alloc] initWithTitle:@"No photo library"
                                                                      message:@"Sorry, cannot find photo library. Please choose a different upload option."
                                                                     delegate:self
                                                            cancelButtonTitle:@"OK"
                                                            otherButtonTitles:nil];
        [noPhotoLibraryAlert show];
    }

}

- (IBAction)cameraPhotoButtonPressed:(id)sender {
    if ([UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypeCamera]) {
        UIImagePickerController *ipc = [[UIImagePickerController alloc] init];
        ipc.sourceType = UIImagePickerControllerSourceTypeCamera;
        ipc.showsCameraControls = YES;
        ipc.delegate = self;
        [self presentViewController:ipc animated:YES completion:nil];
    } else {
        UIAlertView *noCameraAlert = [[UIAlertView alloc] initWithTitle:@"No camera"
                                                                message:@"Sorry, cannot find camera source. Please choose a different upload option."
                                                               delegate:self
                                                      cancelButtonTitle:@"OK"
                                                      otherButtonTitles:nil];
        [noCameraAlert show];
    }
}

- (void)imagePickerController:(UIImagePickerController *)picker
didFinishPickingMediaWithInfo:(NSDictionary *)info {
    NSLog(@"imagePickerController didFinishPickingMediaWithInfo");
    self.uploadedImage = info[UIImagePickerControllerOriginalImage];
    [self dismissViewControllerAnimated:YES completion:nil];
    [self setImageView];
}

- (void)setImageView {
    NSLog(@"setImageView called");
    self.uploadedImageView.image = self.uploadedImage;
}

@end
