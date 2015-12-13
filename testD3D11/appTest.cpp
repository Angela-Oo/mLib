
#include "main.h"


unsigned int test0(unsigned int size) {
	std::vector<std::vector<unsigned int>> faces(size, std::vector<unsigned int>(3));
	for (unsigned int i = 0; i < faces.size(); i++) {
		faces[i][0] = i * 3 + 0;
		faces[i][1] = i * 3 + 1;
		faces[i][2] = i * 3 + 2;
	}
	unsigned int sum = 0;
	for (unsigned int i = 0; i < faces.size(); i++) {
		sum += faces[i][0];
		sum += faces[i][1];
		sum += faces[i][2];
	}
	return sum;
}

unsigned int test1(unsigned int size) {
	std::vector<unsigned int> faces(size * 3);
	for (unsigned int i = 0; i < size; i++) {
		faces[3 * i + 0] = i * 3 + 0;
		faces[3 * i + 1] = i * 3 + 1;
		faces[3 * i + 2] = i * 3 + 2;
	}
	unsigned int sum = 0;
	for (unsigned int i = 0; i < size; i++) {
		sum += faces[3 * i + 0];
		sum += faces[3 * i + 1];
		sum += faces[3 * i + 2];
	}
	return sum;
}

unsigned int test2(unsigned int size) {
	MeshDataf::Indices faces;
	faces.reserve(size);
	for (unsigned int i = 0; i < size; i++) {
		unsigned int curr[] = { i * 3 + 0, i * 3 + 1, i * 3 + 2 };
		faces.addFace(curr, 3);
		//faces[3*i+0] = i*3+0;
		//faces[3*i+1] = i*3+1;
		//faces[3*i+2] = i*3+2;
	}
	unsigned int sum = 0;
	for (unsigned int i = 0; i < size; i++) {
		auto f = faces.getFace(i);
		sum += f[0];
		sum += f[1];
		sum += f[2];
		//sum += faces[3*i+0];
		//sum += faces[3*i+1];
		//sum += faces[3*i+2];
	}
	return sum;
}

//unsigned int test3(unsigned int size) {
//	MeshDataf::Indices0 faces;
//	faces.reserve(size);
//	for (unsigned int i = 0; i < size; i++) {
//		unsigned int curr[] = {i*3+0, i*3+1, i*3+2};
//		faces.addFace(curr, 3);
//		//faces[3*i+0] = i*3+0;
//		//faces[3*i+1] = i*3+1;
//		//faces[3*i+2] = i*3+2;
//	}
//	unsigned int sum = 0;
//	for (unsigned int i = 0; i < size; i++) {
//		auto f = faces.getFace(i);
//		sum += f[0];
//		sum += f[1];
//		sum += f[2];
//		//sum += faces[3*i+0];
//		//sum += faces[3*i+1];
//		//sum += faces[3*i+2];
//	}
//	return sum;
//}

