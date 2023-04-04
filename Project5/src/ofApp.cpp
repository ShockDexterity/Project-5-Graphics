#include "ofApp.h"
#include "CameraMatrices.h"

using namespace glm;

void ofApp::reloadShaders()
{
	shadersNeedReload = false;

	// reload shaders
	shader.load("shaders/my.vert", "shaders/my.frag");
}

//--------------------------------------------------------------
void ofApp::setup()
{
	reloadShaders();

	cube.load("models/cube.ply");
}

//--------------------------------------------------------------
void ofApp::update()
{
	if (shadersNeedReload) { reloadShaders(); }

	// time since last frame
	const float dt { static_cast<float>(ofGetLastFrameTime()) };

	// update position
	camera.position += mat3(rotate(camHead, yAxis)) * velocity * dt;
	camera.rotation = rotate(camHead, yAxis) * rotate(camPitch, xAxis);
}

void ofApp::updateCameraRotation(const float dx, const float dy)
{
	camHead -= dx;
	camPitch -= dy;
	camPitch = glm::clamp(camPitch, radians(-89.0f), radians(89.0f));
}

//--------------------------------------------------------------
void ofApp::draw()
{
	// aspect ratio
	const float width { static_cast<float>(ofGetViewportWidth()) };
	const float height { static_cast<float>(ofGetViewportHeight()) };
	const float aspect { width / height };

	const CameraMatrices camMatrices { camera, aspect, 0.1f, 10.0f };
}

//--------------------------------------------------------------
void ofApp::keyPressed(const int key)
{
	constexpr int speed { 5 };
	switch (key)
	{
	case '`':
		shadersNeedReload = true;
		break;

	case 'a':
	case 'A':
	case OF_KEY_LEFT:
		velocity.x = -speed;
		break;

	case 'd':
	case 'D':
	case OF_KEY_RIGHT:
		velocity.x = speed;
		break;

	case 'q':
	case 'Q':
		velocity.y = -speed;
		break;

	case 'e':
	case 'E':
		velocity.y = speed;
		break;

	case 'w':
	case 'W':
	case OF_KEY_UP:
		velocity.z = -speed;
		break;

	case 's':
	case 'S':
	case OF_KEY_DOWN:
		velocity.z = speed;
		break;

	default: break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(const int key)
{
	switch (key)
	{
	case 'a':
	case 'A':
	case OF_KEY_LEFT:
		velocity.x = 0;
		break;

	case 'd':
	case 'D':
	case OF_KEY_RIGHT:
		velocity.x = 0;
		break;

	case 'q':
	case 'Q':
		velocity.y = 0;
		break;

	case 'e':
	case 'E':
		velocity.y = 0;
		break;

	case 'w':
	case 'W':
	case OF_KEY_UP:
		velocity.z = 0;
		break;

	case 's':
	case 'S':
	case OF_KEY_DOWN:
		velocity.z = 0;
		break;

	default: break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(const int x, const int y)
{
	if (prevX != 0 && prevY != 0)
	{
		//update camera rotation based on mouse movement
		const float dx { static_cast<float>(x - prevX) };
		const float dy { static_cast<float>(y - prevY) };

		updateCameraRotation(mouseSensitivity * dx, mouseSensitivity * dy);
	}

	//remember where the mouse was this frame
	prevX = x;
	prevY = y;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) { }

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) { }

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) { }

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) { }

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) { }

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) { }

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) { }

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) { }
