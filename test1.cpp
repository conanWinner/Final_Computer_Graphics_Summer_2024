#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <GL/glut.h>
#include <utility>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <regex>
#include <typeinfo>
#include <cctype>

#define f0r(i, a, b) for(int i = a; i < b; i++) 

using namespace std;

vector<vector<float>> v;

void trimString(std::string& str) {

	std::regex pattern("\\s+");

	str = std::regex_replace(str, pattern, "");

}

//đọc từ file -> lưu vào mảng
void readFile(string fileName) {
	ifstream inputFile(fileName);

	if (!inputFile.is_open()) {
		cerr << "errrr" << '\n';
	}

	string line;
	vector<float> tmp;
	int one = 0;
	while (getline(inputFile, line, ',')) {
		trimString(line);
		if (!line.empty()) {
			tmp.push_back(stof(line));
			if (one == 0) {
				if (tmp.size() == 2) {
					v.push_back(tmp);
					tmp.clear();
					one = 1;
				}
			}	
			if (tmp.size() == 3) {
				v.push_back(tmp);
				tmp.clear();
			}
		}
		
	}

	inputFile.close();
}



float arr[][3] = {
			{0, 0, 0},
			{0, 0, 0},
			{0, 0, 0},
			{0, 0, 0}
};

vector<vector<int>> face = {
	{0,1,2}, //ABC
  	{0,2,3}, //ACD
	{0,3,1}, //ADB
	{3,2,1}  //DCB
};

// tính tích có hướng
void crossProduct(const float v1[], const float v2[], float result[]) {
	result[0] = v1[1] * v2[2] - v1[2] * v2[1];
	result[1] = v1[2] * v2[0] - v1[0] * v2[2];
	result[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

// Hàm tính tích vô hướng
float dotProduct(const float v1[], const float v2[]) {
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

// Hàm tính vector từ 3 điểm -> tích có thướng
void getNormalVector(const float p1[], const float p2[], const float p3[], float normal[]) {
	float v1[3] = { p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2] };
	float v2[3] = { p3[0] - p1[0], p3[1] - p1[1], p3[2] - p1[2] };
	crossProduct(v1, v2, normal);
}

// Hàm tính vector từ điểm đến camera
void getCameraVector(const float p[], const float camera[], float cameraVec[]) {
	cameraVec[0] = - p[0] + camera[0];
	cameraVec[1] = - p[1] + camera[1];
	cameraVec[2] = - p[2] + camera[2];
}

void plotTriangle(vector<int> f) {
	//===================== ABC =================
	glVertex3fv(arr[f[0]]);
	glVertex3fv(arr[f[1]]);
	glVertex3fv(arr[f[1]]);
	glVertex3fv(arr[f[2]]);
	glVertex3fv(arr[f[2]]);
	glVertex3fv(arr[f[0]]);
}

void saveArr() {
	f0r(i, 1, v.size()) {
		f0r(j, 0, v[i].size()) {
			arr[i - 1][j] = v[i][j];
		}
	}
}

void mydisplay() {

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluLookAt(
		5.0, 5.0, 5.0, // eye position
		0.0, 0.0, 1.0, // center position
		0.0, 1.0, 0.0  // up vector
	);

	//clear screen
	glClear(GL_COLOR_BUFFER_BIT);
	//the order of the vertices in a triangle is important!

	// Camera position
	float camera[3] = { 5.0, 5.0, 5.0 };

	glBegin(GL_LINES);
	/*
	f0r(i, 0, face.size()) {
		plotTriangle(face[i]);
	}
	*/

	f0r(i, 0, face.size()) {
		vector<int> f = face[i];
		float p1[3], p2[3], p3[3];
		float normal[3], cameraVec[3];
		float dot;

		// Get points of the face
		for (int j = 0; j < 3; ++j) {
			if (j == 0) { p1[0] = arr[f[j]][0]; p1[1] = arr[f[j]][1]; p1[2] = arr[f[j]][2]; }
			if (j == 1) { p2[0] = arr[f[j]][0]; p2[1] = arr[f[j]][1]; p2[2] = arr[f[j]][2]; }
			if (j == 2) { p3[0] = arr[f[j]][0]; p3[1] = arr[f[j]][1]; p3[2] = arr[f[j]][2]; }
		}

		//Hàm tính vector từ 3 điểm->tích có thướng
		getNormalVector(p1, p2, p3, normal);

		// eye
		getCameraVector(p1, camera, cameraVec);

		// Check if the face is back-facing
		dot = dotProduct(normal, cameraVec);

		if (dot > 0) { // Face is visible
			plotTriangle(f);
		}
	}

	glEnd();
	glutSwapBuffers();
}

void initOpenGL() {
	//setup projection type
	//glFrustrum: define viewing volume
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-2, 2, -1, 1.5, -10, 10);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//setup background color, or clear color
	glClearColor(0.1f, 0.7f, 0.7f, 1.0f);
}

int main(int argc, char** argv) {
	readFile("test.txt");

	saveArr();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(320, 200);
	glutCreateWindow("Drawing a Tetrahedron");
	initOpenGL();
	glutDisplayFunc(mydisplay);
	
	glutMainLoop();
	return 0;
}