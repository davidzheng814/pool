import cv2
import math
import numpy as np

def show(image, lines=None, scale=1.0, useblank=False):
    cv2.namedWindow('image', cv2.WINDOW_NORMAL)

    if useblank:
        image = 255*np.ones(image.shape)

    if lines is not None:
        for x1, y1, x2, y2 in lines:
            cv2.line(image,(x1,y1),(x2,y2),0,1)
    
    image = np.uint8(image)
    image = cv2.resize(image, (0, 0), fx=scale, fy=scale)
    cv2.imshow('image',image)
    cv2.waitKey(0)

def load_image(image_path):
    #Load image
    img = cv2.imread(image_path, cv2.IMREAD_COLOR)

    return img

def convert_to_endpoints(lines, shape):
    newlines = []
    for rho,theta in lines:
        a = np.cos(theta)
        b = np.sin(theta)
        x0 = a*rho
        y0 = b*rho
        x1 = int(x0 + shape[1]*(-b))
        y1 = int(y0 + shape[0]*(a))
        x2 = int(x0 - shape[1]*(-b))
        y2 = int(y0 - shape[0]*(a))
        newlines.append((x1, y1, x2, y2))

    return newlines

def find_table(img):
    width, height = (img.shape[1], img.shape[0])
    img = cv2.blur(img, (3,3))

    tbl_color = np.asarray([153, 131, 102])
    threshold = 65

    color_dist = np.sum((img - tbl_color)**2, axis=2)
    tbl_mask = 255*(color_dist < threshold**2)
    tbl_mask = np.uint8(tbl_mask)

    show(tbl_mask, scale=0.5)
    edges = cv2.Canny(tbl_mask, 100, 200)
    lines = cv2.HoughLinesP(edges, 2, 3*math.pi/180, 100)[0]
    show(edges, scale=0.5)
    show(image, scale=0.5, lines=lines, useblank=True)

image = load_image('images/pool.jpg')
table = find_table(image)

# show(image, scale=0.5)

