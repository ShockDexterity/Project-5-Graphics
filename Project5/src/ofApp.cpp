#include "ofApp.h"

#include "calcTangents.h"
#include "CameraMatrices.h"

using namespace glm;

void ofApp::reloadShaders()
{
	shadersNeedReload = false;

	// reload shaders
	shieldShader.load("shaders/object.vert", "shaders/object.frag");
	rvShader.load("shaders/object.vert", "shaders/object.frag");
	swordShader.load("shaders/object.vert", "shaders/object.frag");
	skyboxShader.load("shaders/skybox.vert", "shaders/skybox.frag");
}

//--------------------------------------------------------------
void ofApp::setup()
{
	ofDisableArbTex();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// cube mesh
	cubeMesh.load("models/cube.ply");

	//----------------------------------------------------------------------------------------------

	// rv mesh
	rvMesh.load("models/breaking.ply");
	calcTangents(rvMesh);
	rvVbo.setMesh(rvMesh, GL_STATIC_DRAW);

	// rv color (with mipmaps)
	rvDiffuse.load("textures/RV_RV_Albedo.png");
	rvDiffuse.getTexture().generateMipmap();
	rvDiffuse.getTexture().setTextureMinMagFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

	// rv normal (with mipmaps)
	rvNormal.load("textures/RV_RV_Normal.png");
	rvNormal.getTexture().generateMipmap();
	rvNormal.getTexture().setTextureMinMagFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

	//rv specular
	rvMetallic.load("textures/RV_RV_Metallic.png");

	//----------------------------------------------------------------------------------------------

	// sword mesh
	swordMesh.load("models/sword.ply");
	calcTangents(swordMesh);
	swordVbo.setMesh(swordMesh, GL_STATIC_DRAW);

	// sword color (with mipmaps)
	swordDiffuse.load("textures/TEX_Greatsword_COL.png");
	swordDiffuse.getTexture().generateMipmap();
	swordDiffuse.getTexture().setTextureMinMagFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

	// sword normal (with mipmaps)
	swordNormal.load("textures/TEX_Greatsword_NRM.png");
	swordNormal.getTexture().generateMipmap();
	swordNormal.getTexture().setTextureMinMagFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

	// sword specular
	swordMetallic.load("textures/TEX_Greatsword_MET.png");

	// assert(shieldDiffuse.getWidth() != 0 && shieldDiffuse.getHeight() != 0);

	// load skybox images
	skybox.load("textures/skybox_front.png",
	            "textures/skybox_back.png",
	            "textures/skybox_right.png",
	            "textures/skybox_left.png",
	            "textures/skybox_top.png",
	            "textures/skybox_bottom.png");
	skybox.getTexture().generateMipmap();


	reloadShaders();
}

//--------------------------------------------------------------
void ofApp::update()
{
	if (shadersNeedReload) { reloadShaders(); }

	// time since last frame
	const float dt { static_cast<float>(ofGetLastFrameTime()) };

	// **************** UNCOMMENT THIS IF YOU WANT IT TO MOVE FORWARD ****************
	// fun fact: the first "frame" starts when the debug window pops up.
	// if (dt < 1.0f) { spotLight.translate(vec3(0.0f, 0.0f, -dt)); }

	// update position
	camera.position += mat3(rotate(camHead, yAxis)) * velocity * dt;
	camera.rotation = rotate(camHead, yAxis) * rotate(camPitch, xAxis);
}

//--------------------------------------------------------------
void ofApp::updateCameraRotation(const float dx, const float dy)
{
	camHead -= dx;
	camPitch -= dy;
	camPitch = clamp(camPitch, radians(-89.0f), radians(89.0f));
}

