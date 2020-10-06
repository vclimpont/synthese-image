#define _USE_MATH_DEFINES
#include "ray.cpp"
#include "sphere.h"
#include "Light.cpp"
#include "lodepng.h"
#include "BoxMaker.h"
#include "Box.h"
#include <iostream>
#include <random>
using namespace std;

std::random_device rd;
std::mt19937 e2(rd());

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
    return r;
}

Vector3 GetNormalizedDirectionFromPoints(Vector3 pa, Vector3 pb)
{
    Vector3 dirRay = pa - pb;
    dirRay = Vector3::normalize(dirRay);

    return dirRay;
}

float GetRandomFloatBetween(float a, float b)
{
    std::uniform_real_distribution<> dist(a, b);
    return dist(e2);
}

Vector3 GetRandomVectorBetween(float minX, float maxX, float minY, float maxY, float minZ, float maxZ)
{
    float randX = GetRandomFloatBetween(minX, maxX);
    float randY = GetRandomFloatBetween(minY, maxY);
    float randZ = GetRandomFloatBetween(minZ, maxZ);

    return Vector3(randX, randY, randZ);
}

Vector3 GetRandomVectorBetween(float offset)
{
    float randX = GetRandomFloatBetween(-offset, offset);
    float randY = GetRandomFloatBetween(-offset, offset);
    float randZ = GetRandomFloatBetween(-offset, offset);

    return Vector3(randX, randY, randZ);
}

Vector3 GetRandomPointOnSphereWithDensity(Sphere s)
{
    float r1 = GetRandomFloatBetween(0, 1);
    float r2 = GetRandomFloatBetween(0, 1);

    float R2 = 2 * s.GetRadius();
    float pi2R1 = 2 * M_PI * r1;
    float sqrtR2 = sqrt(r2 * (1 - r2));

    float x = s.GetCenter().x + R2 * cos(pi2R1) * sqrtR2;
    float y = s.GetCenter().y + R2 * sin(pi2R1) * sqrtR2;
    float z = s.GetCenter().z + s.GetRadius() * (1 - 2 * r2);

    return Vector3(x, y, z);
}

void GetRandomPointsToLight(Vector3 randPointsOnLight[], int nbRays, Light l)
{
    if (l.GetRadius() == 0)
    {
        randPointsOnLight[0] = l.GetPosition();
    }
    else
    {
        for (int i = 0; i < nbRays; i++)
        {
            Vector3 lightPos = l.GetPosition();
            float lightRadius = l.GetRadius();
            Vector3 randomVector = GetRandomVectorBetween(lightRadius);
            randPointsOnLight[i] = lightPos + randomVector;
        }
    }
}

float GetAngleFrom(Vector3 pa, Vector3 pb)
{
    float angle = Vector3::dot(pa, pb);
    angle = abs(angle);

    return angle;
}

