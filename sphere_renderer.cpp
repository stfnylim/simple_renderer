#define UNICODE
#include <windows.h>
#undef max
#undef min
#include <cstdint>
#include <iostream>
#include <format>
#include <vector>
#include <climits>
#include <optional>
#include <array>
#include <cstdint>
#include <cmath>
#include <sstream> // Required for std::stringstream
#include <algorithm> // Required for std::max and std::min
#include <variant>


using Vec3 = std::array<double, 3>; // 3D vector type
extern const int CWIDTH = 1920;
extern const int CHEIGHT = 1080;
extern uint32_t* framebuffer;
const double VWIDTH = 1.0;
const double VHEIGHT = 1.0 * ((double)CHEIGHT / (double)CWIDTH); // Maintain aspect ratio
const Vec3 BACKGROUND_COLOR = {0, 0, 0}; // Background color in RGB format
Vec3 trace_ray(Vec3 O, Vec3 D, int t_min, int t_max);

Vec3 operator-(const Vec3& a, const Vec3& b) {
    // Overload the - operator to subtract two vectors
    return {a[0] - b[0], a[1] - b[1], a[2] - b[2]};
}

Vec3 operator*(const Vec3& v, double scalar) {
    // Overload the * operator to scale a vector
    return {v[0] * scalar, v[1] * scalar, v[2] * scalar};
}

Vec3 operator*(double scalar, const Vec3& v) {
    // Overload the * operator to scale a vector
    return {v[0] * scalar, v[1] * scalar, v[2] * scalar};
}

Vec3 operator+(const Vec3& a, const Vec3& b) {
    // Overload the + operator to add two vectors
    return {a[0] + b[0], a[1] + b[1], a[2] + b[2]};
}

Vec3 operator/(const Vec3& v, double scalar) {
    // Overload the / operator to scale a vector
    if (scalar == 0) {
        throw std::runtime_error("Division by zero in vector scaling.");
    }
    return {v[0] / scalar, v[1] / scalar, v[2] / scalar};
}

double dot_product(const Vec3& a, const Vec3& b) {
    // Calculate the dot product of two vectors
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}



enum class LightType {
    AMBIENT,
    POINT,
    DIRECTIONAL
};

struct AmbientData {
};

struct PointData {
    Vec3 position;
};

struct DirectionalData {
    Vec3 direction;
};

class Light {
private:
    LightType type;
    float intensity;
    std::variant<AmbientData, PointData, DirectionalData> data;

public:
    Light(LightType t, double i)
        :type(t), intensity(i), data(AmbientData{}) {
        if (t != LightType::AMBIENT) {
            throw std::invalid_argument("Light type must be AMBIENT for this constructor.");
            }
        }
    Light(LightType t, double i, const Vec3& pos)
        :type(t), intensity(i) {
        switch (t) {
            case LightType::POINT:
                data = PointData{pos};
                break;
            case LightType::DIRECTIONAL:
                data = DirectionalData{pos}; // Assuming direction is given as a position for simplicity
                break;
            default:
                throw std::invalid_argument("Invalid light type for this constructor.");}
        }
    
    // Getter and setter
    LightType getType() const {return type;}
    float getIntensity() const {return intensity;}

    void setIntensity(float i) {intensity = i;}

    Vec3 getPosition() const {
        if (type != LightType::POINT) {
            throw std::runtime_error("Position is only available for POINT lights.");
        }
        return std::get<PointData>(data).position;
    }

    Vec3 getDirection() const {
        if (type != LightType::DIRECTIONAL) {
            throw std::runtime_error("Direction is only available for DIRECTIONAL lights.");
        }
        return std::get<DirectionalData>(data).direction;
    }

    void setPosition(const Vec3& pos) {
        if (type != LightType::POINT) {
            throw std::runtime_error("Position can only be set for POINT lights.");
        }
        std::get<PointData>(data).position = pos;
    }

