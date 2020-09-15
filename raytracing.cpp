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
    Sphere s1 = Sphere(Vector3(375, 310, 100), 50.0f);
    Sphere s2 = Sphere(Vector3(150, 175, 35), 30.0f);
    Sphere s3 = Sphere(Vector3(265, 128, 50), 30.0f);
    Sphere s4 = Sphere(Vector3(80, 152, 75), 10.0f);
    Sphere s5 = Sphere(Vector3(210, 375, 10), 75.0f);
    const int nbSphere = 4;
    Sphere spheres[nbSphere]{ s1, s3, s4, s5};
    Vector3 light = Vector3(50, 50, 200);

    const char* filename = "test.png";

    //generate some image
    unsigned width = 512, height = 512;
    std::vector<unsigned char> image;
    image.resize(width * height * 4);

    for (unsigned y = 0; y < height; y++)
        for (unsigned x = 0; x < width; x++) {
            
            int index = 4 * width * y + 4 * x;
            ChangeColor(image, index, 255, 0, 0, 255);
           
            for (int i = 0; i < nbSphere; i++)
            {
                float n1 = hit_sphere(Ray(Vector3(x, y, 0), dirRay), spheres[i]);

                if (n1 >= 0)
                {
                    ChangeColor(image, index, 255, 255, 255, 255);

                    for (int j = 0; j < nbSphere; j++)
                    {
                        Vector3 p = Vector3(x, y, n1);
                        Vector3 dir = light - p;
                        float length = dir.length();
                        dir = Vector3::normalize(dir);
                        p = p + dir * 0.01f;
                        Ray ray = Ray(p, dir);
                
                        float n2 = hit_sphere(ray, spheres[j]);


                        if(n2 >= 0 && n2 < length)
                        {
                            ChangeColor(image, index, 0, 0, 0, 255);
                        }
                    }
                }
            }
        }

    encodeOneStep(filename, image, width, height);
}