// Model_MDL.cpp: implementation of the Model_MDL class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// @PORT {
#include <gl/gl.h>			// OpenGL extensions
#include <gl/glu.h>			// OpenGL Utilities extensions
// }

#include "Model_MDL.h"
#include <string.h>
 

//#include "fstream.h"

//#include <afx.h>
//#pragma warning( disable : 4244 ) // double to float

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Model_MDL::Model_MDL():droitier(true),cur_scope(0),optimize(false),munition(0),le_fps_ori_seq2(0.0f),
le_fps_ori_seq1(0.0f),m_frame(0),fps_arme(0),fps2(0),seq2(1),seq1(1),scope(0),multi(0),
id_son(0),nb_boite(0),fps_weapon (0.0f),ang_dos(0.0f),m_pstudiohdr(NULL) 
{
	m_sequence=0;


g_texnum = 1;
vec3_origin[0] = 0;
vec3_origin[1] = 0;
vec3_origin[2] = 0;
g_lambert = 1.5;
m_skinnum = 0;
g_vright[0] = 0;
g_vright[1] = 0;
g_vright[2] = 0;
g_smodels_total = 0;
m_origin[0] = 0;
m_origin[1] = 0;
m_origin[2] = 0;
m_angles[0] = 0;
m_angles[1] = 0;
m_angles[2] = 0;
m_bodynum	= 0;
tete[0]=0;
tete[1]=0;
tete[2]=0;
flash[0]=0;
flash[1]=0;
flash[2]=0;
seq1=0;
seq2=0;
fps1=0.0f;
fps2=0.0f;
maxframes=0;
dir[0] = 0;
dir[1] = 1;
dir[2] = 0;
  
					
}

Model_MDL::~Model_MDL()
{
 	
g_texnum = 1;
 
}

int Model_MDL::calculeboite (vec3_t src,vec3_t normal)
{

		float nearest=10000.00f;
		int id_nearest=-1;
		vec3_t lemin(0,0,0);
		vec3_t lemax(0,0,0);
	/*glPushMatrix ();
		glTranslatef (m_origin[0],  m_origin[1],  m_origin[2]);
 		glRotatef (-m_angles[1],  0, 0, 1);
		glRotatef (0,  0, 1, 0);
		glRotatef (0,  1, 0, 0);  
	*/
	//	glDisable (GL_TEXTURE_2D);
		//glDisable (GL_CULL_FACE);
		//if (g_viewerSettings.transparency < 1.0f && !g_viewerSettings.use3dfx)
		//	glDisable (GL_DEPTH_TEST);
		//else
		//	glEnable (GL_DEPTH_TEST);

		//if (g_viewerSettings.use3dfx)
		//	glColor4f (1, 0, 0, 0.2f);
		//else
		//	glColor4f (1, 0, 0, 0.5f);
		

		/*glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
		glEnable (GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		*/
		nb_boite=m_pstudiohdr->numhitboxes;
		mstudiobbox_t *pbboxes = (mstudiobbox_t *) ((byte *) m_pstudiohdr + m_pstudiohdr->hitboxindex);
		int i;
		
		for (i = 0; i < m_pstudiohdr->numhitboxes; i++)
		{
			vec3_t v[8], v2[8], bbmin, bbmax;

			VectorCopy (pbboxes[i].bbmin, bbmin);
			VectorCopy (pbboxes[i].bbmax, bbmax);

			v[0][0] = bbmin[0];
			v[0][1] = bbmax[1];
			v[0][2] = bbmin[2];

			v[1][0] = bbmin[0];
			v[1][1] = bbmin[1];
			v[1][2] = bbmin[2];

			v[2][0] = bbmax[0];
			v[2][1] = bbmax[1];
			v[2][2] = bbmin[2];

			v[3][0] = bbmax[0];
			v[3][1] = bbmin[1];
			v[3][2] = bbmin[2];

			v[4][0] = bbmax[0];
			v[4][1] = bbmax[1];
			v[4][2] = bbmax[2];

			v[5][0] = bbmax[0];
			v[5][1] = bbmin[1];
			v[5][2] = bbmax[2];

			v[6][0] = bbmin[0];
			v[6][1] = bbmax[1];
			v[6][2] = bbmax[2];

			v[7][0] = bbmin[0];
			v[7][1] = bbmin[1];
			v[7][2] = bbmax[2];

			VectorTransform2 (v[0], g_bonetransform[pbboxes[i].bone], v2[0]);
			VectorTransform2 (v[1], g_bonetransform[pbboxes[i].bone], v2[1]);
			VectorTransform2 (v[2], g_bonetransform[pbboxes[i].bone], v2[2]);
			VectorTransform2 (v[3], g_bonetransform[pbboxes[i].bone], v2[3]);
			VectorTransform2 (v[4], g_bonetransform[pbboxes[i].bone], v2[4]);
			VectorTransform2 (v[5], g_bonetransform[pbboxes[i].bone], v2[5]);
			VectorTransform2 (v[6], g_bonetransform[pbboxes[i].bone], v2[6]);
			VectorTransform2 (v[7], g_bonetransform[pbboxes[i].bone], v2[7]);
			
			lemin[0]=100000;
			lemin[1]=100000;
			lemin[2]=100000;
			lemax[0]=-100000;
			lemax[1]=-100000;
			lemax[2]=-100000;
			float ang=(-m_angles[1])*(Q_PI/180);
			for (int j = 0; j < 8; j++)
			{

				poboite[i][j]=v2[j];
				vec3_t inter;
				inter=v2[j];
				poboite[i][j][1] = inter[1]*cos(ang) + inter[0]*sin(ang);  
				poboite[i][j][0] = -inter[1]*sin(ang) + inter[0]*cos(ang);  
				poboite[i][j]=(vec3_t)m_origin + poboite[i][j];
			
				for (int k = 0; k < 3; k++)
				{
					if (lemin[k]>poboite[i][j][k]) {lemin[k]=poboite[i][j][k];}
					if (lemax[k]<poboite[i][j][k]) {lemax[k]=poboite[i][j][k];}
				}
			}


			vec3_t ret;
			float leret[3];
			if (raybox(lemin,lemax,src,normal,leret)==true)
			{
				ret[0]=leret[0];
				ret[1]=leret[1];
				ret[2]=leret[2];
				vec3_t ret2 = src-ret;
				float t=ret2.len();
				if (t>0 && t<nearest)
				{
					pos_touche=ret;
					nearest=t;
					id_nearest=i;

				}
			}
			
			/*vec3_t pos=(bbmax+bbmin)/2; // centre
			vec3_t pos2;
			VectorTransform2 (pos, g_bonetransform[pbboxes[i].bone], pos2);
			vec3_t inter;
			inter=pos2;
			pos2[1] = inter[1]*cos(ang) + inter[0]*sin(ang);  
			pos2[0] = -inter[1]*sin(ang) + inter[0]*cos(ang);  
			pos2=(vec3_t)m_origin + pos2;
			
			
			vec3_t v_rayon=(bbmax-bbmin); // centre
			float rayon=v_rayon.len();
			float t=intersect_sphere (src,normal,pos2,rayon);
			if (t>0 && t<nearest)
			{
				nearest=t;
				id_nearest=i;

			}
			*/
			
			
			/*vec3_t pos2=(lemax+lemin)/2;
			vec3_t v_rayon=(lemax-lemin);
			float rayon=v_rayon.len();
			float t=intersect_sphere (src,normal,pos2,rayon);
			if (t>0 && t<nearest)
			{
				nearest=t;
				id_nearest=i;

			}*/

			/*drawBox (v2);
			vec3_t pos=(bbmax+bbmin)/2; // centre
			vec3_t pos2;*/
			
		//	VectorTransform2 (pos, g_bonetransform[pbboxes[i].bone], pos2);
			
			/*pos2=pos2+m_origin;
			vec3_t v_rayon=(bbmax-bbmin); // centre
			float rayon=v_rayon.len();
			float t=intersect_sphere (src,normal,pos2,rayon);
			if (t>0 && t<nearest)
			{
				nearest=t;
				id_nearest=i;

			}*/
			

		}
		idgroupe_touche=-1;
		if (id_nearest!=-1)
		{
			idgroupe_touche=pbboxes[id_nearest].group ;
			//		drawBox (poboite[id_nearest]);
		}
		/*glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
		glEnable (GL_TEXTURE_2D);
		glEnable (GL_CULL_FACE);
		glEnable (GL_DEPTH_TEST);
		glPopMatrix ();*/
		//glShadeModel (GL_SMOOTH);
		
		idboite_touche=id_nearest;
		return id_nearest;


}
float
Model_MDL::intersect_sphere(const vec3_t& r, //point E
                 const vec3_t& rv, // direction
                 const vec3_t& s, // centre du cercle
                 float sr) // rayon
{ ///http://www.acm.org/crossroads/xrds3-4/raytracing.html
		//sr = rayon
	vec3_t q = s-r;   
    float c = q.len();   //q vecteur entre origine et E 
						//c = distance entre origine et E
	float v = q.dot(rv);    // rv=EO=   q=s-r=V
	//v= distance entre r et le point d'intersection perpendiculaire a la normal

    float d = sr*sr - (c*c-v*v); // sr : rayon     c:
	//d = sqrt( sr^2 - (c^2 - v^2))
    
    if(d < 0.0f) return -1.0f;
    return v-sqrt(d);
}

