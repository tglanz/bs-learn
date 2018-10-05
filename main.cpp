#include "BsApplication.h"

#include "Scene/BsSceneObject.h"
#include "Mesh/BsMesh.h"
#include "Image/BsTexture.h"
#include "Material/BsMaterial.h"

#include "Resources/BsBuiltinResources.h"

#include "Importer/BsImporter.h"
#include "Importer/BsMeshImportOptions.h"
#include "Importer/BsTextureImportOptions.h"

#include "Components/BsCCamera.h"
#include "Components/BsCRenderable.h"
#include "Components/BsCLight.h"

using namespace bs;

HSceneObject setupCamera();
HSceneObject setupMonkey();
HSceneObject setupLights();

int main()
{
	const bool shouldStartInFullscreen = false;
	VideoMode videoMode(1280, 720);
	Application::startUp(videoMode, "bs-learn", shouldStartInFullscreen);

	auto cameraSO = setupCamera();
	auto lightsSO = setupLights();
	auto monkeySO = setupMonkey();

    Application::instance().runMainLoop();
    Application::shutDown();

    return 0;
}

HSceneObject setupCamera()
{
	HSceneObject cameraSO = SceneObject::create("SceneCamera");
	cameraSO->setPosition(Vector3(0.0f, 0.0f, 5.0f));
	cameraSO->lookAt(Vector3(.0f, .0f, .0f));

	HCamera camera = cameraSO->addComponent<CCamera>();

	// set camera on primary window (monitor)
	SPtr<RenderWindow> primaryWindow = gApplication().getPrimaryWindow();
	camera->getViewport()->setTarget(primaryWindow);

	// set some 3d configurations
	camera->setProjectionType(PT_PERSPECTIVE);
	camera->setHorzFOV(Degree(90));
	camera->setMSAACount(4);

	// calculate and set aspect ratio
	RenderWindowProperties windowProps = primaryWindow->getProperties();
	float aspectRatio = (float)windowProps.width / (float)windowProps.height;
	camera->setAspectRatio(aspectRatio);
	gDebug().logDebug("Aspect ratio: " + toString(aspectRatio));

	return cameraSO;
}

HSceneObject setupLights()
{
	HSceneObject rootSO = SceneObject::create("lights-root");
	rootSO->setPosition(Vector3::ZERO);

	HSceneObject redSO = SceneObject::create("red-light");
	redSO->setParent(rootSO);
	redSO->setPosition(Vector3(-5.0f, 1.0f, .0f));
	HLight red = redSO->addComponent<CLight>();
	red->setType(LightType::Radial);
	red->setColor(Color::Red);
	red->setAttenuationRadius(10000);
	red->setIntensity(10000);

	HSceneObject greenSO = SceneObject::create("green-light");
	greenSO->setParent(rootSO);
	greenSO->setPosition(Vector3(5.0f, 1.0f, .0f));
	HLight green = greenSO->addComponent<CLight>();
	green->setType(LightType::Radial);
	green->setColor(Color::Green);
	green->setAttenuationRadius(10000);
	green->setIntensity(10000);

	HSceneObject blueSO = SceneObject::create("blue-light");
	blueSO->setParent(rootSO);
	blueSO->setPosition(Vector3(.0f, -1.0f, 5.0f));
	HLight blue = blueSO->addComponent<CLight>();
	blue->setType(LightType::Radial);
	blue->setColor(Color::fromARGB(0xff0000ff));
	blue->setAttenuationRadius(10000);
	blue->setIntensity(10000);

	return rootSO;
}

HSceneObject setupMonkey()
{
	HShader shader = gBuiltinResources().getBuiltinShader(BuiltinShader::Standard);
	HMaterial material = Material::create(shader);

	HTexture texture = gImporter().import<Texture>("d:/src/bs-learn/resources/monkey.jpg");
	material->setTexture("gAlbedoTex", texture);

	// Retrieve and print out various texture properties
	auto& props = texture->getProperties();
	gDebug().logDebug("Width: " + toString(props.getWidth()));
	gDebug().logDebug("Height: " + toString(props.getHeight()));
	gDebug().logDebug("Format: " + toString(props.getFormat()));
	gDebug().logDebug("Num. mip maps: " + toString(props.getNumMipmaps()));
	
	auto& meshImportOptions = MeshImportOptions::create();
	meshImportOptions->setImportScale(1.0f);
	meshImportOptions->setImportNormals(true);
	HMesh mesh = gImporter().import<Mesh>("d:/src/bs-learn/resources/monkey.fbx");

	HSceneObject sceneObject = SceneObject::create("monkey object");
	sceneObject->setPosition(Vector3(0, 0, 0));
	sceneObject->setScale(Vector3(1.0f, 1.0f, 1.0f));
	HRenderable renderable = sceneObject->addComponent<CRenderable>();

	renderable->setMesh(mesh);
	renderable->setMaterial(material);

	return sceneObject;
}