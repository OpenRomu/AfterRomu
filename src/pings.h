#ifndef __Pings_h__
#define __Pings_h__
#include "texture.h"
#include <vector>
#include "dsutil.h"
#include "player.h"
#include "font.h"
using namespace std;

class Pings
{
  public:
    class Coords
    {
      public:
        float c00[2];
        float c10[2];
        float c11[2];
        float c01[2];
    };
    bool visible;
    IDirectPlay8Peer *m_dp;
    Pings(IDirectPlay8Peer *dp);
    ~Pings();
    void frame(float delta);
    void print(int m_width, int m_height, vector<Aplayer *> *joueur_list, int nb_gign, int nb_terro, bool CFT_ON,
               bool TEAM_ON, int CFT_eta, long CFT_count, long timing, int FRIENDLY, bool SNIPER_ON);
    DWORD get_lagg(DPNID id);
    int W[1];
    int H[1];
    float tps;

  private:
    Texture *m_tex[1];
    vector<Coords *> m_coords;
    Font *m_font;
    float c00[2];
    float c10[2];
    float c11[2];
    float c01[2];
};

#endif // __Romu_h__