void Model_MDL::drawBox (vec3_t *v)
{
	glBegin (GL_QUAD_STRIP);
	for (int i = 0; i < 10; i++)
		glVertex3fv (v[i & 7]);
	glEnd ();
	
	glBegin  (GL_QUAD_STRIP);
	glVertex3fv (v[6]);
	glVertex3fv (v[0]);
	glVertex3fv (v[4]);
	glVertex3fv (v[2]);
	glEnd ();

	glBegin  (GL_QUAD_STRIP);
	glVertex3fv (v[1]);
	glVertex3fv (v[7]);
	glVertex3fv (v[3]);
	glVertex3fv (v[5]);
	glEnd ();

}
void Model_MDL::CalcBoneAdj( )
{
	int					i, j;
	float				value;
	mstudiobonecontroller_t *pbonecontroller;
	
	pbonecontroller = (mstudiobonecontroller_t *)((byte *)m_pstudiohdr + m_pstudiohdr->bonecontrollerindex);

	for (j = 0; j < m_pstudiohdr->numbonecontrollers; j++)
	{
		i = pbonecontroller[j].index;
		if (i <= 3)
		{
			// check for 360% wrapping
			if (pbonecontroller[j].type & STUDIO_RLOOP)
			{
				value = m_controller[i] * (360.0f/256.0f) + pbonecontroller[j].start;
			}
			else 
			{
				value = m_controller[i] / 255.0f;
				if (value < 0) value = 0;
				if (value > 1.0f) value = 1.0f;
				value = (1.0f - value) * pbonecontroller[j].start + value * pbonecontroller[j].end;
			}
		}
		else
		{
			//value = m_controller[i] / 255.0f;
			value = m_mouth / 64.0f;
			if (value > 1.0f) value = 1.0f;
			value = (1.0f - value) * pbonecontroller[j].start + value * pbonecontroller[j].end;
		}
		switch(pbonecontroller[j].type & STUDIO_TYPES)
		{
		case STUDIO_XR:
		case STUDIO_YR:
		case STUDIO_ZR:
			m_adj[j] = value * (Q_PI / 180.0f);
			break;
		case STUDIO_X:
		case STUDIO_Y:
		case STUDIO_Z:
			m_adj[j] = value;
			break;
		}
	}
}


void Model_MDL::CalcBoneQuaternion( int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *q )
{
	int					j, k;
	v4_t				q1, q2;
	v3_t				angle1, angle2;
	mstudioanimvalue_t	*panimvalue;

	for (j = 0; j < 3; j++)
	{
		if (panim->offset[j+3] == 0)
		{
			angle2[j] = angle1[j] = pbone->value[j+3]; // default;
		}
		else
		{
			panimvalue = (mstudioanimvalue_t *)((byte *)panim + panim->offset[j+3]);
			k = frame;
			while (panimvalue->num.total <= k)
			{
				k -= panimvalue->num.total;
				panimvalue += panimvalue->num.valid + 1;
			}
			// Bah, missing blend!
			if (panimvalue->num.valid > k)
			{
				angle1[j] = panimvalue[k+1].value;

				if (panimvalue->num.valid > k + 1)
				{
					angle2[j] = panimvalue[k+2].value;
				}
				else
				{
					if (panimvalue->num.total > k + 1)
						angle2[j] = angle1[j];
					else
						angle2[j] = panimvalue[panimvalue->num.valid+2].value;
				}
			}
			else
			{
				angle1[j] = panimvalue[panimvalue->num.valid].value;
				if (panimvalue->num.total > k + 1)
				{
					angle2[j] = angle1[j];
				}
				else
				{
					angle2[j] = panimvalue[panimvalue->num.valid + 2].value;
				}
			}
			angle1[j] = pbone->value[j+3] + angle1[j] * pbone->scale[j+3];
			angle2[j] = pbone->value[j+3] + angle2[j] * pbone->scale[j+3];
		}

		if (pbone->bonecontroller[j+3] != -1)
		{
			angle1[j] += m_adj[pbone->bonecontroller[j+3]];
			angle2[j] += m_adj[pbone->bonecontroller[j+3]];
		}
	}

	if (!VectorCompare( angle1, angle2 ))
	{
		AngleQuaternion( angle1, q1 );
		AngleQuaternion( angle2, q2 );
		QuaternionSlerp( q1, q2, s, q );
	}
	else
	{
		AngleQuaternion( angle1, q );
	}
}


void Model_MDL::CalcBonePosition( int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *pos )
{
	int					j, k;
	mstudioanimvalue_t	*panimvalue;

	for (j = 0; j < 3; j++)
	{
		pos[j] = pbone->value[j]; // default;
		if (panim->offset[j] != 0)
		{
			panimvalue = (mstudioanimvalue_t *)((byte *)panim + panim->offset[j]);
			
			k = frame;
			// find span of values that includes the frame we want
			while (panimvalue->num.total <= k)
			{
				k -= panimvalue->num.total;
				panimvalue += panimvalue->num.valid + 1;
			}
			// if we're inside the span
			if (panimvalue->num.valid > k)
			{
				// and there's more data in the span
				if (panimvalue->num.valid > k + 1)
				{
					pos[j] += (panimvalue[k+1].value * (1.0f - s) + s * panimvalue[k+2].value) * pbone->scale[j];
				}
				else
				{
					pos[j] += panimvalue[k+1].value * pbone->scale[j];
				}
			}
			else
			{
				// are we at the end of the repeating values section and there's another section with data?
				if (panimvalue->num.total <= k + 1)
				{
					pos[j] += (panimvalue[panimvalue->num.valid].value * (1.0f - s) + s * panimvalue[panimvalue->num.valid + 2].value) * pbone->scale[j];
				}
				else
				{
					pos[j] += panimvalue[panimvalue->num.valid].value * pbone->scale[j];
				}
			}
		}
		if (pbone->bonecontroller[j] != -1)
		{
			pos[j] += m_adj[pbone->bonecontroller[j]];
		}
	}
}


void Model_MDL::CalcRotations ( v3_t *pos, v4_t *q, mstudioseqdesc_t *pseqdesc, mstudioanim_t *panim, float f )
{
	int					i;
	int					frame;
	mstudiobone_t		*pbone;
	float				s;

	frame = (int)f;
	s = (f - frame);

	// add in programatic controllers
	CalcBoneAdj( );

	pbone		= (mstudiobone_t *)((byte *)m_pstudiohdr + m_pstudiohdr->boneindex);
	for (i = 0; i < m_pstudiohdr->numbones; i++, pbone++, panim++) 
	{
		CalcBoneQuaternion( frame, s, pbone, panim, q[i] );
		CalcBonePosition( frame, s, pbone, panim, pos[i] );
	}

	if (pseqdesc->motiontype & STUDIO_X)
		pos[pseqdesc->motionbone][0] = 0.0;
	if (pseqdesc->motiontype & STUDIO_Y)
		pos[pseqdesc->motionbone][1] = 0.0;
	if (pseqdesc->motiontype & STUDIO_Z)
		pos[pseqdesc->motionbone][2] = 0.0;
}


mstudioanim_t * Model_MDL::GetAnim( mstudioseqdesc_t *pseqdesc )
{
	mstudioseqgroup_t	*pseqgroup;
	pseqgroup = (mstudioseqgroup_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqgroupindex) + pseqdesc->seqgroup;

	if (pseqdesc->seqgroup == 0)
	{
		return (mstudioanim_t *)((byte *)m_pstudiohdr + pseqgroup->data + pseqdesc->animindex);
	}

	return (mstudioanim_t *)((byte *)m_panimhdr[pseqdesc->seqgroup] + pseqdesc->animindex);
}
mstudioevent_t * Model_MDL::GetEvents( mstudioseqdesc_t *pseqdesc )
{
	mstudioseqgroup_t	*pseqgroup;
	pseqgroup = (mstudioseqgroup_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqgroupindex) + pseqdesc->seqgroup;

	//if (pseqdesc->seqgroup == 0)
	//{
		return (mstudioevent_t *)((byte *)m_pstudiohdr + pseqgroup->data + pseqdesc->eventindex);
	//}

	//return (mstudioanim_t *)((byte *)m_panimhdr[pseqdesc->seqgroup] + pseqdesc->animindex);
}

void Model_MDL::SlerpBones( v4_t q1[], v3_t pos1[], v4_t q2[], v3_t pos2[], float s )
{
	int			i;
	v4_t		q3;
	float		s1;

	if (s < 0) s = 0;
	else if (s > 1.0f) s = 1.0f;

	s1 = 1.0f - s;

	for (i = 0; i < m_pstudiohdr->numbones; i++)
	{
		QuaternionSlerp( q1[i], q2[i], s, q3 );
		q1[i][0] = q3[0];
		q1[i][1] = q3[1];
		q1[i][2] = q3[2];
		q1[i][3] = q3[3];
		pos1[i][0] = pos1[i][0] * s1 + pos2[i][0] * s;
		pos1[i][1] = pos1[i][1] * s1 + pos2[i][1] * s;
		pos1[i][2] = pos1[i][2] * s1 + pos2[i][2] * s;
	}
}


