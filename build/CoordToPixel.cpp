#include "CoordToPixel.h"
#include "utils/ZBuffer.h"

img::EasyImage coordToPixel(Lines2D &lines, double size, img::Color backgroundColor, bool zBuffer) {
    std::list<Point2D> points;
    for (auto &i: lines) {
        points.push_back(i.p1);
        points.push_back(i.p2);
    }
    std::list<double> xPoints;
    std::list<double> yPoints;
    for (auto &i: points) {
        xPoints.push_back(i.x);
        yPoints.push_back(i.y);
    }

    double xMax = *std::max_element(xPoints.begin(), xPoints.end());
    double xMin = *std::min_element(xPoints.begin(), xPoints.end());
    double yMax = *std::max_element(yPoints.begin(), yPoints.end());
    double yMin = *std::min_element(yPoints.begin(), yPoints.end());

    double xRange = xMax - xMin;
    double yRange = yMax - yMin;

    double imageX = size * (xRange / std::max(xRange, yRange));
    double imageY = size * (yRange / std::max(xRange, yRange));

    double d = 0.95 * (imageX / xRange);

    double DCx = d * ((xMax + xMin)/2.0);
    double DCy = d * ((yMax + yMin)/2.0);
    double dx = (imageX/2.0) - DCx;
    double dy = (imageY/2.0) - DCy;

    img::EasyImage image(lround(imageX), lround(imageY), backgroundColor);

    ZBuffer buffer(image.get_width(), image.get_height());

    for (auto &i : lines) {
        i.p1.x = i.p1.x * d + dx;
        i.p1.y = i.p1.y * d + dy;
        i.p2.x = i.p2.x * d + dx;
        i.p2.y = i.p2.y * d + dy;

        if (zBuffer) {
            image.draw_zbuf_line(buffer, lround(i.p1.x), lround(i.p1.y), i.z1, lround(i.p2.x), lround(i.p2.y), i.z2, i.color);
        } else {
            image.draw_line(lround(i.p1.x), lround(i.p1.y), lround(i.p2.x), lround(i.p2.y), i.color);
        }
    }
    return image;
}
