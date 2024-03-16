#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>

#include "Model.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "Player.h"



class ClockPlatform
{
public:
	ClockPlatform();
	ClockPlatform(GLint a);

	void render(Player* player, GLuint uniformModel, GLfloat deltaTime);

	void const printAngles();

	GLfloat hourAngle;
	GLfloat	minuteAngle;
	GLfloat	secondAngle;

	void updateHourSpeed(GLfloat speed);
	void updateMinuteSpeed(GLfloat speed);
	void updateSecondSpeed(GLfloat speed);


	GLfloat hourAngleSpeed;
	GLfloat	minuteAngleSpeed;
	GLfloat	secondAngleSpeed;
private:
	Model RingModel;
	Model HourModel;
	Model MinuteModel;
	Model SecondModel;

	glm::mat4 ringModelMatrix;
	glm::mat4 hourModelMatrix;
	glm::mat4 minuteModelMatrix;
	glm::mat4 secondModelMatrix;





	void const renderRing(GLuint uniformModel);
	void const renderHourHand(GLuint uniformModel);
	void const renderMinuteHand(GLuint uniformModel, Player* player);
	void const renderSecondHand(GLuint uniformModel, Player* player);

	void updateAngles(GLfloat deltaTime);



	bool checkMinuteCollision(Player* player);
	bool checkSecondCollision(Player* player);

};

