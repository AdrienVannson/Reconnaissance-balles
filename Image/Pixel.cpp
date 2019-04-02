#include "Pixel.hpp"

const Pixel Pixel::ROUGE = Pixel(255, 0, 0);
const Pixel Pixel::VERT = Pixel(0, 255, 0);
const Pixel Pixel::BLEU = Pixel(0, 0, 255);
const Pixel Pixel::NOIR = Pixel(0, 0, 0);
const Pixel Pixel::BLANC = Pixel(255, 255, 255);


Pixel::Pixel (const int rouge, const int vert, const int bleu)
{
    m_couleurs[I_ROUGE] = rouge;
    m_couleurs[I_VERT] = vert;
    m_couleurs[I_BLEU] = bleu;
}