void testCollisions() {
	BoundingBox3f bb0(vec3f(0, 0, 0), vec3f(1, 1, 1));
	BoundingBox3f bb1(vec3f(-1, -1, -1), vec3f(-0.5f, 0.5f, 0.5f));
	for (unsigned int i = 0; i < 10000; i++) {
		bb1.translate(vec3f(0.001f, 0.001f, 0.001f));
		if (bb0.intersects(bb1)) {
			std::cout << i << std::endl;
			std::cout << bb1 << std::endl;
			break;
		}
	}

	{
		Timer cTime;
		TriMeshf s0 = Shapesf::sphere(0.5f, vec3f(-2.0f + 1300 * 0.002f), 50, 50);
		TriMeshf s1 = Shapesf::sphere(0.5f, vec3f(1.0f), 15, 15);
		TriMeshAcceleratorBVHf accels0(s0);
		TriMeshAcceleratorBVHf accels1(s1);
		unsigned int numCols = 0;
		for (unsigned int i = 0; i < 10000; i++) {
			if (accels0.collision(accels1)) {
				numCols++;
			}
		}
		std::cout << "bvh collision time:\t" << cTime.getElapsedTimeMS() << " ms | " << numCols << std::endl;
	}

	Timer tbvh;
	for (unsigned int i = 0; i < 10000; i++) {
		TriMeshf s0 = Shapesf::sphere(0.5f, vec3f(-2.0f + i*0.002f), 50, 50);
		TriMeshf s1 = Shapesf::sphere(0.5f, vec3f(1.0f), 15, 15);
		TriMeshAcceleratorBVHf accels0(s0);
		TriMeshAcceleratorBVHf accels1(s1);
		//std::cout << i << std::endl;
		if (accels0.collision(accels1)) {
			std::cout << "BVH:\t" << i << std::endl;
			break;
		}
	}
	std::cout << "tbvh:\t" << tbvh.getElapsedTimeMS() << std::endl;

	Timer tbrute;
	for (unsigned int i = 0; i < 10000; i++) {
		TriMeshf s0 = Shapesf::sphere(0.5f, vec3f(-2.0f + i*0.002f), 50, 50);
		TriMeshf s1 = Shapesf::sphere(0.5f, vec3f(1.0f), 15, 15);
		TriMeshAcceleratorBruteForcef accels0(s0);
		TriMeshAcceleratorBruteForcef accels1(s1);
		//std::cout << i << std::endl;
		if (accels0.collision(accels1)) {
			std::cout << "BruteForce:\t" << i << std::endl;
			break;
		}
	}
	std::cout << "tbrute:\t" << tbrute.getElapsedTimeMS() << std::endl;
}

void AppTest::init(ml::ApplicationData &app)
{

	MeshDataf meshData = MeshIOf::loadFromFile("scans/gates381.ply");
	ml::TriMeshf triMesh(meshData);
	//triMesh.setColor(vec4f(0.0f, 1.0f, 0.0f, 1.0f));
	//auto p = meshData.getBoundingBox().getBottomPlane();

;
	std::vector<ml::TriMeshf> meshes;
	meshes.push_back(triMesh);

	m_mesh.load(app.graphics, ml::TriMeshf(ml::meshutil::createUnifiedMesh(meshes)));

	auto lambdaPoints = [=](ml::vec3f& v) { v = ml::vec3f(-2.f*(float)rand() / RAND_MAX, -2.f*(float)rand() / RAND_MAX, (float)rand() / RAND_MAX); };
	std::vector<ml::vec3f> points(5000);
	std::for_each(points.begin(), points.end(), lambdaPoints);

	m_pointCloud.load(app.graphics, ml::meshutil::createPointCloudTemplate(ml::Shapesf::box(0.01f), points));

	m_shaderManager.init(app.graphics);
	m_shaderManager.registerShader("shaders/pointCloud.hlsl", "pointCloud");

	m_constants.init(app.graphics);

	bbox3f bb = ml::TriMeshf(ml::meshutil::createUnifiedMesh(meshes)).getBoundingBox();
	std::cout << bb << std::endl;


	//vec3f eye(1.0f, 2.0f, 3.0f);
	ml::vec3f eye(-0.5f, -0.5f, 1.5f);
	ml::vec3f worldUp(0.0f, 0.0f, 1.0f);
	//m_camera = ml::Cameraf(eye, worldUp, ml::vec3f::eX, 60.0f, (float)app.window.getWidth() / app.window.getHeight(), 0.01f, 10.0f);
	m_camera = Cameraf(eye, vec3f::eY, vec3f::eZ, 60.0f, (float)app.window.getWidth() / app.window.getHeight(), 0.01f, 10.0f);

	m_font.init(app.graphics, "Calibri");


	m_canvas.init(app.graphics);
	m_canvas.addCircle(vec2f(500, 100), 50, RGBColor::Green, 0.51f);
	ColorImageR8G8B8A8 image(500, 500);
	image.setPixels(RGBColor::Blue);
	for (unsigned int x = 0; x < 500; x++) {
		for (unsigned int y = 0; y < 250; y++)
			image(x, y) = RGBColor::Red;
	}
	//ColorImageR8G8B8A8 image; FreeImageWrapper::loadImage("refined.png", image);
	//Bitmap bmp(image.getWidth(), image.getHeight());
	//for (unsigned int i = 0; i < image.getWidth()*image.getHeight(); i++) {
	//	const vec4uc& p = image.getDataPointer()[i];
	//	bmp.ptr()[i] = RGBColor(p.x, p.y, p.z, p.w);
	//}
	m_canvas.addBillboard(bbox2i(vec2i(100, 100), vec2i(500, 500)), image, 0.5f);
	m_canvas.addBillboard(bbox2i(vec2i(100, 100), vec2i(500, 500)), image, 0.5f);
	m_canvas.addBox(bbox2i(vec2i(50, 50), vec2i(10, 10)), RGBColor::Yellow, 0.4f);
}

