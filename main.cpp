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

const int NB_DIRECTIONS = 8;
const int DELTAS_DIRECTIONS[NB_DIRECTIONS][2] = {
    {-1, -1},
    {-1, 0},
    {-1, 1},

    {0, -1},
    {0, 1},

    {1, -1},
    {1, 0},
    {1, 1}
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
    /*for (int iLigne=0; iLigne<img.rows; iLigne++) {
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
    }*/

    //imgDepart = img.clone();

    cv::Mat sontBalles (imgDepart.rows, imgDepart.cols, 0);

    //cv::inRange(img, cv::Scalar(0, 100, 100), cv::Scalar(40, 150, 150), sontBalles);
    //cv::inRange(img, cv::Scalar(40, 80, 80), cv::Scalar(80, 120, 120), sontBalles);

    // Détection des balles
    for (int x=0; x<sontBalles.cols; x++) {
        for (int y=0; y<sontBalles.rows; y++) {
            cv::Vec3b &couleur = img.at<cv::Vec3b>(cv::Point(x, y));

            bool estBalle = true;

            if (std::abs(couleur[1]-couleur[2]) > 30) {
                estBalle = false;
            }

            if (couleur[0] > std::min(couleur[1], couleur[2])) {
                estBalle = false;
            }

            sontBalles.at<uchar>(cv::Point(x, y)) = 255 * estBalle;
        }
    }

    // Nettoyage (inutile ?)
    const cv::Mat avantNettoyage = sontBalles.clone();

    for (int iIteration=0; iIteration<3; iIteration++) {
        for (int x=1; x<sontBalles.cols-1; x++) {
            for (int y=1; y<sontBalles.rows-1; y++) {

                int nbVoisinsBalles = 0;

                for (int xVoisin=x-1; xVoisin<=x+1; xVoisin++) {
                    for (int yVoisin=y-1; yVoisin<=y+1; yVoisin++) {
                        if (xVoisin != x || yVoisin != y) {
                            if (avantNettoyage.at<uchar>(cv::Point(xVoisin, yVoisin))) {
                                nbVoisinsBalles++;
                            }
                        }
                    }
                }

                if (nbVoisinsBalles <= 5) {
                    sontBalles.at<uchar>(cv::Point(x, y)) = 0;
                }
                else if (nbVoisinsBalles >= 8) {
                    sontBalles.at<uchar>(cv::Point(x, y)) = 255;
                }
            }
        }
    }

    cv::imshow("est balle", sontBalles);

    int iCouleur = 0;

    for (int x=0; x<sontBalles.cols; x++) {
        for (int y=0; y<sontBalles.rows; y++) {
            uchar &color = sontBalles.at<uchar>(cv::Point(x, y));
            const bool estBalle = color == 255;

            if (estBalle) {
                const auto points = getPointsZone(sontBalles, cv::Point(x, y));

                if (points.size() > 1000 && points.size() < 10000) {

                    long long xTotal = 0;
                    long long yTotal = 0;

                    for (const auto point : points) {
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

                    if (rayon > 40) { // L'objet détecté n'est pas une balle
                        continue;
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
    /*std::vector<std::string> filenames = {
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

    cv::waitKey(0);*/

    cv::VideoCapture cap (0);

    if (!cap.isOpened()) {
        std::cerr << "Erreur webcam" << std::endl;
        return -1;
    }

    cv::namedWindow("webcam", 1);

    while (true) {
        cv::Mat img;
        cap >> img;

        reconnaissanceBalles(img);

        cv::imshow("webcam", img);

        if (cv::waitKey(1) != 255) {
            return 0;
        }
    }
}
