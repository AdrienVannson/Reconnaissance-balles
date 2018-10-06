#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <cmath>

#include "Image/Image.hpp"

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


void reconnaissanceBalles (Image &imgDepart)
{
    Image image = imgDepart.clone();
    image.normaliser();

    image.afficher("Normalisé");

    Image sontBalles (image.nbLignes(), image.nbColonnes());

    for (int iLigne=0; iLigne<image.nbLignes(); iLigne++) {
        for (int iColonne=0; iColonne<image.nbColonnes(); iColonne++) {
            const Pixel pixel = image.pixel(iLigne, iColonne);

            if (pixel[0] >= 62 && pixel[0] <= 91
             && pixel[1] >= 98 && pixel[1] <= 134
             && pixel[2] >= 50 && pixel[2] <= 80) {
                sontBalles.setPixel(iLigne, iColonne, Pixel::BLANC);
            }
        }
    }

    sontBalles.afficher("Est balle");

    // Nettoyage (inutile ?)
    /*const cv::Mat avantNettoyage = sontBalles.clone();

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
    }*/

    //cv::imshow("est balle", sontBalles);

    /*int iCouleur = 0;

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
    }*/
}

int main ()
{
    std::vector<std::string> filenames = {
        "pictures/010.jpg"
    };

    for (auto filename : filenames) {
        cv::Mat cvImage = cv::imread(filename);
        cv::resize(cvImage, cvImage, cv::Size(PICTURE_HEIGHT * cvImage.cols / cvImage.rows, PICTURE_HEIGHT));

        Image image (cvImage);

        reconnaissanceBalles(image);
        image.afficher("Image ("+filename+")");
    }

    Image::attendreFenetres();

    /*cv::VideoCapture cap (0);

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
    }*/
}
