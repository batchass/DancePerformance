#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	//ofSetVerticalSync(true);
	ofSetFrameRate(60);
	
	
	//Kinect
	ofxKinectNui::InitSetting initSetting;
	initSetting.grabVideo = false;
	initSetting.grabDepth = false;
	initSetting.grabAudio = false;
	initSetting.grabLabel = false;
	initSetting.grabSkeleton = true;
	initSetting.grabCalibratedVideo = false;
	initSetting.grabLabelCv = false;
	initSetting.videoResolution = NUI_IMAGE_RESOLUTION_320x240;
	initSetting.depthResolution = NUI_IMAGE_RESOLUTION_320x240;
	kinect.init(initSetting);
	kinect.open();
	kinect.addKinectListener(this, &testApp::kinectPlugged, &testApp::kinectUnplugged);
	
	kinectSource = &kinect;
	angle = kinect.getCurrentAngle();
	bPlugged = kinect.isConnected();
	nearClipping = kinect.getNearClippingDistance();
	farClipping = kinect.getFarClippingDistance();

	
	//Particles
	mouseAttract	= false;
	doMouseXY		= false;		// pressing left mmouse button moves mouse in XY plane
	doMouseYZ		= false;		// pressing right mouse button moves mouse in YZ plane
	doRender		= true;
	forceTimer		= false;


	rotSpeed		= 0;
	mouseMass		= 1;


	ballImage.loadImage("ball.png");
	
	width = ofGetWidth();
	height = ofGetHeight();
	
	//	physics.verbose = true;			// dump activity to log
	physics.setGravity(ofVec3f(0, GRAVITY, 0));
	
	// set world dimensions, not essential, but speeds up collision
	physics.setWorldSize(ofVec3f(-width/2, -height, -width/2), ofVec3f(width/2, height, width/2));
	physics.setSectorCount(SECTOR_COUNT);
    physics.setDrag(0.97f);
	physics.setDrag(1);		// FIXTHIS
	physics.enableCollision();
	
	initScene();
	
	
	//integration kinect + physics
	canvas.allocate(width, height);
	canvasTrace.allocate(width, height);
	minZ = 0;
	maxZ = 0;
}

//--------------------------------------------------------------
void testApp::update() {


	//kinect
	kinectSource->update();

	//PARTICLE
	width = ofGetWidth();
	height = ofGetHeight();
	
	

	// Update kinect coords
	float scale = 3;
	for(int i = 0; i < kinect::nui::SkeletonFrame::SKELETON_COUNT; ++i){
		for(int j = 0; j < kinect::nui::SkeletonData::POSITION_COUNT; ++j){
			if(kinect.skeletonPoints[i][0].z > 0){
				currentSkeletonIndex = i;
				//if(kinect.skeletonPoints[i][0].x<minZ) minZ = kinect.skeletonPoints[i][0].x;
				//if(kinect.skeletonPoints[i][0].x>maxZ) maxZ = kinect.skeletonPoints[i][0].x;
				//cout << minZ << " " << maxZ << endl;
				float x = ofMap(kinect.skeletonPoints[i][j].x, 0, 310, -width/2, width/2);
				float y = (kinect.skeletonPoints[i][j].y-ofGetMouseY()) * scale;
				float z = ofMap(kinect.skeletonPoints[i][j].z, 0, 40000, width/2, -width/2);
				bone[(i*kinect::nui::SkeletonData::POSITION_COUNT) + j]->moveTo(ofVec3f(x, y, z));
			}else{
				bone[(i*kinect::nui::SkeletonData::POSITION_COUNT) + j]->moveTo(ofVec3f(9999999, 9999999, 9999999));
			}
		}
	}
	
	physics.update();
}

