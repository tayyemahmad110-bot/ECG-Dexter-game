#pragma once
// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

// Include GLEW and GLFW
#include "glew/glew.h"
#include "glfw/glfw3.h"

// Include GLM
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

// Include our helper function for loading shaders
#include "Shaders/Utils/shaderLoader.hpp"

// Global variables
#include "Globals/globals.hpp"

// Game state & player variables (per-frame baseline at 60 FPS)
glm::vec2 playerPos(-0.5f, -0.4f);
float playerSpeed = 0.015f;
float velocityY = 0.0f;
float gravity = -0.0015f;
float jumpStrength = 0.038f;
bool isGrounded = true;
float groundLevel = -0.4f;

int playerHP = 3;
int maxHP = 3;
int score = 0;
float invulnerableTimer = 0.0f;
float cameraX = 0.0f;

std::vector<glm::vec2> lasers;
float laserSpeed = 0.045f;
float lastLaserTime = 0.0f;

struct Enemy {
	glm::vec2 pos;
	float rotation;
};
std::vector<Enemy> enemies;
float enemySpeed = 0.011f;
float lastEnemyTime = 0.0f;

struct PowerCell {
	glm::vec2 basePos;
	bool collected;
};
std::vector<PowerCell> powerCells;

std::vector<float> cratePositions = { 1.8f, 4.5f, 7.2f, 10.5f, 13.0f };

bool gameWon = false;
bool gameOver = false;
float finishLineX = 15.0f;

unsigned int transformLoc;
unsigned int transformLoc2;

