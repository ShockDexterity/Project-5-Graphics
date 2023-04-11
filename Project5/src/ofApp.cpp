#include "ofApp.h"

#include "calcTangents.h"
#include "CameraMatrices.h"

using namespace glm;

/*
 *	PART 1 (60 Points)
 *
 *	DONE (10) The scene must have a skybox
 *
 *	DONE (10) The scene must have at least one object or surface with a color texture applied to it
 *			(enable mipmaps)
 *
 *	DONE (10) The scene must have at least one object with a normal map applied to it.
 *			(you should enable mipmaps)
 *
 *	(10) All objects in the scene (at least 1) must be illuminated by at least two light sources
 *
 *	DONE (10) All diffuse objects in the scene (at least 1) should have an irradiance map applied to them for ambient lighting
 *			that is consistent with the skybox. You may use the mipmap hack or load an irradiance cubemap.
 *			Both count for full credit, but a true irradiance map is preferred
 *
 *	DONE (10) All shaders should have appropriate gamma correction, both when sampling color textures
 *			and when calculating the final fragment color
 *
 * **********************************************************************************************************************
 *
 *	PART 2 (40 Points)
 *
 *	(20) The scene must have at least one object that exhibits specular reflections of the skybox
 *			and highlights from all the light sources defined in Part 1
 *
 *	(10) The reflections should exhibit the Fresnel effect using Schlick's approximation
 *
 *	(10) For the highlights, you should implement the Cook-Torrance model shown in the
			video on physically-based specularity, using the GGX model for the microfacet distribution, the
			Smith height-correlated function for microfacet masking and shadowing, and Schlick's
			approximation for the Fresnel effect
 *	
 */

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


	// shield mesh
	shieldMesh.load("models/shield.ply");
	calcTangents(shieldMesh);
	shieldVbo.setMesh(shieldMesh, GL_STATIC_DRAW);

	// shield color (with mipmaps)
	shieldDiffuse.load("textures/shield_diffuse.png");
	shieldDiffuse.getTexture().generateMipmap();
	shieldDiffuse.getTexture().setTextureMinMagFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

	// shield normal (with mipmaps)
	shieldNormal.load("textures/shield_normal.png");
	shieldNormal.getTexture().generateMipmap();
	shieldNormal.getTexture().setTextureMinMagFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);


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

	// draw shield
	{
		shieldShader.begin();

		// lighting
		shieldShader.setUniform3f("lightDir", normalize(yAxis));
		shieldShader.setUniform3f("lightColor", vec3(1));
		shieldShader.setUniform3f("ambientColor", vec3(0.0f));

		// other stuff
		const mat4 shieldModel { translate(vec3(0.0f, 0.0f, -2.0f)) };
		const mat4 shieldMvp { camMatrices.getProj() * camMatrices.getView() * shieldModel };
		shieldShader.setUniformMatrix4f("mvp", shieldMvp);
		shieldShader.setUniformMatrix3f("normalMatrix", mat3());
		shieldShader.setUniformTexture("diffuseTex", shieldDiffuse, 0);
		shieldShader.setUniformTexture("normalTex", shieldNormal, 1);
		shieldShader.setUniformTexture("envMap", skybox.getTexture(), 2);

		// draw
		shieldVbo.drawElements(GL_TRIANGLES, shieldVbo.getNumIndices());
		shieldShader.end();
	}

	// draw sword
	{
		swordShader.begin();

		// lighting
		swordShader.setUniform3f("lightDir", normalize(yAxis));
		swordShader.setUniform3f("lightColor", vec3(1));
		swordShader.setUniform3f("ambientColor", vec3(0.0f));

		// other stuff

		const mat4 swordModel {
			translate(vec3(0.0f, 0.0f, -5.0f))
			* rotate(radians(-45.0f), zAxis)
			* rotate(radians(90.0f), yAxis)
			* scale(vec3(100.0f))
		};
		const mat4 swordMvp { vp * swordModel };
		swordShader.setUniformMatrix4f("mvp", swordMvp);
		swordShader.setUniformMatrix3f("normalMatrix", mat3());
		swordShader.setUniformTexture("diffuseTex", swordDiffuse, 0);
		swordShader.setUniformTexture("normalTex", swordNormal, 1);
		swordShader.setUniformTexture("envMap", skybox.getTexture(), 2);

		// draw
		swordVbo.drawElements(GL_TRIANGLES, swordVbo.getNumIndices());
		swordShader.end();
	}

	// draw rv
	{
		rvShader.begin();

		// lighting
		rvShader.setUniform3f("lightDir", normalize(yAxis));
		rvShader.setUniform3f("lightColor", vec3(1));
		rvShader.setUniform3f("ambientColor", vec3(0.0f));

		// other stuff

		const mat4 rvModel { translate(vec3(0.0f, 0.0f, -10.0f)) };
		const mat4 rvMvp { vp * rvModel };
		rvShader.setUniformMatrix4f("mvp", rvMvp);
		rvShader.setUniformMatrix3f("normalMatrix", mat3());
		rvShader.setUniformTexture("diffuseTex", rvDiffuse, 0);
		rvShader.setUniformTexture("normalTex", rvNormal, 1);
		rvShader.setUniformTexture("envMap", skybox.getTexture(), 2);

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
