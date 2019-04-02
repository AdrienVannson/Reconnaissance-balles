#include "Image.hpp"

Image::Image (const int nbLignes, const int nbColonnes) :
    m_image (nbLignes, nbColonnes, CV_8UC3, cv::Scalar(0, 0, 0))
{}

Image::Image (const cv::Mat &source) :
    m_image (source)
{}

Image Image::clone () const
{
    return Image(m_image.clone());
}

void Image::afficher (const std::string &titre)
{
    cv::imshow(titre, m_image);
}

void Image::attendreFenetres ()
{
    cv::waitKey(0);
}

void Image::enregistrer (const std::string nomFichier) const
{
    cv::imwrite(nomFichier, m_image);
}

void Image::normaliser ()
{
    for (int iLigne=0; iLigne<nbLignes(); iLigne++) {
        for (int iColonne=0; iColonne<nbColonnes(); iColonne++) {
            Pixel pixel = this->pixel(iLigne, iColonne);

            int somme = 0;
            for (int iCouleur=0; iCouleur<Pixel::NB_COULEURS; iCouleur++) {
                somme += pixel[iCouleur];
            }

            if (somme < 20) {
                pixel = Pixel(0, 0, 0);
            }
            else {
                for (int iCouleur=0; iCouleur<Pixel::NB_COULEURS; iCouleur++) {
                    pixel[iCouleur] = 255 * pixel[iCouleur] / somme;
                }
            }

            setPixel(iLigne, iColonne, pixel);
        }
    }
}
