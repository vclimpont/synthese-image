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

Vector3 ClampColor(Vector3 color)
{
    Vector3 clampedColor = color;
    clampedColor.x = clampedColor.x > 255 ? 255 : clampedColor.x;
    clampedColor.y = clampedColor.y > 255 ? 255 : clampedColor.y;
    clampedColor.z = clampedColor.z > 255 ? 255 : clampedColor.z;

    return clampedColor;
}

Vector3 CalculateColor(Light l, float angle, float length)
{
    Vector3 color = (l.GetColor() * l.GetIntensity() * angle) / (length * length * M_PI);
    //std::cout << l.GetColor() << " " << l.GetIntensity() << " " << angle << " " << (length) << " " << (length * length * M_PI) << " " << color << "    ";

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

float CastRayToSphere(Ray ray, Sphere s)
{
    float n1 = hit_sphere(ray, s); // try to hit a sphere

    if (n1 >= 0) // if ray hits a sphere
    {
        return n1;
    }

    return -1;
}

bool CastRayToLight(Ray ray, float lengthToLight, Sphere spheres[], int nbSphere)
{
    int j = 0;

    while (j < nbSphere)
    {
        float n2 = hit_sphere(ray, spheres[j]); // try to hit another sphere

        if (n2 >= 0 && n2 < lengthToLight) // if it hits the sphere and so it can't reach the light
        {
            return false;
        }

        j++;
    }

    return true;
}

float GetMinRayToSpheres(Ray ray, Sphere& sphere_i, Sphere spheres[], int nbSphere)
{
    float rayDistMin = INFINITY;

    for (int i = 0; i < nbSphere; i++)
    {
        float rayDist = CastRayToSphere(ray, spheres[i]);
        if (rayDist != -1 && rayDist < rayDistMin )
        {
            rayDistMin = rayDist;
            sphere_i = spheres[i];
        }
    }
    
    return rayDistMin;
}


Vector3 GetReflectDirection(Vector3 dirRay, Vector3 norm)
{
    Vector3 r = norm * Vector3::dot(dirRay * -1.0f, norm) * 2 + dirRay;
    //cout << dirRay << " " << r;
    return r;
}

Vector3 GetPerspectiveDirection(Vector3 pixelPoint, Vector3 persPoint)
{
    Vector3 dirRay = pixelPoint - persPoint;
    dirRay = Vector3::normalize(dirRay);

    return dirRay;
}

void GetLightIntensityOnSurface(Vector3& colSurface, Ray rayToSphere, Sphere spheres[], int nbSphere, Light lights[], int nbLights, int& nbBounce)
{
    Sphere sphere_i = Sphere(Vector3(0, 0, 0), 0, Vector3(0, 0, 0), true);
    float n1 = GetMinRayToSpheres(rayToSphere, sphere_i, spheres, nbSphere);

    if (n1 != INFINITY)
    {
        Vector3 rayPoint = rayToSphere.GetPosition();
        Vector3 intersectPoint = rayPoint + rayToSphere.GetDirection() * n1;
        if (sphere_i.GetDiffuse() == true)
        {
            for (int k = 0; k < nbLights; k++)
            {
                Vector3 p = intersectPoint;
                Vector3 dir = lights[k].GetPosition() - p;    // direction vector towards light k
                float length = dir.length();
                dir = Vector3::normalize(dir);
                p = p + dir * 0.001f;
                Ray ray = Ray(p, dir);

                if (CastRayToLight(ray, length, spheres, nbSphere))
                {
                    Vector3 norm = p - sphere_i.GetCenter();
                    norm = Vector3::normalize(norm);
                    float angle = Vector3::dot(norm, dir);
                    angle = abs(angle);

                    colSurface = colSurface + CalculateColor(lights[k], angle, length); // add the light color and intensity to the current color
                }
            }

            //std::cout << colSurface << " ";
            colSurface = colSurface * sphere_i.GetAlbedo();

        }
        else if(nbBounce < 10)
        {
            Vector3 p = intersectPoint;
            Vector3 norm = p - sphere_i.GetCenter();
            norm = Vector3::normalize(norm);
            Vector3 r = GetReflectDirection(rayToSphere.GetDirection(), norm);
            r = Vector3::normalize(r);
            p = p + r * 0.001f;
            Ray ray = Ray(p, r);

            nbBounce++;
            GetLightIntensityOnSurface(colSurface, ray, spheres, nbSphere, lights, nbLights, nbBounce);
        }
    }
    else if (rayToSphere.GetPosition().z != 0)
    {
        //colSurface = Vector3(255, 255, 255);
    }
    else
    {
        colSurface = Vector3(20, 20, 20);
    }
}

int main()
{
    //Vector3 dirRay = Vector3(0, 0, 1);
    Vector3 persPoint = Vector3(512, 512, -600);

    const int nbSphere = 8;
    Sphere s1 = Sphere(Vector3(200, 200, 200), 150.0f, Vector3(0, 1, 0), true);
    Sphere s2 = Sphere(Vector3(814, 200, 200), 150.0f, Vector3(1, 0, 0), true);
    Sphere s3 = Sphere(Vector3(512, 200, 200), 100.0f, Vector3(1, 1, 0), true);
    Sphere s4 = Sphere(Vector3(30, 650, 400), 200.0f, Vector3(1, 1, 1), false);
    Sphere s5 = Sphere(Vector3(1000, 650, 400), 200.0f, Vector3(1, 1, 1), false);
    Sphere s6 = Sphere(Vector3(512, 600, 1000), 450.0f, Vector3(1, 1, 1), false);
    Sphere s7 = Sphere(Vector3(512, 11030, 300), 10000.0f, Vector3(1, 1, 1), true);
    Sphere s8 = Sphere(Vector3(512, 512, 12000), 10000.0f, Vector3(0, 1, 1), true);
    Sphere spheres[nbSphere]{ s1, s2, s3, s4, s5, s6, s7, s8 };

    const int nbLights = 7;
    Light l1 = Light(Vector3(0, 0, 100), Vector3(1, 1, 1), 100000000.0f);
    Light l2 = Light(Vector3(1000, 0, 100), Vector3(1, 1, 1), 100000000.0f);
    Light l7 = Light(Vector3(512, 512, 100), Vector3(1, 1, 1), 80000000.0f);
    Light l3 = Light(Vector3(300, 600, 10050), Vector3(1, 1, 1), 1000000000.0f);
    Light l4 = Light(Vector3(700, 600, 10050), Vector3(1, 1, 1), 1000000000.0f);
    Light l5 = Light(Vector3(1000, 800, 50), Vector3(0, 0, 1), 800000000.0f);
    Light l6 = Light(Vector3(0, 800, 50), Vector3(1, 0, 0), 80000000.0f);
    Light lights[nbLights]{ l1, l2, l3, l4, l5, l6, l7};

    float maxIntensity = GetMaxIntensity(lights, nbLights);

    const char* filename = "test.png";

    //generate some image
    unsigned width = 1024, height = 1024;
    std::vector<unsigned char> image;
    image.resize(width * height * 4);

    for (unsigned y = 0; y < height; y++) {
        for (unsigned x = 0; x < width; x++) {

            int index = 4 * width * y + 4 * x;
            Vector3 colSurface = Vector3(0, 20, 20);
            Vector3 pixelPoint = Vector3(x, y, 0);
            Vector3 dirRay = GetPerspectiveDirection(pixelPoint, persPoint);

            Ray rayToSphere = Ray(pixelPoint, dirRay);
            int nbBounce = 0;
            GetLightIntensityOnSurface(colSurface, rayToSphere, spheres, nbSphere, lights, nbLights, nbBounce);

            Vector3 clampedColor = ClampColor(colSurface);
            ChangeColor(image, index, clampedColor.x, clampedColor.y, clampedColor.z, 255);
        }
    }

    encodeOneStep(filename, image, width, height);
}