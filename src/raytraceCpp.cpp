#include <os.h>
#include <SDL/SDL_config.h>
#include <SDL/SDL.h>
#include <math.h>

const int WIDTH = 320;
const int HEIGHT = 240;
const float FOV = 0.698;

class Vec3 {
public:
    float x, y, z;
    Vec3();
    Vec3(float x, float y, float z);

    Vec3 normalize() const;
    Vec3 operator+(const Vec3 &other) const;
    Vec3 operator*(const float &other) const;
};

Vec3::Vec3(): x(0), y(0), z(0) {}
Vec3::Vec3(float x, float y, float z): x(x), y(y), z(z) {}
Vec3 Vec3::normalize() const {
    float mag = sqrt(x*x + y*y + z*z);
    return Vec3(x/mag, y/mag, z/mag);
}
Vec3 Vec3::operator+(const Vec3 &other) const {
    return Vec3(x+other.x, y+other.y, z+other.z);
}
Vec3 Vec3::operator*(const float &other) const {
    return Vec3(x*other, y*other, z*other);
}

float mod(const float &x, const float &n) {
    return x - floor(x/n) * n;
}

Vec3 mod(const Vec3 &x, const float &n) {
    return Vec3(mod(x.x, n), mod(x.y,n), mod(x.z,n));
}

float distance(const Vec3 &a, const Vec3 &b) {
    float diffX = a.x - b.x;
    float diffY = a.y - b.y;
    float diffZ = a.z - b.z;
    return sqrt(diffX*diffX + diffY*diffY + diffZ*diffZ);
}

float fractalDistance(const Vec3 &x) {
    return distance(mod(x, 1.0f), Vec3(0.5f, 0.5f, 0.5f)) - 0.2f;
}

float lightIntensity(const Vec3 &x, const Vec3 &light, const Vec3 &camera) {
    float fog = 1.0f/pow(distance(x, camera) + 1.0f, 0.2f);
    return 1.0f/pow(distance(mod(x, 1.0f), light), 5.0f) * fog;
}

bool raytrace(const Vec3 &startPos, const Vec3 &direction, Vec3 *outVec) {
    Vec3 rayPos = startPos;
    float d = 1.0f;
    while(d > 0.05) {
        rayPos = rayPos + direction * d;
        d = fractalDistance(rayPos);

        if(distance(rayPos, startPos) > 30.0f) {
            return false;
        }
    }
    *outVec = rayPos;
    return true;
}

Vec3 fragmentShader(const Vec3 &cameraPos, const Vec3 &lightPos, int x, int y) {
    float angleX = FOV * (2.0f*((float)x/WIDTH) - 1.0f);
    float angleY = FOV * (2.0f*((float)y/HEIGHT) - 1.0f);
    Vec3 direction = Vec3(tan(angleX), tan(angleY), 1.0f).normalize();

    Vec3 rayResult;
    bool found = raytrace(cameraPos, direction, &rayResult);
    if(!found) {
        return Vec3(0.0f, 0.0f, 0.0f);
    }
    float shade = lightIntensity(rayResult, lightPos, cameraPos);
    return Vec3(shade, 0, 0);
}

void drawScreen(SDL_Surface *screen) {
    Vec3 cameraPos(0.0f, 0.0f, -3.0f);
    Vec3 lightPos(0.0f, 1.0f, -0.5f);
    SDL_Rect rect;
    rect.w = 1;
    rect.h = 1;

    for(int x = 0; x < WIDTH; x++) {
        rect.x = x;
        for(int y = 0; y < HEIGHT; y++) {
            rect.y = y;
            Vec3 color = fragmentShader(cameraPos, lightPos, x, y) * 255.0f;
            SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, color.x, color.y, color.z));
        }
    }
    SDL_Flip(screen);
}

void waitForKey() {
    SDL_Event event;
    while(event.type != SDL_KEYDOWN) {
        SDL_WaitEvent(&event);
    }
}

int main(void) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Surface *screen = SDL_SetVideoMode(WIDTH, HEIGHT, has_colors ? 16 : 8, SDL_SWSURFACE);
    drawScreen(screen);

    waitForKey();

    SDL_Quit();
    return 0;
}