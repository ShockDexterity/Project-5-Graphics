#pragma once

#include "Camera.h"
#include "ofMain.h"
#include "ofxCubemap.h"
#include "Spotlight.h"

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

	void mousePressed(int x, int y, int button) override {}
	void mouseReleased(int x, int y, int button) override {}
	void mouseEntered(int x, int y) override {}
	void mouseExited(int x, int y) override {}
	void windowResized(int w, int h) override {}
	void dragEvent(ofDragInfo dragInfo) override {}
	void gotMessage(ofMessage msg) override {}

private:
	// meshes
	ofMesh cubeMesh {};
	ofMesh rvMesh {};
	ofMesh swordMesh {};
	ofMesh shieldMesh {};

	// vbos
	ofVbo rvVbo {};
	ofVbo swordVbo {};
	ofVbo shieldVbo {};

	// cubemaps
	ofxCubemap skybox {};

	// shaders
	ofShader rvShader {};
	ofShader swordShader {};
	ofShader shieldShader {};
	ofShader skyboxShader {};

	// diffuse textures
	ofImage rvDiffuse {};
	ofImage swordDiffuse {};
	ofImage shieldDiffuse {};

	//specular textures
	ofImage rvMetallic {};
	ofImage swordMetallic {};

	// normal maps
	ofImage rvNormal {};
	ofImage swordNormal {};
	ofImage shieldNormal {};

	void drawCube(const glm::mat4& proj, const glm::mat4& view);

	// camera stuffs
	int prevX { 0 }, prevY { 0 }; // previous mouse coordinates
	float camHead { 0.0f }, camPitch { 0.0f }; // head and pitch of the camera
	glm::vec3 velocity { 0.0f, 0.0f, 0.0f };
	const float mouseSensitivity { 0.02f };
	Camera camera { glm::vec3(0.0f) };

	void updateCameraRotation(float dx, float dy);

	// constant axes
	const glm::vec3 xAxis { 1.0f, 0.0f, 0.0f };
	const glm::vec3 yAxis { 0.0f, 1.0f, 0.0f };
	const glm::vec3 zAxis { 0.0f, 0.0f, 1.0f };

	// reloading shaders
	bool shadersNeedReload { true };
	void reloadShaders();

	// spotlight
	SpotLight spotLight {
		/*color*/ glm::vec3(1.0f),
		/*intensity*/ 0.5f,
		/*cutoff*/ glm::cos(glm::radians(45.0f)),
		/*position*/ glm::vec3(0, 0, -8),
		/*direction*/ -zAxis
	};
};
