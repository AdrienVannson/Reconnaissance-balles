#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <cmath>

#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

using namespace std;

const int PICTURE_HEIGHT = 420;

const int NB_COULEURS = 3;
const cv::Vec3b COULEURS[NB_COULEURS] = {
    cv::Vec3b(255, 0, 0),
    cv::Vec3b(0, 255, 0),
    cv::Vec3b(0, 0, 255)
};

const int NB_DIRECTIONS = 4;
const int DELTAS_DIRECTIONS[NB_DIRECTIONS][2] = {
    {-1, 0},
    {1, 0},
    {0, -1},
    {0, 1}
};


std::vector<cv::Point> getPointsZone (cv::Mat &sontBalles, const cv::Point posDepart)
{
    std::vector<cv::Point> points;

    std::queue<cv::Point> aVoir;
    aVoir.push(posDepart);

    while (aVoir.size()) {
        cv::Point pos = aVoir.front();
        aVoir.pop();

        if (sontBalles.at<uchar>(pos) != 255) {
            continue;
        }

        sontBalles.at<uchar>(pos) = 0;
        points.push_back(pos);

        for (int iDirection=0; iDirection<NB_DIRECTIONS; iDirection++) {
            cv::Point nouvellePos (pos.x + DELTAS_DIRECTIONS[iDirection][0],
                                   pos.y + DELTAS_DIRECTIONS[iDirection][1]);

            if (nouvellePos.x < 0 || nouvellePos.x >= sontBalles.cols
             || nouvellePos.y < 0 || nouvellePos.y >= sontBalles.rows) {
                continue;
            }

            aVoir.push(nouvellePos);
        }
    }

    return points;
}

void reconnaissanceBalles (cv::Mat &imgDepart)
{
    cv::Mat img = imgDepart.clone();

    // Normalisation
    for (int iLigne=0; iLigne<img.rows; iLigne++) {
        for (int iColonne=0; iColonne<img.cols; iColonne++) {
            cv::Vec3b &couleur = img.at<cv::Vec3b>(cv::Point(iColonne, iLigne));

            int somme = 0;
            for (int iChannel=0; iChannel<3; iChannel++) {
                somme += couleur[iChannel];
            }

            if (somme < 20) {
                couleur = cv::Vec3b(0, 0, 0);
            }
            else {
                for (int iChannel=0; iChannel<3; iChannel++) {
                    couleur[iChannel] = 255 * couleur[iChannel] / somme;
                }
            }
        }
    }

    cv::Mat sontBalles;
    cv::inRange(img, cv::Scalar(0, 100, 100), cv::Scalar(40, 150, 150), sontBalles);

    int iCouleur = 0;

    // Flood-fill pour détecter les balles
    for (int x=0; x<sontBalles.cols; x++) {
        for (int y=0; y<sontBalles.rows; y++) {
            uchar &color = sontBalles.at<uchar>(cv::Point(x, y));
            const bool estBalle = color == 255;

            if (estBalle) {
                const auto points = getPointsZone(sontBalles, cv::Point(x, y));

                if (points.size() > 1000) {

                    // TODO: placer le centre afin de minimiser la distance max
                    long long xTotal = 0;
                    long long yTotal = 0;

                    for (const auto point : points) {
                        //img.at<cv::Vec3b>(point) = COULEURS[iCouleur];

                        xTotal += point.x;
                        yTotal += point.y;
                    }

                    const int xCentre = xTotal / (int)points.size();
                    const int yCentre = yTotal / (int)points.size();

                    // Calcul du rayon
                    int rayon = 0;
                    for (const auto point : points) {
                        const int dist = sqrt( (point.x-xCentre)*(point.x-xCentre) + (point.y-yCentre)*(point.y-yCentre) );
                        rayon = max(dist, rayon);
                    }

                    cv::circle(imgDepart, cv::Point(xCentre, yCentre), rayon, COULEURS[iCouleur], 5);

                    iCouleur = (iCouleur+1) % NB_COULEURS;
                }
            }
        }
    }
}

int main ()
{
    std::vector<std::string> filenames = {
        "images/000.jpg",
        "images/001.jpg",
        "images/002.jpg",
        "images/003.jpg",
        "images/004.png",
    };

    for (auto filename : filenames) {
        cv::Mat img = cv::imread(filename);

        cv::resize(img, img, cv::Size(PICTURE_HEIGHT * img.cols / img.rows, PICTURE_HEIGHT));

        //cv::imshow("Avant ("+filename+")", img);
        reconnaissanceBalles(img);
        cv::imshow("Apres ("+filename+")", img);
    }

    cv::waitKey(0);

    return 0;
}
