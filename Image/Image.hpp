#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <string>

#include <opencv2/opencv.hpp>

#include "Pixel.hpp"


class Image
{
public:
    Image (const int nbLignes, const int nbColonnes);
    Image (const cv::Mat &source=cv::Mat());

    Image clone () const;

    void afficher (const std::string &titre);
    static void attendreFenetres ();

    void enregistrer (const std::string nomFichier) const;

    void normaliser ();


    cv::Mat& image ()
    {
        return m_image;
    }

    const cv::Mat& image () const
    {
        return m_image;
    }


    inline int nbLignes () const
    {
        return m_image.rows;
    }

    inline int nbColonnes () const
    {
        return m_image.cols;
    }


    inline Pixel pixel (const int iLigne, const int iColonne) const
    {
        const cv::Vec3b &pixel = m_image.at<cv::Vec3b>(cv::Point(iColonne, iLigne));
        return Pixel (pixel[2], pixel[1], pixel[0]);
    }

    inline void setPixel (const int iLigne, const int iColonne, const Pixel nouveauPixel)
    {
        cv::Vec3b &pixel = m_image.at<cv::Vec3b>(cv::Point(iColonne, iLigne));

        pixel[2] = nouveauPixel.couleurs()[Pixel::I_ROUGE];
        pixel[1] = nouveauPixel.couleurs()[Pixel::I_VERT];
        pixel[0] = nouveauPixel.couleurs()[Pixel::I_BLEU];
    }


private:
    cv::Mat m_image;

};

#endif // IMAGE_HPP
