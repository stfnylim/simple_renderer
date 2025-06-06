#define UNICODE
#include <windows.h>
#include <cstdint>
#include <iostream>
#include <format>
#include <vector>
#include <climits>
#include <optional>
#include <array>
#include <cstdint>
#include <cmath>

extern const int CWIDTH = 1920;
extern const int CHEIGHT = 1080;
extern uint32_t* framebuffer;
const int VWIDTH = 1;
const int VHEIGHT = 1;
const std::vector<int> BACKGROUND_COLOR = {0, 0, 0}; // Background color in RGB format

std::vector<int> trace_ray(std::vector<int> O, std::vector<int> D, int t_min, int t_max);

class Sphere {
public:
    Sphere(int x, int y, int z, int radius, std::vector<int> rgb) : center{x, y, z}, radius(radius), rgb(rgb) {}

    std::vector<int> center; // Center of the sphere
    int radius; // Radius of the sphere
    std::vector<int> rgb; // Color of the sphere in RGB format
};
std::vector<int> intersect_ray_sphere(std::vector<int> O, std::vector<int> D, Sphere sphere);


// Declaration moved after the Sphere class definition
std::array<Sphere, 2> SCENE_SPHERES = {
    Sphere(0, 0, 10, 1, {255, 0, 0}), // Example sphere at origin with radius 100
    Sphere(0, 4, 1, 1, {0, 255, 0}) // Another sphere at (200, 100, -50) with radius 50
};

std::vector<int> canvas_to_viewport(int x, int y) {
    // Convert canvas coordinates to viewport coordinates
    std::vector<int> D(3);
    D[0] = x * VWIDTH / CWIDTH ; // Adjust x to be positive
    D[1] = y * VHEIGHT / CHEIGHT; // Adjust y to be positive
    D[2] = 1; // Z coordinate, can be set to any value as needed

    return D;
}

void render_sphere() {
    std::vector<int> O(3, 0); // origin of camera

    for (int x = 0; x < CWIDTH; ++x) {
        for (int y = 0; y < CHEIGHT; y++) {
            // Calculate the distance from the camera to the pixel
            int cx = x - CWIDTH / 2; // Center the image
            int cy = y - CHEIGHT / 2; // Center the image
            std::vector<int> D(3); // pixel position
            D = canvas_to_viewport(cx, cy);
            std::vector<int> color = trace_ray(O, D, 1, 1000); // Trace the ray from camera origin O in direction D

            // draw canvas pixel with the color returned by trace_ray
            uint8_t r = color[0]; // Red component
            uint8_t g = color[1]; // Green component
            uint8_t b = color[2]; // Blue component

            framebuffer[y * CWIDTH + x] = (255 << 24) | (r << 16) | (g << 8) | b;  // ARGB

            }
        }
    }

std::vector<int> trace_ray(std::vector<int> O, std::vector<int> D, int t_min, int t_max){
    int closest_t = INT_MAX;
    std::optional<Sphere> closest_sphere;
    for (int i=0;i<SCENE_SPHERES.size(); ++i) {
        Sphere sphere = SCENE_SPHERES[i];
        // Calculate intersection of ray with sphere
        std::vector<int> intersection = intersect_ray_sphere(O, D, sphere);

        if (intersection.size() >= 2) {
            int t1 = intersection[0];
            int t2 = intersection[1];
            if (t1 >= t_min && t1 <= t_max && t1 < closest_t) {
                closest_t = t1;
                closest_sphere = sphere;
            }
            if (t2 >= t_min && t2 <= t_max && t2 < closest_t) {
                closest_t = t2;
                closest_sphere = sphere;
            }
        }
    }
    if (closest_sphere.has_value()) {
        return closest_sphere->rgb; // Return the color of the closest sphere
    } else {
        // std::cout << "No sphere intersected by ray.\n";
        return BACKGROUND_COLOR;

    }
        // Here you would calculate the actual intersection and update closest_t and closest_sphere

}

std::vector<int> operator-(const std::vector<int>& a, const std::vector<int>& b) {
    // Overload the - operator to subtract two vectors
    return {a[0] - b[0], a[1] - b[1], a[2] - b[2]};
}

int dot_product(const std::vector<int>& a, const std::vector<int>& b) {
    // Calculate the dot product of two vectors
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

std::vector<int> intersect_ray_sphere(std::vector<int> O, std::vector<int> D, Sphere sphere) {
    // This function would calculate the intersection of a ray with a sphere
    // For now, we will just print the ray parameters

    // Here you would implement the actual intersection logic
    int r = sphere.radius;
    std::vector<int> CO = O - sphere.center;

    int a = dot_product(D, D);
    int b = 2 * dot_product(CO, D);
    int c = dot_product(CO, CO) - r * r;

    int discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return {}; // No intersection
    }
    // Calculate the two intersection points
    int t1 = (-b - static_cast<int>(sqrt(discriminant))) / (2 * a);
    int t2 = (-b + static_cast<int>(sqrt(discriminant))) / (2 * a);
    return {t1, t2}; // Return the two intersection points
    
}







// int main() {
//     std::cout << "Sphere Renderer" << std::endl;

//     // Allocate memory for the framebuffer
//     framebuffer = new uint32_t[CWIDTH * CHEIGHT];

//     // Start sphere rendering
//     render_sphere();

//     // Free allocated memory
//     delete[] framebuffer;

//     return 0;
// }