//--------------------------------------------------------------
void testApp::draw() {


	ofBackground(100, 100, 100);
	ofSetColor(255);

	//Particles
	if(doRender) {
		
		
		// enable back-face culling (so we can see through the walls)
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);

		ofEnableAlphaBlending();
		glEnable(GL_DEPTH_TEST);
		
		// center scene
		glPushMatrix();
		glTranslatef(width/2, 0, -width / 3);
		static float rot = 0;
		glRotatef(rot, 0, 1, 0);
		rot += rotSpeed;
		
		if(forceTimer) {
			float translateMax = forceTimer;
			glTranslatef(ofRandom(-translateMax, translateMax), ofRandom(-translateMax, translateMax), ofRandom(-translateMax, translateMax));
			forceTimer--;
		}
		
		glBegin(GL_QUADS);
		// draw right wall
		glColor3f(.1, 0.1, 0.1);		glVertex3f(width/2, height+1, width/2);
		glColor3f(0, 0, 0);				glVertex3f(width/2, -height, width/2);
		glColor3f(0.05, 0.05, 0.05);	glVertex3f(width/2, -height, -width/2);
		glColor3f(.15, 0.15, 0.15);		glVertex3f(width/2, height+1, -width/2);
		
		// back wall
		glColor3f(.1, 0.1, 0.1);		glVertex3f(width/2, height+1, -width/2);
		glColor3f(0, 0, 0);				glVertex3f(width/2, -height, -width/2);
		glColor3f(0.05, 0.05, 0.05);	glVertex3f(-width/2, -height, -width/2);
		glColor3f(.15, 0.15, 0.15);		glVertex3f(-width/2, height+1, -width/2);
		
		// left wall
		glColor3f(.1, 0.1, 0.1);		glVertex3f(-width/2, height+1, -width/2);
		glColor3f(0, 0, 0);				glVertex3f(-width/2, -height, -width/2);
		glColor3f(0.05, 0.05, 0.05);	glVertex3f(-width/2, -height, width/2);
		glColor3f(.15, 0.15, 0.15);		glVertex3f(-width/2, height+1, width/2);
		
		// front wall
		glColor3f(0.05, 0.05, 0.05);	glVertex3f(width/2, -height, width/2);
		glColor3f(.15, 0.15, 0.15);		glVertex3f(width/2, height+1, width/2);
		glColor3f(.1, 0.1, 0.1);		glVertex3f(-width/2, height+1, width/2);
		glColor3f(0, 0, 0);				glVertex3f(-width/2, -height, width/2);
		
		// floor
		glColor3f(.2, 0.2, 0.2);
		glVertex3f(width/2, height+1, width/2);
		glVertex3f(width/2, height+1, -width/2);
		glVertex3f(-width/2, height+1, -width/2);
		glVertex3f(-width/2, height+1, width/2);
		glEnd();
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		

		//canvas
		glRotatef(rot, 0, -1, 0);
		canvas.begin();

		ofSetColor(255, 125);
		canvasTrace.draw(0,0);

		glTranslatef(width/2, 0, -width / 3);
		glRotatef(rot, 0, 1, 0);

		//particles
		glAlphaFunc(GL_GREATER, 0.5);

		//ofEnableNormalizedTexCoords();
		//ballImage.getTextureReference().bind();
		for(int i=0; i<physics.numberOfParticles(); i++) {
			msa::physics::Particle3D *p = physics.getParticle(i);
			if(p->isFixed()) glColor4f(1, 0, 0, 1);
			else glColor4f(1, 1, 1, 1);

			// draw ball
			glPushMatrix();
			glTranslatef(p->getPosition().x, p->getPosition().y, p->getPosition().z);
			glRotatef(180-rot, 0, 1, 0);
			drawParticle(p->getRadius());
			glPopMatrix();
			
			//draw shadow
			float alpha = ofMap(p->getPosition().y, -height * 1.5, height, 0, 1);
			if(alpha>0) {
				glPushMatrix();
				glTranslatef(p->getPosition().x, height, p->getPosition().z);
				glRotatef(-90, 1, 0, 0);
				glColor4f(255, 255, 255, alpha * alpha * alpha * alpha);
				drawParticle(p->getRadius() * alpha);
				glPopMatrix();
			}
			
		}
		//ballImage.getTextureReference().unbind();
		//ofDisableNormalizedTexCoords();

		
		canvas.end();

		glPopMatrix();

		
		
		
		canvasTrace.begin();
			ofSetColor(255);
			canvas.draw(0,0);
		canvasTrace.end();
		
		canvasTrace.draw(0,0);
		
		canvas.begin();
			ofClear(0);
		canvas.end();
		
	}

	


	/*
	// HEAD
	pLine.clear();
	pLine.addVertex(src[NUI_SKELETON_POSITION_HIP_CENTER].x, src[NUI_SKELETON_POSITION_HIP_CENTER].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_SPINE].x, src[NUI_SKELETON_POSITION_SPINE].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_SHOULDER_CENTER].x, src[NUI_SKELETON_POSITION_SHOULDER_CENTER].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_HEAD].x, src[NUI_SKELETON_POSITION_HEAD].y);
	pLine.draw();
	
	// BODY_LEFT
	pLine.clear();
	pLine.addVertex(src[NUI_SKELETON_POSITION_SHOULDER_CENTER].x, src[NUI_SKELETON_POSITION_SHOULDER_CENTER].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_SHOULDER_LEFT].x, src[NUI_SKELETON_POSITION_SHOULDER_LEFT].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_ELBOW_LEFT].x, src[NUI_SKELETON_POSITION_ELBOW_LEFT].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_WRIST_LEFT].x, src[NUI_SKELETON_POSITION_WRIST_LEFT].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_HAND_LEFT].x, src[NUI_SKELETON_POSITION_HAND_LEFT].y);
	pLine.draw();

	// BODY_RIGHT
	pLine.clear();
	pLine.addVertex(src[NUI_SKELETON_POSITION_SHOULDER_CENTER].x, src[NUI_SKELETON_POSITION_SHOULDER_CENTER].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_SHOULDER_RIGHT].x, src[NUI_SKELETON_POSITION_SHOULDER_RIGHT].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_ELBOW_RIGHT].x, src[NUI_SKELETON_POSITION_ELBOW_RIGHT].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_WRIST_RIGHT].x, src[NUI_SKELETON_POSITION_WRIST_RIGHT].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_HAND_RIGHT].x, src[NUI_SKELETON_POSITION_HAND_RIGHT].y);
	pLine.draw();

	// LEG_LEFT
	pLine.clear();
	pLine.addVertex(src[NUI_SKELETON_POSITION_HIP_CENTER].x, src[NUI_SKELETON_POSITION_HIP_CENTER].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_HIP_LEFT].x, src[NUI_SKELETON_POSITION_HIP_LEFT].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_KNEE_LEFT].x, src[NUI_SKELETON_POSITION_KNEE_LEFT].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_ANKLE_LEFT].x, src[NUI_SKELETON_POSITION_ANKLE_LEFT].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_FOOT_LEFT].x, src[NUI_SKELETON_POSITION_FOOT_LEFT].y);
	pLine.draw();

	// LEG_RIGHT
	pLine.clear();
	pLine.addVertex(src[NUI_SKELETON_POSITION_HIP_CENTER].x, src[NUI_SKELETON_POSITION_HIP_CENTER].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_HIP_RIGHT].x, src[NUI_SKELETON_POSITION_HIP_RIGHT].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_KNEE_RIGHT].x, src[NUI_SKELETON_POSITION_KNEE_RIGHT].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_ANKLE_RIGHT].x, src[NUI_SKELETON_POSITION_ANKLE_RIGHT].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_FOOT_RIGHT].x, src[NUI_SKELETON_POSITION_FOOT_RIGHT].y);
	pLine.draw();*/

	

	glDisable(GL_BLEND);
	glColor4f(1, 1, 1, 1);

	ofDrawBitmapString(" FPS: " + ofToString(ofGetFrameRate(), 2)
                + " | Number of particles: " + ofToString(physics.numberOfParticles(), 2)
                + " | Number of springs: " + ofToString(physics.numberOfSprings(), 2)
                + " | Mouse Mass: " + ofToString(mouseNode.getMass(), 2)
                + "\nLook at source code keyPressed to see keyboard shortcuts"
			   , 20, 15);
}

