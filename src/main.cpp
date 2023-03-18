#include <SFML/Graphics.hpp>
#include <complex>
#include <cmath>
#include <iostream>
#include <thread>

struct Color {
    unsigned char r,g,b,a;
};

sf::Color hsvToRgb(double h, double s, double v) {
    // Make sure hue is in [0, 360[
    while (h >= 360.) h -= 360.;
    while (h < 0.) h += 360.;

    double c = v * s;
    double x = c * (1 - std::abs(std::fmod(h / 60., 2.) - 1));
    double m = v - c;

    double r, g, b;
    if (h < 60.) {
        r = c; g = x; b = 0;
    } else if (h < 120.) {
        r = x; g = c; b = 0;
    } else if (h < 180.) {
        r = 0; g = c; b = x;
    } else if (h < 240.) {
        r = 0; g = x; b = c;
    } else if (h < 300.) {
        r = x; g = 0; b = c;
    } else {
        r = c; g = 0; b = x;
    }

    return sf::Color((r + m) * 255, (g + m) * 255, (b + m) * 255);
}


sf::Color isMandelbrot(std::complex<double> c) {
    const size_t power = 2;
    const size_t max_iterations = 500;
    std::complex<double> z = {0., 0.};
    size_t iteration_count = 0;

    for (size_t i = 0; i < max_iterations; ++i) {
        z = pow(z, power) + c;

        if (pow(z.real(), 2) + pow(z.imag(), 2) > 4) {
            iteration_count = i;
            break;
        }
    }

    double value = (double)iteration_count / max_iterations;
    return hsvToRgb(240./360., 100., value);
}


void draw(unsigned char* pixels, size_t dim, size_t start, size_t end) {

    for (size_t i = start * dim; i < end * dim; ++i) {

        size_t xCoord = (i % dim), yCoord = (i / dim);
        std::complex<double> c = {xCoord * 4. / dim - 2, yCoord * 4. / dim - 2};
        sf::Color color = {255, 255, 255, 255};

        color = isMandelbrot(c);
        
        size_t j = i * 4;
        pixels[j] = color.r;
        pixels[j + 1] = color.g;
        pixels[j + 2] = color.b;
        pixels[j + 3] = color.a;
    }

}

void display_loading_bar(int& time_loading, std::string& sep) {
    std::cout << "\033[0G"; // Put the cursor at the begin of the line
    std::cout << sep;
    for (size_t i = 0 ; i < sep.length() / 4; ++i) {
        sep.replace(time_loading + 1, 1, "#"); // Replace the next space by a #
        time_loading++;
    }

    std::cout.flush(); // Clean the line
}

int main(int argc, char** argv) {
    const size_t dimension = 800;
    const size_t size = dimension * dimension;
    std::string separator = "[                                    ]";
    int time_loading = 0;

    unsigned char* pixels = new unsigned char[size * 4];
    
    std::thread t1(draw, pixels, dimension, 0, dimension/8),
                t2(draw, pixels, dimension, dimension/8, dimension/4),
                t3(draw, pixels, dimension, dimension/4, dimension/2),
                t4(draw, pixels, dimension, dimension/2, dimension);
    
    std::cout << "Creation of the fractal. . ." << std::endl;

    t1.join();
    display_loading_bar(time_loading, separator);
    t2.join();
    display_loading_bar(time_loading, separator);
    t3.join();
    display_loading_bar(time_loading, separator);
    t4.join();
    display_loading_bar(time_loading, separator);

    sf::Image image;
    image.create(dimension, dimension, pixels);

    image.saveToFile("Fractal.png");
    display_loading_bar(time_loading, separator);

    std::cout << std::endl << "Image saved !" << std::endl;

    sf::Texture textureFractal;
    textureFractal.loadFromImage(image);

    sf::Sprite spriteFractal;
    spriteFractal.setTexture(textureFractal, true);

        // create the window
    sf::RenderWindow window(sf::VideoMode(dimension, dimension), "Fractal");

    // run the program as long as the window is open
    while (window.isOpen()) {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // clear the window with black color
        window.clear(sf::Color::Black);

        window.draw(spriteFractal);

        // end the current frame
        window.display();
    }

    delete[] pixels;

    return 0;
}