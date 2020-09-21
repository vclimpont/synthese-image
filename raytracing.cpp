#define _USE_MATH_DEFINES
#include "ray.cpp"
#include "sphere.cpp"
#include "Light.cpp"
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

float GetMaxIntensity(Light lights[], int nbLights)
{
    float max = 0.0f;

    for (int i = 0; i < nbLights; i++)
    {
        float intens = lights[i].GetIntensity();
        if (intens > max)
        {
            max = intens;
        }
    }

    return max;
}

Vector3 ClampColor(Vector3 color, float max)
{
    Vector3 clampedColor = (color / (255 * max)) * 255;

    return clampedColor;
}

Vector3 ClampColor(Vector3 color)
{
    Vector3 clampedColor = color;
    clampedColor.x = clampedColor.x > 255 ? 255 : clampedColor.x;
    clampedColor.y = clampedColor.y > 255 ? 255 : clampedColor.y;
    clampedColor.z = clampedColor.z > 255 ? 255 : clampedColor.z;

    return clampedColor;
}

Vector3 CalculateColor(Light l, float angle, Vector3 surfaceColor, float length)
{
    Vector3 color = (l.GetColor() * l.GetIntensity() * angle * surfaceColor) / (length * length * M_PI);

    return color;
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

    const int nbSphere = 4;
    Sphere s1 = Sphere(Vector3(250, 100, 100), 50.0f);
    Sphere s2 = Sphere(Vector3(100, 250, 80), 50.0f);
    Sphere s3 = Sphere(Vector3(412, 250, 120), 50.0f);
    Sphere s4 = Sphere(Vector3(250, 412, 100), 50.0f);
   // Sphere s5 = Sphere(Vector3(210, 375, 100), 75.0f);
    Sphere spheres[nbSphere]{ s1, s2, s3, s4};


    const int nbLights = 2;
    //Light l1 = Light(Vector3(250, 250, 50), Vector3(255, 1, 1), 150000.0f);
    Light l2 = Light(Vector3(512, 0, 100), Vector3(1, 1, 255), 200000.0f);
    Light l3 = Light(Vector3(0, 512, 120), Vector3(1, 255, 1), 250000.0f);
    //Light l4 = Light(Vector3(512, 512, 80), Vector3(255, 255, 1), 150000.0f);
    Light lights[nbLights]{ l2, l3 };

    float maxIntensity = GetMaxIntensity(lights, nbLights);

    const char* filename = "test.png";

    //generate some image
    unsigned width = 512, height = 512;
    std::vector<unsigned char> image;
    image.resize(width * height * 4);

    for (unsigned y = 0; y < height; y++)
        for (unsigned x = 0; x < width; x++) {
            
            int index = 4 * width * y + 4 * x;
            ChangeColor(image, index, 1, 1, 1, 255);
           
            for (int i = 0; i < nbSphere; i++)
            {
                float n1 = hit_sphere(Ray(Vector3(x, y, 0), dirRay), spheres[i]); // try to hit a sphere

                if (n1 >= 0) // if ray hits a sphere
                {
                    ChangeColor(image, index, 1, 1, 1, 255); // make it black

                    for (int k = 0; k < nbLights; k++)
                    {
                        bool hitLight = true;
                        int j = 0;
                        Vector3 c = lights[k].GetColor();

                        Vector3 p = Vector3(x, y, n1);  // intersect point with the sphere n1
                        Vector3 dir = lights[k].GetPosition() - p;    // direction vector towards 
                        float length = dir.length();
                        dir = Vector3::normalize(dir);
                        p = p + dir * 0.01f;
                        Ray ray = Ray(p, dir);

                        while (j < nbSphere && hitLight == true)
                        {
                            float n2 = hit_sphere(ray, spheres[j]); // try to hit another sphere

                            if (n2 >= 0 && n2 < length) // if it hits the sphere and so it can't reach the light
                            {
                                hitLight = false;
                            }

                            j++;
                        }

                        if (hitLight == true)
                        {
                            Vector3 col = Vector3((int)image[index], (int)image[index + 1], (int)image[index + 2]); // get the current color

                            Vector3 norm = p - spheres[i].GetCenter();
                            norm = Vector3::normalize(norm);
                            float theta = Vector3::dot(norm, dir);
                            float angle = abs(theta);

                            Vector3 l = ClampColor(CalculateColor(lights[k], angle, col, length)); // add the light color and intensity to the current color
                            ChangeColor(image, index, l.x, l.y, l.z, 255);
                        }
                    }
                }
            }
        }

    encodeOneStep(filename, image, width, height);
}