
#include <SDL2/SDL.h>
#include <glad/glad.h>

#include "vmath.h"
#include "gl.h"
#include "cam.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>
#include <iostream>

using std::cout;
using std::endl;

SDL_Window* window = null;
SDL_GLContext gl_context = null;

enum class mode {
	cam, 
	idle
};

void plt_setup() {

	SDL_Init(0);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	window = SDL_CreateWindow("Hello World", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

	gl_context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, gl_context);
	SDL_GL_SetSwapInterval(1);

	gladLoadGL();

	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
	ImGui_ImplOpenGL3_Init();

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(debug_proc, 0);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
}

void plt_shutdown() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	gl_check_leaked_handles();
	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);
	window = null; gl_context = null;
	SDL_Quit();	
}

void render_frame() {

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(window);
}

void begin_frame() {

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window);
	ImGui::NewFrame();

	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

int main(int, char**) {

	plt_setup();

	shader s; s.load("mesh.v", "mesh.f");
	mesh m; m.init(); m.push_cube(v3(0, 0, 0), 1.0f);

	camera_free cam; cam.reset();
	i32 mx = 0, my = 0, last_mx = 0, last_my = 0, w = 1280, h = 720;
	mode ui_mode = mode::idle;

	const u8* keys = SDL_GetKeyboardState(NULL);

	bool running = true;
	while(running) {
	
		begin_frame();
		ImGuiIO& io = ImGui::GetIO();

		SDL_Event e;
		while(SDL_PollEvent(&e)) {

			ImGui_ImplSDL2_ProcessEvent(&e);

			switch(e.type) {
			case SDL_QUIT: {
				running = false;
			} break;

			case SDL_WINDOWEVENT: {
				if(e.window.event == SDL_WINDOWEVENT_RESIZED ||
					e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
					w = e.window.data1;
					h = e.window.data2;
					glViewport(0, 0, w, h);
				}
			} break;

			case SDL_KEYDOWN: {
				if(e.key.keysym.sym == SDLK_ESCAPE) {
					running = false;
				}
			} break;

			case SDL_MOUSEMOTION: {
				if(ui_mode == mode::cam) {
					i32 dx = (e.motion.x - mx);
					i32 dy = (e.motion.y - my);
					cam.move(dx, dy);
				}
				mx = e.motion.x;
				my = e.motion.y;
			} break;

			case SDL_MOUSEBUTTONDOWN: {
				if(!io.WantCaptureMouse) {
					ui_mode = mode::cam;
					SDL_CaptureMouse(SDL_TRUE);
					SDL_SetRelativeMouseMode(SDL_TRUE);
				}
				last_mx = e.button.x;
				last_my = e.button.y;
			} break;

			case SDL_MOUSEBUTTONUP: {
				if(!io.WantCaptureMouse && ui_mode == mode::cam) {
					ui_mode = mode::idle;
					SDL_CaptureMouse(SDL_FALSE);
					SDL_SetRelativeMouseMode(SDL_FALSE);
					SDL_WarpMouseInWindow(window, last_mx, last_my);
				}
			} break;
			}
		}

		f32 dT = (SDL_GetTicks() - cam.lastUpdate) / 1000.0f;
		cam.lastUpdate = SDL_GetTicks();
		if (ui_mode == mode::cam) {
			if(keys[SDL_SCANCODE_W]) {
				cam.pos += cam.front * cam.speed * dT;
			}
			if(keys[SDL_SCANCODE_S]) {
				cam.pos -= cam.front * cam.speed * dT;
			}
			if(keys[SDL_SCANCODE_A]) {
				cam.pos -= cam.right * cam.speed * dT;
			}
			if(keys[SDL_SCANCODE_D]) {
				cam.pos += cam.right * cam.speed * dT;
			}
			if(keys[SDL_SCANCODE_SPACE]) {
				cam.pos.y += cam.speed * dT;
			}
			if(keys[SDL_SCANCODE_LSHIFT]) {
				cam.pos.y -= cam.speed * dT;
			}
		}

		ImGui::ShowDemoWindow();

		s.use();
		m4 transform = proj(cam.fov, (f32)w / (f32)h, 0.01f, 2000.0f) * cam.view();
		glUniformMatrix4fv(s.getUniform("transform"), 1, GL_FALSE, transform.a);
		
		m.update();
		m.render();

		render_frame();
	}

	s.destroy();
	m.destroy();

	plt_shutdown();

	return 0;
}
