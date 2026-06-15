#include "color256.h"
#include "config.h"
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <string>
#include <map>

//reference white point D_65
Color D_65_2deg{0.9505, 1.00000, 1.089};

Color lerp(double t, Color C1, Color C2)
{
    Color OUT;

    OUT.x = C1.x + t * (C2.x - C1.x);
    OUT.y = C1.y + t * (C2.y - C1.y);
    OUT.z = C1.z + t * (C2.z - C1.z);

    return OUT;
}

double f(double t)
{
    if(t > 0.008856) return cbrt(t);

    return 7.787 * t + (16.0/116.0);
}

std::ostream &operator<<(std::ostream &os, const Color &color)
{
    return os << color.x << ", " << color.y << ", " << color.z;
}

Color hex2rgb(const std::string& hex)
{

    int R = std::stoi(hex.substr(0, 2), nullptr, 16);
    int G = std::stoi(hex.substr(2, 2), nullptr, 16);
    int B = std::stoi(hex.substr(4, 2), nullptr, 16);

    return Color(R, G, B);
}

std::string rgb2hex(const Color &RGB)
{
    std::stringstream stream;

    stream << std::setfill('0') << std::setw(2) << std::hex << (int)RGB.x;
    stream << std::setfill('0') << std::setw(2) << std::hex << (int)RGB.y;
    stream << std::setfill('0') << std::setw(2) << std::hex << (int)RGB.z;

    return stream.str();
}

Color rgb2lab(Color RGB)
{
    RGB.x /= 255.0;
    RGB.y /= 255.0;
    RGB.z /= 255.0;

    //SRGB to linear
    RGB.x = ( RGB.x > 0.04045 ) ? pow((RGB.x+0.055)/1.055, 2.4) : (RGB.x/12.92);
    RGB.y = ( RGB.y > 0.04045 ) ? pow((RGB.y+0.055)/1.055, 2.4) : (RGB.y/12.92);
    RGB.z = ( RGB.z > 0.04045 ) ? pow((RGB.z+0.055)/1.055, 2.4) : (RGB.z/12.92);


    Color XYZ;

    //Linear to XYZ

    //RGB.x *= 100;
    //RGB.y *= 100;
    //RGB.z *= 100;

    XYZ.x = (0.4124 * RGB.x + 0.3576 * RGB.y + 0.1805 * RGB.z);
    XYZ.y = (0.2126 * RGB.x + 0.7152 * RGB.y + 0.0722 * RGB.z);
    XYZ.z = (0.0193 * RGB.x + 0.1192 * RGB.y + 0.9505 * RGB.z);

    //Color D_65_2deg{0.95047, 1.00000, 1.08883};

    double x = XYZ.x / D_65_2deg.x;
    double y = XYZ.y / D_65_2deg.y;
    double z = XYZ.z / D_65_2deg.z;


    Color LAB;

    LAB.x = 116 * f(y) - 16;
    LAB.y = 500 * (f(x) - f(y));
    LAB.z = 200 * (f(y) - f(z));

    return LAB;

}

Color lab2rgb(Color LAB)
{
    Color Fxyz;

    Fxyz.y = (LAB.x + 16) / 116;
    Fxyz.x = (LAB.y / 500.0) + Fxyz.y;
    Fxyz.z = Fxyz.y - (LAB.z / 200.0);
    
    Color xyz;

    xyz.x = (pow(Fxyz.x, 3) > 0.008856) ? pow(Fxyz.x, 3) : (Fxyz.x - (16.0/116.0)) / 7.787;
    xyz.y = (pow(Fxyz.y, 3) > 0.008856) ? pow(Fxyz.y, 3) : (Fxyz.y - (16.0/116.0)) / 7.787;
    xyz.z = (pow(Fxyz.z, 3) > 0.008856) ? pow(Fxyz.z, 3) : (Fxyz.z - (16.0/116.0)) / 7.787;

    //here we get X, Y, Z

    Color XYZ;

    XYZ.x = xyz.x * D_65_2deg.x;
    XYZ.y = xyz.y * D_65_2deg.y;
    XYZ.z = xyz.z * D_65_2deg.z;

    Color RGB;

    RGB.x = XYZ.x * 3.2406 + XYZ.y * -1.5372 + XYZ.z * -0.4986;
    RGB.y = XYZ.x * -0.9689 + XYZ.y * 1.8758 + XYZ.z * 0.0415;
    RGB.z = XYZ.x * 0.0557 + XYZ.y * -0.2040 + XYZ.z * 1.0570;

    RGB.x = (RGB.x > 0.0031308) ? (1.055 * pow(RGB.x, 1.0/2.4) - 0.055) : 12.92 * RGB.x;
    RGB.y = (RGB.y > 0.0031308) ? (1.055 * pow(RGB.y, 1.0/2.4) - 0.055) : 12.92 * RGB.y;
    RGB.z = (RGB.z > 0.0031308) ? (1.055 * pow(RGB.z, 1.0/2.4) - 0.055) : 12.92 * RGB.z;

    RGB.x = std::round(std::clamp((RGB.x * 255), 0.0, 255.0));
    RGB.y = std::round(std::clamp((RGB.y * 255), 0.0, 255.0));
    RGB.z = std::round(std::clamp((RGB.z * 255), 0.0, 255.0));

    return RGB;
}