    void setDirection(const Vec3& dir) {
        if (type != LightType::DIRECTIONAL) {
            throw std::runtime_error("Direction can only be set for DIRECTIONAL lights.");
        }
        std::get<DirectionalData>(data).direction = dir;
    }

    void print() const {
        std::cout << "Light {\n";
        std::cout << "  Type = ";

        switch (type) {
            case LightType::AMBIENT:
                std::cout << "AMBIENT\n";
                break;
            case LightType::POINT:
                std::cout << "POINT\n";
                std::cout << "  Position = (" 
                          << std::get<PointData>(data).position[0] << ", "
                          << std::get<PointData>(data).position[1] << ", "
                          << std::get<PointData>(data).position[2] << ")\n";
                break;
            case LightType::DIRECTIONAL:
                std::cout << "DIRECTIONAL\n";
                std::cout << "  Direction = (" 
                          << std::get<DirectionalData>(data).direction[0] << ", "
                          << std::get<DirectionalData>(data).direction[1] << ", "
                          << std::get<DirectionalData>(data).direction[2] << ")\n";
                break;
        }

        std::cout << "  Intensity = " << intensity << "\n";
        std::cout << "}\n";
    }
};



class Sphere {
public:
    Sphere(double x, double y, double z, double radius, Vec3 rgb) : center{x, y, z}, radius(radius), rgb(rgb) {}

    Vec3 center; // Center of the sphere
    double radius; // Radius of the sphere
    Vec3 rgb; // Color of the sphere in RGB format
};

// Vec3 operator*(Vec3 v, double scalar) {
//     return {v[0] * scalar, v[1] * scalar, v[2] * scalar};
// }

std::vector<double> intersect_ray_sphere(Vec3 O, Vec3 D, Sphere sphere);


// Declaration moved after the Sphere class definition
std::array<Sphere, 3> SCENE_SPHERES = {
    Sphere(0, 0, 3, 0.75, {255, 0, 0}), // Example sphere at origin with radius 100
    Sphere(1, 0, 4, 0.3, {0, 255, 0}), // Another sphere at (200, 100, -50) with radius 50
    Sphere(-1, 0, 4, 0.3, {0, 0, 255}) // Another sphere at (200, 100, -50) with radius 50
};

std::array<Light, 3> SCENE_LIGHTS = {
    Light(LightType::AMBIENT, 0.3), // Ambient light with intensity 0.5
    Light(LightType::POINT, 0.5, Vec3{2.0f, 2.0f, 2.0f}), // Point light at (2, 2, 2) with intensity 1.0
    Light(LightType::DIRECTIONAL, 0.5, Vec3{1.0f, -1.0f, -1.0f}) // Directional light with direction (1, -1, -1)
};

Vec3 canvas_to_viewport(int x, int y) {
    // Convert canvas coordinates to viewport coordinates
    Vec3 D;
    D[0] = (double)x * VWIDTH / CWIDTH ; // Adjust x to be positive
    D[1] = -(double)y * VHEIGHT / CHEIGHT; // Adjust y to be positive
    D[2] = 1.0; // Z coordinate, can be set to any value as needed

    return D;
}