void testApp::drawParticle(float r)
{
	ofCircle(0,0,r);
			
	//ofSphere(r);

	/*glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2f(-r, -r);
	glTexCoord2f(1, 0); glVertex2f(r, -r);
	glTexCoord2f(1, 1); glVertex2f(r, r);
	glTexCoord2f(0, 1); glVertex2f(-r, r);
	glEnd();*/
}


//--------------------------------------------------------------
void testApp::exit() {
	kinect.close();
	kinect.removeKinectListener(this);


#ifdef USE_TWO_KINECTS
	kinect2.close();
#endif
}

//--------------------------------------------------------------
void testApp::keyPressed (int key) {
	//particles
	switch(key) {
		case 'a': 
			toggleMouseAttract();
			break;
		case 'p': 
			for(int i=0; i<100; i++) addRandomParticle(); 
			break;
		case 'P': 
			for(int i=0; i<100; i++) 
				killRandomParticle(); break;
		case 's': 
			addRandomSpring(); 
			break;
		case 'S': 
			killRandomSpring(); 
			break;
		case 'c': 
			physics.isCollisionEnabled() ? physics.disableCollision() : physics.enableCollision(); 
			break;
		case 'C': 
			killRandomConstraint(); 
			break;
		case 'r': 
			doRender ^= true; 
			break;
		case 'f': 
			addRandomForce(FORCE_AMOUNT); break;
		case 'F': 
			addRandomForce(FORCE_AMOUNT * 3); 
			break;
		case 'l': 
			lockRandomParticles(); 
			break;
		case 'u': 
			unlockRandomParticles(); 
			break;
		case ' ': 
			initScene(); 
			break;
		case 'x': 
			doMouseXY = true; 
			break;
		case 'z': 
			doMouseYZ = true; 
			break;
		case ']': 
			rotSpeed += 0.01f; 
			break;
		case '[': 
			rotSpeed -= 0.01f; 
			break;
		case '+': 
			mouseNode.setMass(mouseNode.getMass() +0.1); 
			for(int i = 0; i < kinect::nui::SkeletonFrame::SKELETON_COUNT; ++i){
				for(int j = 0; j < kinect::nui::SkeletonData::POSITION_COUNT; ++j){
					bone[(i*kinect::nui::SkeletonData::POSITION_COUNT) + j]->setMass(bone[(i*kinect::nui::SkeletonData::POSITION_COUNT) + j]->getMass() +0.1);
				}
			}
			break;
		case '-': 
			mouseNode.setMass(mouseNode.getMass() -0.1); 
			for(int i = 0; i < kinect::nui::SkeletonFrame::SKELETON_COUNT; ++i){
				for(int j = 0; j < kinect::nui::SkeletonData::POSITION_COUNT; ++j){
					bone[(i*kinect::nui::SkeletonData::POSITION_COUNT) + j]->setMass(bone[(i*kinect::nui::SkeletonData::POSITION_COUNT) + j]->getMass() -0.1);
				}
			}
			break;
		case 'm': 
			bool collision = mouseNode.hasCollision();
			if(collision){
				mouseNode.disableCollision();
				for(int i = 0; i < kinect::nui::SkeletonFrame::SKELETON_COUNT; ++i){
					for(int j = 0; j < kinect::nui::SkeletonData::POSITION_COUNT; ++j){
						bone[(i*kinect::nui::SkeletonData::POSITION_COUNT) + j]->disableCollision();
					}
				}
			}else{
				mouseNode.enableCollision();
				for(int i = 0; i < kinect::nui::SkeletonFrame::SKELETON_COUNT; ++i){
					for(int j = 0; j < kinect::nui::SkeletonData::POSITION_COUNT; ++j){
						bone[(i*kinect::nui::SkeletonData::POSITION_COUNT) + j]->enableCollision();
					}
				}
			}
			break;
	}
}