void Model_MDL::AdvanceFrame( float dt,int qui )
{
	int lemax;
	int lemin;
	float avant;
	float le_fps_ori;;
	if (qui==0)// l'aciton ou le deplacement =qui 
	{
		m_sequence=seq1;
		m_frame=fps1;
		le_fps_ori=	le_fps_ori_seq1;

	}
	else
	{
		m_sequence=seq2;
		m_frame=fps2;
		le_fps_ori=	le_fps_ori_seq2;
	}
	avant=m_frame;
	mstudioseqdesc_t	*pseqdesc;
	pseqdesc = (mstudioseqdesc_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqindex) + m_sequence;
	lemax=pseqdesc->numframes;
	le_fps_ori=pseqdesc->fps;

	lemin=0;
	int res;
	res=strcmp(pseqdesc->label,"shoot1");
	//res=res || strcmp(pseqdesc->label,"pullpin");
	if (res==0)
	{
		if (fps_weapon==-1)
		{
			lemax=pseqdesc->numframes;

		}
		else
		{
			lemax=fps_weapon;
		}
			
		le_fps_ori=pseqdesc->fps*multi;
	}
	//res=strncmp( pseqdesc->label, "ref_shoot" , 9 );
	//if (res==0)
	//if (strcmp(pseqdesc->label,"shoot")==0)
	/*{
		le_fps_ori=pseqdesc->fps;
		//if (m_frame>=pseqdesc->numframes-1)
			//lemin=29;
		if (fps_weapon==-1)
		{
			le_fps_ori=pseqdesc->fps;
		}
		else
		{
			le_fps_ori=fps_weapon;
		}
	}
	else
	{*/
	//}
	//if (dt > 0.1f) dt = 0.1f;
	m_frame += dt * le_fps_ori;//pseqdesc->fps;
	//m_frame += dt * le_fps_ori;

	



	if (lemax <= 1) 
	{
		m_frame = 0;


	}
	else
	{
		// TODO: need better algorithm for lower end
		// wrap
		if (m_frame <= lemin)
		{
			//m_frame = pseqdesc->numframes - m_frame - 1.5f;
			//if (res==0)
			//	m_frame = pseqdesc->numframes-2 ;
			//else
				m_frame = lemax-1 ;

		} 
		else
		{
			m_frame -= (int)(m_frame / (lemax - 1)) * (lemax - 1);
			/*if (m_frame <avant)
			{
				m_frame = pseqdesc->numframes-1 ;
			}*/
		}
	}

	AniDepFini=false;
	//if ((m_frame>=pseqdesc->numframes-1) && (pseqdesc->flags=!0))
	/*if (pseqdesc->numevents >=1)
	{
		//mstudioseqdesc_t	*pseqdesc;

		//pseqdesc = (mstudioseqdesc_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqindex);
		
		mstudioevent_t	*zeevent;
		//zeevent = (mstudioevent_t *)((byte *)pseqdesc + pseqdesc->eventindex);
		zeevent= GetEvents (pseqdesc);
		int i;
		//istringstream c;
		//c=(istringstream) zeevent->options;
		//zeevent->options >> i;
		sscanf(zeevent->options, "%d", &i); 

		if (m_frame>=pseqdesc->fps)
		{
			AniDepFini=true;
		}
	}*/
	
	
	if (avant>m_frame )
	{
		
		if (qui==0)
		{
			AniActFini=true;
		}
		else
		{
			
			/*res=strcmp(pseqdesc->label,"start_reload");
			if (res==0)
			{
				AniDepFini=false;
				SetSequence2(TEXT("after_reload"),1);
				m_frame=0;
			}
			else
			{*/
				AniDepFini=true;
				//if (res!=0)
					if (qui!=3){m_frame = lemax-1 ;}
				//if (res==0)
					//m_frame = 29.00f ;
			//}

		}
	}
	else
	{
		if (qui==0)
		{
			AniActFini=false;
		}
		else
		{
			AniDepFini=false;
		}
	}
	if (qui==0)
	{
		seq1=m_sequence;
		fps1=m_frame;
	}
	else
	{
		seq2=m_sequence;
		fps2=m_frame;
	}

}