Vector3 GetLightIntensityOnSurface(Vector3 colSurface, Ray rayToSphere, Sphere spheres[], int nbSphere, Light lights[], int nbLights, int& nbBounce, int& pathLength)
{
    Vector3 newColSurfaceFromLights = colSurface;
    Sphere sphere_i = Sphere(Vector3(0, 0, 0), 0, Vector3(0, 0, 0), true);
    float n1 = GetMinRayToSpheres(rayToSphere, sphere_i, spheres, nbSphere);

    if (n1 != INFINITY)
    {
        const int nbRays = 1;
        Vector3 rayPoint = rayToSphere.GetPosition();
        Vector3 intersectPoint = rayPoint + rayToSphere.GetDirection() * n1;
        if (sphere_i.GetDiffuse() == true)
        {
            for (int k = 0; k < nbLights; k++)
            {
                Vector3 randPointsOnLight[nbRays]{ Vector3(0, 0, 0) };
                GetRandomPointsToLight(randPointsOnLight, nbRays, lights[k]);
                Vector3 lightIntensityOnSurface = Vector3(0, 0, 0);

                for (int i = 0; i < nbRays; i++)
                {
                    Vector3 p = intersectPoint;
                    Vector3 dir = randPointsOnLight[i] - p;    // direction vector towards light k
                    float length = dir.length();
                    dir = Vector3::normalize(dir);
                    p = p + dir * 0.01f;
                    Ray ray = Ray(p, dir);

                    if (CastRayToLight(ray, length, spheres, nbSphere))
                    {
                        Vector3 norm = GetNormalizedDirectionFromPoints(p, sphere_i.GetCenter());
                        float angle = GetAngleFrom(norm, dir);

                        lightIntensityOnSurface = lightIntensityOnSurface + CalculateColor(lights[k], angle, length); // add the light color and intensity to the current color
                    }
                }

                newColSurfaceFromLights = newColSurfaceFromLights + lightIntensityOnSurface;
            }

            newColSurfaceFromLights = (newColSurfaceFromLights * sphere_i.GetAlbedo()) / nbRays;
            /*Vector3 colSurfaceFromSpheres = Vector3(0, 0, 0);

            for (int i = 0; i < 3; i++)
            {
                if (pathLength >= 3)
                {
                    break;
                }

                Vector3 rpos = GetRandomPointOnSphereWithDensity(sphere_i);
                Vector3 p = intersectPoint;
                Vector3 dir = GetNormalizedDirectionFromPoints(p, rpos);
                p = p + dir * 0.1f;
                Ray ray = Ray(p, dir);
                int bouncesOnMirrors = 0;
                Vector3 colOnSphereJ = GetLightIntensityOnSurface(Vector3(0, 0, 0), ray, spheres, nbSphere, lights, nbLights, bouncesOnMirrors, pathLength);

                Sphere sphere_j = Sphere(Vector3(0, 0, 0), 0, Vector3(0, 0, 0), true);
                float n2 = GetMinRayToSpheres(ray, sphere_j, spheres, nbSphere);

                if (n2 != INFINITY)
                {
                    Vector3 pj = intersectPoint + dir * n2;

                    Vector3 norm_i = GetNormalizedDirectionFromPoints(p, sphere_i.GetCenter());
                    float angle_i = GetAngleFrom(norm_i, dir);

                    Vector3 norm_j = GetNormalizedDirectionFromPoints(pj, sphere_j.GetCenter());
                    float angle_j = GetAngleFrom(norm_j, dir * -1);

                    if (colOnSphereJ.length() >= 10000)
                    {
                        std::cout << p << " " << pj << " " << colOnSphereJ << " ||||| ";
                    }
                    colOnSphereJ = (colOnSphereJ * angle_i * angle_j) / (n2 * n2);

                    colSurfaceFromSpheres = colSurfaceFromSpheres + (sphere_i.GetAlbedo() * colOnSphereJ);
                    pathLength++;
                }
            }

            newColSurfaceFromLights = newColSurfaceFromLights + colSurfaceFromSpheres;*/
        }
        else if(nbBounce < 10)
        {
            Vector3 p = intersectPoint;
            Vector3 norm = p - sphere_i.GetCenter();
            norm = Vector3::normalize(norm);
            Vector3 r = GetReflectDirection(rayToSphere.GetDirection(), norm);
            r = Vector3::normalize(r);
            p = p + r * 0.01f;
            Ray ray = Ray(p, r);

            nbBounce++;
            return GetLightIntensityOnSurface(newColSurfaceFromLights, ray, spheres, nbSphere, lights, nbLights, nbBounce, pathLength);
        }
    }

    return newColSurfaceFromLights;
}

