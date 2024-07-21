// blob.h: interface for the blob class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLOB_H__87FE24EB_044B_466C_A651_2E26F9963F24__INCLUDED_)
#define AFX_BLOB_H__87FE24EB_044B_466C_A651_2E26F9963F24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "geom.h"
#include "world.h"

//#include "dplay8.h"
//#include "dsound.h"       
#include "son.h"


class blob  
{
public:
	CSound* playersound;
	world_t * my_world;
	vec3_t pos;
	blob();
	void move(vec3_t dir);
	void render();
	void SetWorld(world_t * world) {
		my_world=world; 
	
	}
	void SetSound(CSound * lessons){
		playersound=(CSound*) lessons;
		
		
	}
	void ping();

	virtual ~blob();

};

#endif // !defined(AFX_BLOB_H__87FE24EB_044B_466C_A651_2E26F9963F24__INCLUDED_)