void testApp::keyReleased  (int key){
	//particles
	switch(key) {
		case 'x': doMouseXY = false; break;
		case 'z': doMouseYZ = false; break;
	}
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y) {
	//particles
	static int oldMouseX = -10000;
	static int oldMouseY = -10000;
	int velX = x - oldMouseX;
	int velY = y - oldMouseY;
	if(doMouseXY) mouseNode.moveBy(ofVec3f(velX, velY, 0));
	if(doMouseYZ) mouseNode.moveBy(ofVec3f(velX, 0, velY));
	oldMouseX = x;
	oldMouseY = y;
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

	//particles
	switch(button) {
		case 0:	doMouseXY = true; mouseMoved(x, y); break;
		case 2: doMouseYZ = true; mouseMoved(x, y); break;
	}
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	//particles
	doMouseXY = doMouseYZ = false;
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
}

//--------------------------------------------------------------
void testApp::kinectPlugged(){
	bPlugged = true;
}

//--------------------------------------------------------------
void testApp::kinectUnplugged(){
	bPlugged = false;
}


void testApp::initScene() {
	// clear all particles and springs etc
	physics.clear();
	
	// you can add your own particles to the physics system
	physics.addParticle(&mouseNode);
	mouseNode.makeFixed();
	mouseNode.setMass(MIN_MASS);
	mouseNode.moveTo(ofVec3f(0, 0, 0));
	mouseNode.setRadius(NODE_MAX_RADIUS);


	for(int i = 0; i < kinect::nui::SkeletonFrame::SKELETON_COUNT; ++i){
		for(int j = 0; j < kinect::nui::SkeletonData::POSITION_COUNT; ++j){
			msa::physics::Particle3D * joint = new msa::physics::Particle3D();
			physics.addParticle(joint);
			joint->makeFixed();
			joint->setMass(MIN_MASS);
			joint->moveTo(ofVec3f(0, 0, 0));
			joint->setRadius(NODE_MAX_RADIUS);

			bone.push_back(joint);
		}
	}
}