Palette generate_256(std::array<Color, 16> base16, Color bg, Color fg)
{
    std::array<Color, 8> base_8_lab;

    if(rgb2lab(bg).x > rgb2lab(fg).x)
    {
        Color t = bg;
        bg = fg;
        fg = t;
    }

    //convert first 8 colors to LAB
    for(int i = 0; i < 7; i++) { base_8_lab.at(i) = rgb2lab(base16.at(i)); }

    Color bg_lab = rgb2lab(bg);
    Color fg_lab = rgb2lab(fg);

    std::array<Color, 256> pallete;

    for(int i = 0; i < 16; i++) {pallete[i] = base16[i];}

    //what color we on
    int index = 16;


    //generate the 6*6*6 cube
    //by interpolating near colors
    for(int i = 0; i < 6; i++)
    {
        Color c0 = lerp(((float)i)/5.f, bg_lab, base_8_lab[1]);
        Color c1 = lerp(((float)i)/5.f, base_8_lab[2], base_8_lab[3]);
        Color c2 = lerp(((float)i)/5.f, base_8_lab[4], base_8_lab[5]);
        Color c3 = lerp(((float)i)/5.f, base_8_lab[6], fg_lab);

        for(int j = 0; j < 6; j++)
        {
            Color c4 = lerp(((float)j)/5.f, c0, c1);
            Color c5 = lerp(((float)j)/5.f, c2, c3);

            for(int k = 0; k < 6; k++)
            {
                Color c6 = lerp(((float)k)/5.f, c4, c5);
                pallete.at(index) = lab2rgb(c6);
                index++;
            }
        }
    }

    //generate grayscale colors
    for(int i = 0; i < 24; i++)
    {
        float t = ((float)(i+1)) / 25.f;
        Color lab = lerp(t, bg_lab, fg_lab);
        pallete.at(index) = lab2rgb(lab);
        index++;
    }

    return pallete;
}




void print_color(Color c, Color BG, Color FG)
{

    //if bag lighter than fg, we swap
    if(rgb2lab(BG).x > rgb2lab(FG).x)
    {
        Color t = FG;
        FG = BG;
        BG = t;
    }

    int R = c.x;
    int G = c.y;
    int B = c.z;

    R = std::clamp(R, 0, 255);
    G = std::clamp(G, 0, 255);
    B = std::clamp(B, 0, 255);

    int BG_R = BG.x;
    int BG_G = BG.y;
    int BG_B = BG.z;

    BG_R = std::clamp(BG_R, 0, 255);
    BG_G = std::clamp(BG_G, 0, 255);
    BG_B = std::clamp(BG_B, 0, 255);

    int FG_R = FG.x;
    int FG_G = FG.y;
    int FG_B = FG.z;

    FG_R = std::clamp(FG_R, 0, 255);
    FG_G = std::clamp(FG_G, 0, 255);
    FG_B = std::clamp(FG_B, 0, 255);

    if( rgb2lab(c).x > 50 ) std::cout << "\033[38;2;" << BG_R << ";" << BG_G << ";" << BG_B << "m";
    else                                 std::cout << "\033[38;2;" << FG_R << ";" << FG_G << ";" << FG_B << "m";

    std::cout << "\033[48;2;" << R << ";" << G << ";" << B << "m";
}

void print_256(const std::array<Color, 256> palette)
{

    std::cout << "Base 16 colors, of which first 8 are used for pallete generation:\n";

    for(int i = 0; i < 16; i++)
    {
        std::string id = std::to_string(i);
        //while(id.length() < 5) id = id+" ";
        while(id.length() < 3) id = " "+id;

        print_color(palette[i], palette[0], palette[7]);
        //std::cout << "R: " << R << "G: " << G << "B: " << B << "\n"; 
        std::cout << id << " " << rgb2hex(palette[i]);

        if(i == 7) std::cout << "\033[m\n";
    }

    std::cout << "\033[m\n";
    std::cout << "\nThe generated 216 colors: \n"; //<< "\033[m   ";


    for(int i = 0; i < 6; i++)
    {
        for(int j = 0; j < 7; j++)
        {
            //only draw first 6 lines
            if((j+1) % 7 != 0)
            {
                for(int k = 0; k < 6; k++)
                {
                    int index = 16 + (36 * i) + (6 * k) + j;
                    std::string id = std::to_string(index);
                    //while(id.length() < 5) id = id+" ";
                    while(id.length() < 3) id = " "+id;
                    while(id.length() < 5) id = id+" ";

                    print_color(palette[index], palette[0], palette[7]);
                    std::cout << id << " " << rgb2hex(palette[index]) << "\033[m  ";
                }
            }
            else
            {
                std::cout << "\t\t\t\t\t\t\t\t\t\t";
            }
            //grayscale at the end
            int idx = 7*i+j;
            if(idx < 25)
            {
                if(idx == 0) { std::cout << "\033[m\tGrayscale:\n"; continue; }
                idx+=231;
                print_color(palette[idx], palette[0], palette[7]);
                //std::cout << "R: " << R << "G: " << G << "B: " << B << "\n"; 
                std::cout << "\t" << idx << " " << " " << rgb2hex(palette[idx]) << "\033[m\n";
                //if(j==5) std::cout << "\033[m\n";
                continue;
            }
            std::cout << "\033[m\n";
        }
    }
}

void print_colors(const ColorConfig& colors)
{

    const std::map<std::string, std::string> kvp(colors.global.key_value_pairs.begin(), colors.global.key_value_pairs.end());

    Color fg = kvp.contains("background") ? hex2rgb(kvp.at("foreground")) : Color(255, 255, 255); 
    Color bg = kvp.contains("background") ? hex2rgb(kvp.at("background")) : Color(0, 0, 0); 

    for(const auto [k, v] : kvp) 
    {
        std::string key = k;
        print_color(hex2rgb(v), bg, fg);
        std::cout << key.append(20-key.length(), ' ');
        std::cout << "\033[m\n";
    }
}
