#include "TextureLoader.h"
#include <iostream>
#include "stb_image.h" // Zde je stb_image potøeba

GLuint TextureLoader::LoadTexture(const std::string& path, bool flip) {
    // Použijeme kód z vašeho zadání
    stbi_set_flip_vertically_on_load(flip);

    int width, height, channels;
    // Naèteme obrázek
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

    if (!data) {
        std::cerr << "Chyba nacitani textury: " << path << std::endl;
        stbi_image_free(data);
        return 0;
    }

    // Zjistíme formát (RGB nebo RGBA)
    GLenum format;
    if (channels == 1)
        format = GL_RED;
    else if (channels == 3)
        format = GL_RGB;
    else if (channels == 4)
        format = GL_RGBA;
    else {
        std::cerr << "Neznamy format textury: " << path << " s " << channels << " kanaly." << std::endl;
        stbi_image_free(data);
        return 0;
    }

    // Vytvoøíme OpenGL texturu
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Nahrajeme data do GPU
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Nastavíme parametry textury (z vašeho zadání)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data); // Uvolníme data z CPU pamìti
    glBindTexture(GL_TEXTURE_2D, 0);

    std::cout << "Textura nahrana: " << path << " (ID: " << textureID << ")" << std::endl;
    return textureID;
}