int main()
{
    BoxMaker bm = BoxMaker(Vector3(0, 0, 0), Vector3(1, 1, 1));
    const int nbSpheres = 2;
    Sphere s1 = Sphere(Vector3(0.2f, 0.7f, 0.5f), 0.01f, Vector3(0, 0, 0), true);
    Sphere s2 = Sphere(Vector3(0.25f, 0.75f, 0.5f), 0.01f, Vector3(0, 0, 0), true);
    Sphere spheres[nbSpheres]{ s1, s2 };
    bm.AddSpheresToInitialBox(spheres, nbSpheres);
    bm.SplitBox(&bm.initialBox);
    bm.DisplaySpheresOfBox(bm.initialBox);


    //Vector3 dirRay = Vector3(0, 0, 1);
    //Vector3 persPoint = Vector3(512, 512, -600);
    //const int nbRaysPerPixels = 1;
    //const float randomOffsetPerPixels = 1;
    //     
    //const int nbSpheres = 8;
    //Sphere s1 = Sphere(Vector3(200, 200, 200), 150.0f, Vector3(0, 1, 0), true); // green sphere
    //Sphere s2 = Sphere(Vector3(814, 200, 200), 150.0f, Vector3(1, 0, 0), true); // red sphere
    //Sphere s3 = Sphere(Vector3(512, 200, 200), 100.0f, Vector3(1, 1, 0), true); // yellow sphere
    //Sphere s4 = Sphere(Vector3(30, 650, 400), 200.0f, Vector3(1, 1, 1), false); // mirror left
    //Sphere s5 = Sphere(Vector3(1000, 650, 400), 200.0f, Vector3(1, 1, 1), false); // mirror right
    //Sphere s6 = Sphere(Vector3(512, 600, 1000), 450.0f, Vector3(1, 1, 1), false);   // mirror mid
    //Sphere s7 = Sphere(Vector3(512, 11100, 300), 10000.0f, Vector3(1, 1, 1), true); // floor
    //Sphere s8 = Sphere(Vector3(512, 512, 12000), 10000.0f, Vector3(0, 1, 1), true); // background
    //Sphere spheres[nbSpheres]{ s1, s2, s3, s4, s5, s6, s7, s8 };

    //const int nbLights = 7;
    //Light l1 = Light(Vector3(0, 0, 100), Vector3(1, 1, 1), 80000000.0f, 10.0f);
    //Light l2 = Light(Vector3(1000, 0, 100), Vector3(1, 1, 1), 80000000.0f, 10.0f);
    //Light l7 = Light(Vector3(512, 512, 100), Vector3(1, 1, 1), 100000000.0f, 20.0f);
    //Light l3 = Light(Vector3(-1000, 600, 1150), Vector3(1, 1, 1), 300000000.0f, 25.0f);
    //Light l4 = Light(Vector3(2000, 600, 1150), Vector3(1, 1, 1), 300000000.0f, 25.0f);
    //Light l5 = Light(Vector3(1000, 800, 50), Vector3(0, 0, 1), 200000000.0f, 10.0f);
    //Light l6 = Light(Vector3(0, 800, 50), Vector3(1, 0, 0), 200000000.0f, 10.0f);
    //Light lights[nbLights]{ l1, l2, l3, l4, l5, l6, l7 };

    //const char* filename = "test.png";

    ////generate some image
    //unsigned width = 1024, height = 1024;
    //std::vector<unsigned char> image;
    //image.resize(width * height * 4);

    //for (unsigned y = 0; y < height; y++) {
    //    for (unsigned x = 0; x < width; x++) {

    //        int index = 4 * width * y + 4 * x;
    //        Vector3 colSurface = Vector3(0, 0, 0);

    //        for (int i = 0; i < nbRaysPerPixels; i++)
    //        {
    //            Vector3 randomOffset = Vector3(GetRandomFloatBetween(-randomOffsetPerPixels, randomOffsetPerPixels), GetRandomFloatBetween(-randomOffsetPerPixels, randomOffsetPerPixels), 0);
    //            Vector3 pixelPoint = Vector3(x, y, 0);// +randomOffset;
    //            Vector3 dirRay = GetNormalizedDirectionFromPoints(pixelPoint, persPoint);

    //            Ray rayToSphere = Ray(pixelPoint, dirRay);
    //            int nbBounce = 0;
    //            int pathLength = 0;
    //            colSurface = colSurface + GetLightIntensityOnSurface(Vector3(0,0,0), rayToSphere, spheres, nbSpheres, lights, nbLights, nbBounce, pathLength);
    //        }
    //        colSurface = colSurface / nbRaysPerPixels;

    //        Vector3 clampedColor = ClampColor(colSurface);
    //        ChangeColor(image, index, clampedColor.x, clampedColor.y, clampedColor.z, 255);
    //    }
    //}

    //encodeOneStep(filename, image, width, height);
}