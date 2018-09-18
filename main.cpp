#include <iostream>
#include <vector>
#include <string>

#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

const int PICTURE_HEIGHT = 420;

void colorRecognition (cv::Mat &img)
{
    // Normalisation
    for (int iLigne=0; iLigne<img.rows; iLigne++) {
        for (int iColonne=0; iColonne<img.cols; iColonne++) {
            cv::Vec3b &color = img.at<cv::Vec3b>(cv::Point(iColonne, iLigne));

            int somme = 0;
            for (int iChannel=0; iChannel<3; iChannel++) {
                somme += color[iChannel];
            }

            if (somme < 20) {
                color = cv::Vec3b(0, 0, 0);
            }
            else {
                for (int iChannel=0; iChannel<3; iChannel++) {
                    color[iChannel] = 255 * color[iChannel] / somme;
                }
            }
        }
    }

    cv::inRange(img, cv::Scalar(0, 100, 100), cv::Scalar(40, 150, 150), img);
}

int main ()
{
    std::vector<std::string> filenames = {
        "images/000.jpg",
        "images/001.jpg",
        "images/002.jpg",
        "images/003.jpg"
    };

    for (auto filename : filenames) {
        cv::Mat img = cv::imread(filename);

        cv::resize(img, img, cv::Size(PICTURE_HEIGHT * img.cols / img.rows, PICTURE_HEIGHT));

        cv::imshow("Avant ("+filename+")", img);
        colorRecognition(img);
        cv::imshow("Apres ("+filename+")", img);
    }

    cv::waitKey(0);

    return 0;
}
