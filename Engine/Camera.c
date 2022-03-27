#include "Camera.h"

struct Camera Camera_Create(unsigned int width, unsigned int height, vec3 position)
{
	struct Camera cam =
		{
			.orientation = {0.0f, 0.0f, -1.0f},
			.up = {0.0f, 1.0f, 0.0f},
			.firstClick = true,
			.speed = 0.1f,
			.sensitivity = 100.0f,
			.width = width,
			.height = height};

	glm_vec3_copy(position, cam.position);

	return cam;
}

void Camera_Matrix(struct Camera camera, float FOV_deg, float near_plane, float far_plane, const struct Shader shader, const char *uniform)
{
	// Initializes matrices since otherwise they will be the null matrix
	mat4 view = GLM_MAT4_ZERO_INIT;
	mat4 projection = GLM_MAT4_ZERO_INIT;

	// Makes camera look in the right direction from the right position
	vec3 center;
	glm_vec3_add(camera.position, camera.orientation, center);

	glm_lookat(camera.position, center, camera.up, view);

	// Adds perspective to the scene
	glm_perspective(glm_rad(FOV_deg), (float)camera.width / (float)camera.height, near_plane, far_plane, projection);

	mat4 res;
	glm_mat4_mul(projection, view, res);

	// Exports the camera matrix to the Vertex Shader
	Shader_Uniform_Mat4(shader, uniform, res);
}

void Camera_Inputs(struct Camera *camera)
{
	vec3 speed = {camera->speed, camera->speed, camera->speed};

	// Handles key inputs
	if (Input_Key_Pressed(window.input, SDL_SCANCODE_W))
	{
		printf("%s was pressed\n", SDL_GetKeyName(SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_W)));
		// Position += speed * Orientation;
		glm_vec3_muladd(speed, camera->orientation, camera->position);
	}
	if (Input_Key_Pressed(window.input, SDL_SCANCODE_A))
	{
		printf("A is pressed\n");
		// Position += speed * -Normalize(Cross(Orientation, Up));
		vec3 cross;
		glm_vec3_crossn(camera->orientation, camera->up, cross);
		glm_vec3_negate(cross);
		glm_vec3_muladd(speed, cross, camera->position);
	}
	if (Input_Key_Pressed(window.input, SDL_SCANCODE_S))
	{
		printf("S is pressed\n");
		// Position += speed * -Orientation;
		vec3 tmp;
		glm_vec3_negate_to(camera->orientation, tmp);
		glm_vec3_muladd(speed, tmp, camera->position);
	}
	if (Input_Key_Pressed(window.input, SDL_SCANCODE_D))
	{
		printf("D is pressed\n");
		// Position += speed * Normalize(Cross(Orientation, Up));
		vec3 cross;
		glm_vec3_crossn(camera->orientation, camera->up, cross);
		glm_vec3_muladd(speed, cross, camera->position);
	}
	if (Input_Key_Pressed(window.input, SDL_SCANCODE_SPACE))
	{
		// Position += speed * Up;
		glm_vec3_muladd(speed, camera->up, camera->position);
	}
	if (Input_Key_Pressed(window.input, SDL_SCANCODE_LCTRL))
	{
		// Position += speed * -Up;
		vec3 tmp;
		glm_vec3_negate_to(camera->up, tmp);
		glm_vec3_muladd(speed, tmp, camera->position);
	}
	if (Input_Key_Pressed(window.input, SDL_SCANCODE_LSHIFT))
	{
		camera->speed = 0.4f;
	}
	else if (Input_Key_Released(window.input, SDL_SCANCODE_LSHIFT))
	{
		camera->speed = 0.1f;
	}

	// Handles mouse inputs
	if (Input_Mouse_Button_Down(window.input, MOUSE_BTN_LEFT))
	{
		int xpos;
		int ypos;
		SDL_PumpEvents();
		SDL_GetMouseState(&xpos, &ypos);

		if (camera->firstClick)
		{
			// There is a time delay from when the SDL_WarpMouseInWindow is called and the actual
			//	mouse position being the same from the SDL_GetMouseState call :/
			if (xpos != (window.width / 2) && ypos != (window.heigh / 2))
			{
				SDL_WarpMouseInWindow(window.sdl_window, (window.width / 2), (window.heigh / 2));
				return;
			}
			window.input.lastX = (float)(window.width / 2);
			window.input.lastY = (float)(window.width / 2);
			camera->firstClick = false;
		}

		printf("mouse pos : %d, %d\n", xpos, ypos);

		float xoffset = (float)xpos - window.input.lastX;
		float yoffset = window.input.lastY - (float)ypos;

		const float sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		window.input.yaw += xoffset;
		window.input.pitch += yoffset;

		if (window.input.pitch > 89.0f)
			window.input.pitch = 89.0f;
		if (window.input.pitch < -89.0f)
			window.input.pitch = -89.0f;

		vec3 direction;
		direction[0] = (float)(cos(glm_rad(window.input.yaw)) * cos(glm_rad(window.input.pitch)));
		direction[1] = (float)(sin(glm_rad(window.input.pitch)));
		direction[2] = (float)(sin(glm_rad(window.input.yaw)) * cos(glm_rad(window.input.pitch)));
		glm_vec3_norm(direction);

		glm_vec3_copy(direction, camera->orientation);

		SDL_WarpMouseInWindow(window.sdl_window, (window.width / 2), (window.heigh / 2));

		//// Calculate the new Front vector
		// glm::vec3 front;
		// front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		// front.y = sin(glm::radians(Pitch));
		// front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		// m_Front = glm::normalize(front);

		//// Also re-calculate the Right and Up vector
		// m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp)); // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		// m_Up = glm::normalize(glm::cross(m_Right, m_Front));
	}
	else if (Input_Mouse_Button_Released(window.input, MOUSE_BTN_LEFT))
	{
		// Makes sure the next time the camera looks around it doesn't jump
		camera->firstClick = true;
	}
}