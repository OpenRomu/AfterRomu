#include "stdafx.h"
#include "exception.h"
#include "romu.h"
#include "image.h"
#include "math.h"
#include "fastmath.h"
Romu::Romu(const char* filename)
: m_tex(0),duree(0),state(0),position(0),scroll(0)
{ 
	float inc = 1.0f/1.0f;//0.0625f;
	

    Image img;
    img.load(filename);
    m_tex = new Texture(img.data(), img.width(), img.height(), GL_ALPHA);
    m_coords.resize(256);
    m_coords[0] = new Coords;
    if(!m_coords[0]) throw out_of_memory();
    m_coords[0]->c00[0] = 0; m_coords[0]->c00[1] = 0;
    m_coords[0]->c10[0] = 0+inc; m_coords[0]->c10[1] = 0;
    m_coords[0]->c11[0] = 0+inc, m_coords[0]->c11[1] = 0+inc;
    m_coords[0]->c01[0] = 0; m_coords[0]->c01[1] = 0+inc;
 
strcpy(Message_Tchat,"\0");

 	m_font = new Font("data/env/font.tga");
	m_font->taille=12;

    sprintf(Message,"%s",TEXT("********************************************************Bienvenue sur romustrike ****jeu multijoueur gratuit*******SVP*Entrez, sans crainte, une adresse email valide, elle ne sera pas utilise pour du spawn, cela vous permettra de recevoir un mail contenant vos identifiants.*******************************************************************"));    
}

Romu::~Romu()
{
   if(m_tex) {
        m_tex->destroy();
        delete m_tex;
        m_tex = 0;
    }

    for(int c = 0; c < m_coords.size(); c++) {
        if(m_coords[c]) {
            delete m_coords[c];
        }
    }
 
if(m_font) {
   	delete (m_font);
	m_font=0;
}

}


void Romu::init(void)
{
duree=0;

}
void Romu::show(void)
{
state=0;

}
void Romu::hide(void)
{
state=1;

}
void Romu::tchat_scroll(char * txt)
{
sprintf(Message_Tchat,"                                                  %s                                                   \0",txt);
		
scroll=0;

}

void
Romu::print(int x, int y,int width,int height,float fps,int engine_state)
{
	
	float fps2;
	fps2=fps;

	/*if (fps2>0.01)
	fps2=0.01f;
      */   
	
	if ((state==0)&& (position<height))
	{
		position=position+fps2*500;
	}
	
	if ((state==0)&& !(position<height))
		state=3;
	
	
	if ((state==1)&& (position>0))
	{
		position=position-fps2*500;
	}
	
	if ((state==1)&& !(position>0))
		state=3;
	
	
	
	if (position>height)
	{
		position=height;
	}
	if (position<0)
	{
		position=0;
	}
	scroll=scroll+fps*5;

	int l=0;
	if (strlen(Message_Tchat)>0)
		l=strlen(Message_Tchat);
	else
		l=strlen(Message);
	
	
	if(scroll>=(l-50))
	{
		scroll=0;
		if (strlen(Message_Tchat)>0)
		strcpy(Message_Tchat,"");
	}



	if (duree<6.28)
	{
		duree=duree+fps ;

	}
else
{
	duree=0;
}

   
//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);    

 	glEnable(GL_BLEND);
  // //
		//(state==3)
	     glDisable(GL_DEPTH_TEST);
	
	//glColor4f(1.0f,1.0f,1.0f,0.1f);	

	if (!((engine_state==64 ||engine_state==51) && strlen(Message_Tchat)==0))
	{
		
		if (scroll<-1.0f)
		scroll=-1.0f;

		if (strlen(Message_Tchat)>0)
		{
			letext=string (Message_Tchat,floor(scroll)+1,50);
			if (int(scroll)%2==0)
				glColor4f(1.0f,0.2f,0.0f,1.0f);
			else
				glColor4f(0.0f,1.0f,0.2f,1.0f);
		}
		else
		{
			letext=string (Message,floor(scroll)+1,50);  //FIXME ici plantage ?? message accueil , scrool=1.0023
				glColor4f(1.0f,1.0f,1.0f,1.0f);
		}
			glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//

	 m_font->print( (width/2-25*12)-((int)floor(scroll*12) % 12 ), 10 , letext.c_str());	
	 
	}

	if (position>0  )
	{
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//
	glColor4f( 0.9f,0.9f,0.9f,0.5f);
	m_tex->bind();
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	Coords* z = m_coords[0];
	glTexCoord2fv(z->c00); glVertex2s(x ,y+(height-position));
    glTexCoord2fv(z->c10); glVertex2s(x+width,y+(height-position));
    glTexCoord2fv(z->c11); glVertex2s(x+width,y+height+(height-position));
    glTexCoord2fv(z->c01); glVertex2s(x,y+height+(height-position));
    glEnd();
	
 }



//	 glDisable(GL_BLEND);
	
   
}