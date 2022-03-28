#include "Camera.h"

struct Camera Camera_Create(unsigned int width, unsigned int height, vec3 position, float FOV_deg, float near_plane, float far_plane)
{
	// TODO : Change vectors to Up, Front, Left and WorldUp
	struct Camera cam =
		{
			.orientation = {0.0f, 0.0f, -1.0f},
			.up = {0.0f, 1.0f, 0.0f},
			.firstClick = true,
			.speed = 0.001f,
			.sensitivity = 100.0f,
			.width = width,
			.height = height};

	glm_vec3_copy(position, cam.position);

	// Calculate Peojection matrix
	mat4 projection = GLM_MAT4_ZERO_INIT;
	glm_perspective(glm_rad(FOV_deg), (float)cam.width / (float)cam.height, near_plane, far_plane, projection);
	glm_mat4_copy(projection, cam.projection);

	return cam;
}

void Camera_View_Projection_To_Shader(struct Camera camera, const struct Shader shader, const char *uniform)
{
	// Initializes matrices since otherwise they will be the null matrix
	mat4 view = GLM_MAT4_ZERO_INIT;

	// Makes camera look in the right direction from the right position
	vec3 center;
	glm_vec3_add(camera.position, camera.orientation, center);

	glm_lookat(camera.position, center, camera.up, view);

	mat4 res;
	glm_mat4_mul(camera.projection, view, res);

	// Exports the camera matrix to the Vertex Shader
	Shader_Uniform_Mat4(shader, uniform, res);
}

void Camera_Inputs(struct Camera *camera)
{
	vec3 speed = {camera->speed, camera->speed, camera->speed};

	// Handles key inputs
	if (Input_Key_Down(window.input, SDL_SCANCODE_W))
	{
		// Position += speed * Orientation;
		glm_vec3_muladd(speed, camera->orientation, camera->position);
	}
	if (Input_Key_Down(window.input, SDL_SCANCODE_A))
	{
		// Position += speed * -Normalize(Cross(Orientation, Up));
		vec3 cross;
		glm_vec3_crossn(camera->orientation, camera->up, cross);
		glm_vec3_negate(cross);
		glm_vec3_muladd(speed, cross, camera->position);
	}
	if (Input_Key_Down(window.input, SDL_SCANCODE_S))
	{
		// Position += speed * -Orientation;
		vec3 tmp;
		glm_vec3_negate_to(camera->orientation, tmp);
		glm_vec3_muladd(speed, tmp, camera->position);
	}
	if (Input_Key_Down(window.input, SDL_SCANCODE_D))
	{
		// Position += speed * Normalize(Cross(Orientation, Up));
		vec3 cross;
		glm_vec3_crossn(camera->orientation, camera->up, cross);
		glm_vec3_muladd(speed, cross, camera->position);
	}
	if (Input_Key_Down(window.input, SDL_SCANCODE_SPACE))
	{
		// Position += speed * Up;
		glm_vec3_muladd(speed, camera->up, camera->position);
	}
	if (Input_Key_Down(window.input, SDL_SCANCODE_LCTRL))
	{
		// Position += speed * -Up;
		vec3 tmp;
		glm_vec3_negate_to(camera->up, tmp);
		glm_vec3_muladd(speed, tmp, camera->position);
	}
	if (Input_Key_Down(window.input, SDL_SCANCODE_LSHIFT))
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
		int rel_x;
		int rel_y;

		if (camera->firstClick)
		{
			SDL_SetRelativeMouseMode(SDL_TRUE);
			camera->firstClick = false;
		}
		SDL_PumpEvents();
		SDL_GetRelativeMouseState(&rel_x, &rel_y);
		// printf("mouse relative pos : %d, %d\n", tmp_x, tmp_y);

		const float sensitivity = 0.1f;
		const float xoffset = (float)rel_x * sensitivity;
		const float yoffset = (float)-rel_y * sensitivity; // negative so up is up

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

		// SDL_WarpMouseInWindow(window.sdl_window, (window.width / 2), (window.heigh / 2));

		// Also re-calculate the Right and Up vector
		// Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		// vec3 right;
		// glm_vec3_crossn(camera->orientation, world_up, right);

		// vec3 up;
		// glm_vec3_crossn(right, camera->orientation);
	}
	else if (Input_Mouse_Button_Released(window.input, MOUSE_BTN_LEFT))
	{
		SDL_SetRelativeMouseMode(SDL_FALSE);
		camera->firstClick = true;
	}
}