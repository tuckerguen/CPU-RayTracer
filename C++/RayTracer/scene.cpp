#include "scene.h"
#include "frame_buffer.h"
#include "utilMethods.h"
#include "intersection.h"
#include <iostream>

using namespace std;

// Load a Layout (.rtl) file
void Scene::Load(char* file)
{
	FILE* pSceneFile = fopen(file, "r");
	
	if (!pSceneFile) {
		cout << "Failed to load " << file << "." << endl;
		char* errorMsg = strerror(errno); // GNU-specific version, Linux default
		printf("Error %s", errorMsg);
	}
	else
		cout << "Successfully loaded " << file << "." << endl;

	// Get sphere, triangle mesh, and light counts
	sphereCount = 0;
	triangleMeshCount = 0;
	lightCount = 0;
	fscanf(pSceneFile, "%d %d %d\n", &lightCount, &sphereCount, &triangleMeshCount);

	sphereList = new Sphere[sphereCount];
	triangleMeshList = new TriangleMesh[triangleMeshCount];
	lightList = new Light[lightCount];

	//fseek(pSceneFile, 0L, SEEK_SET);

	// Step through the file and create/load the lights
	for (int i = 0; i < lightCount; i++)
	{
		char label;
		int type; 
		Vertex pos;
		Color col;
		
		fscanf(pSceneFile, "%c %d %f %f %f %lf %lf %lf\n", 
			&label, &type, &pos.x, &pos.y, &pos.z, &col.r, &col.g, &col.b);
		
		if (label == 'L') {
			lightList[i].isPoint = type;
			lightList[i].position = pos;
			lightList[i].color = col;
		}
	}

	// Step through the file and create/load the lights
	for (int i = 0; i < sphereCount; i++)
	{
		char label;
		Vertex pos;
		Color cAmb, cDif, cSpec;
		float r, ka, kd, ks, ns, nr, kl, kr;

		fscanf(pSceneFile, "%c %f %f %f %f %lf %lf %lf %lf %lf %lf %lf %lf %lf %f %f %f %f %f %f %f\n",
			&label, &pos.x, &pos.y, &pos.z, &r, &cAmb.r, &cAmb.g, &cAmb.b, &cDif.r, &cDif.g, &cDif.b,
			&cSpec.r, &cSpec.g, &cSpec.b, &ka, &kd, &ks, &ns, &nr, &kl, &kr);

		if (label == 'S') {
			sphereList[i].center = pos;
			sphereList[i].radius = r;
			sphereList[i].ambientColor = cAmb;
			sphereList[i].diffuseColor = cDif;
			sphereList[i].specularColor = cSpec;
			sphereList[i].ka = ka;
			sphereList[i].kd = kd;
			sphereList[i].ks = ks;
			sphereList[i].ns = ns;
			sphereList[i].nr = nr;
			sphereList[i].kReflect = kl;
			sphereList[i].kRefract = kr;
		}
	}

	// Step through the file and create/load the lights
	for (int i = 0; i < triangleMeshCount; i++)
	{
		char label;
		char meshFile[255];
		Vertex pos;
		float rotX, rotY, rotZ;
		float scale;
		Color cAmb, cDif, cSpec;
		float ka, kd, ks, ns, nr, kl, kr;

		fscanf(pSceneFile, "%c %s %f %f %f %f %f %f %f %lf %lf %lf %lf %lf %lf %lf %lf %lf %f %f %f %f %f %f %f\n",
			&label, &meshFile, &scale, &rotX, &rotY, &rotZ, &pos.x, &pos.y, &pos.z, &cAmb.r, &cAmb.g, &cAmb.b, 
			&cDif.r, &cDif.g, &cDif.b, &cSpec.r, &cSpec.g, &cSpec.b, &ka, &kd, &ks, &ns, &nr, &kl, &kr);

		if (label == 'M') {
			triangleMeshList[i].Load(meshFile, scale, rotX, rotY, rotZ, pos.x, pos.y, pos.z);
			triangleMeshList[i].ambientColor = cAmb;
			triangleMeshList[i].diffuseColor = cDif;
			triangleMeshList[i].specularColor = cSpec;
			triangleMeshList[i].ka = ka;
			triangleMeshList[i].kd = kd;
			triangleMeshList[i].ks = ks;
			triangleMeshList[i].ns = ns;
			triangleMeshList[i].nr = nr;
			triangleMeshList[i].kReflect = kl;
			triangleMeshList[i].kRefract = kr;
		}
	}

	cout << "Number of lights Loaded: " << lightCount << endl;
	cout << "Number of spheres Loaded: " << sphereCount << endl;
	cout << "Number of triangle meshes Loaded: " << triangleMeshCount << endl;
}