void Model_MDL::SetUpBonesIni ( void )
{
	int					i;

	mstudiobone_t		*pbones;
	mstudioseqdesc_t	*pseqdesc;
	mstudioanim_t		*panim;

	static v3_t		pos[MAXSTUDIOBONES];
	float				bonematrix[3][4];
	static v4_t		q[MAXSTUDIOBONES];

	static v3_t		pos2[MAXSTUDIOBONES];
	static v4_t		q2[MAXSTUDIOBONES];
	static v3_t		pos3[MAXSTUDIOBONES];
	static v4_t		q3[MAXSTUDIOBONES];
	static v3_t		pos4[MAXSTUDIOBONES];
	static v4_t		q4[MAXSTUDIOBONES];


	if (m_sequence >=  m_pstudiohdr->numseq) {
		m_sequence = 0;
	}

	pseqdesc = (mstudioseqdesc_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqindex) + m_sequence;

	
	panim = GetAnim( pseqdesc );
	CalcRotations( pos, q, pseqdesc, panim, m_frame );

	if (pseqdesc->numblends > 1)
	{
		float				s;

		panim += m_pstudiohdr->numbones;
		CalcRotations( pos2, q2, pseqdesc, panim, m_frame );
		s = m_blending[0] / 255.0f;

		SlerpBones( q, pos, q2, pos2, s );

		if (pseqdesc->numblends == 4)
		{
			panim += m_pstudiohdr->numbones;
			CalcRotations( pos3, q3, pseqdesc, panim, m_frame );

			panim += m_pstudiohdr->numbones;
			CalcRotations( pos4, q4, pseqdesc, panim, m_frame );

			s = m_blending[0] / 255.0f;
			SlerpBones( q3, pos3, q4, pos4, s );

			s = m_blending[1] / 255.0f;
			SlerpBones( q, pos, q3, pos3, s );
		}
	}

	pbones = (mstudiobone_t *)((byte *)m_pstudiohdr + m_pstudiohdr->boneindex);

	for (i = 0; i < m_pstudiohdr->numbones; i++) {
		QuaternionMatrix( q[i], bonematrix );

		bonematrix[0][3] = pos[i][0];
		bonematrix[1][3] = pos[i][1];
		bonematrix[2][3] = pos[i][2];

		if (pbones[i].parent == -1) {
			memcpy(g_bonetransform[i], bonematrix, sizeof(float) * 12);
		} 
		else {
			R_ConcatTransforms (g_bonetransform[pbones[i].parent], bonematrix, g_bonetransform[i]);
		}
	}
}
void Model_MDL::SetUpBones ( int qui ,int lerendu)
{
	int					i;
	mstudiobone_t		*pbones;
	mstudioseqdesc_t	*pseqdesc;
	mstudioanim_t		*panim;

	static v3_t		pos[MAXSTUDIOBONES];
	float				bonematrix[3][4];
	float				bonematrix2[3][4];
	float				bonematrix3[3][4];
	static v4_t		q[MAXSTUDIOBONES];

	static v3_t		pos2[MAXSTUDIOBONES];
	static v4_t		q2[MAXSTUDIOBONES];
	static v3_t		pos3[MAXSTUDIOBONES];
	static v4_t		q3[MAXSTUDIOBONES];
	static v3_t		pos4[MAXSTUDIOBONES];
	static v4_t		q4[MAXSTUDIOBONES];


	if (m_sequence >=  m_pstudiohdr->numseq) {
		m_sequence = 0;
	}
	
	pseqdesc = (mstudioseqdesc_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqindex) + m_sequence;
	//if (pseqdesc->numblends > 1)
		//pseqdesc->animindex =5;

	panim = GetAnim( pseqdesc );


	CalcRotations( pos, q, pseqdesc, panim, m_frame );

	/*if (pseqdesc->numblends ==9)
	{
		float				s;

		panim += m_pstudiohdr->numbones;
		CalcRotations( pos2, q2, pseqdesc, panim, m_frame );
		s = 4 / 255.0f;

		SlerpBones( q, pos, q2, pos2, s );

		/*if (pseqdesc->numblends == 4)
		{
			panim += m_pstudiohdr->numbones;
			CalcRotations( pos3, q3, pseqdesc, panim, m_frame );

			panim += m_pstudiohdr->numbones;
			CalcRotations( pos4, q4, pseqdesc, panim, m_frame );

			s = m_blending[0] / 255.0f;
			SlerpBones( q3, pos3, q4, pos4, s );

			s = m_blending[1] / 255.0f;
			SlerpBones( q, pos, q3, pos3, s );
		}
	}*/
	pbones = (mstudiobone_t *)((byte *)m_pstudiohdr + m_pstudiohdr->boneindex);
	
	int trouve=0;
	int id_pelvis=0; //vs2005 debug

	for (i = 0; i < m_pstudiohdr->numbones; i++) {
		//if (pbones[i].parent == -1) {continue;}
		if (strcmp(pbones[i].name,"Bip01 Pelvis")==0)
		{
			id_pelvis=i;
			if (qui==1) {continue;}
		}
		if (lie==0){

			if ((pbones[i].parent == id_pelvis) )
			{
				trouve ++;
				if (qui==1)
				{
					if (trouve==2){
						break;
					}
				}
				else
				{
					
				}
			}
			if ((trouve==1) && (qui==0)){
				continue;			
			}
		}
		v3_t ang;
		QuaternionMatrix( q[i], bonematrix );
		bonematrix[0][3] = pos[i][0];
		bonematrix[1][3] = pos[i][1];
		bonematrix[2][3] = pos[i][2];


		/*	if ((pbones[i].parent == id_pelvis) && (trouve>=2) )
			
			{
				ang[2]=0;//-ang_dos*(Q_PI*2 / 360);
				ang[0]=3*m_angles[2]*(Q_PI*2 / 360);
				//ang[0]=-3*m_angles[2];
				ang[1]=0;// -ang_dos*(Q_PI*2 / 360);
				AngleQuaternion (ang,q[i]);
				//AngleIMatrix(ang,bonematrix);
				QuaternionMatrix( q[i], bonematrix2 );
				bonematrix2[0][3] = pos[i][0];
				bonematrix2[1][3] = pos[i][1];
				bonematrix2[2][3] = pos[i][2];
				memcpy(bonematrix3,bonematrix, sizeof(float) * 12);
				R_ConcatTransforms (bonematrix2, bonematrix3, bonematrix);
				//memcpy(bonematrix,bonematrix3, sizeof(float) * 12);
			}
				
		bonematrix[0][3] = pos[i][0];
		bonematrix[1][3] = pos[i][1];
		bonematrix[2][3] = pos[i][2];
			*/


		if ( (strcmp(pbones[i].name,"Bip01 Spine1")==0) ||(strcmp(pbones[i].name,"Bip01 Spine2")==0)||(strcmp(pbones[i].name,"Bip01 Spine")==0))
		//if ((strcmp(pbones[i].name,"Bip01 Spine")==0))
		{
			
			
			/*ang[0]=0;
			ang[1]=0;
			ang[2]=10;
			ang[2]= ang_dos ;
			AngleIMatrix(ang,bonematrix2);
			//AngleQuaternion (ang,q[i]);
			memcpy(bonematrix3,bonematrix, sizeof(float) * 12);
			R_ConcatTransforms (bonematrix3, bonematrix2, bonematrix);*/

			//if (m_angles[2]!=0)
			//{
			/*--------------------------------
			ang[2]=-ang_dos*(Q_PI/ 180);
			ang[0]=0;
			ang[1]=0;// -ang_dos*(Q_PI*2 / 360);
			AngleQuaternion (ang,q[i]);
			QuaternionMatrix( q[i], bonematrix2 );
			memcpy(bonematrix3,bonematrix, sizeof(float) * 12);
			R_ConcatTransforms (bonematrix3, bonematrix2, bonematrix);
			------------------------------------*/
			//if ((strcmp(pbones[i].name,"Bip01 Spine")==0) || (strcmp(pbones[i].name,"Bip01 Spine1")==0) || (strcmp(pbones[i].name,"Bip01 Spine2")==0))
			
			float an1;
			float an2;
			//if ((strcmp(pbones[i].name,"Bip01 Spine3")==0) || (strcmp(pbones[i].name,"Bip01 Spine1")==0) || (strcmp(pbones[i].name,"Bip01 Spine2")==0))
			if ((strcmp(pbones[i].name,"Bip01 Spine")==0))
			{
				an1=-ang_dos*(Q_PI/ 180);
				an2=(3*m_angles[2])*(Q_PI/180);
			}
			else
			{
				an1=-ang_dos*(Q_PI/ 180);
				an2=0;

			}
			ang[2]=an1;//-ang_dos*(Q_PI*2 / 360);
			ang[0]=an2;//*(Q_PI / 180);
			ang[1]=0;// -ang_dos*(Q_PI*2 / 360);

			AngleQuaternion (ang,q[i]);
			//AngleIMatrix(ang,bonematrix);
			QuaternionMatrix( q[i], bonematrix2 );
			bonematrix2[0][3] = pos[i][0];
			bonematrix2[1][3] = pos[i][1];
			bonematrix2[2][3] = pos[i][2];
			memcpy(bonematrix3,bonematrix, sizeof(float) * 12);
			R_ConcatTransforms (bonematrix2, bonematrix3, bonematrix);
			//memcpy(bonematrix,bonematrix3, sizeof(float) * 12);
			

			bonematrix[0][3] = pos[i][0];
			bonematrix[1][3] = pos[i][1];
			bonematrix[2][3] = pos[i][2];

		}

		


		/*bonematrix[0][3] = pos[i][0];
		bonematrix[1][3] = pos[i][1];
		bonematrix[2][3] = pos[i][2];*/


		if ((pbones[i].parent == -1) && (lie==0)){
		
			/*ang[1]=0;
			ang[0]=0;
			ang[2]=-m_angles[1]*(Q_PI / 180);;
			
			//AngleIMatrix(ang,bonematrix2);
			static v4_t qq;
			AngleQuaternion (ang,qq);
			QuaternionMatrix( q[i], bonematrix );
			QuaternionMatrix( qq, bonematrix2);
			bonematrix[0][3] = pos[i][0];
			bonematrix[1][3] = pos[i][1];
			bonematrix[2][3] = pos[i][2];
			//AngleIMatrix(ang,bonematrix);
			//memcpy(bonematrix3,bonematrix, sizeof(float) * 12);
			R_ConcatTransforms (bonematrix, bonematrix2, bonematrix3);

			//AngleQuaternion (ang,q[i]);
			//QuaternionMatrix( q[i], bonematrix2 );
			///memcpy(bonematrix3,bonematrix, sizeof(float) * 12);
			//R_ConcatTransforms (bonematrix3, bonematrix2, bonematrix);
			//bonematrix[0][3] = pos[i][0];
			bonematrix[1][3] = pos[i][1];
			bonematrix[2][3] = pos[i][2];
			memcpy(g_bonetransform[i], bonematrix3, sizeof(float) * 12);
			*/

			if (lerendu==0)
			{
				ang[0]=0;
				ang[1]=0;
				ang[2]=-m_angles[1]* (Q_PI*2 / 360);
				
				//AngleIMatrix(ang,bonematrix2);
				AngleQuaternion (ang,q[i]);
				QuaternionMatrix( q[i], bonematrix );
				bonematrix[0][3] = pos[i][0];
				bonematrix[1][3] = pos[i][1];
				bonematrix[2][3] = pos[i][2];

				//memcpy(bonematrix3,bonematrix, sizeof(float) * 12);
				//R_ConcatTransforms (bonematrix3, bonematrix2, bonematrix);
			}
	
			memcpy(g_bonetransform[i], bonematrix, sizeof(float) * 12);


		} 
		else {
			if ((lie==1) && (strcmp(pbones[i].name,"Bip01 R Hand")==0))
			{
				//R_ConcatTransforms (matrice_pere, bonematrix, g_bonetransform[i]);
			memcpy(g_bonetransform[i],matrice_pere, sizeof(float) * 12);


			}
			else
			{
				R_ConcatTransforms (g_bonetransform[pbones[i].parent], bonematrix, g_bonetransform[i]);
			
			}

		
		}
		if (lie==0)
		{
			if (strcmp(pbones[i].name,"Bip01 R Hand")==0)
			//if (pbones[i].name =="Bip01 R Hand")
			{
				
				memcpy(matrice_pere, g_bonetransform[i], sizeof(float) * 12);

			}
		}		
		//"Bip01 Head"
		if (strcmp(pbones[i].name,"Bip01 Head")==0)
		{
 			tete[0]=g_bonetransform[i][0][3] ;
			tete[1]=g_bonetransform[i][1][3] ;
			tete[2]=g_bonetransform[i][2][3] ;

		}
		
		if (strcmp(pbones[i].name,"flash")==0)
		{
			
			flash[0]=g_bonetransform[i][0][3] ;
			flash[1]=g_bonetransform[i][1][3] ;
			flash[2]=g_bonetransform[i][2][3] ;
			
		}
	/*	if (strcmp(pbones[pbones[i].parent].name,"Bip01 R Hand")==0)
		{
			
			tete[0]=g_bonetransform[i][0][3] ;
			tete[1]=g_bonetransform[i][1][3] ;
			tete[2]=g_bonetransform[i][2][3] ;


		}*/

		
	}

}



/*
================
Model_MDL::TransformFinalVert
================
*/
void Model_MDL::Lighting (float *lv, int bone, int flags, v3_t normal)
{
	float 	illum;
	float	lightcos;
 

	illum = (float) g_ambientlight;

	if (flags & STUDIO_NF_FLATSHADE)
	{
		illum += g_shadelight * 0.8f;
	} 
	else 
	{
		float r;
		lightcos = DotProduct (normal, g_blightvec[bone]); // -1 colinear, 1 opposite
		

		if (lightcos > 1.0f)
			lightcos = 1.0f;

		illum += g_shadelight;

		r = g_lambert;
		if (r <= 1.0f) r = 1.0f;

		lightcos = (lightcos + (r - 1.0f)) / r; 		// do modified hemispherical lighting
		if (lightcos > 0) 
		{
			illum -= g_shadelight * lightcos; 
		}
		if (illum <= 0)
			illum = 0;
	}

	if (illum > 255) 
		illum = 255;
	*lv = illum / 255;	// Light from 0 to 1.0
}


