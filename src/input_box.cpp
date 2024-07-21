// input_box.cpp: implementation of the input_box class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "input_box.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

input_box::input_box():max_len(15),actif(false),visible(false)
{
fond.SetTexture("suivant_droite.bmp");
activ_fond.SetTexture("fond.bmp");
libelle.visible=true;
fond.visible=true;
activ_fond.visible=true;
text.visible=true;
}

input_box::~input_box()
{
	
	


}
void input_box::SetText(char * t)
{
text.SetText(t);
}
void input_box::SetLibelle(char * t)
{
libelle.SetText(t);
}

void input_box::draw()
{

}

void input_box::frame(int mouseX,int mouseY,int m_width,int m_height,bool mousekik,bool isdebug,float delta)
{
	 fond.frame(mouseX,mouseY,m_width,m_height,mousekik,isdebug,delta);
	 if(actif)
 	 	activ_fond.frame(mouseX,mouseY,m_width,m_height,mousekik,isdebug,delta*1.3f);
 libelle.frame(m_width,m_height,isdebug,delta);
 	text.frame(m_width,m_height,isdebug,delta);
		
}

void input_box::SetXY(int in_X,int in_Y,int in_W,int in_H)
{
libelle.SetXY(in_X,in_Y,in_W,in_H);
text.SetXY(in_X+in_W/8,in_Y+in_H/5.0f,in_W,in_H);
fond.SetXY(in_X,in_Y+in_H/4.0f);
activ_fond.SetXY(in_X+in_W/8,in_Y+in_H/4.0f);

}
void input_box::SetFont(Font * in_font)
{
libelle.SetFont(in_font);
fond.SetFont(in_font);
activ_fond.SetFont(in_font);

text.SetFont(in_font);
}
