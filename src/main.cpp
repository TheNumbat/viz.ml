
#include <SDL2/SDL.h>
#include <glad/glad.h>

#include "vmath.h"
#include "gl.h"
#include "cam.h"
#include "data.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>

#include <iostream>
#include <iomanip>
#include <vector>

#include <bhtsne/sp_tsne.h>
#include <nlopt/nlopt.hpp>

using std::cout;
using std::endl;

SDL_Window* window = null;
SDL_GLContext gl_context = null;

void plt_setup() {

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

	window = SDL_CreateWindow("Viz", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

	gl_context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, gl_context);
	SDL_GL_SetSwapInterval(1);

	gladLoadGL();

	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
	ImGui_ImplOpenGL3_Init();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_SAMPLE_SHADING);
	glMinSampleShading(1.0f);

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback((GLDEBUGPROC)debug_proc, 0);
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
}

enum class uimode : i32 {
	cam = 0, 
	idle
};

enum class vizmode : i32 {
	axis = 0,
	count
};

const char* viz_names[vizmode::count] = {
	"Axis"
};

struct uistate {

	uimode mode = uimode::idle;
	vizmode viz = vizmode::axis;
	i32 mx = 0, my = 0, last_mx = 0, last_my = 0, w = 1280, h = 720;

	i32 t_x = 405, t_y = 406, t_z = 407;
};

i32 main(i32, char**) {

	plt_setup();
	ImGui::GetStyle().WindowRounding = 0.0f;

	uistate ui;
	{
		camera_orbit cam; cam.reset();

		scene sc; sc.init(ui.w, ui.h);

		dataset* d = new dataset; d->load("images.dat", "labels.dat");
		d->transform_axis(sc, ui.t_x, ui.t_y, ui.t_z);

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
						ui.w = e.window.data1;
						ui.h = e.window.data2;
						sc.update_wh(ui.w, ui.h);
					}
				} break;

				case SDL_KEYDOWN: {
					if(e.key.keysym.sym == SDLK_ESCAPE) {
						running = false;
					}
				} break;

				case SDL_MOUSEMOTION: {
					if(ui.mode == uimode::cam) {
						i32 dx = (e.motion.x - ui.mx);
						i32 dy = (e.motion.y - ui.my);
						cam.move(dx, dy);
					}
					ui.mx = e.motion.x;
					ui.my = e.motion.y;
				} break;

				case SDL_MOUSEBUTTONDOWN: {
					if(!io.WantCaptureMouse && ui.mode == uimode::idle) {
						ui.mode = uimode::cam;
						SDL_CaptureMouse(SDL_TRUE);
						SDL_SetRelativeMouseMode(SDL_TRUE);
					}
					ui.last_mx = e.button.x;
					ui.last_my = e.button.y;
				} break;

				case SDL_MOUSEBUTTONUP: {
					if(!io.WantCaptureMouse && ui.mode == uimode::cam) {
						ui.mode = uimode::idle;
						SDL_CaptureMouse(SDL_FALSE);
						SDL_SetRelativeMouseMode(SDL_FALSE);
						SDL_WarpMouseInWindow(window, ui.last_mx, ui.last_my);
					}
				} break;

				case SDL_MOUSEWHEEL: {
					if(!io.WantCaptureMouse) {
						cam.radius -= e.wheel.y / 5.0f;
						if(cam.radius < 0.5f) cam.radius = 0.5f;
						cam.updatePos();
					}
				} break;
				}
			}

			m4 transform = proj(cam.fov, (f32)ui.w / (f32)ui.h, 0.01f, 2000.0f) * cam.view() * scale(v3(0.1f, 0.1f, 0.1f));
			sc.render(transform);

			{
				ImGui::SetNextWindowPos({0, 0});
				ImGui::SetNextWindowSize({ui.w / 5.0f, (f32)ui.h});
				ImGui::Begin("Viz", null, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);

				ImGui::Combo("Type", (i32*)&ui.viz, viz_names, (i32)vizmode::count);
				ImGui::Separator();

				switch(ui.viz) {
				case vizmode::axis: {
					ImGui::Text("Axes");
					ImGui::SliderInt("X", &ui.t_x, 0, 784);
					ImGui::SliderInt("Y", &ui.t_y, 0, 784);
					ImGui::SliderInt("Z", &ui.t_z, 0, 784);
					ImGui::Separator();
				} break;
				}

				if(ImGui::Button("Apply")) {
					switch(ui.viz) {
					case vizmode::axis: {
						d->transform_axis(sc, ui.t_x, ui.t_y, ui.t_z);
					} break;
					}
				}

				ImGui::End();
			}

			render_frame();
		}

		sc.destroy();
		delete d;
	}

	plt_shutdown();

	return 0;
}