void Model_MDL::Chrome (int *pchrome, int bone, v3_t normal)
{
	float n;

	if (g_chromeage[bone] != g_smodels_total)
	{
		// calculate vectors from the viewer to the bone. This roughly adjusts for position
		v3_t chromeupvec;		// g_chrome t vector in world reference frame
		v3_t chromerightvec;	// g_chrome s vector in world reference frame
		v3_t tmp;				// vector pointing at bone in world reference frame
		VectorScale( m_origin, -1, tmp );
		tmp[0] += g_bonetransform[bone][0][3];
		tmp[1] += g_bonetransform[bone][1][3];
		tmp[2] += g_bonetransform[bone][2][3];
		VectorNormalize( tmp );
		CrossProduct( tmp, g_vright, chromeupvec );
		VectorNormalize( chromeupvec );
		CrossProduct( tmp, chromeupvec, chromerightvec );
		VectorNormalize( chromerightvec );

		VectorIRotate( chromeupvec, g_bonetransform[bone], g_chromeup[bone] );
		VectorIRotate( chromerightvec, g_bonetransform[bone], g_chromeright[bone] );

		g_chromeage[bone] = g_smodels_total;
	}

	// calc s coord
	n = DotProduct( normal, g_chromeright[bone] );
	pchrome[0] = (int) ((n + 1.0f) * 32.0f); // FIX: make this a float

	// calc t coord
	n = DotProduct( normal, g_chromeup[bone] );
	pchrome[1] = (int) ((n + 1.0f) * 32.0f); // FIX: make this a float
}


/*
================
Model_MDL::SetupLighting
	set some global variables based on entity position
inputs:
outputs:
	g_ambientlight
	g_shadelight
================
*/
void Model_MDL::SetupLighting ()
{
	int i;
	g_ambientlight = 32;
	g_shadelight = 192.0f;
g_lightvec[0] = 20;
	g_lightvec[1] = 20;
	g_lightvec[2] = 20;
float u[3][4];
u[0][0]=0;
u[0][1]=0;
u[0][2]=m_angles[0];
u[1][0]=0;
u[1][1]=m_angles[1];
u[1][2]=0;
u[2][0]=m_angles[2 ];
u[2][1]=0;
u[2][2]=0;

float angle=6.28f*(fmod(m_angles[1],360)/360);

	g_lightvec[0] = 20.0f+cos(angle)*100.0f;
	g_lightvec[1] = 20.0f+sin(angle)*100.0f;
	g_lightvec[2] = -60.0f;
	
VectorRotate(g_lightvec,matrice_pere,g_lightvec);

	g_lightcolor[0] = 1.0f;
	g_lightcolor[1] = 1.0f;
	g_lightcolor[2] = 1.0f;

	// TODO: only do it for bones that actually have textures
	 
	

	
	for (i = 0; i < m_pstudiohdr->numbones; i++)
	{
		VectorIRotate( g_lightvec, g_bonetransform[i], g_blightvec[i] );
		 
	}
	

	
}


/*
=================
Model_MDL::SetupModel
	based on the body part, figure out which mesh it should be using.
inputs:
	currententity
outputs:
	pstudiomesh
	pmdl
=================
*/

void Model_MDL::SetupModel ( int bodypart )
{
	int index;

	if (bodypart > m_pstudiohdr->numbodyparts)
	{
		bodypart = 0;
	}

	mstudiobodyparts_t   *pbodypart = (mstudiobodyparts_t *)((byte *)m_pstudiohdr + m_pstudiohdr->bodypartindex) + bodypart;

	index = m_bodynum / pbodypart->base;
	index = index % pbodypart->nummodels;

	m_pmodel = (mstudiomodel_t *)((byte *)m_pstudiohdr + pbodypart->modelindex) + index;
}


/*
================
Model_MDL::DrawModel
inputs:
	currententity
	r_entorigin
================
*/
void Model_MDL::Draw(int justelerendu,double lumiere[3] )
{
	int i;

	g_smodels_total++; // render data cache cookie

	g_pxformverts = &g_xformverts[0];
	g_pvlightvalues = &g_lightvalues[0];

	if (m_pstudiohdr->numbodyparts == 0)
		return;
	if (justelerendu!=0)
	{
		glPushMatrix ();
		glTranslatef (m_origin[0],  m_origin[1],  m_origin[2]);
		glRotatef (-m_angles[1],  0, 0, 1);
		glRotatef (0,  0, 1, 0);
		glRotatef (0,  1, 0, 0);
	}
		

	/*else
	{
		glPushMatrix ();
		glTranslatef (m_origin[0],  m_origin[1],  m_origin[2]);
		glRotatef (-m_angles[1],  0, 0, 1);
		glRotatef (0,  0, 1, 0);
		glRotatef (0,  1, 0, 0);

	}*/

    if ((justelerendu==0) || (justelerendu==2) || (justelerendu==5)) 
	{
		//}

		if (lie==0)
		{
			m_sequence=seq1;
			m_frame=fps1;
			SetUpBones(0,justelerendu);
			m_sequence=seq2;
			m_frame=fps2;
			SetUpBones(1,justelerendu);

		}
		else 
		{
			m_sequence=seq1;
			m_frame=fps1;
			SetUpBones(0,justelerendu);
		}
        SetupLighting();
		/*if (lie==0){
			glTranslatef (m_origin[0],  m_origin[1],  m_origin[2]);
		}*/
		if (justelerendu==0) return;
	} 
	
	if (justelerendu!=5) 
	{
		for (i=0 ; i < m_pstudiohdr->numbodyparts ; i++) 
		{
			SetupModel( i );
			DrawPoints( );
		}
	}
//	if (AfficheBoite) calculeboite( );
	//glColor4f(1.0f,1.0f,1.0f,1.0f); // BART fix
		if (justelerendu!=0) 
				glPopMatrix ();


	

}
void Model_MDL::DrawIni(void )
{
	int i;

	g_smodels_total++; // render data cache cookie

	g_pxformverts = &g_xformverts[0];
	g_pvlightvalues = &g_lightvalues[0];

	if (m_pstudiohdr->numbodyparts == 0)
		return;

	glPushMatrix ();
    glTranslatef (m_origin[0],  m_origin[1],  m_origin[2]);
if(!droitier)
{

		glScalef(1,-1,1);
	glCullFace(GL_BACK);
}
    glRotatef (m_angles[1],  0, 0, 1);
    glRotatef (m_angles[0],  0, 1, 0);
    glRotatef (m_angles[2],  1, 0, 0);

	SetUpBonesIni();
	SetupLighting();

//

	for (i=0 ; i < m_pstudiohdr->numbodyparts ; i++) 
	{
		SetupModel( i );
		DrawPoints( );
	}

	glColor4f(1.0f,1.0f,1.0f,1.0f); // BART fix

	glCullFace(GL_FRONT);
	glPopMatrix ();
}
/*
================

================
*/
void Model_MDL::DrawPoints ( )
{
	int					i, j;
	mstudiomesh_t		*pmesh;
	byte				*pvertbone;
	byte				*pnormbone;
	v3_t				*pstudioverts;
	v3_t				*pstudionorms;
	mstudiotexture_t	*ptexture;
	float 				*av;
	float				*lv;
	float				lv_tmp;
	short				*pskinref;
	lv_tmp=0.0f;
	pvertbone = ((byte *)m_pstudiohdr + m_pmodel->vertinfoindex);
	pnormbone = ((byte *)m_pstudiohdr + m_pmodel->norminfoindex);
	ptexture = (mstudiotexture_t *)((byte *)m_ptexturehdr + m_ptexturehdr->textureindex);

	pmesh = (mstudiomesh_t *)((byte *)m_pstudiohdr + m_pmodel->meshindex);

	pstudioverts = (v3_t *)((byte *)m_pstudiohdr + m_pmodel->vertindex);
	pstudionorms = (v3_t *)((byte *)m_pstudiohdr + m_pmodel->normindex);

	pskinref = (short *)((byte *)m_ptexturehdr + m_ptexturehdr->skinindex);
	if (m_skinnum != 0 && m_skinnum < m_ptexturehdr->numskinfamilies)
		pskinref += (m_skinnum * m_ptexturehdr->numskinref);

	for (i = 0; i < m_pmodel->numverts; i++)
	{

		VectorTransform (pstudioverts[i], g_bonetransform[pvertbone[i]], g_pxformverts[i]);
		
	}
/*
	for (i = 0; i < m_pmodel->numnorms; i++)
	{

		
	}
*/
//
// clip and draw all triangles
//

	lv = (float *)g_pvlightvalues;
	for (j = 0; j < m_pmodel->nummesh; j++) 
	{
		int flags;
		flags = ptexture[pskinref[pmesh[j].skinref]].flags;		
		for (i = 0; i < pmesh[j].numnorms; i++, lv += 3, pstudionorms++, pnormbone++)
		{
			if(!optimize)
				Lighting (&lv_tmp, *pnormbone,flags, (float *)pstudionorms);//flags

			// FIX: move this check out of the inner loop
			if (flags & STUDIO_NF_CHROME) {
				int idx = (float (*)[3])lv - g_pvlightvalues;
				Chrome( g_chrome[idx], *pnormbone, (float *)pstudionorms );
			}

			lv[0] = lv_tmp * g_lightcolor[0];
			lv[1] = lv_tmp * g_lightcolor[1];
			lv[2] = lv_tmp * g_lightcolor[2];
		}
	}

	
	for (j = 0; j < m_pmodel->nummesh; j++) 
	{
		float s, t;
		short		*ptricmds;

		pmesh = (mstudiomesh_t *)((byte *)m_pstudiohdr + m_pmodel->meshindex) + j;
		ptricmds = (short *)((byte *)m_pstudiohdr + pmesh->triindex);

		s = 1.0f/(float)ptexture[pskinref[pmesh->skinref]].width;
		t = 1.0f/(float)ptexture[pskinref[pmesh->skinref]].height;

		glEnable(GL_TEXTURE_2D);
		glBindTexture( GL_TEXTURE_2D, ptexture[pskinref[pmesh->skinref]].index );

		if (ptexture[pskinref[pmesh->skinref]].flags & STUDIO_NF_CHROME)
		{
			while (i = *(ptricmds++))
			{
				if (i < 0)
				{
					glBegin( GL_TRIANGLE_FAN );
					i = -i;
				}
				else
				{
					glBegin( GL_TRIANGLE_STRIP );
				}


				for( ; i > 0; i--, ptricmds += 4)
				{
//					int _s = (int)g_chrome[ptricmds[1]][0]*s;
//					int _t = (int)g_chrome[ptricmds[1]][1]*t;

					// FIX: put these in as integer coords, not floats
					glTexCoord2f(g_chrome[ptricmds[1]][0]*s, g_chrome[ptricmds[1]][1]*t);
					
					lv = g_pvlightvalues[ptricmds[1]];
					glColor4f( lv[0], lv[1], lv[2], 1.0f );

					av = g_pxformverts[ptricmds[0]];
					glVertex3f(av[0], av[1], av[2]);
				}
				glEnd( );
			}	
		} 
		else 
		{
			while (i = *(ptricmds++))
			{
				//glPolygonMode(GL_BACK,GL_FILL);	
				if (i < 0)
				{
					glBegin( GL_TRIANGLE_FAN );
					i = -i;
				}
				else
				{
					glBegin( GL_TRIANGLE_STRIP );
				}


				for( ; i > 0; i--, ptricmds += 4)
				{
					//float _s = (float)ptricmds[2]*s;
					//float _t = (float)ptricmds[3]*t;

					// FIX: put these in as integer coords, not floats
					glTexCoord2f(ptricmds[2]*s, ptricmds[3]*t);
					
					lv = g_pvlightvalues[ptricmds[1]];
					glColor4f( lv[0], lv[1], lv[2], 1.0f );

					av = g_pxformverts[ptricmds[0]];
					glVertex3f(av[0], av[1], av[2]);
				}
//glPolygonMode(GL_FRONT,GL_FILL);

				glEnd( );
			}	
		}
	}
}

