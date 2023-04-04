#pragma once

#include "ofMain.h"
#include "Camera.h"
#include "ofxCubemap.h"

class ofApp final : public ofBaseApp
{
public:
	void setup() override;
	void update() override;
	void draw() override;

	void keyPressed(int key) override;
	void keyReleased(int key) override;
	void mouseMoved(int x, int y) override;
	void mouseDragged(int x, int y, int button) override;
	void mousePressed(int x, int y, int button) override;
	void mouseReleased(int x, int y, int button) override;
	void mouseEntered(int x, int y) override;
	void mouseExited(int x, int y) override;
	void windowResized(int w, int h) override;
	void dragEvent(ofDragInfo dragInfo) override;
	void gotMessage(ofMessage msg) override;

private:
	// meshes
	ofMesh mesh {};

	// cubemaps
	ofxCubemap cubemap {};

	// shaders
	ofShader shader {};

	// camera stuffs
	int prevX { 0 }, prevY { 0 }; // previous mouse coordinates
	float camHead { 0.0f }, camPitch { 0.0f }; // head and pitch of the camera
	glm::vec3 velocity { 0.0f, 0.0f, 0.0f };
	const float mouseSensitivity { 0.02f };
	Camera camera { glm::vec3(0.0f, 0.0f, 2.0f) };

	void updateCameraRotation(float dx, float dy);

	// constant axes
	const glm::vec3 xAxis { 1.0f, 0.0f, 0.0f };
	const glm::vec3 yAxis { 0.0f, 1.0f, 0.0f };
	const glm::vec3 zAxis { 0.0f, 0.0f, 1.0f };

	// reloading shaders
	bool shadersNeedReload { true };
	void reloadShaders();
};