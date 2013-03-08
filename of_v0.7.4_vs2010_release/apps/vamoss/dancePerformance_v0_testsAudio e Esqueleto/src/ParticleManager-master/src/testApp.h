#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"

#include "MSAPhysics3D.h"

#define	SPRING_MIN_STRENGTH		0.005
#define SPRING_MAX_STRENGTH		0.1

#define	SPRING_MIN_WIDTH		1
#define SPRING_MAX_WIDTH		3

#define NODE_MIN_RADIUS			5
#define NODE_MAX_RADIUS			15

#define MIN_MASS				1
#define MAX_MASS				3

#define MIN_BOUNCE				0.2
#define MAX_BOUNCE				0.9

#define	FIX_PROBABILITY			10		// % probability of a particle being fixed on creation

#define FORCE_AMOUNT			10

#define EDGE_DRAG				0.98

#define	GRAVITY					1

#define MAX_ATTRACTION			10
#define MIN_ATTRACTION			3

#define SECTOR_COUNT			1		// currently there is a bug at sector borders

//--------------------------------------------------------
class testApp : public ofSimpleApp{

public:

	void setup();
	void update();
	void draw();

	
	void keyPressed (int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);


	void initScene();
	void addRandomParticle();
	void addRandomSpring();
	void killRandomParticle();
	void killRandomSpring();
	void killRandomConstraint();
	void toggleMouseAttract();
	void addRandomForce(float f);
	void lockRandomParticles();
	void unlockRandomParticles();



	bool				mouseAttract;
	bool				doMouseXY;		// pressing left mmouse button moves mouse in XY plane
	bool				doMouseYZ;		// pressing right mouse button moves mouse in YZ plane
	bool				doRender;
	int					forceTimer;


	float				rotSpeed;
	float				mouseMass;

	static int			width;
	static int			height;


	msa::physics::World3D		physics;
	msa::physics::Particle3D	mouseNode;

	ofImage					ballImage;
};

#endif