//--------------------------------------------------------------
void ofApp::draw()
{
	// aspect ratio
	const float width { static_cast<float>(ofGetViewportWidth()) };
	const float height { static_cast<float>(ofGetViewportHeight()) };
	const float aspect { width / height };

	const CameraMatrices camMatrices { camera, aspect, 0.1f, 10.0f };

	// "view projection" matrix, to be multiplied with a model later
	const mat4 vp { camMatrices.getProj() * camMatrices.getView() };

	drawCube(camMatrices.getProj(), camMatrices.getView());

	// draw sword
	{
		swordShader.begin();

		// ambient / directional lighting
		swordShader.setUniform3f("lightDir", normalize(yAxis));
		swordShader.setUniform3f("lightColor", vec3(1));
		swordShader.setUniform3f("ambientColor", vec3(0.0f));
		swordShader.setUniform3f("cameraPosition", camMatrices.getCamera().position);

		// spotlight
		swordShader.setUniform3f("spotLightColor", spotLight.getColorIntensity());
		swordShader.setUniform3f("spotLightConeDir", spotLight.getDirection());
		swordShader.setUniform3f("spotLightPos", spotLight.getPosition());
		swordShader.setUniform1f("spotLightCutoff", spotLight.getCutoff());

		// other stuff
		const mat4 swordModel {
			translate(vec3(0.0f, 0.0f, -5.0f))
			* rotate(radians(-45.0f), zAxis)
			* rotate(radians(90.0f), yAxis)
			* scale(vec3(100.0f))
		};
		const mat4 swordMvp { vp * swordModel };
		swordShader.setUniformMatrix4f("mvp", swordMvp);
		swordShader.setUniformMatrix4f("model", swordModel);
		swordShader.setUniformMatrix3f("normalMatrix", transpose(inverse(swordModel)));
		swordShader.setUniformTexture("diffuseTex", swordDiffuse, 0);
		swordShader.setUniformTexture("normalTex", swordNormal, 1);
		rvShader.setUniformTexture("metallicTex", swordMetallic, 2);
		swordShader.setUniformTexture("envMap", skybox.getTexture(), 3);

		// draw
		swordVbo.drawElements(GL_TRIANGLES, swordVbo.getNumIndices());
		swordShader.end();
	}

	// draw rv
	{
		rvShader.begin();

		// ambient / directional lighting
		rvShader.setUniform3f("lightDir", normalize(-yAxis + zAxis));
		rvShader.setUniform3f("lightColor", vec3(1));
		rvShader.setUniform3f("ambientColor", vec3(0.0f));
		rvShader.setUniform3f("cameraPosition", camMatrices.getCamera().position);

		// spotlight
		rvShader.setUniform3f("spotLightColor", spotLight.getColorIntensity());
		rvShader.setUniform3f("spotLightConeDir", spotLight.getDirection());
		rvShader.setUniform3f("spotLightPos", spotLight.getPosition());
		rvShader.setUniform1f("spotLightCutoff", spotLight.getCutoff());

		// other stuff
		const mat4 rvModel { translate(vec3(0.0f, 0.0f, -10.0f)) };
		const mat4 rvMvp { vp * rvModel };
		rvShader.setUniformMatrix4f("mvp", rvMvp);
		rvShader.setUniformMatrix4f("model", rvModel);
		rvShader.setUniformMatrix3f("normalMatrix", transpose(inverse(rvModel)));
		rvShader.setUniformTexture("diffuseTex", rvDiffuse, 0);
		rvShader.setUniformTexture("normalTex", rvNormal, 1);
		rvShader.setUniformTexture("metallicTex", rvMetallic, 2);
		rvShader.setUniformTexture("envMap", skybox.getTexture(), 3);
		rvShader.setUniform1f("roughness", 0.01f);

		// draw
		rvVbo.drawElements(GL_TRIANGLES, rvVbo.getNumIndices());
		rvShader.end();
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(const int key)
{
	constexpr int speed { 5 };
	switch (key)
	{
		case '`': shadersNeedReload = true;
			break;

		case 'a':
		case 'A':
		case OF_KEY_LEFT: velocity.x = -speed;
			break;

		case 'd':
		case 'D':
		case OF_KEY_RIGHT: velocity.x = speed;
			break;

		case 'q':
		case 'Q': velocity.y = -speed;
			break;

		case 'e':
		case 'E': velocity.y = speed;
			break;

		case 'w':
		case 'W':
		case OF_KEY_UP: velocity.z = -speed;
			break;

		case 's':
		case 'S':
		case OF_KEY_DOWN: velocity.z = speed;
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
		case OF_KEY_LEFT: velocity.x = 0;
			break;

		case 'd':
		case 'D':
		case OF_KEY_RIGHT: velocity.x = 0;
			break;

		case 'q':
		case 'Q': velocity.y = 0;
			break;

		case 'e':
		case 'E': velocity.y = 0;
			break;

		case 'w':
		case 'W':
		case OF_KEY_UP: velocity.z = 0;
			break;

		case 's':
		case 'S':
		case OF_KEY_DOWN: velocity.z = 0;
			break;

		default: break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(const int x, const int y)
{
	//remember where the mouse was this frame
	prevX = x;
	prevY = y;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(const int x, const int y, const int button)
{
	// 0 = left and 2 = right
	if (button == 0 or button == 2)
	{
		if (prevX != 0 && prevY != 0)
		{
			//update camera rotation based on mouse movement
			const float dx { static_cast<float>(x - prevX) };
			const float dy { static_cast<float>(y - prevY) };

			updateCameraRotation(mouseSensitivity * dx, mouseSensitivity * dy);
		}
	}
	//remember where the mouse was this frame
	prevX = x;
	prevY = y;
}

//--------------------------------------------------------------
void ofApp::drawCube(const mat4& proj, const mat4& view)
{
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL); // pass depth test

	skyboxShader.begin();

	skyboxShader.setUniformMatrix4f("mvp", proj * mat4(mat3(view)));
	skyboxShader.setUniformTexture("cubemap", skybox.getTexture(), 0);

	cubeMesh.draw();

	skyboxShader.end();

	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
}
