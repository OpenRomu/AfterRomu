
#ifndef __romu_h__
#define __romu_h__
#include "texture.h"
//#include <vector>     
//#include "dsutil.h"
#include "font.h"
#include <string>
using namespace std;

class Romu {
public:
    class Coords {
    public:
        float   c00[2];
        float   c10[2];
        float   c11[2];
        float   c01[2];
    };
public:
    Romu(const char* filename);
    ~Romu();
	Font*           m_font;
	float   duree;
	float position;
	float scroll;
	void tchat_scroll(char * txt);
	string letext;
	char  Message[1000];
	char  Message_Tchat[1000];
	int state; // ouverture fermeture
    void print(int x, int y,int width,int height ,float fps,int engine_state);
	void init(void);
	void show(void);
	void hide(void);

private:
    Texture* m_tex;
   
    vector<Coords*> m_coords;
 
    
};

#endif // __Romu_h__
