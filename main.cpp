#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <cmath>
#include <cstring>

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


double getNorme (const int deltaLignes, const int deltaColonnes)
{
    return sqrt(deltaLignes*deltaLignes + deltaColonnes*deltaColonnes);
}


vector<pair<int, int>> getPointsZone (Image &sontBalles, const pair<int, int> posDepart)
{
    vector<pair<int, int>> points;

    queue<pair<int, int>> aVoir;
    aVoir.push(posDepart);

    while (aVoir.size()) {
        const int iLigne = aVoir.front().first;
        const int iColonne = aVoir.front().second;
        aVoir.pop();

        if (sontBalles.pixel(iLigne, iColonne)[0] == 0) {
            continue;
        }

        sontBalles.setPixel(iLigne, iColonne, Pixel::NOIR);
        points.push_back(make_pair(iLigne, iColonne));

        for (int iDirection=0; iDirection<NB_DIRECTIONS; iDirection++) {
            const int iNouvelleLigne = iLigne + DELTAS_DIRECTIONS[iDirection][0];
            const int iNouvelleColonne = iColonne + DELTAS_DIRECTIONS[iDirection][1];

            if (iNouvelleLigne < 0 || iNouvelleLigne >= sontBalles.nbLignes()
             || iNouvelleColonne < 0 || iNouvelleColonne >= sontBalles.nbColonnes()) {
                continue;
            }

            aVoir.push(make_pair(iNouvelleLigne, iNouvelleColonne));
        }
    }

    return points;
}


void reconnaissanceBalles (Image &imgDepart)
{
    Image image = imgDepart.clone();
    image.normaliser();

    Image sontBalles (image.nbLignes(), image.nbColonnes());

    for (int iLigne=0; iLigne<image.nbLignes(); iLigne++) {
        for (int iColonne=0; iColonne<image.nbColonnes(); iColonne++) {
            const Pixel pixel = image.pixel(iLigne, iColonne);

            if (pixel[2] <= 30) { // Peu de bleu
                if (abs(pixel[0] - pixel[1]) <= 70) { // Autant de rouge que de vert
                    sontBalles.setPixel(iLigne, iColonne, Pixel::BLANC);
                }
            }
        }
    }

    // Erosion et dilatation
    const int tailleErosion = 2;
    const int tailleDilatation = 3;

    cv::erode(sontBalles.image(),
              sontBalles.image(),
              cv::getStructuringElement(cv::MORPH_ELLIPSE,
                                        cv::Size(2*tailleErosion + 1, 2*tailleErosion + 1),
                                        cv::Point(tailleErosion, tailleErosion))
    );

    cv::dilate(sontBalles.image(),
              sontBalles.image(),
              cv::getStructuringElement(cv::MORPH_ELLIPSE,
                                        cv::Size(2*tailleDilatation + 1, 2*tailleDilatation + 1),
                                        cv::Point(tailleDilatation, tailleDilatation))
    );

    //sontBalles.afficher("Est balle");

    int iCouleur = 0;

    for (int iLigne=0; iLigne<sontBalles.nbLignes(); iLigne++) {
        for (int iColonne=0; iColonne<sontBalles.nbColonnes(); iColonne++) {
            if (sontBalles.pixel(iLigne, iColonne)[0]) {
                const vector<pair<int, int>> points = getPointsZone(sontBalles, make_pair(iLigne, iColonne));

                if (points.size() >= 200) {

                    long long iLigneTotal = 0;
                    long long iColonneTotal = 0;

                    for (const pair<int, int> point : points) {
                        iLigneTotal += point.first;
                        iColonneTotal += point.second;
                    }

                    const int iLigneCentre = iLigneTotal / (int)points.size();
                    const int iColonneCentre = iColonneTotal / (int)points.size();

                    // Calcul du rayon
                    int rayon = 0;
                    for (const pair<int, int> point : points) {
                        const int dist = getNorme(point.first - iLigneCentre, point.second - iColonneCentre);
                        rayon = max(dist, rayon);
                    }

                    //cv::circle(imgDepart.image(), cv::Point(iColonneCentre, iLigneCentre), rayon, COULEURS[iCouleur], 5);
                    cv::circle(imgDepart.image(), cv::Point(iColonneCentre, iLigneCentre), rayon, cv::Scalar(0, 0, 255), 5);

                    iCouleur = (iCouleur+1) % NB_COULEURS;
                }
            }
        }
    }
}

int main (int argc, char* argv[])
{
    if (argc <= 2) {
        cerr << "Error: too few arguments" << endl;
        return 0;
    }

    vector<string> fichiers;
    for (int iFichier=2; iFichier<argc; iFichier++) {
        fichiers.push_back(argv[iFichier]);
    }

    if (strcmp(argv[1], "train") == 0) {
        if (argc != 3) {
            cerr << "Error: a single file is required for training" << endl;
            return 0;
        }

        cv::Mat cvImage = cv::imread(string(argv[2]));

        Image image (cvImage);
        image.normaliser();

        vector<int> rouges, verts, bleus;

        for (int iLigne=0; iLigne<image.nbLignes(); iLigne++) {
            for (int iColonne=0; iColonne<image.nbColonnes(); iColonne++) {
                const Pixel pixel = image.pixel(iLigne, iColonne);

                if (pixel[0] == 0 && pixel[1] == 0 && pixel[2] == 0) { // Ignorer les pixels noirs
                    continue;
                }

                rouges.push_back(pixel[Pixel::I_ROUGE]);
                verts.push_back(pixel[Pixel::I_VERT]);
                bleus.push_back(pixel[Pixel::I_BLEU]);
            }
        }

        sort(rouges.begin(), rouges.end());
        sort(verts.begin(), verts.end());
        sort(bleus.begin(), bleus.end());

        cout << "Seuils :\n";

        cout << "Rouge : " << rouges[ (int)(0.05*rouges.size()) ] << " " << rouges[ (int)(0.95*rouges.size()) ] << "\n";
        cout << "Vert : " << verts[ (int)(0.05*verts.size()) ] << " " << verts[ (int)(0.95*verts.size()) ] << "\n";
        cout << "Bleu : " << bleus[ (int)(0.05*bleus.size()) ] << " " << bleus[ (int)(0.95*bleus.size()) ] << "\n";
    }
    else if (strcmp(argv[1], "detect") == 0) {

        for (const string &nomFichier : fichiers) {
            cerr << "Traitement: " << nomFichier << endl;

            string nomFichierSortie;
            for (int iCaractere=(int)nomFichier.size()-1; iCaractere>=0; iCaractere--) {
                if (nomFichier[iCaractere] == '/') {
                    break;
                }
                nomFichierSortie = nomFichier[iCaractere] + nomFichierSortie;
            }

            cv::Mat cvImage = cv::imread(nomFichier);
            cv::resize(cvImage, cvImage, cv::Size(PICTURE_HEIGHT * cvImage.cols / cvImage.rows, PICTURE_HEIGHT));

            Image image (cvImage);

            reconnaissanceBalles(image);
            //image.afficher("Image ("+nomFichier+")");

            image.enregistrer("outputs/"+nomFichierSortie);
        }

        //Image::attendreFenetres();

    }
    else {
        cerr << "Error: wrong command name" << endl;
        return 0;
    }
}