void Scene::RayTrace(FrameBuffer* fb) {
	//Dimensions of image plane
	const int numPixelsX = fb->GetWidth();
	const int numPixelsY = fb->GetHeight();
	int prevPercentDone = 0;

	//Iterate over all virtual pixels in frame buffer (image plane)
	//Shift image plane to have center at z = 0 in world space
	for (int i = 0; i < numPixelsX ; i++) {
		for (int j = 0; j < numPixelsY; j++) {
			//Map framebuffer pixels to image plane coordinates
			float x = ((imagePlaneWidth / (float)numPixelsX) * (float)i) - (imagePlaneWidth / 2.0f);
			float y = ((imagePlaneHeight / (float)numPixelsY) * (float)j)- (imagePlaneHeight / 2.0f);

			//Compute center of virtual pixel
			float pixelWidth = (float)imagePlaneWidth / (float)numPixelsX;
			float pixelHeight = (float)imagePlaneHeight / (float)numPixelsY;
			float xCenter = x + pixelWidth / 2.0f;
			float yCenter = y + pixelHeight / 2.0f;

			//printf("pixel (%d, %d), coord (%f, %f) center @ (%f, %f)\n", i, j, x, y, xCenter, yCenter);

			//Get vector u = ray cast from center of projection through virtual pixel center
			Vertex pPix = Vertex();
			pPix.x = xCenter; 
			pPix.y = yCenter; 
			pPix.z = imagePlaneZ;

			//Point of projection is 0, 0, 0 for this assignment
			Vertex pPrp = Vertex();
			pPrp.x = 0.0; 
			pPrp.y = 0.0; 
			pPrp.z = 0.0;

			Vector u = subtract(pPix, pPrp);
			u.normalize();

			Color pixelColor = ShootRay(pPrp, u, maxDepth, attenuationFactor);

			fb->SetPixel(i, j, pixelColor);

			int percentDone = (int)(100 * ((float)(i * numPixelsX + j) / (float)(numPixelsX * numPixelsY)));
			
			if (percentDone % 1 == 0 && prevPercentDone != percentDone)
				printf("%d ", percentDone);
			prevPercentDone = percentDone;
		}
	}
}

Color Scene::ShootRay(Vertex P0, Vector u, int depth, float attenuationFactor) {
	
	Color result = Color();

	//intersection closest to camera
	Intersection minIntersection = Intersection();
	minIntersection.distance = FLT_MAX;

	//Find any intersections with spheres
	for (int i = 0; i < sphereCount; i++) {
		Sphere sphere = sphereList[i];
		Intersection intersection = intersectSphere(P0, u, sphere);
		if (intersection.distance < minIntersection.distance && intersection.distance > FLT_MIN + 1) {	
			minIntersection = intersection;
		}
	}

	//Find any intersections with triangle meshes
	for (int i = 0; i < triangleMeshCount; i++) {
		TriangleMesh tMesh = triangleMeshList[i];
		Intersection intersection = intersectTriangleMesh(P0, u, tMesh);
		if (intersection.distance < minIntersection.distance && intersection.distance > FLT_MIN + 1) {
			minIntersection = intersection;
		}
	}

	//Base case 1 => No intersection
	if (minIntersection.distance == FLT_MAX) {
		return result;
	}

	//Flip normal if inside
	Vector N = minIntersection.normal;
	if (minIntersection.isInside) {
		N = multiply(N, -1.0);
	}
	//If intersection, continue computations and ray casting on that intersection
	Color localIntensity = Color();

	depth = depth - 1;
	if (depth > 0) {
		//if(depth < 3)
			//printf("%d\n", depth);
		//Base case => If non reflective or refractive surface
		if (minIntersection.obj.kReflect == 0 && minIntersection.obj.kRefract == 0) {
			return phongIllumination(minIntersection.point, N, minIntersection.obj, lightList, lightCount, Ia);
		}
		//If reflective surface
		if (minIntersection.obj.kReflect > 0) {
			if (!minIntersection.isInside) {
				localIntensity = phongIllumination(minIntersection.point, N, minIntersection.obj, lightList, lightCount, Ia);
				result = localIntensity;

				//Shoot reflected ray
				//Calculate reflection vector
				Vector N2 = multiply(N, dot(multiply(u, 2), N));
				Vector R = multiply(subtract(u, N2), 1.0);
				R.normalize();

				//Reduce attenuation factor
				float krg = attenuationFactor;

				//Shoot reflected ray from intersection point in direction of reflected ray
				Color I = ShootRay(minIntersection.point, R, depth, krg);

				//If there was no intersection I is black (0,0,0) so we need no condition
				result = result + I * minIntersection.obj.kReflect * (double)krg;
			}
		}

		//Shoot refracted ray
		//If object is refractive
		if (minIntersection.obj.kRefract > 0) {
			//Compute refraction ray
			//Compute cos of angle between u and surface normal = thetai
			float UdotN = dot(u, N);
			float cosThetai = UdotN / (u.magnitude() * N.magnitude());
			float cosThetai2 = 1.0 - pow(cosThetai, 2);
			//Compute cos of angle between refracted ray and (negative) surface normal = thetaR
			float radicand = (1.0 - (cosThetai2));
			float cosThetaR = sqrt(radicand);

			Vector v1 = multiply(u, minIntersection.obj.nr);
			float s1 = cosThetaR - (minIntersection.obj.nr * cosThetai);
			Vector v2 = multiply(N, s1);
			
			//Refraction ray
			Vector T = subtract(v1, v2);
			T.normalize();

			minIntersection.point = add(minIntersection.point.toVector(), multiply(T, 0.5)).toVertex();

			//Reduce attenuation factor
			float krg = attenuationFactor ;

			//Shoot refracted ray from intersection point in direction of refrefracted lected ray
			minIntersection.isInside = !minIntersection.isInside;
			Color I = ShootRay(minIntersection.point, T, depth, krg);

			//If there was no intersection I is black (0,0,0) so we need no condition
			result = result + I * minIntersection.obj.kRefract * (double)krg;
		}
		return result;
	}
	return result;
}