void Model_MDL::UploadTexture(mstudiotexture_t *ptexture, byte *data, byte *pal)
{
	// unsigned *in, int inwidth, int inheight, unsigned *out,  int outwidth, int outheight;
	int		i, j;
	int		row1[256], row2[256], col1[256], col2[256];
	byte	*pix1, *pix2, *pix3, *pix4;
	byte	*tex, *out;

	// convert texture to power of 2
	int outwidth;
	for (outwidth = 1; outwidth < ptexture->width; outwidth <<= 1)
		;

	if (outwidth > 256)
		outwidth = 256;
int outheight;
	for (outheight = 1; outheight < ptexture->height; outheight <<= 1)
		;

	if (outheight > 256)
		outheight = 256;

	tex = out = (byte *)malloc( outwidth * outheight * 4);

	for (i = 0; i < outwidth; i++)
	{
		col1[i] = (i + 0.25) * (ptexture->width / (float)outwidth);
		col2[i] = (i + 0.75) * (ptexture->width / (float)outwidth);
	}

	for (i = 0; i < outheight; i++)
	{
		row1[i] = (int)((i + 0.25) * (ptexture->height / (float)outheight)) * ptexture->width;
		row2[i] = (int)((i + 0.75) * (ptexture->height / (float)outheight)) * ptexture->width;
	}

	// scale down and convert to 32bit RGB
	for (i=0 ; i<outheight ; i++)
	{
		for (j=0 ; j<outwidth ; j++, out += 4)
		{
			pix1 = &pal[data[row1[i] + col1[j]] * 3];
			pix2 = &pal[data[row1[i] + col2[j]] * 3];
			pix3 = &pal[data[row2[i] + col1[j]] * 3];
			pix4 = &pal[data[row2[i] + col2[j]] * 3];

			out[0] = (pix1[0] + pix2[0] + pix3[0] + pix4[0])>>2;
			out[1] = (pix1[1] + pix2[1] + pix3[1] + pix4[1])>>2;
			out[2] = (pix1[2] + pix2[2] + pix3[2] + pix4[2])>>2;
			out[3] = 0xff;
		}
	}

	glGenTextures(1, &g_texnum);

	glBindTexture( GL_TEXTURE_2D, g_texnum );
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, 
					  outwidth, outheight, 
					  GL_RGBA, GL_UNSIGNED_BYTE, 
					  tex);
	
    
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// ptexture->width = outwidth;
	// ptexture->height = outheight;
	ptexture->index = g_texnum;
	g_texnum++;

	free( tex );
}




studiohdr_t *Model_MDL::LoadModel(const char* modelname )
{
	FILE *fp;
	long size;
	void *buffer;
	
	// load the model
	if( (fp = fopen( modelname, "rb" )) == 0) {
        return 0;
	}

	fseek( fp, 0, SEEK_END );
	size = ftell( fp );
	fseek( fp, 0, SEEK_SET );
	buffer = malloc( size );
	fread( buffer, size, 1, fp );

	int					i;
	byte				*pin;
	studiohdr_t			*phdr;
	mstudiotexture_t	*ptexture;

	pin = (byte *)buffer;
	phdr = (studiohdr_t *)pin;

	ptexture = (mstudiotexture_t *)(pin + phdr->textureindex);
	if (phdr->textureindex != 0)
	{
		for (i = 0; i < phdr->numtextures; i++)
		{
			UploadTexture( &ptexture[i], 
						   pin + ptexture[i].index, 
						   pin + ptexture[i].width * ptexture[i].height + ptexture[i].index );
		}
	}

	// UNDONE: free texture memory

	return (studiohdr_t *)buffer;
}


studioseqhdr_t *Model_MDL::LoadDemandSequences(const char* modelname )
{
	FILE *fp;
	long size;
	void *buffer;

	// load the model
	if( (fp = fopen( modelname, "rb" )) == 0) {
        return 0;
	}

	fseek( fp, 0, SEEK_END );
	size = ftell( fp );
	fseek( fp, 0, SEEK_SET );
	buffer = malloc( size );
	fread( buffer, size, 1, fp );

	fclose( fp );

	return (studioseqhdr_t *)buffer;
}

void Model_MDL::Cleanup(void)
{
if (m_pstudiohdr )
delete m_pstudiohdr;

}

void Model_MDL::Load(const char *name )
{
	
	
	m_pstudiohdr = LoadModel( name );
	yeux[0]=m_pstudiohdr->eyeposition[0];
	yeux[1]=m_pstudiohdr->eyeposition[1];
	yeux[2]=m_pstudiohdr->eyeposition[2];
	
	// preload textures
	if (m_pstudiohdr->numtextures == 0)
	{
		char texturename[256];

		strcpy( texturename, name );
		strcpy( &texturename[strlen(texturename) - 4], "T.mdl" );

		m_ptexturehdr = LoadModel( texturename );
	}
	else
	{
		m_ptexturehdr = m_pstudiohdr;
	}
	
	
	// preload animations
	if (m_pstudiohdr->numseqgroups > 1)
	{
		for (int i = 1; i < m_pstudiohdr->numseqgroups; i++)
		{
			char seqgroupname[256];

			strcpy( seqgroupname, name );
			sprintf( &seqgroupname[strlen(seqgroupname) - 4], "%02d.mdl", i );

			m_panimhdr[i] = LoadDemandSequences( seqgroupname );
		}
	}

	SetSequence( 0 );
	SetController( 0, 0.0 );
	SetController( 1, 0.0 );
	SetController( 2, 0.0 );
	SetController( 3, 0.0 );
	SetMouth( 0 );
	SetBlending( 0, 0.0 );
	SetBlending( 1, 0.0 );


}

////////////////////////////////////////////////////////////////////////

