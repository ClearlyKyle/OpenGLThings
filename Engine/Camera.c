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
	mat4 projection = GLM_MAT4_IDENTITY_INIT;
	glm_perspective(glm_rad(FOV_deg), (float)cam.width / (float)cam.height, near_plane, far_plane, projection);
	glm_mat4_copy(projection, cam.projection);

	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction
	// vector pointing to the right so we initially rotate a bit to the left.
	cam.yaw = -90.0f;
	cam.pitch = 0.0f;
	cam.lastX = (int)(800.0f / 2.0f);
	cam.lastY = (int)(800.0f / 2.0f);

	return cam;
}

void Camera_Set_Orientation(struct Camera *camera, vec3 new_orientation)
{
	glm_vec3_copy(new_orientation, camera->orientation);
}

void Camera_Set_Position(struct Camera *camera, vec3 new_position)
{
	glm_vec3_copy(new_position, camera->position);
}

void Camera_Get_View_Matrix(struct Camera camera, mat4 view_matrix)
{
	// Initializes matrices since otherwise they will be the null matrix
	// view_matrix = GLM_MAT4_ZERO;

	// Makes camera look in the right direction from the right position
	vec3 center;
	glm_vec3_add(camera.position, camera.orientation, center);

	glm_lookat(camera.position, center, camera.up, view_matrix);
}

void Camera_View_Projection_To_Shader(struct Camera camera, const struct Shader shader, const char *uniform)
{
	// Initializes matrices since otherwise they will be the null matrix
	mat4 view_matrix;

	Camera_Get_View_Matrix(camera, view_matrix);

	mat4 projection_matrix;
	glm_mat4_mul(camera.projection, view_matrix, projection_matrix);

	// Exports the camera matrix to the Vertex Shader
	Shader_Uniform_Mat4(shader, uniform, projection_matrix);
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
		camera->speed = 0.02f;
	}
	else if (Input_Key_Released(window.input, SDL_SCANCODE_LSHIFT))
	{
		camera->speed = 0.001f;
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

		camera->yaw += xoffset;
		camera->pitch += yoffset;

		if (camera->pitch > 89.0f)
			camera->pitch = 89.0f;
		if (camera->pitch < -89.0f)
			camera->pitch = -89.0f;

		vec3 direction;
		direction[0] = (float)(cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch)));
		direction[1] = (float)(sin(glm_rad(camera->pitch)));
		direction[2] = (float)(sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch)));
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

void Camera_Print_Values(struct Camera camera)
{
	printf("[CAMERA] Information...\n");
	printf("Position    \t: {%ff, %ff, %ff}\n", camera.position[0], camera.position[1], camera.position[2]);
	printf("Orientation \t: {%ff, %ff, %ff}\n", camera.orientation[0], camera.orientation[1], camera.orientation[2]);
	printf("Up          \t: {%ff, %ff, %ff}\n", camera.up[0], camera.up[1], camera.up[2]);

	printf("Pitch \t: %ff\n", camera.pitch);
	printf("Yawe  \t: %ff\n", camera.yaw);
}