#pragma once
#include <string>
#include <GL/glew.h>

/**
 * @brief Naète 2D texturu ze souboru pomocí stb_image.
 */
class TextureLoader {
public:
    /**
     * @brief Naète texturu a vrátí její OpenGL ID.
     * @param path Cesta k souboru (napø. "../assets/wall.jpg")
     * @param flip True pro otoèení obrázku (stb_image to naèítá obrácenì)
     * @return OpenGL ID textury (GLuint). Vrací 0 pøi chybì.
     */
    static GLuint LoadTexture(const std::string& path, bool flip = true);
};