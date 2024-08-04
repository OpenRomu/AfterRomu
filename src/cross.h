#ifndef __cross_h__
#define __cross_h__
#include "texture.h"
#include <vector>
#include "dsutil.h"

using namespace std;

class Cross {
public:
    class Coords {
    public:
        float c00[2];
        float c10[2];
        float c11[2];
        float c01[2];
    };

    Cross(const char* petite, const char* snipper, const char* noir);
    ~Cross();

    void print(int m_width, int m_height);
    int courante;
    int position;
    int W[3];
    int H[3];

private:
    Texture* m_tex[3];
    vector<Coords*> m_coords;

    float c00[2];
    float c10[2];
    float c11[2];
    float c01[2];
};

#endif // __Romu_h__