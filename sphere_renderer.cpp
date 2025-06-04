#define UNICODE
#include <windows.h>
#include <cstdint>
#include <iostream>
#include <format>
#include <vector>

const int WIDTH = 800;
const int HEIGHT = 600;

void RenderSphere() {
    std::vector<uint32_t> O(3, 0); // origin of camera

    for (int x = -WIDTH/2; x < WIDTH/2; ++x) {
        for (int y = -HEIGHT/2; y < HEIGHT/2; y++) {
            // Calculate the distance from the camera to the pixel
            std::cout << std::format("Rendering pixel at ({}, {})\n", x, y);
            }
        }
    }







int main() {
    std::cout << "Sphere Renderer" << std::endl;
    //start sphere rendering
    RenderSphere();
    
    return 0;
}