double length(const Vec3& v){
    // Calculate the length of a vector
    return std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

double computer_lighting(Vec3 P, Vec3 N) {
    double intensity = 0.0;
    Vec3 L; // Light direction vector
    for (int i=0; i<SCENE_LIGHTS.size(); ++i){
        if (SCENE_LIGHTS[i].getType() == LightType::AMBIENT) {
            intensity += SCENE_LIGHTS[i].getIntensity();

        } else{
            if (SCENE_LIGHTS[i].getType() == LightType::POINT) {
                L = SCENE_LIGHTS[i].getPosition() - P;

            } else {
                L = SCENE_LIGHTS[i].getDirection();

            };

            double n_dot_l = dot_product(N, L);
            if (n_dot_l > 0) {
                intensity += SCENE_LIGHTS[i].getIntensity() * n_dot_l / (length(N)*length(L));
            };

        }
    }

    return intensity;
}

void render_sphere() {
    Vec3 O = {0,0,0}; // Camera at origin
    int intersections_found = 0;
    int black_pixels = 0;
    
    std::cout << "Starting render...\n";
    std::cout << "VWIDTH: " << VWIDTH << ", VHEIGHT: " << VHEIGHT << "\n";

    for (int x = 0; x < CWIDTH; ++x) {
        for (int y = 0; y < CHEIGHT; y++) {
            int cx = x - CWIDTH / 2; // Center the image
            int cy = y - CHEIGHT / 2; // Center the image
            Vec3 D = canvas_to_viewport(cx, cy);
            
            // Debug: Print first few rays
            if (black_pixels < 5) {
                std::cout << "Pixel (" << x << "," << y << ") -> Ray O=(" 
                          << O[0] << "," << O[1] << "," << O[2] << ") D=(" 
                          << D[0] << "," << D[1] << "," << D[2] << ")\n";
                black_pixels++;
            }
            
            Vec3 color = trace_ray(O, D, 1, 1000);

            // Count non-black pixels
            if (color[0] != 0 || color[1] != 0 || color[2] != 0) {
                intersections_found++;
                if (intersections_found < 10) {
                    std::cout << "HIT! Pixel (" << x << "," << y << ") color=(" 
                              << color[0] << "," << color[1] << "," << color[2] << ")\n";
                }
            }

            uint8_t r = color[0];
            uint8_t g = color[1]; 
            uint8_t b = color[2];
            framebuffer[y * CWIDTH + x] = (255 << 24) | (r << 16) | (g << 8) | b;
        }
    }
    std::cout << "Render complete. Total intersections: " << intersections_found << "\n";
}

Vec3 get_max_rgb(Vec3 color) {
    // Ensure RGB values are within the range [0, 255]
    color[0] = std::max(0.0, std::min(255.0, color[0]));
    color[1] = std::max(0.0, std::min(255.0, color[1]));
    color[2] = std::max(0.0, std::min(255.0, color[2]));
    return color;
}

Vec3 trace_ray(Vec3 O, Vec3 D, int t_min, int t_max){
    double closest_t = INT_MAX;
    std::optional<Sphere> closest_sphere;
    for (int i=0;i<SCENE_SPHERES.size(); ++i) {
        Sphere sphere = SCENE_SPHERES[i];
        // Calculate intersection of ray with sphere
        std::vector<double> intersection = intersect_ray_sphere(O, D, sphere);

        if (intersection.size() >= 2) {
            double t1 = intersection[0];
            double t2 = intersection[1];
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

        Vec3 P = O + D * closest_t; // Calculate intersection point
        Vec3 N = P - closest_sphere->center;
        N = N / length(N); // Normalize the normal vector
        Vec3 max_rgb = get_max_rgb(closest_sphere->rgb * computer_lighting(P,N));
        return max_rgb; // Return the color of the closest sphere
    } else {
        // std::cout << "No sphere intersected by ray.\n";
        return BACKGROUND_COLOR;

    }
        // Here you would calculate the actual intersection and update closest_t and closest_sphere

}



std::vector<double> intersect_ray_sphere(Vec3 O, Vec3 D, Sphere sphere) {
    // This function would calculate the intersection of a ray with a sphere
    // For now, we will just print the ray parameters

    // Here you would implement the actual intersection logic
    double r = sphere.radius;
    Vec3 CO = O - sphere.center;

    double a = dot_product(D, D);
    double b = 2 * dot_product(CO, D);
    double c = dot_product(CO, CO) - r * r;

    double discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return {}; // No intersection
    }
    // Calculate the two intersection points
    double t1 = (-b - static_cast<double>(sqrt(discriminant))) / (2 * a);
    double t2 = (-b + static_cast<double>(sqrt(discriminant))) / (2 * a);
 
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