void drawRect(glm::mat4 model, glm::vec4 color)
{
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform4fv(transformLoc2, 1, glm::value_ptr(color));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void resetGame(GLFWwindow* win)
{
	playerPos = glm::vec2(-0.5f, groundLevel);
	velocityY = 0.0f;
	isGrounded = true;
	cameraX = 0.0f;
	playerHP = maxHP;
	score = 0;
	invulnerableTimer = 0.0f;

	lasers.clear();
	enemies.clear();
	powerCells.clear();
	gameWon = false;
	gameOver = false;

	for (float x = 1.2f; x < finishLineX; x += 2.6f)
	{
		powerCells.push_back({ glm::vec2(x, -0.15f), false });
	}

	if (win != NULL)
	{
		glfwSetWindowTitle(win, "Dexter's Lab 2D - Escape the Glitched Lab!");
	}

	std::cout << "\nDEXTER'S LAB: Escape to the Blast Door!" << std::endl;
	std::cout << "Controls: A/D Move | W Jump | SPACE Shoot | R Restart" << std::endl;
}

// Ex 4: adjusting the viewport when resizing the window
void window_callback(GLFWwindow* window, int new_width, int new_height)
{
	glViewport(0, 0, new_width, new_height);
}

int main(void)
{
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		(void)getchar();
		return -1;
	}

#ifdef __APPLE__
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	window = glfwCreateWindow(windowWidth, windowHeight, "Dexter's Lab 2D", NULL, NULL);
	if (window == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window.");
		(void)getchar();
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Optional V-Sync to help cap frame rate

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		(void)getchar();
		glfwTerminate();
		return -1;
	}

	glViewport(0, 0, windowWidth, windowHeight);

	glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	GLuint programID = LoadShaders("SimpleVertexShader.vert", "SimpleFragmentShader.frag");

	// Unit quad scaled per object via model matrix
	GLfloat vertices[] = {
		0.5f,  0.5f, 0.0f,	// top right
		0.5f, -0.5f, 0.0f,	// bottom right
		-0.5f, -0.5f, 0.0f,	// bottom left
		-0.5f,  0.5f, 0.0f	// top left 
	};

	GLuint indices[] = {
		0, 3, 1,
		1, 3, 2,
	};

	// Create VAO, VBO and IBO
	GLuint vao, vbo, ibo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer
	(
		0,                    // attribute 0, must match the layout location in the shader
		3,                    // size of each attribute (3 floats)
		GL_FLOAT,             // type of data
		GL_FALSE,             // should be normalized?
		3 * sizeof(GL_FLOAT), // stride for this attribute
		0                     // initial offset for array buffer
	);
	glEnableVertexAttribArray(0);

	// Set callback for window resizing
	glfwSetFramebufferSizeCallback(window, window_callback);

	transformLoc = glGetUniformLocation(programID, "transform");
	transformLoc2 = glGetUniformLocation(programID, "color");

	resetGame(window);

	float lastFrameTime = (float)glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		glfwSwapBuffers(window);
		glfwPollEvents();

		float currentTime = (float)glfwGetTime();
		float dt = currentTime - lastFrameTime;
		lastFrameTime = currentTime;
		if (dt > 0.05f) dt = 0.05f; // Clamp delta time for stability
		float frameScale = dt * 60.0f;

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		if ((gameWon || gameOver) && glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
			resetGame(window);

		if (!gameWon && !gameOver)
		{
			// Horizontal movement and crate collision
			float prevX = playerPos.x;
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && playerPos.x > cameraX - 0.9f)
				playerPos.x -= playerSpeed * frameScale;
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				playerPos.x += playerSpeed * frameScale;

			for (float crateX : cratePositions)
			{
				if (abs(playerPos.x - crateX) < 0.08f && playerPos.y < groundLevel + 0.08f)
				{
					playerPos.x = prevX;
				}
			}

			if (playerPos.x > cameraX - 0.5f)
			{
				cameraX = playerPos.x + 0.5f;
			}

			// Jump and gravity
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && isGrounded)
			{
				velocityY = jumpStrength;
				isGrounded = false;
			}
			playerPos.y += velocityY * frameScale;
			if (!isGrounded)
				velocityY += gravity * frameScale;

			float currentFloor = groundLevel;
			for (float crateX : cratePositions)
			{
				if (abs(playerPos.x - crateX) < 0.08f && playerPos.y >= groundLevel + 0.07f)
				{
					currentFloor = groundLevel + 0.10f;
				}
			}

			if (playerPos.y <= currentFloor)
			{
				playerPos.y = currentFloor;
				velocityY = 0.0f;
				isGrounded = true;
			}

			// Check blast door win condition
			if (playerPos.x >= finishLineX)
			{
				gameWon = true;
				glfwSetWindowTitle(window, "MISSION ACCOMPLISHED! (Press R to Replay)");
				std::cout << "\nVICTORY! Final Score: " << score << " | Press 'R' to play again." << std::endl;
			}

			// Shoot lasers
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && (currentTime - lastLaserTime > 0.25f))
			{
				lasers.push_back(glm::vec2(playerPos.x + 0.06f, playerPos.y));
				lastLaserTime = currentTime;
			}

			// Spawn enemies
			if (currentTime - lastEnemyTime > 1.6f)
			{
				Enemy e;
				e.pos = glm::vec2(cameraX + 1.2f, groundLevel + 0.08f);
				e.rotation = 0.0f;
				enemies.push_back(e);
				lastEnemyTime = currentTime;
			}

			// Update lasers
			for (int i = 0; i < (int)lasers.size(); i++)
			{
				lasers[i].x += laserSpeed * frameScale;
				if (lasers[i].x > cameraX + 1.2f)
				{
					lasers.erase(lasers.begin() + i);
					i--;
				}
			}

			// Update enemies
			for (int i = 0; i < (int)enemies.size(); i++)
			{
				enemies[i].pos.x -= enemySpeed * frameScale;
				enemies[i].rotation += 0.06f * frameScale;
				if (enemies[i].pos.x < cameraX - 1.2f)
				{
					enemies.erase(enemies.begin() + i);
					i--;
				}
			}

			// Laser vs Enemy collision
			for (int i = 0; i < (int)lasers.size(); i++)
			{
				bool hit = false;
				for (int j = 0; j < (int)enemies.size(); j++)
				{
					if (abs(lasers[i].x - enemies[j].pos.x) < 0.09f && abs(lasers[i].y - enemies[j].pos.y) < 0.09f)
					{
						enemies.erase(enemies.begin() + j);
						hit = true;
						score += 100;
						std::cout << "Robot neutralized! Score: " << score << std::endl;
						break;
					}
				}
				if (hit)
				{
					lasers.erase(lasers.begin() + i);
					i--;
				}
			}

			// Power cell pickups
			for (auto& cell : powerCells)
			{
				if (!cell.collected)
				{
					float animatedY = cell.basePos.y + sin(currentTime * 4.0f) * 0.05f;
					if (abs(playerPos.x - cell.basePos.x) < 0.08f && abs(playerPos.y - animatedY) < 0.12f)
					{
						cell.collected = true;
						score += 250;
						if (playerHP < maxHP) playerHP++;
						std::cout << "Power Cell collected! HP: " << playerHP << "/" << maxHP << " | Score: " << score << std::endl;
					}
				}
			}

			// Player vs Enemy collision
			for (int i = 0; i < (int)enemies.size(); i++)
			{
				if (abs(playerPos.x - enemies[i].pos.x) < 0.08f && abs(playerPos.y - enemies[i].pos.y) < 0.14f)
				{
					enemies.erase(enemies.begin() + i);
					i--;

					if (currentTime > invulnerableTimer)
					{
						playerHP -= 1;
						invulnerableTimer = currentTime + 1.0f;
						std::cout << "Damage taken! HP: " << playerHP << "/" << maxHP << std::endl;

						if (playerHP <= 0)
						{
							gameOver = true;
							glfwSetWindowTitle(window, "DEXTER FAILED! (Press R to Retry)");
							std::cout << "\nGAME OVER! Final Score: " << score << " | Press 'R' to try again." << std::endl;
						}
					}
				}
			}
		}

		// Render
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(programID);
		glBindVertexArray(vao);

		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, 0.0f, 0.0f));
		auto makeModel = [&](float x, float y, float sx, float sy)
			{
				glm::mat4 trans(1.0f);
				trans = glm::translate(trans, glm::vec3(x, y, 0.0f));
				trans = glm::scale(trans, glm::vec3(sx, sy, 1.0f));
				return view * trans;
			};

		// Lab background
		for (float i = -2.0f; i < finishLineX + 2.0f; i += 1.5f)
		{
			drawRect(makeModel(i, 0.2f, 0.3f, 2.0f), glm::vec4(0.15f, 0.15f, 0.2f, 1.0f));
			drawRect(makeModel(i, 0.3f, 0.15f, 0.1f), glm::vec4(0.0f, 0.8f, 1.0f, 1.0f));
			drawRect(makeModel(i, 0.0f, 0.15f, 0.1f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
			drawRect(makeModel(i + 0.75f, 0.7f, 1.5f, 0.05f), glm::vec4(0.3f, 0.3f, 0.4f, 1.0f));
		}

		// Floor & crates
		drawRect(makeModel(cameraX, -0.75f, 3.0f, 0.5f), glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
		for (float crateX : cratePositions)
		{
			drawRect(makeModel(crateX, groundLevel - 0.03f, 0.10f, 0.10f), glm::vec4(0.45f, 0.35f, 0.2f, 1.0f));
		}

		// Blast door at finish line
		drawRect(makeModel(finishLineX, -0.27f, 0.24f, 0.46f), glm::vec4(0.25f, 0.28f, 0.32f, 1.0f));
		drawRect(makeModel(finishLineX, -0.28f, 0.19f, 0.42f), glm::vec4(0.45f, 0.50f, 0.58f, 1.0f));
		drawRect(makeModel(finishLineX, -0.28f, 0.012f, 0.42f), glm::vec4(0.12f, 0.14f, 0.18f, 1.0f));
		drawRect(makeModel(finishLineX, -0.01f, 0.10f, 0.035f), glm::vec4(0.0f, 1.0f, 0.3f, 1.0f));
		drawRect(makeModel(finishLineX - 0.15f, -0.28f, 0.03f, 0.05f), glm::vec4(0.0f, 0.85f, 1.0f, 1.0f));

		// Power cells (bob + rotate + pulse)
		for (const auto& cell : powerCells)
		{
			if (!cell.collected)
			{
				float animatedY = cell.basePos.y + sin(currentTime * 4.0f) * 0.05f;
				float pulse = 1.0f + 0.2f * sin(currentTime * 6.0f);

				glm::mat4 trans(1.0f);
				trans = glm::translate(trans, glm::vec3(cell.basePos.x, animatedY, 0.0f));
				trans = glm::rotate(trans, currentTime * 3.0f, glm::vec3(0.0f, 0.0f, 1.0f));
				trans = glm::scale(trans, glm::vec3(pulse, pulse, 1.0f));

				drawRect(view * glm::scale(trans, glm::vec3(0.06f, 0.06f, 1.0f)), glm::vec4(0.0f, 0.9f, 1.0f, 1.0f));
				drawRect(view * glm::scale(trans, glm::vec3(0.03f, 0.03f, 1.0f)), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
			}
		}

		// Player (Dexter)
		bool visible = (currentTime > invulnerableTimer) || ((int)(currentTime * 15) % 2 == 0);
		if (!gameOver && visible)
		{
			drawRect(makeModel(playerPos.x, playerPos.y - 0.06f, 0.05f, 0.04f), glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
			drawRect(makeModel(playerPos.x, playerPos.y, 0.07f, 0.10f), glm::vec4(0.95f, 0.95f, 0.95f, 1.0f));
			drawRect(makeModel(playerPos.x, playerPos.y + 0.08f, 0.06f, 0.06f), glm::vec4(1.0f, 0.8f, 0.6f, 1.0f));
			drawRect(makeModel(playerPos.x, playerPos.y + 0.12f, 0.08f, 0.04f), glm::vec4(1.0f, 0.4f, 0.0f, 1.0f));
			drawRect(makeModel(playerPos.x + 0.02f, playerPos.y + 0.08f, 0.06f, 0.02f), glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
			drawRect(makeModel(playerPos.x + 0.04f, playerPos.y - 0.02f, 0.03f, 0.03f), glm::vec4(0.5f, 0.0f, 0.5f, 1.0f));
		}

		// Lasers
		for (int i = 0; i < (int)lasers.size(); i++)
		{
			drawRect(makeModel(lasers[i].x, lasers[i].y, 0.1f, 0.02f), glm::vec4(1.0f, 0.1f, 0.1f, 1.0f));
		}

		// Robot enemies
		for (int i = 0; i < (int)enemies.size(); i++)
		{
			glm::mat4 trans(1.0f);
			trans = glm::translate(trans, glm::vec3(enemies[i].pos.x, enemies[i].pos.y, 0.0f));
			trans = glm::rotate(trans, enemies[i].rotation, glm::vec3(0.0f, 0.0f, 1.0f));

			drawRect(view * glm::scale(trans, glm::vec3(0.08f, 0.08f, 1.0f)), glm::vec4(0.5f, 0.5f, 0.55f, 1.0f));
			drawRect(view * glm::scale(glm::translate(trans, glm::vec3(-0.02f, 0.01f, 0.0f)), glm::vec3(0.03f, 0.03f, 1.0f)), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			drawRect(view * glm::scale(glm::translate(trans, glm::vec3(0.0f, 0.055f, 0.0f)), glm::vec3(0.015f, 0.03f, 1.0f)), glm::vec4(1.0f, 0.8f, 0.0f, 1.0f));
		}

		// HUD health bar (screen space, no camera view)
		for (int h = 0; h < maxHP; h++)
		{
			glm::mat4 trans(1.0f);
			trans = glm::translate(trans, glm::vec3(-0.85f + h * 0.09f, 0.85f, 0.0f));
			trans = glm::scale(trans, glm::vec3(0.07f, 0.05f, 1.0f));

			glm::vec4 hpColor = (h < playerHP) ? glm::vec4(0.0f, 1.0f, 0.2f, 1.0f) : glm::vec4(0.4f, 0.1f, 0.1f, 1.0f);
			drawRect(trans, hpColor);
		}
	}

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(programID);

	glfwTerminate();

	return 0;
}