void testApp::addRandomParticle() {
	float posX		= ofRandom(-width/2, width/2);
	float posY		= ofRandom(0, height);
	float posZ		= ofRandom(-width/2, width/2);
	float mass		= ofRandom(MIN_MASS, MAX_MASS);
	float bounce	= ofRandom(MIN_BOUNCE, MAX_BOUNCE);
	float radius	= ofMap(mass, MIN_MASS, MAX_MASS, NODE_MIN_RADIUS, NODE_MAX_RADIUS);
	
	// physics.makeParticle returns a particle pointer so you can customize it
	msa::physics::Particle3D *p = physics.makeParticle(ofVec3f(posX, posY, posZ));
	
	// and set a bunch of properties (you don't have to set all of them, there are defaults)
	p->setMass(mass)->setBounce(bounce)->setRadius(radius)->enableCollision()->makeFree();
	
	// add an attraction to the mouseNode
	if(mouseAttract) physics.makeAttraction(&mouseNode, p, ofRandom(MIN_ATTRACTION, MAX_ATTRACTION));
}

void testApp::addRandomSpring() {
	int k = 0;
	/*for(int i=0; i<physics.numberOfParticles(); i++) {
		msa::physics::Particle3D *a = physics.getParticle(i);
		msa::physics::Particle3D *b = bone[(i%kinect::nui::SkeletonData::POSITION_COUNT) + (kinect::nui::SkeletonData::POSITION_COUNT*currentSkeletonIndex)];
		physics.makeSpring(a, b, ofRandom(SPRING_MIN_STRENGTH, SPRING_MAX_STRENGTH), ofRandom(10, 30));
		k++;
	}*/
	for(int i=0; i<physics.numberOfParticles(); i++) {
		msa::physics::Particle3D *a = physics.getParticle(i);
		msa::physics::Particle3D *b = &mouseNode;
		physics.makeSpring(a, b, ofRandom(SPRING_MIN_STRENGTH, SPRING_MAX_STRENGTH), ofRandom(SPRING_MIN_WIDTH, SPRING_MAX_WIDTH));
	}

}


