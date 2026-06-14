#ifndef __Panel_h__
#define __Panel_h__
#include "texture.h"
#include <vector>
#include "dsutil.h"

using namespace std;

class Panel
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

    Panel(const char *lesammo, const char *ammo, const char *smoke, const char *vie, const char *players,
          const char *lazer);
    ~Panel();
    void hit(void);
    void hit_Romu(void);

    void frame(float delta);
    void print(int m_width, int m_height, int nbammo, int vie, int nb_player, char *weapon);
    GLuint courante;
    int position;
    int W[5];
    int H[5];
    float tps_players;
    float tps_vie;

  private:
    Texture *m_tex[7];
    vector<Coords *> m_coords;

    float c00[2];
    float c10[2];
    float c11[2];
    float c01[2];
};

#endif // __Romu_h__