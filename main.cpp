#include "BsApplication.h"

#include "Platform/BsCursor.h"
#include "Utility/BsTime.h"
#include "Scene/BsSceneObject.h"
#include "Mesh/BsMesh.h"
#include "Image/BsTexture.h"
#include "Material/BsMaterial.h"

#include "Input/BsInput.h"
#include "Input/BsMouse.h"
#include "Input/BsVirtualInput.h"

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
void registerVirtualInput();

class CFlyCamera : public Component {

    public:
        CFlyCamera(const HSceneObject& parent)
            : Component(parent)
        { }

    private:

        VirtualButton mForward;
        VirtualButton mBackward;
        VirtualButton mRight;
        VirtualButton mLeft;
        VirtualButton mUp;
        VirtualButton mDown;

		VirtualAxis mHorizontal;
		VirtualAxis mVertical;

		Radian mYaw;
		Radian mPitch;

        void onInitialized() override
        {
            HCamera camera = SO()->getComponent<CCamera>();

            mForward = VirtualButton("forward");
            mBackward = VirtualButton("backward");
            mRight = VirtualButton("right");
            mLeft = VirtualButton("left");
            mUp = VirtualButton("up");
            mDown = VirtualButton("down");
			mVertical = VirtualAxis("vertical");
			mHorizontal = VirtualAxis("horizontal");
        }

		void update() override
		{
			orient();
			move();
		}

		void move()
		{
			float moveSpeed = 10.0f;

			bool isForward = gVirtualInput().isButtonHeld(mForward);
			bool isBackward = gVirtualInput().isButtonHeld(mBackward);
			bool isRight = gVirtualInput().isButtonHeld(mRight);
			bool isLeft = gVirtualInput().isButtonHeld(mLeft);
			bool isUp = gVirtualInput().isButtonHeld(mUp);
			bool isDown = gVirtualInput().isButtonHeld(mDown);

			Vector3 direction = Vector3::ZERO;
			if (isForward) direction += SO()->getTransform().getForward();
			if (isBackward) direction -= SO()->getTransform().getForward();
			if (isRight) direction += SO()->getTransform().getRight();
			if (isLeft) direction -= SO()->getTransform().getRight();
			if (isUp) direction += Vector3(.0f, 1.0f, .0f);
			if (isDown) direction -= Vector3(.0f, 1.0f, .0f);

			if (direction.squaredLength() != 0)
			{
				direction.normalize();

				float frameDelta = gTime().getFrameDelta();
				Vector3 velocity = direction * moveSpeed;
				SO()->move(velocity * frameDelta);
			}
		}

		void orient()
		{	
			float frameDelta = gTime().getFrameDelta();
			float speed = .4f * frameDelta;

			mYaw += Radian(gVirtualInput().getAxisValue(mHorizontal) * speed);
			mPitch += Radian(gVirtualInput().getAxisValue(mVertical) * speed);

			mYaw.wrap();
			mPitch.wrap();

			Quaternion pitchRotation(Vector3::UNIT_X, mPitch);
			Quaternion yawRotation(Vector3::UNIT_Y, mYaw);

			Quaternion rotation = pitchRotation * yawRotation;
			rotation.normalize();

			SO()->setRotation(rotation);
		}

};

int main()
{
    const bool shouldStartInFullscreen = false;
    VideoMode videoMode(1280, 720);
    Application::startUp(videoMode, "bs-learn", shouldStartInFullscreen);
	
	registerVirtualInput();
    auto cameraSO = setupCamera();
    auto lightsSO = setupLights();
    auto monkeySO = setupMonkey();

    Application::instance().runMainLoop();
    Application::shutDown();

    return 0;
}

HSceneObject setupCamera()
{
	Cursor::instance().hide();

    HSceneObject cameraSO = SceneObject::create("camera");
    cameraSO->setPosition(Vector3(4.0f, -3.0f, 10.0f));
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

	cameraSO->addComponent<CFlyCamera>();

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

    HSceneObject sceneObject = SceneObject::create("monkey");
    sceneObject->setPosition(Vector3(0, 0, 0));
    sceneObject->setScale(Vector3(1.0f, 1.0f, 1.0f));
    HRenderable renderable = sceneObject->addComponent<CRenderable>();

    renderable->setMesh(mesh);
    renderable->setMaterial(material);

    return sceneObject;
}

void registerVirtualInput()
{
	auto& inputConfig = gVirtualInput().getConfiguration();

	inputConfig->registerButton("forward", BC_W);
	inputConfig->registerButton("backward", BC_S);
	inputConfig->registerButton("left", BC_A);
	inputConfig->registerButton("right", BC_D);
	inputConfig->registerButton("up", BC_SPACE);
	inputConfig->registerButton("down", BC_LCONTROL);

	VIRTUAL_AXIS_DESC axisXDesc;
	axisXDesc.type = (int)InputAxis::MouseX;
	
	VIRTUAL_AXIS_DESC axisYDesc;
	axisYDesc.type = (int)InputAxis::MouseY;

	inputConfig->registerAxis("horizontal", axisXDesc);
	inputConfig->registerAxis("vertical", axisYDesc);
}