void AppTest::render(ml::ApplicationData &app)
{
	m_timer.frame();

	m_canvas.render();

	ConstantBuffer constants;
	mat4f proj = Cameraf::visionToGraphicsProj(app.window.getWidth(), app.window.getHeight(), 1108.51f, 1108.51f, 0.1f, 10.0f);
	//for (unsigned int i = 0; i < 100; i++) {
	//	vec3f p = vec3f(0.0f, 0.0f, 10.0f - (float)i*0.1f);
	//	std::cout << p << "\t\t" << proj * p << std::endl;
	//	std::cout << p << "\t\t" << m_camera.getPerspective() * p << std::endl;
	//	}
	constants.worldViewProj = proj * m_camera.getCamera();
	constants.modelColor = ml::vec4f(1.0f, 1.0f, 1.0f, 1.0f);
	m_constants.updateAndBind(constants, 0);
	app.graphics.castD3D11().getShaderManager().bindShaders("defaultBasic");
	m_mesh.render();

	m_shaderManager.bindShaders("pointCloud");
	m_constants.bind(0);
	m_pointCloud.render();

	m_font.drawString(app.graphics, "FPS: " + ml::convert::toString(m_timer.framesPerSecond()), ml::vec2i(10, 5), 24.0f, ml::RGBColor::Red);
}

void AppTest::resize(ml::ApplicationData &app)
{
	m_camera.updateAspectRatio((float)app.window.getWidth() / app.window.getHeight());

}

void AppTest::keyDown(ml::ApplicationData &app, UINT key)
{

}

