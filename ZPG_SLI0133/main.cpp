#include "Application.h"
#include <glm/glm.hpp>
#include <iostream> 
#include <vector> 

int main() {
    try {
        Application app(1024, 728, "OpenGL Scenes");
        app.start();
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}