int Model_MDL::SetSequence( int iSequence )
{
	if (iSequence > m_pstudiohdr->numseq)
		iSequence = 0;
	if (iSequence < 0)
		iSequence = m_pstudiohdr->numseq-1;

	m_sequence = iSequence;
	m_frame = 0;

	return m_sequence;
}
int Model_MDL::SetSequence2(const char* name,int qui)
{
	mstudioseqdesc_t	*pseqdesc;
	//mstudioanim_t		*panim;
	m_sequence=0;
	maxframes=1;
	for(int i = 0; i < m_pstudiohdr->numseq; i++) {
		pseqdesc = (mstudioseqdesc_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqindex) + i;
		//panim = GetAnim( pseqdesc );
		//panim->offset 
		if (strcmp(pseqdesc->label,name)==0)
		{
			if (qui==1) 
				maxframes=pseqdesc->numframes;

			int lenlab=strlen(pseqdesc->label);
			int lenname=strlen(name);


			if (qui==0)
			{
				seq1 = i;
				le_fps_ori_seq1=pseqdesc->fps;
			}
			else
			{
				seq2 = i;
				le_fps_ori_seq2=pseqdesc->fps;
			}
			m_sequence=i;
			//m_frame = 0;
			//pseqdesc->animindex =1;
			//m_blending[0]=8;
			//SetBlending (0,90);
			//m_pstudiohdr->numseqgroups =5;
			//m_pstudiohdr->seqindex =5;
			//pseqdesc->seqgroup =5;
			//pseqdesc->blendtype [0]=1;
			//pseqdesc->blendstart [1]=-90;
			//pseqdesc->blendend [1]=90;
			//SetBlending (0,90);
			//SetBlending (1,90);
			break;
		}
	
	}

	return m_sequence;
}

void Model_MDL::ExtractBbox( float *mins, float *maxs )
{
	mstudioseqdesc_t	*pseqdesc;

	pseqdesc = (mstudioseqdesc_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqindex);
	
	mins[0] = pseqdesc[ m_sequence ].bbmin[0];
	mins[1] = pseqdesc[ m_sequence ].bbmin[1];
	mins[2] = pseqdesc[ m_sequence ].bbmin[2];

	maxs[0] = pseqdesc[ m_sequence ].bbmax[0];
	maxs[1] = pseqdesc[ m_sequence ].bbmax[1];
	maxs[2] = pseqdesc[ m_sequence ].bbmax[2];
}



void Model_MDL::GetSequenceInfo( float *pflFrameRate, float *pflGroundSpeed )
{
	mstudioseqdesc_t	*pseqdesc;

	pseqdesc = (mstudioseqdesc_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqindex) + (int)m_sequence;

	if (pseqdesc->numframes > 1)
	{
		*pflFrameRate = 256 * pseqdesc->fps / (pseqdesc->numframes - 1);
		*pflGroundSpeed = (float) sqrt( pseqdesc->linearmovement[0]*pseqdesc->linearmovement[0]+ pseqdesc->linearmovement[1]*pseqdesc->linearmovement[1]+ pseqdesc->linearmovement[2]*pseqdesc->linearmovement[2] );
		*pflGroundSpeed = *pflGroundSpeed * pseqdesc->fps / (pseqdesc->numframes - 1);
	}
	else
	{
		*pflFrameRate = 256.0f;
		*pflGroundSpeed = 0.0f;
	}
}


float Model_MDL::SetController( int iController, float flValue )
{
	mstudiobonecontroller_t	*pbonecontroller = (mstudiobonecontroller_t *)((byte *)m_pstudiohdr + m_pstudiohdr->bonecontrollerindex);

	// find first controller that matches the index
	int i;
	for (i = 0; i < m_pstudiohdr->numbonecontrollers; i++, pbonecontroller++)
	{
		if (pbonecontroller->index == iController)
			break;
	}
	if (i >= m_pstudiohdr->numbonecontrollers)
		return flValue;
	// wrap 0..360 if it's a rotational controller
	if (pbonecontroller->type & (STUDIO_XR | STUDIO_YR | STUDIO_ZR))
	{
		// ugly hack, invert value if end < start
		if (pbonecontroller->end < pbonecontroller->start)
			flValue = -flValue;

		// does the controller not wrap?
		if (pbonecontroller->start + 359.0 >= pbonecontroller->end)
		{
			if (flValue > ((pbonecontroller->start + pbonecontroller->end) / 2.0) + 180)
				flValue = flValue - 360;
			if (flValue < ((pbonecontroller->start + pbonecontroller->end) / 2.0) - 180)
				flValue = flValue + 360;
		}
		else
		{
			if (flValue > 360)
				flValue = flValue - (int)(flValue / 360.0f) * 360.0f;
			else if (flValue < 0)
				flValue = flValue + (int)((flValue / -360.0f) + 1) * 360.0f;
		}
	}

	float setting = 255 * (flValue - pbonecontroller->start) / (pbonecontroller->end - pbonecontroller->start);

	if (setting < 0) setting = 0;
	if (setting > 255) setting = 255;
	m_controller[iController] = (byte) setting;

	return setting * (1.0f / 255.0f) * (pbonecontroller->end - pbonecontroller->start) + pbonecontroller->start;
}


float Model_MDL::SetMouth( float flValue )
{
	mstudiobonecontroller_t	*pbonecontroller = (mstudiobonecontroller_t *)((byte *)m_pstudiohdr + m_pstudiohdr->bonecontrollerindex);

	// find first controller that matches the mouth
	for (int i = 0; i < m_pstudiohdr->numbonecontrollers; i++, pbonecontroller++)
	{
		if (pbonecontroller->index == 4)
			break;
	}

	// wrap 0..360 if it's a rotational controller
	if (pbonecontroller->type & (STUDIO_XR | STUDIO_YR | STUDIO_ZR))
	{
		// ugly hack, invert value if end < start
		if (pbonecontroller->end < pbonecontroller->start)
			flValue = -flValue;

		// does the controller not wrap?
		if (pbonecontroller->start + 359.0 >= pbonecontroller->end)
		{
			if (flValue > ((pbonecontroller->start + pbonecontroller->end) / 2.0f) + 180)
				flValue = flValue - 360;
			if (flValue < ((pbonecontroller->start + pbonecontroller->end) / 2.0f) - 180)
				flValue = flValue + 360;
		}
		else
		{
			if (flValue > 360)
				flValue = flValue - (int)(flValue / 360.0f) * 360.0f;
			else if (flValue < 0)
				flValue = flValue + (int)((flValue / -360.0f) + 1) * 360.0f;
		}
	}

	float setting = 64 * (flValue - pbonecontroller->start) / (pbonecontroller->end - pbonecontroller->start);

	if (setting < 0) setting = 0;
	if (setting > 64) setting = 64;
	m_mouth = (byte) setting;

	return setting * (1.0f / 64.0f) * (pbonecontroller->end - pbonecontroller->start) + pbonecontroller->start;
}


float Model_MDL::SetBlending( int iBlender, float flValue )
{
	mstudioseqdesc_t	*pseqdesc;

	pseqdesc = (mstudioseqdesc_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqindex) + (int)m_sequence;

	if (pseqdesc->blendtype[iBlender] == 0)
		return flValue;

	if (pseqdesc->blendtype[iBlender] & (STUDIO_XR | STUDIO_YR | STUDIO_ZR))
	{
		// ugly hack, invert value if end < start
		if (pseqdesc->blendend[iBlender] < pseqdesc->blendstart[iBlender])
			flValue = -flValue;

		// does the controller not wrap?
		if (pseqdesc->blendstart[iBlender] + 359.0 >= pseqdesc->blendend[iBlender])
		{
			if (flValue > ((pseqdesc->blendstart[iBlender] + pseqdesc->blendend[iBlender]) / 2.0) + 180)
				flValue = flValue - 360;
			if (flValue < ((pseqdesc->blendstart[iBlender] + pseqdesc->blendend[iBlender]) / 2.0) - 180)
				flValue = flValue + 360;
		}
	}

	float setting = 255 * (flValue - pseqdesc->blendstart[iBlender]) / (pseqdesc->blendend[iBlender] - pseqdesc->blendstart[iBlender]);

	if (setting < 0) setting = 0;
	if (setting > 255) setting = 255;

	m_blending[iBlender] = (byte) setting;

	return setting * (1.0f / 255.0f) * (pseqdesc->blendend[iBlender] - pseqdesc->blendstart[iBlender]) + pseqdesc->blendstart[iBlender];
}



int Model_MDL::SetBodygroup( int iGroup, int iValue )
{
	if (iGroup > m_pstudiohdr->numbodyparts)
		return -1;

	mstudiobodyparts_t *pbodypart = (mstudiobodyparts_t *)((byte *)m_pstudiohdr + m_pstudiohdr->bodypartindex) + iGroup;

	int iCurrent = (m_bodynum / pbodypart->base) % pbodypart->nummodels;

	if (iValue >= pbodypart->nummodels)
		return iCurrent;

	m_bodynum = (m_bodynum - (iCurrent * pbodypart->base) + (iValue * pbodypart->base));

	return iValue;
}


int Model_MDL::SetSkin( int iValue )
{
	/*if (iValue < m_pstudiohdr->numskinfamilies)
	{
		return m_skinnum;
	}

	m_skinnum = iValue;

	return iValue;*/

	if (!m_pstudiohdr)
		return 0;

	if (iValue >= m_pstudiohdr->numskinfamilies)
	{
		return m_skinnum;
	}

	m_skinnum = iValue;

	return iValue;

}

//////////////////////////////////////////////////////////////////
// mathlib functions

void Model_MDL::ClearBounds (v3_t mins, v3_t maxs)
{
	mins[0] = mins[1] = mins[2] = 99999;
	maxs[0] = maxs[1] = maxs[2] = -99999;
}

void Model_MDL::AddPointToBounds (v3_t v, v3_t mins, v3_t maxs)
{
	int		i;
	vec_t	val;

	for (i=0 ; i<3 ; i++)
	{
		val = v[i];
		if (val < mins[i])
			mins[i] = val;
		if (val > maxs[i])
			maxs[i] = val;
	}
}

