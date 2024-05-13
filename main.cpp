#include "Buffer.h"
#include "Camera.h"
#include "Model.h"
#include "Program.h"
#include "Texture.h"
#include <chrono>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <thread>
#include "Bumper.h"
#include "AABB.h"

const GLint SCR_WIDTH = 1366;
const GLint SCR_HEIGHT = 768;


void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, GLchar const* message,
	void const* user_param) {
	if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		return;
	throw "none";
	auto const src_str = [source]() {
		switch (source) {
		case GL_DEBUG_SOURCE_API: return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:  return "WINDOW SYSTEM";
		case GL_DEBUG_SOURCE_SHADER_COMPILER:  return "SHADER COMPILER";
		case GL_DEBUG_SOURCE_THIRD_PARTY:  return "THIRD PARTY";
		case GL_DEBUG_SOURCE_APPLICATION:  return "APPLICATION";
		case GL_DEBUG_SOURCE_OTHER:  return "OTHER";
		default:  return "UNKNOWN SOURCE";
		}
		}();
		auto const type_str = [type]() {
			switch (type) {
			case GL_DEBUG_TYPE_ERROR:  return "ERROR";
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:  return "DEPRECATED_BEHAVIOR";
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "UNDEFINED_BEHAVIOR";
			case GL_DEBUG_TYPE_PORTABILITY:  return "PORTABILITY";
			case GL_DEBUG_TYPE_PERFORMANCE:  return "PERFORMANCE";
			case GL_DEBUG_TYPE_MARKER:  return "MARKER";
			case GL_DEBUG_TYPE_OTHER:  return "OTHER";
			default:  return "UNKNOWN TYPE";
			}
			}();

			auto const severity_str = [severity]() {
				switch (severity) {
				case GL_DEBUG_SEVERITY_NOTIFICATION:  return "NOTIFICATION";
				case GL_DEBUG_SEVERITY_LOW:  return "LOW";
				case GL_DEBUG_SEVERITY_MEDIUM:  return "MEDIUM";
				case GL_DEBUG_SEVERITY_HIGH:  return "HIGH";
				default:  return "UNKNOWN SEVERITY";
				}
				}();
				std::cout << src_str << ", " << type_str << ", " << severity_str << ", " << id
					<< ": " << message << '\n';
}





// Create Sky Map
// Import Models


glm::vec3 skyboxVertices[] =
{
	//   Coordinates
	{-1.0f, -1.0f,  1.0f},//        7--------6
	{ 1.0f, -1.0f,  1.0f},//       /|       /|
	{ 1.0f, -1.0f, -1.0f},//      4--------5 |
	{-1.0f, -1.0f, -1.0f},//      | |      | |
	{-1.0f,  1.0f,  1.0f},//      | 3------|-2
	{ 1.0f,  1.0f,  1.0f},//      |/       |/
	{ 1.0f,  1.0f, -1.0f},//      0--------1
	{-1.0f,  1.0f, -1.0f}
};

GLuint skyboxIndices[] =
{
	// Right Face
	1, 2, 6,
	6, 5, 1,
	// Left Face
	0, 4, 7,
	7, 3, 0,
	// Top Face
	4, 5, 6,
	6, 7, 4,
	// Bottom Face
	0, 3, 2,
	2, 1, 0,
	// Back Face
	0, 1, 5,
	5, 4, 0,
	// Front Face
	3, 7, 6,
	6, 2, 3
};


std::string sky_faces[] =
{
	"resources/textures/skybox/right.jpg",
	"resources/textures/skybox/left.jpg",
	"resources/textures/skybox/top.jpg",
	"resources/textures/skybox/bottom.jpg",
	"resources/textures/skybox/front.jpg",
	"resources/textures/skybox/back.jpg",
};

constexpr float floor_y = 0.0f;
glm::vec3 floorVertices[] = {
	{-1.0f, floor_y,  1.0f},
	{ 1.0f, floor_y,  1.0f},
	{ 1.0f, floor_y, -1.0f},
	{-1.0f, floor_y, -1.0f}
};

