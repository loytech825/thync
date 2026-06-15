#pragma once
#include <iostream>
#include <array>
/*******************************************************************/
/*Attempt at a C++ port of https://github.com/jake-stewart/color256*/
/*******************************************************************/

struct ColorConfig;

//general class for color variables
// RGB, LAB, HSV...
struct Color{
    double x, y, z;  
};

typedef std::array<Color, 256> Palette;


std::ostream& operator<<(std::ostream& os, const Color& color);

/*
    @brief Turns a hex string into an rgb color
    @param hex Code of format RRGGBB
*/
Color hex2rgb(const std::string& hex);

/*
    @brief Turns a RGB color into a hex string
    @param RGB a color object
    @return string of format RRGGBB
*/
std::string rgb2hex(const Color& RGB);

/*
    @brief Converts RGB to LAB
    @param RGB 0-255 range RGB color
    @return the color in LAB
*/
Color rgb2lab(Color RGB);

/*
    @brief Converts LAB to RGB
    @param LAB color in LAB
    @return the color in RGB 0-255
*/
Color lab2rgb(Color LAB);

/*
    @brief Generates a full 256 color pallete
*/
Palette generate_256(std::array<Color, 16> base8, Color bg, Color fg);

/*
    @brief showcases the 256 colors
*/
void print_256(const std::array<Color, 256> palette);

/*
 * @brief Prints a color config
 */
void print_colors(const ColorConfig& colors);
