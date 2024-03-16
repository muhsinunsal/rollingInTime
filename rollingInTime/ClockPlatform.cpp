#include "ClockPlatform.h"
#include "GLFW/glfw3.h"
#include <random>

GLfloat periotToRadians(GLfloat period) {
	return glm::root_two_pi<GLfloat>() / period;
}

ClockPlatform::ClockPlatform()
{

};

ClockPlatform::ClockPlatform(GLint a)
{
	RingModel = Model();
	RingModel.LoadModel("Models/ClockRing.dae");
	HourModel = Model();
	HourModel.LoadModel("Models/hour.dae");
	MinuteModel = Model();
	MinuteModel.LoadModel("Models/minute.dae");
	SecondModel = Model();
	SecondModel.LoadModel("Models/second.dae");


	ringModelMatrix = glm::mat4(1.0f);
	hourModelMatrix = glm::mat4(1.0f);;
	minuteModelMatrix = glm::mat4(1.0f);;
	secondModelMatrix = glm::mat4(1.0f);;

	hourAngle = glm::radians(0.0f);
	minuteAngle = glm::radians(0.0f);
	secondAngle = glm::radians(0.0f);
}

void ClockPlatform::render(Player* player, GLuint uniformModel, GLfloat deltaTime) {
	updateAngles(deltaTime);
	renderRing(uniformModel);
	renderHourHand(uniformModel);
	renderMinuteHand(uniformModel, player);
	renderSecondHand(uniformModel, player);

}
void const ClockPlatform::printAngles()
{
	printf("[ClockPlatform] Angles: h: %f \tm: %f\ts: %f\n", glm::degrees(hourAngle), glm::degrees(minuteAngle), glm::degrees(secondAngle));
	printf("[ClockPlatform] Anglular Speeds: h: %f \tm: %f\ts: %f\n", glm::degrees(hourAngleSpeed), glm::degrees(minuteAngleSpeed), glm::degrees(secondAngleSpeed));
}
;
void const ClockPlatform::renderRing(GLuint uniformModel) {

	ringModelMatrix = glm::mat4(1.0f);
	ringModelMatrix = glm::rotate(ringModelMatrix, hourAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(ringModelMatrix));
	RingModel.RenderModel();

};
void const ClockPlatform::renderHourHand(GLuint uniformModel) {

	hourModelMatrix = glm::mat4(1.0f);
	hourModelMatrix = glm::translate(hourModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(hourModelMatrix));
	HourModel.RenderModel();
};
void const ClockPlatform::renderMinuteHand(GLuint uniformModel, Player* player) {

	minuteModelMatrix = glm::mat4(1.0f);
	minuteModelMatrix = glm::rotate(minuteModelMatrix, minuteAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(minuteModelMatrix));
	MinuteModel.RenderModel();

	if (glm::degrees(minuteAngle) >= 180.0f &&
		glm::degrees(minuteAngle) <= 181.0f) {
		checkMinuteCollision(player);
	}
};
void const ClockPlatform::renderSecondHand(GLuint uniformModel, Player* player) {

	secondModelMatrix = glm::mat4(1.0f);
	secondModelMatrix = glm::translate(secondModelMatrix, glm::vec3(0.0f, 3.5f, 0.0f));
	secondModelMatrix = glm::rotate(secondModelMatrix, secondAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(secondModelMatrix));
	SecondModel.RenderModel();
	if (glm::degrees(secondAngle) >= 180.0f &&
		glm::degrees(secondAngle) <= 181.0f) {
		checkSecondCollision(player);
	}
};
void ClockPlatform::updateAngles(GLfloat deltaTime) {
	if (glm::degrees(hourAngle) > 360.0f) {
		hourAngle = 0.0f;
	}

	if (glm::degrees(minuteAngle) > 360.0f) {
		minuteAngle = 0.0f;
	}

	if (glm::degrees(secondAngle) > 360.0f) {
		secondAngle = 0.0f;
	}



	hourAngle += hourAngleSpeed * deltaTime;
	minuteAngle += minuteAngleSpeed * deltaTime;
	secondAngle += secondAngleSpeed * deltaTime;

	float mod1 = fmod(glm::degrees(minuteAngle), 30.0f);

	//if (mod1 == 0.0f) {
	//	printf("A: %f\n", mod1);
	//	PlaySound(TEXT("Sound/tick.wav"), NULL, SND_FILENAME | SND_ASYNC);
	//}
	//if (fmod(glm::degrees(secondAngle),30.0f)) {
	//	PlaySound(TEXT("Sound/tick.wav"), NULL, SND_FILENAME | SND_ASYNC);
	//}

	//printAngles();
}
void ClockPlatform::updateHourSpeed(GLfloat speed) {
	hourAngleSpeed = speed;
}
void ClockPlatform::updateMinuteSpeed(GLfloat speed) {
	minuteAngleSpeed = speed;
}
void ClockPlatform::updateSecondSpeed(GLfloat speed) {
	secondAngleSpeed = speed;
}

bool ClockPlatform::checkMinuteCollision(Player* player) {
	if (!player->getisJumped()) {
		player->health = 0;
		PlaySound(TEXT("Sound/glass.wav"), NULL, SND_FILENAME | SND_ASYNC);
	}
	return false;
};
bool ClockPlatform::checkSecondCollision(Player* player) {
	if (player->getisJumped()) {
		player->health = 0;
		PlaySound(TEXT("Sound/glass.wav"), NULL, SND_FILENAME | SND_ASYNC);
	}
	//printf("[Player] Position x: %f\ty: %f\tz: %f\t", pos.x,pos.y,pos.z);
	return false;
};