void AppTest::keyPressed(ml::ApplicationData &app, UINT key)
{
	const float distance = 0.1f;
	const float theta = 5.0f;

	if (key == KEY_S) m_camera.move(-distance);
	if (key == KEY_W) m_camera.move(distance);
	if (key == KEY_A) m_camera.strafe(-distance);
	if (key == KEY_D) m_camera.strafe(distance);
	if (key == KEY_E) m_camera.jump(distance);
	if (key == KEY_Q) m_camera.jump(-distance);

	if (key == KEY_UP) m_camera.lookUp(theta);
	if (key == KEY_DOWN) m_camera.lookUp(-theta);
	if (key == KEY_LEFT) m_camera.lookRight(theta);
	if (key == KEY_RIGHT) m_camera.lookRight(-theta);

	if (key == KEY_F1) {
		auto color = app.graphics.castD3D11().captureBackBuffer();
		FreeImageWrapper::saveImage("screenshot.png", color);
	}

	if (key == 'R') {
		//float fovX = m_camera.getFoV();
		//float fovY = fovX/m_camera.getAspect();
		//float focalLengthX = 0.5f * (float)app.window.width() / tan(0.5f * math::degreesToRadians(fovX));
		//float focalLengthY = 0.5f * (float)app.window.height() / tan(0.5f * math::degreesToRadians(fovY));
		//mat4f intrinsics = 
		//	ml::mat4f(
		//	focalLengthX, 0.0f, (float)app.window.width()/2.f, 0.0f,
		//	0.0f, focalLengthY, (float)app.window.height()/2.f, 0.0f,
		//	0.0f, 0.0f, 1.0f, 0.0f,
		//	0.0f, 0.0f, 0.0f, 1.0f);
		//std::cout << intrinsics << std::endl;
		mat4f intrinsics = Cameraf::graphicsToVisionProj(m_camera.getPerspective(), app.window.getWidth(), app.window.getHeight());
		//std::cout << intrinsics << std::endl;
		mat4f intrinsicsInverse = intrinsics.getInverse();

		ml::mat4f projToCam = m_camera.getPerspective().getInverse();
		ml::mat4f camToWorld = m_camera.getCamera().getInverse();
		ml::mat4f trans = camToWorld * projToCam;
		ml::ColorImageR32G32B32 image(app.window.getWidth(), app.window.getHeight());

		const std::string testFilename = "scans/gates381.ply";
		ml::MeshDataf meshData = ml::MeshIOf::loadFromFile(testFilename);
		ml::TriMeshf triMesh(meshData);

		//std::cout << triMesh.getBoundingBox() << std::endl;

		ml::Timer c0;
		c0.start();
		ml::TriMeshAcceleratorBVHf accel(triMesh, false);
		//ml::TriMeshAcceleratorBruteForcef accel(triMesh);
		std::cout << "time construct " << c0.getElapsedTimeMS() << std::endl;

		std::vector<const TriMeshRayAcceleratorf*> accelVec;
		accelVec.push_back(&accel);
		int s = sizeof(accel);

		ml::Timer c;
		c.start();
#pragma omp parallel for
		for (int y_ = 0; y_ < (int)app.window.getHeight(); y_++) {
			unsigned int y = (unsigned int)y_;
			for (unsigned int x = 0; x < app.window.getWidth(); x++) {
				//std::cout << " tyring ray " << i << " " << j << std::endl;

				float depth0 = 0.5f;
				float depth1 = 1.0f;
				vec4f p0 = camToWorld*intrinsicsInverse*vec4f((float)x*depth0, (float)y*depth0, depth0, 1.0f);
				vec4f p1 = camToWorld*intrinsicsInverse*vec4f((float)x*depth1, (float)y*depth1, depth1, 1.0f);

				vec3f eye = m_camera.getEye();
				Rayf r(m_camera.getEye(), (p1.getVec3() - p0.getVec3()).getNormalized());

				//mat4f tmp = mat4f::rotationZ(45.0f);
				//r = tmp * r;

				//ml::vec4f p((float)j, (float)i, 0.5f, 1.0f);
				//p.x /= app.window.width();
				//p.y /= app.window.height();
				//p.x = 2.0f*p.x - 1.0f;
				//p.y = 1.0f-2.0f*p.y;
				//p = trans * p;
				//p /= p.w;
				//Rayf r1(m_camera.getEye(), (ml::vec3f(p.x,p.y,p.z)-m_camera.getEye()).getNormalized());

				float t, u, v;
				//const ml::TriMeshf::Trianglef* tri = accel.intersect(r, t, u, v);
				const ml::TriMeshf::Triangle* tri;
				unsigned int objIdx;
				TriMeshRayAcceleratorf::Intersection intersect = TriMeshRayAcceleratorf::getFirstIntersection(r, accelVec, objIdx);
				t = intersect.t;
				u = intersect.u;
				v = intersect.v;
				tri = intersect.triangle;

				if (tri) {
					image(x, y) = tri->getSurfaceColor(u, v).getVec3();
				}
				else {
					image(x, y) = 0;
				}

			}
		}
		double elapsed = c.getElapsedTimeMS();
		std::cout << "time trace " << elapsed << std::endl;
		unsigned int raysPerSec = (unsigned int)((double)(app.window.getHeight()*app.window.getWidth()) / (elapsed / 1000.0));
		std::cout << "million rays/s " << (float)raysPerSec / 1000000.0 << std::endl;

		ml::FreeImageWrapper::saveImage("test.jpg", image);

	}


}

void AppTest::mouseDown(ml::ApplicationData &app, ml::MouseButtonType button)
{

}

void AppTest::mouseWheel(ml::ApplicationData &app, int wheelDelta)
{
	const float distance = 0.002f;
	m_camera.move(distance * wheelDelta);
}

void AppTest::mouseMove(ml::ApplicationData &app)
{
	const float distance = 0.01f;
	const float theta = 0.5f;

	ml::vec2i posDelta = app.input.mouse.pos - app.input.prevMouse.pos;

	if (app.input.mouse.buttons[ml::MouseButtonRight])
	{
		m_camera.strafe(-distance * posDelta.x);
		m_camera.jump(distance * posDelta.y);
	}

	if (app.input.mouse.buttons[ml::MouseButtonLeft])
	{
		m_camera.lookRight(-theta * posDelta.x);
		m_camera.lookUp(theta * posDelta.y);
	}

}
