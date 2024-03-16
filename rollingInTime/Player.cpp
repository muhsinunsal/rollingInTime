#include "Player.h"
#include "GLFW/glfw3.h"

Player::Player() {}

Player::Player(glm::vec3 startPosition)
{
	health = 5.0f;
	point = 0;

	playerModel = Model();
	playerModel.LoadModel("Models/hourglass.dae");

	size = glm::vec3(1.7f, 3.5f, 2.75f);

	moveSpeed = 10.0f;

	currentPosition = startPosition;

	currentRotationAngle = glm::vec3(0.0f, 0.0f, 0.0f);
	targetRotationAngle = currentRotationAngle;

	rotationSpeed = glm::vec3(50.0f);

	direction = Direction::Up;
	isStrait = true;
	isJumped = false;
}


void Player::render(GLuint uniformModel, GLfloat deltaTime)
{

	//printf("[Player] Health: %f\n",health); 
	glm::mat4 playerModelMatrix = glm::mat4(1.0f);

	switch (direction)
	{
	case Direction::Up:
		break;
	case Direction::Right:
		playerModelMatrix = glm::translate(playerModelMatrix, glm::vec3(0.0f, size.y, 0.0f));

		break;
	case Direction::Down:
		playerModelMatrix = glm::translate(playerModelMatrix, glm::vec3(0.0f, size.x, 0.0f));
		break;
	case Direction::Left:
		playerModelMatrix = glm::translate(playerModelMatrix, glm::vec3(0.0f, size.y, 0.0f));
		break;
	}

	if (~isStrait) {
		//playerModelMatrix = glm::translate(playerModelMatrix, glm::vec3(0.0f, size.y, 0.0f));
	}

	if (isJumped) {
		currentPosition.y = 3.5f + 0.2f;
	}
	else {
		currentPosition.y = 0.2f;
	}

	playerModelMatrix = glm::translate(playerModelMatrix, currentPosition);
	//playerModelMatrix = glm::translate(playerModelMatrix, glm::vec3(0.0f,0.0f,moveSpeed));



	playerModelMatrix = glm::rotate(playerModelMatrix, glm::radians(currentRotationAngle.x), glm::vec3(1.0f, 0.0f, 0.0f));

	if (~isStrait) {
		//playerModelMatrix = glm::translate(playerModelMatrix, glm::vec3(0.0f, -size.y , 0.0f));
	}

	switch (direction)
	{
	case Direction::Up:
		break;
	case Direction::Right:
		playerModelMatrix = glm::translate(playerModelMatrix, glm::vec3(0.0f, -size.y, 0.0f));

		break;
	case Direction::Down:
		playerModelMatrix = glm::translate(playerModelMatrix, glm::vec3(0.0f, -size.x, 0.0f));

		break;
	case Direction::Left:
		playerModelMatrix = glm::translate(playerModelMatrix, glm::vec3(0.0f, -size.y, 0.0f));

		break;
	}


	//printf("Current: %d\tOld: %d\tIsStrait: %d\n", direction, oldDirection, isStrait);
	//printf("Jumped: %d\n", isJumped);


	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(playerModelMatrix));
	playerModel.RenderModel();

	//printf("Rotation Positon: x: %.3f\ty: %.3f\tz: %.3f\n", currentRotationAngle.x, currentRotationAngle.y, currentRotationAngle.z);
	//printf("Rotation Target: x: %.3f\ty: %.3f\tz: %.3f\n", targetRotationAngle.x, targetRotationAngle.y, targetRotationAngle.z);
	//printf("Rotation Speed: x: %.3f\ty: %.3f\tz: %.3f\n", rotationSpeed.x, rotationSpeed.y, rotationSpeed.z);
	//printf("------------------------------------------------------------------\n");

}

void Player::rotateControl(int key, bool isGameRunning)
{
	if (isGameRunning) {
		if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT)
		{
			PlaySound(TEXT("Sound/whoosh.wav"), NULL, SND_FILENAME | SND_ASYNC);

			isStrait = ~isStrait;

			if (isStrait) {
				glm::vec3 tempSize = size;
				size.x = tempSize.y;
				size.y = tempSize.x;

				//printf("Size x: %f\ty: %f\tz: %f\n", size.x, size.y, size.z);
				//printf("TempSize x: %f\ty: %f\tz: %f\n", tempSize.x, tempSize.y, tempSize.z);
			}

			if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT)
			{
				direction = static_cast<Direction>(static_cast<int>(direction) + 1);

				if (direction > Direction::Left) {

					direction = Direction::Up;
					point++;
				}
				currentRotationAngle.x += 90.0f;
			}
		}
		if (key == GLFW_KEY_SPACE)
		{
			PlaySound(TEXT("Sound/whoosh.wav"), NULL, SND_FILENAME | SND_ASYNC);

			isJumped = ~isJumped;
		}

		if (key == GLFW_KEY_UP)
		{
			PlaySound(TEXT("Sound/whoosh.wav"), NULL, SND_FILENAME | SND_ASYNC);

			isJumped = true;

		}
		if (key == GLFW_KEY_DOWN)
		{
			PlaySound(TEXT("Sound/whoosh.wav"), NULL, SND_FILENAME | SND_ASYNC);

			isJumped = false;

		}
	}
}

void Player::moveControl(bool* keys, GLfloat deltaTime)
{
	GLfloat velocity = moveSpeed * deltaTime;

	if (keys[GLFW_KEY_LEFT_SHIFT])
	{
		velocity = velocity * 2;
	}

	if (keys[GLFW_KEY_W])
	{
		isJumped = true;
	}

	if (keys[GLFW_KEY_S])
	{
		isJumped = false;
	}

	if (keys[GLFW_KEY_A])
	{
		if (currentPosition.z > 7.5f) {
			currentPosition.z -= velocity;
		}

	}
	if (keys[GLFW_KEY_D])
	{
		if (currentPosition.z < 34.0f) {
			currentPosition.z += velocity;
		}
	}

}

bool Player::getisJumped()
{
	return isJumped;
}

Direction Player::getDirection()
{
	return direction;
}

glm::vec3 Player::getPosition() {
	return currentPosition;
}
