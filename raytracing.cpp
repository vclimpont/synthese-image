#include "ray.cpp"
#include "sphere.cpp"
#include "lodepng.h"
#include <iostream>
using namespace std;

float hit_sphere(Ray ray, Sphere sphere)
{
    Vector3 oc = ray.GetPosition() - sphere.GetCenter();
    float a = Vector3::dot(ray.GetDirection(), ray.GetDirection());
    float b = 2.0 * Vector3::dot(oc, ray.GetDirection());
    float c = Vector3::dot(oc, oc) - sphere.GetRadius() * sphere.GetRadius();
    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return -1.0;
    }
    else {
        float numerator = -b - sqrt(discriminant);
        if (numerator > 0.0)
        {
            return numerator / (2.0 * a);
        }

        numerator = -b + sqrt(discriminant);
        if (numerator > 0.0)
        {
            return numerator / (2.0 * a);
        }
        else {
            return -1;
        }
    }
}

void ChangeColor(std::vector<unsigned char>& image, int index, int r, int g, int b, int a)
{
    image[index] = r;
    image[index + 1] = g;
    image[index + 2] = b;
    image[index + 3] = a;
}

//Encode from raw pixels to disk with a single function call
//The image argument has width * height RGBA pixels or width * height * 4 bytes
void encodeOneStep(const char* filename, std::vector<unsigned char>& image, unsigned width, unsigned height) {
    //Encode the image
    unsigned error = lodepng::encode(filename, image, width, height);

    //if there's an error, display it
    if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}


int main()
{
    Vector3 dirRay = Vector3(0, 0, 1);
    Sphere s1 = Sphere(Vector3(400, 400, 50), 50.0f);
    Sphere s2 = Sphere(Vector3(200, 200, 25), 10.0f);
    Vector3 light = Vector3(100, 100, 12.5f);

    const char* filename = "test.png";

    //generate some image
    unsigned width = 512, height = 512;
    std::vector<unsigned char> image;
    image.resize(width * height * 4);

    for (unsigned y = 0; y < height; y++)
        for (unsigned x = 0; x < width; x++) {
            
            int index = 4 * width * y + 4 * x;
            float n1 = hit_sphere(Ray(Vector3(x, y, 0), dirRay), s1);
            //float n2 = hit_sphere(Ray(Vector3(x, y, 0), dirRay), s2);

            if (n1 >= 0)
            {
                Vector3 p = Vector3(x, y, n1);
                Vector3 dir = light - p;
                dir = Vector3::normalize(dir);
                p = p + dir * 0.01f;
                Ray ray = Ray(p, dir);
                
                float n2 = hit_sphere(ray, s1);
                
                if (n2 < 0)
                {
                    ChangeColor(image, index, 255, 255, 255, 255);
                }
                else
                {
                    ChangeColor(image, index, 0, 0, 0, 255);
                }
            }
            else
            {
                ChangeColor(image, index, 255, 0, 0, 255);
            }
        }

    encodeOneStep(filename, image, width, height);
}