GLuint floorIndices[] = { 0, 1, 2, 0, 2, 3 };



// Globals
float dt{ 0 };
// Global Camera and stuff
ControllableCamera camera{ SCR_WIDTH, SCR_HEIGHT, {0.0f, 8.0f, 7.0f}, -50.0, -90.0 };
// ControllableCamera camera{ SCR_WIDTH, SCR_HEIGHT};

float last_x{};
float last_y{};
bool cursor_in_focus{ false };
bool is_mdl_pressed_already{ false };

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);


struct
{
	bool show_metric{ false };
	bool show_sky{ true };
	glm::vec4 bg{ 0.3 };
	glm::float32 floor_scale{ 32 };
	glm::float32 floor_size{ 150 };
	bool floor_draw{ true };
	glm::float32 bumper_scale{ 0.3f };
	bool bumper_draw{ true };
	bool bumper_camera{ true };
	bool bumper_control{ true };
} IG_state;

Bumper bumper{ glm::vec3(0, 0.2, 0), 0.3 };

int main(int argc, char* argv[]) {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	auto window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Example", nullptr, nullptr);
	if (!window)
		throw std::runtime_error("Error creating glfw window");
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);


	// glfwSwapInterval(1);

	if (!gladLoadGL(glfwGetProcAddress))
	{
		throw std::runtime_error("Error initializing glad");
	}
	/**
	 * Initialize ImGui
	 */
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");
	ImGui::StyleColorsDark();

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(message_callback, nullptr);

	/**
	 * Compile shader
	 */

	Program program{ "resources/shaders/default.vs", "resources/shaders/default.fs" };
	GLuint vao;
	glCreateVertexArrays(1, &vao);
	// Create Attribute
	glEnableVertexArrayAttrib(vao, 0);
	glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE,
		offsetof(glm::vec2, x));
	glVertexArrayAttribBinding(vao, 0, 0);

	glm::vec2 vertices[] = { {-0.2, -0.2}, {-0.2, 0.2}, {0.2, 0.2}, {0.2, -0.2} };
	Buffer<glm::vec2> vbo{ vertices };
	GLuint indices[] = { 0, 2, 1, 2, 0, 3 };
	Buffer<GLuint> ibo{ indices };
	glVertexArrayVertexBuffer(vao, 0, vbo.getId(), 0, sizeof(glm::vec2));
	glVertexArrayElementBuffer(vao, ibo.getId());


	Program floor_program{ "resources/shaders/floor.vs", "resources/shaders/floor.fs" };
	GLuint floor;
	glCreateVertexArrays(1, &floor);
	// Create Attribute
	glEnableVertexArrayAttrib(floor, 0);
	glVertexArrayAttribFormat(floor, 0, 3, GL_FLOAT, GL_FALSE,
		offsetof(glm::vec3, x));
	glVertexArrayAttribBinding(floor, 0, 0);
	glm::mat4 floor_model{ 1.0f };
	glm::translate(floor_model, glm::vec3(0.0, -1.0f, 0.0f));

	Buffer<glm::vec3> floor_vbo{ floorVertices };
	Buffer<GLuint> floor_ibo{ floorIndices };

	glVertexArrayVertexBuffer(floor, 0, floor_vbo.getId(), 0, sizeof(glm::vec3));
	glVertexArrayElementBuffer(floor, floor_ibo.getId());

	Texture floor_tex[] = {
		{"resources/textures/floor/diffuse.jpg", GL_RGB, Texture::TextureType::DIFFUSE},
		{"resources/textures/floor/diffuse.jpg", GL_RGB, Texture::TextureType::SPECULAR},
		{ "resources/textures/floor/diffuse.jpg", GL_RGB, Texture::TextureType::NORMAL }
	};

	Program model_program{ "resources/shaders/model.vs", "resources/shaders/model.fs" };

	Model ourModel("resources/models/Bumper.fbx");


	Program skybox_program{ "resources/shaders/sky_box.vs", "resources/shaders/sky_box.fs" };
	GLuint skybox;
	glCreateVertexArrays(1, &skybox);
	// Create Attribute
	glEnableVertexArrayAttrib(skybox, 0);
	glVertexArrayAttribFormat(skybox, 0, 3, GL_FLOAT, GL_FALSE,
		offsetof(glm::vec3, x));
	glVertexArrayAttribBinding(skybox, 0, 0);

	Buffer<glm::vec3> skybox_vbo{ skyboxVertices };
	Buffer<GLuint> skybox_ibo{ skyboxIndices };

	glVertexArrayVertexBuffer(skybox, 0, skybox_vbo.getId(), 0, sizeof(glm::vec3));
	glVertexArrayElementBuffer(skybox, skybox_ibo.getId());
	glUseProgram(skybox_program.getId());
	glUniform1i(glGetUniformLocation(skybox_program.getId(), "skybox"), 0);

	// array of 6 strings holding paths to 6 images 
	std::string facesCubeMap[6] =
	{
		"resources/textures/skybox/tile004.jpg", // right
		"resources/textures/skybox/tile006.jpg", // left
		"resources/textures/skybox/tile003.jpg", // top
		"resources/textures/skybox/tile011.jpg", // bottom
		"resources/textures/skybox/tile007.jpg", // front
		"resources/textures/skybox/tile005.jpg" // back
	};

	//creating a cube map texture 
	unsigned int cubeMapTexture;
	glGenTextures(1, &cubeMapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(facesCubeMap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "failed to load texture: " << facesCubeMap[i] << std::endl;
			stbi_image_free(data);
		}
	}


	// timing
	float last_frame{ 0 };
	Bumper test_bumper{ glm::vec3(3, 0.2, 0), 0.3 };
	bumper.collisions.push_back(&test_bumper.box);
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window)) {
		// calculate deltatime
		// --------------------
		float current_frame = static_cast<float>(glfwGetTime());
		dt = current_frame - last_frame;
		last_frame = current_frame;

		processInput(window);




		bumper.update(dt);
		test_bumper.update(dt);
		if (IG_state.bumper_camera) {
			camera.position = bumper.position;
			camera.position.y += 0.5;
		}



		camera.updateMatrix(45.0f, 0.1f, 100.0f);
		//bumper.acceleration -= 0.01;
		//bumper.velocity += bumper.acceleration;
		//bumper.model = glm::translate(bumper.model, )


		glUseProgram(model_program.getId());

		// view/projection transformations
		camera.Matrix(model_program, "camera");

		// render the loaded model
		if (IG_state.bumper_draw) {
			bumper.draw(model_program, ourModel);
			test_bumper.draw(model_program, ourModel);
		}

		if (IG_state.floor_draw) {
			glUseProgram(floor_program.getId());
			floor_tex[0].bind(0);
			glUniformMatrix4fv(glGetUniformLocation(floor_program.getId(), "model"), 1, GL_FALSE, glm::value_ptr(floor_model));
			glUniform1f(glGetUniformLocation(floor_program.getId(), "scale"), IG_state.floor_scale);
			glUniform1f(glGetUniformLocation(floor_program.getId(), "size"), IG_state.floor_size);
			glUniform1i(glGetUniformLocation(floor_program.getId(), "diffuseTex"), 0);
			glUniform1i(glGetUniformLocation(floor_program.getId(), "specularTex"), 1);
			glUniform1i(glGetUniformLocation(floor_program.getId(), "normalTex"), 2);
			glUniform3fv(glGetUniformLocation(floor_program.getId(), "camPos"), 1, glm::value_ptr(camera.position));
			glUniform4fv(glGetUniformLocation(floor_program.getId(), "lightColor"), 1, glm::value_ptr(IG_state.bg));



			camera.Matrix(floor_program, "camera");
			glBindVertexArray(floor);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		}



		if (IG_state.show_sky) {
			glm::mat4 view{ glm::mat3(glm::lookAt(camera.position, camera.position + camera.forward, camera.world_up)) };
			glm::mat4 projection = glm::perspective(glm::radians(105.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
			glDepthFunc(GL_LEQUAL);
			glUseProgram(skybox_program.getId());
			glUniformMatrix4fv(glGetUniformLocation(skybox_program.getId(), "camera"), 1, GL_FALSE, glm::value_ptr(projection * view));

			glBindVertexArray(skybox);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
			glDepthFunc(GL_LESS);
		}


		// Imgui Rendering Menu
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		// ImGui::ShowDemoWindow();
		ImGui::Begin("Debug Menu");
		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		ImGui::BeginTabBar("MyTabBar", tab_bar_flags);
		if (ImGui::BeginTabItem("Globals"))
		{
			ImGui::ColorPicker4("Background", glm::value_ptr(IG_state.bg));
			ImGui::Checkbox("Cubemap", &IG_state.show_sky);
			ImGui::Checkbox("Metric Tool", &IG_state.show_metric);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Camera"))
		{
			ImGui::InputFloat("Pitch", &camera.pitch);
			ImGui::InputFloat("Yaw", &camera.yaw);
			ImGui::DragFloat3("Position", glm::value_ptr(camera.position));
			ImGui::DragFloat3("Forward", glm::value_ptr(camera.forward));
			ImGui::DragFloat3("World Up", glm::value_ptr(camera.world_up));
			ImGui::InputFloat("Fov/Zoom", &camera.zoom);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Floor"))
		{
			ImGui::Checkbox("Draw Floor?", &IG_state.floor_draw);
			ImGui::DragFloat("Scale: ", &IG_state.floor_scale, 1.0f, 1.0f, 50.0f);
			ImGui::DragFloat("Size: ", &IG_state.floor_size, 1.0f, 1.0f, 50.0f);

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Models"))
		{
			ImGui::SeparatorText("Bumper Car");
			ImGui::Checkbox("Draw Bumper?", &IG_state.bumper_draw);
			ImGui::Checkbox("Snap Camera", &IG_state.bumper_camera);
			ImGui::Checkbox("Control Bumper", &IG_state.bumper_control);
			ImGui::InputFloat("Velocity", &bumper.velocity);
			ImGui::DragFloat("Scale", &bumper.scale, 0.02f, 0.02f, 1.0f);
			ImGui::InputFloat("Acceleration Power", &bumper.power);
			ImGui::InputFloat("Breaking Power", &bumper.braking);
			ImGui::InputFloat("Handling Degrees", &bumper.handle);
			ImGui::SeparatorText("HitBox Bumper");
			ImGui::DragFloat("Width", &bumper.box.width, 0.05f, 0.05, 5.0f);
			ImGui::DragFloat("Length", &bumper.box.length, 0.05f, 0.05, 5.0f);
			ImGui::DragFloat("Height", &bumper.box.height, 0.05f, 0.05, 5.0f);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
		ImGui::End();
		if (IG_state.show_metric)
			ImGui::ShowMetricsWindow(&IG_state.show_metric);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}


// process all keyboard inputs
void processInput(GLFWwindow* window)
{
	// If they want to quit
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	// Proccess Mouse Input
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS && !is_mdl_pressed_already) {
		cursor_in_focus = !cursor_in_focus;
		if (cursor_in_focus) { // IF CURSOR IS IN FOCUS
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
	is_mdl_pressed_already = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);

	if (IG_state.bumper_control) {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			bumper.process_keyboard(FORWARD, dt);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			bumper.process_keyboard(BACKWARD, dt);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			bumper.process_keyboard(LEFT, dt);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			bumper.process_keyboard(RIGHT, dt);
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			bumper.process_keyboard(UP, dt);
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			bumper.process_keyboard(DOWN, dt);
		return;
	}



	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.process_keyboard(FORWARD, dt);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.process_keyboard(BACKWARD, dt);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.process_keyboard(LEFT, dt);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.process_keyboard(RIGHT, dt);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.process_keyboard(UP, dt);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.process_keyboard(DOWN, dt);
}



// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (!cursor_in_focus)
	{
		last_x = xpos;
		last_y = ypos;
	}

	float xoffset = xpos - last_x;
	float yoffset = last_y - ypos; // reversed since y-coordinates go from bottom to top

	last_x = xpos;
	last_y = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
