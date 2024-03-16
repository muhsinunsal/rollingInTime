#pragma once
#include "Model.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <vector>

#include <Windows.h>
#include <mmsystem.h>

#pragma comment(lib,"winmm.lib")

enum class Direction {
	Up,
	Right,
	Down,
	Left
};

class Player
{
public:
	Player();
	Player(glm::vec3 startPosition);

	GLfloat health;
	void render(GLuint uniformModel, GLfloat deltaTime);
	void rotateControl(int key, bool isGameRunning);
	void moveControl(bool* keys, GLfloat deltaTime);

	bool getisJumped();
	Direction getDirection();
	glm::vec3 getPosition();

	GLuint point;
private:

	Model playerModel;

	//GLint checkCollision(Obstruct rect1);

	glm::vec3 currentPosition;
	glm::vec3 size;

	GLfloat moveSpeed;

	glm::vec3 currentRotationAngle;
	glm::vec3 targetRotationAngle;

	glm::vec3 rotationSpeed;

	Direction direction;
	GLboolean isJumped;
	GLboolean isStrait;
};
