#include <glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

int main(void) {
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	int width = 960;
	int height = 540;
	window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	/* Sync fps with monitor refresh rate */
	glfwSwapInterval(3);

	/* Init Glew for Modern OpenGL */
	if (glewInit() != GLEW_OK) {
		printf("Error!");
		return -1;
	}
	GLCall(printf("%s\n", glGetString(GL_VERSION)));

	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	{
		int sWidth = 100;
		int sHeight = 100;
		float positions[] = {
			-50.0f, -50.0f, 0.0f, 0.0f,
			 50.0f, -50.0f, 1.0f, 0.0f,
			 50.0f,  50.0f, 1.0f, 1.0f,
			-50.0f,  50.0f, 0.0f, 1.0f
		};
		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		// Create VAO
		VertexArray va;

		// Create VBO
		VertexBuffer vb(positions, 4 * 4 * sizeof(float));

		// Create Attributes layout
		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

		// Create Index Buffer
		IndexBuffer ib(indices, 6);

		// Create Projection and View Matrix
		glm::mat4 proj = glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

		// Create Shader Program
		Shader shader("res/shaders/Basic.shader");
		shader.Bind();

		// Create Texture
		Texture texture("res/textures/profile.png");
		texture.Bind();

		// Set Uniforms
		shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);
		shader.SetUniform1i("u_Texture", 0);

		// Unbind all buffers
		va.UnBind();
		vb.UnBind();
		ib.UnBind();
		shader.UnBind();

		Renderer renderer;

		// Setup ImGui binding
		ImGui::CreateContext();
		ImGui_ImplGlfwGL3_Init(window, true);
		ImGui::StyleColorsDark();

		bool show_demo_window = true;
		bool show_another_window = false;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		glm::vec3 translationA(50, 50, 0);
		glm::vec3 translationB(200, 200, 0);
		float r = 0.0f;
		float increment = 0.01f;
		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window)) {
			/* Render here */
			renderer.Clear();

			ImGui_ImplGlfwGL3_NewFrame();

			{
				glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
				glm::mat4 mvp = proj * view * model;
				shader.Bind();
				shader.SetUniform4f("u_Color", r, 0.3f, 0.0f, 1.0f);
				shader.SetUniformMat4f("u_MVP", mvp);
				renderer.Draw(va, ib, shader);
			}

			{
				glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
				glm::mat4 mvp = proj * view * model;
				shader.Bind();
				shader.SetUniform4f("u_Color", 0.0f, 0.3f, r, 1.0f);
				shader.SetUniformMat4f("u_MVP", mvp);
				renderer.Draw(va, ib, shader);
			}

			if (r > 1.0f || r < 0.0f)
				increment = -increment;

			r += increment;

			{
				ImGui::SliderFloat3("Translation A", &translationA.x, 0.0f, 960.0f);
				ImGui::SliderFloat3("Translation B", &translationB.x, 0.0f, 960.0f);
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}

			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
	}

	// Cleanup
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();

	return 0;
}