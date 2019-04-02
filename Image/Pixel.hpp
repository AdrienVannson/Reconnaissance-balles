#ifndef PIXEL_HPP
#define PIXEL_HPP

#include <array>


class Pixel
{
public:
    static const int NB_COULEURS = 3;

    static const int I_ROUGE = 0;
    static const int I_VERT = 1;
    static const int I_BLEU = 2;

    static const Pixel ROUGE;
    static const Pixel VERT;
    static const Pixel BLEU;
    static const Pixel NOIR;
    static const Pixel BLANC;

    Pixel (const int rouge=0, const int vert=0, const int bleu=0);


    inline std::array<int, Pixel::NB_COULEURS>& couleurs ()
    {
        return m_couleurs;
    }

    inline const std::array<int, Pixel::NB_COULEURS>& couleurs () const
    {
        return m_couleurs;
    }


    inline int operator[] (const int iCouleur) const
    {
        return m_couleurs[iCouleur];
    }

    inline int& operator[] (const int iCouleur)
    {
        return m_couleurs[iCouleur];
    }

private:
    std::array<int, NB_COULEURS> m_couleurs;
};

#endif // PIXEL_HPP