void Model_MDL::AngleMatrix (const v3_t angles, float (*matrix)[4] )
{
	float		angle;
	float		sr, sp, sy, cr, cp, cy;
	
	angle = angles[2] * (Q_PI*2 / 360);
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[1] * (Q_PI*2 / 360);
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[0] * (Q_PI*2 / 360);
	sr = sin(angle);
	cr = cos(angle);

	// matrix = (Z * Y) * X
	matrix[0][0] = cp*cy;
	matrix[1][0] = cp*sy;
	matrix[2][0] = -sp;
	matrix[0][1] = sr*sp*cy+cr*-sy;
	matrix[1][1] = sr*sp*sy+cr*cy;
	matrix[2][1] = sr*cp;
	matrix[0][2] = (cr*sp*cy+-sr*-sy);
	matrix[1][2] = (cr*sp*sy+-sr*cy);
	matrix[2][2] = cr*cp;
	matrix[0][3] = 0.0;
	matrix[1][3] = 0.0;
	matrix[2][3] = 0.0;
}

void Model_MDL::AngleIMatrix (const v3_t angles, float matrix[3][4] )
{
	float		angle;
	float		sr, sp, sy, cr, cp, cy;
	
	angle = angles[2] * (Q_PI*2 / 360);
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[1] * (Q_PI*2 / 360);
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[0] * (Q_PI*2 / 360);
	sr = sin(angle);
	cr = cos(angle);

	// matrix = (Z * Y) * X
	matrix[0][0] = cp*cy;
	matrix[0][1] = cp*sy;
	matrix[0][2] = -sp;
	matrix[1][0] = sr*sp*cy+cr*-sy;
	matrix[1][1] = sr*sp*sy+cr*cy;
	matrix[1][2] = sr*cp;
	matrix[2][0] = (cr*sp*cy+-sr*-sy);
	matrix[2][1] = (cr*sp*sy+-sr*cy);
	matrix[2][2] = cr*cp;
	matrix[0][3] = 0.0;
	matrix[1][3] = 0.0;
	matrix[2][3] = 0.0;
}

void Model_MDL::R_ConcatTransforms (const float in1[3][4], const float in2[3][4], float out[3][4])
{
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
				in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
				in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
				in1[0][2] * in2[2][2];
	out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] +
				in1[0][2] * in2[2][3] + in1[0][3];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
				in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
				in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
				in1[1][2] * in2[2][2];
	out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] +
				in1[1][2] * in2[2][3] + in1[1][3];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
				in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
				in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
				in1[2][2] * in2[2][2];
	out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] +
				in1[2][2] * in2[2][3] + in1[2][3];
}


void Model_MDL::AngleQuaternion( const v3_t angles, v4_t quaternion )
{
	float		angle;
	float		sr, sp, sy, cr, cp, cy;

	// FIXME: rescale the inputs to 1/2 angle
	angle = angles[2] * 0.5f;
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[1] * 0.5f;
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[0] * 0.5f;
	sr = sin(angle);
	cr = cos(angle);

	quaternion[0] = sr*cp*cy-cr*sp*sy; // X
	quaternion[1] = cr*sp*cy+sr*cp*sy; // Y
	quaternion[2] = cr*cp*sy-sr*sp*cy; // Z
	quaternion[3] = cr*cp*cy+sr*sp*sy; // W
}

void Model_MDL::QuaternionMatrix( const v4_t quaternion, float (*matrix)[4] )
{

	matrix[0][0] = 1.0f - 2.0f * quaternion[1] * quaternion[1] - 2.0f * quaternion[2] * quaternion[2];
	matrix[1][0] = 2.0f * quaternion[0] * quaternion[1] + 2.0f * quaternion[3] * quaternion[2];
	matrix[2][0] = 2.0f * quaternion[0] * quaternion[2] - 2.0f * quaternion[3] * quaternion[1];

	matrix[0][1] = 2.0f * quaternion[0] * quaternion[1] - 2.0f * quaternion[3] * quaternion[2];
	matrix[1][1] = 1.0f - 2.0f * quaternion[0] * quaternion[0] - 2.0f * quaternion[2] * quaternion[2];
	matrix[2][1] = 2.0f * quaternion[1] * quaternion[2] + 2.0f * quaternion[3] * quaternion[0];

	matrix[0][2] = 2.0f * quaternion[0] * quaternion[2] + 2.0f * quaternion[3] * quaternion[1];
	matrix[1][2] = 2.0f * quaternion[1] * quaternion[2] - 2.0f * quaternion[3] * quaternion[0];
	matrix[2][2] = 1.0f - 2.0f * quaternion[0] * quaternion[0] - 2.0f * quaternion[1] * quaternion[1];
}

void Model_MDL::QuaternionSlerp( const v4_t p, v4_t q, float t, v4_t qt )
{
	int i;
	float omega, cosom, sinom, sclp, sclq;

	// decide if one of the quaternions is backwards
	float a = 0;
	float b = 0;
	for (i = 0; i < 4; i++) {
		a += (p[i]-q[i])*(p[i]-q[i]);
		b += (p[i]+q[i])*(p[i]+q[i]);
	}
	if (a > b) {
		for (i = 0; i < 4; i++) {
			q[i] = -q[i];
		}
	}

	cosom = p[0]*q[0] + p[1]*q[1] + p[2]*q[2] + p[3]*q[3];

	if ((1.0 + cosom) > 0.00000001) {
		if ((1.0 - cosom) > 0.00000001) {
			omega = acos( cosom );
			sinom = sin( omega );
			sclp = sin( (1.0 - t)*omega) / sinom;
			sclq = sin( t*omega ) / sinom;
		}
		else {
			sclp = 1.0 - t;
			sclq = t;
		}
		for (i = 0; i < 4; i++) {
			qt[i] = sclp * p[i] + sclq * q[i];
		}
	}
	else {
		qt[0] = -p[1];
		qt[1] = p[0];
		qt[2] = -p[3];
		qt[3] = p[2];
		sclp = sin( (1.0 - t) * 0.5 * Q_PI);
		sclq = sin( t * 0.5 * Q_PI);
		for (i = 0; i < 3; i++) {
			qt[i] = sclp * p[i] + sclq * qt[i];
		}
	}
}

bool Model_MDL::raybox(vec3_t leminB,vec3_t lemaxB, vec3_t leorigin,vec3_t ledir,float *lecoord)
{

/*int NUMDIM=	3;
int RIGHT=	0;
int LEFT=	1;
int MIDDLE=	2;*/

#define NUMDIM	3
#define RIGHT	0
#define LEFT	1
#define MIDDLE	2

//char HitBoundingBox(minB,maxB, origin, dir,coord)
double minB[NUMDIM], maxB[NUMDIM];		/*box */
double origin[NUMDIM], dir[NUMDIM];		/*ray */
double coord[NUMDIM];
minB[0]=leminB[0];
minB[1]=leminB[1];
minB[2]=leminB[2];
maxB[0]=lemaxB[0];
maxB[1]=lemaxB[1];
maxB[2]=lemaxB[2];
origin[0]=leorigin[0];
origin[1]=leorigin[1];
origin[2]=leorigin[2];	
dir[0]=ledir[0];
dir[1]=ledir[1];
dir[2]=ledir[2];				/* hit point */
//{
	bool inside = true;
	//char quadrant[NUMDIM];
	int quadrant[NUMDIM];
	register int i;
	int whichPlane;
	double maxT[NUMDIM];
	double candidatePlane[NUMDIM];
	//vec3_t maxT;
	//vec3_t candidatePlane;
	/* Find candidate planes; this loop can be avoided if
   	rays cast all from the eye(assume perpsective view) */
	for (i=0; i<NUMDIM; i++)
		if(origin[i] < minB[i]) {
			quadrant[i] = LEFT;
			candidatePlane[i] = minB[i];
			inside = false;
		}else if (origin[i] > maxB[i]) {
			quadrant[i] = RIGHT;
			candidatePlane[i] = maxB[i];
			inside = false;
		}else	{
			quadrant[i] = MIDDLE;
		}

	/* Ray origin inside bounding box */
	if(inside)	{
		//coord = origin;
		lecoord[0]=origin[0];
		lecoord[1]=origin[1];
		lecoord[2]=origin[2];
		return (true);
	}


	/* Calculate T distances to candidate planes */
	for (i = 0; i < NUMDIM; i++)
		if (quadrant[i] != MIDDLE && dir[i] !=0.)
			maxT[i] = (candidatePlane[i]-origin[i]) / dir[i];
		else
			maxT[i] = -1.;

	/* Get largest of the maxT's for final choice of intersection */
	whichPlane = 0;
	for (i = 1; i < NUMDIM; i++)
		if (maxT[whichPlane] < maxT[i])
			whichPlane = i;

	/* Check final candidate actually inside box */
	if (maxT[whichPlane] < 0.) return (false);
	for (i = 0; i < NUMDIM; i++)
		if (whichPlane != i) {
			lecoord[i] = origin[i] + maxT[whichPlane] *dir[i];
			if (lecoord[i] < minB[i] || lecoord[i] > maxB[i])
				return (false);
		} else {
			lecoord[i] = candidatePlane[i];
		}
	return (true);				/* ray hits box */
}	
