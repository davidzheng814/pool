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
        a = np.sin(theta)
        b = np.tan(theta)

        if a != 0:
            x1 = 0
            y1 = int(rho/a)
            x2 = int(shape[1])
            y2 = int(rho/a - shape[1]/b)
        else:
            x1 = rho
            y1 = 0
            x2 = rho
            y2 = shape[0]
        newlines.append((x1, y1, x2, y2))

    return newlines

def convert_center_to_endpoints(lines, shape):
    newlines = []
    for rho,theta in lines:
        if theta != 0:
            rho1 = (rho/math.sin(theta) + shape[0]/2. )/math.sin(theta)
        a = np.sin(theta)
        b = np.tan(theta)

        if a != 0:
            x1 = 0
            y1 = int(rho1/a)
            x2 = int(shape[1])
            y2 = int(rho1/a - shape[1]/b)
        else:
            x1 = 0
            y1 = int(rho + shape[0]/2)
            x2 = int(shape[1])
            y2 = int(rho + shape[0]/2)
        newlines.append((x1, y1, x2, y2))

    return newlines

def distance(line, point):
    rho, theta = line
    if theta==0:
        x1, y1 = (rho, 0)
        x2, y2 = (rho, 10)
    else:
        x1, y1 = (rho*np.cos(theta),rho*np.sin(theta))
        x2, y2 = (0, rho/np.sin(theta))
    x0, y0 = point
    return math.fabs((y2 - y1)*x0 - (x2 - x1)*y0 + x2*y1 - y2*x1)/math.sqrt((y2-y1)*(y2-y1) + (x2-x1)*(x2-x1))

def line_dist(line1, line2, shape):
    max_rho_dist = 80
    max_th_dist = 5*math.pi/180.
    rho1, th1 = line1
    rho2, th2 = line2

    if math.fabs(rho1-rho2) < max_rho_dist and math.fabs(th1 - th2) < max_th_dist:
        return True
    return False

def find_table(img):
    scale_to_width = 2000.
    img2 = img
    img = cv2.resize(img, (0,0), fx=scale_to_width/img.shape[1], fy=scale_to_width/img.shape[1])
    width, height = (img.shape[1], img.shape[0])

    # Blur Image
    # img = cv2.blur(img, (3,3))
    show(img, scale=0.25)
    # Blob Detection
    tbl_color = np.asarray([153, 131, 102])
    threshold = 75
    color_dist = np.sum((img - tbl_color)**2, axis=2)
    tbl_mask = 255*(color_dist < threshold**2)
    tbl_mask = np.uint8(tbl_mask)

    # img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    # Edge detection
    edges = cv2.Canny(tbl_mask, 50, 200, 3)

    # Finds lines using HoughLines detection
    rho_size = 0.5
    rho_stepsize = 0.5
    max_rho = 3
    center = (width/2., height/2.)
    while True:
        lines = cv2.HoughLines(edges, rho_size, math.pi/180, 100)[0]
        # show(img, scale=0.25, lines=convert_to_endpoints(lines, img.shape), useblank=True)
        for line in lines:
            line[0] = math.sin(line[1])*(line[0]*math.sin(line[1]) - height/2.)
            if line[1] > math.pi:
                line[0] = -line[0]
                line[1] = math.pi*2 - line[1]

        groups = []
        for line in lines:
            found_group = False
            for group in groups:
                if line_dist(line, group[0], img.shape):
                    group.append(line)
                    found_group = True
                    break

            if not found_group:
                groups.append([line])

        lines = []
        for group in groups:
            min_dist = None
            min_line = None
            for line in group:
                if not min_dist:
                    min_dist = distance(line, center)
                    min_line = line
                else:
                    dist = distance(line, center)
                    if dist < min_dist:
                        min_dist = dist
                        min_line = line
            lines.append(min_line)

        if len(lines) == 4:
            break

        rho_size += rho_stepsize
        if rho_size > max_rho:
            return None

    new_edges = cv2.Canny(img2, 50, 200, 3)
    show(new_edges, scale=0.25*0.25)
    circles = cv2.HoughCircles(new_edges, cv2.cv.CV_HOUGH_GRADIENT, dp=1, minDist=10)
    print len(circles)
    print len(lines)
    # Display Edge Image and Line Image
    lines_display = convert_center_to_endpoints(lines, img.shape)
    show(tbl_mask, scale=0.25)
    show(edges, scale=0.25)
    show(img, scale=0.25, lines=lines_display, useblank=True)

# 1, 2, 4, 10
# 5
image = load_image('images/sample4.jpg')
table = find_table(image)