void testApp::killRandomParticle() {
	msa::physics::Particle3D *p = physics.getParticle(floor(ofRandom(0, physics.numberOfParticles())));
	if(p && p != &mouseNode) p->kill();
}

void testApp::killRandomSpring() {
	for(int i=physics.numberOfSprings(); i>0; i--) {
		msa::physics::Spring3D *s = physics.getSpring(i);
		if(s) s->kill();
	}
}

void testApp::killRandomConstraint() {
	msa::physics::Constraint3D *c = physics.getConstraint(floor(ofRandom(0, physics.numberOfConstraints())));
	if(c) c->kill();
}


void testApp::toggleMouseAttract() {
	mouseAttract = !mouseAttract;
	if(mouseAttract) {
		// loop through all particles and add attraction to the closest bone
		/*msa::physics::Particle3D * closestBone;
		float minDistance;
		float tempDistance;
		for(int i=0; i<physics.numberOfParticles(); i++) {
			closestBone = NULL;
			minDistance = 999999999;
			for(int j = 0; j < kinect::nui::SkeletonFrame::SKELETON_COUNT; ++j){
				for(int k = 0; k < kinect::nui::SkeletonData::POSITION_COUNT; ++k){
					tempDistance = bone[(j*kinect::nui::SkeletonData::POSITION_COUNT) + k]->getPosition().distance(physics.getParticle(i)->getPosition());
					if(tempDistance<minDistance){
						closestBone = bone[(j*kinect::nui::SkeletonData::POSITION_COUNT) + k];
						minDistance = tempDistance;
					}
				}
			}
			if(closestBone!=NULL){
				physics.makeAttraction(closestBone, physics.getParticle(i), ofRandom(MIN_ATTRACTION, MAX_ATTRACTION));
			}
		}*/

		int k = 0;
		for(int i=0; i<physics.numberOfParticles(); i++) {
			//physics.makeAttraction(bone[(i%kinect::nui::SkeletonData::POSITION_COUNT) + (kinect::nui::SkeletonData::POSITION_COUNT*currentSkeletonIndex)], physics.getParticle(i), ofRandom(MIN_ATTRACTION, MAX_ATTRACTION));
			
			//test
			physics.makeAttraction(&mouseNode, physics.getParticle(i), ofRandom(MIN_ATTRACTION, MAX_ATTRACTION));
			
			k++;
		}
	} else {
		// loop through all existing attractsions and delete them
		for(int i=0; i<physics.numberOfAttractions(); i++) physics.getAttraction(i)->kill();
	}
}

void testApp::addRandomForce(float f) {
	forceTimer = f;
	for(int i=0; i<physics.numberOfParticles(); i++) {
		msa::physics::Particle3D *p = physics.getParticle(i);
		if(p->isFree()) p->addVelocity(ofVec3f(ofRandom(-f, f), ofRandom(-f, f), ofRandom(-f, f)));
	}
}

void testApp::lockRandomParticles() {
	for(int i=0; i<physics.numberOfParticles(); i++) {
		msa::physics::Particle3D *p = physics.getParticle(i);
		if(ofRandom(0, 100) < FIX_PROBABILITY) p->makeFixed();
		else p->makeFree();
	}
	mouseNode.makeFixed();
}

void testApp::unlockRandomParticles() {
	for(int i=0; i<physics.numberOfParticles(); i++) {
		msa::physics::Particle3D *p = physics.getParticle(i);
		p->makeFree();
	}
	mouseNode.makeFixed();
}
