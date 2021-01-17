#include "ModuleEditor.h"

#include "Globals.h"
#include "Application.h"
#include "SceneImporter.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleScene.h"

#include "ImGuizmo.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "GL/glew.h"
#include "SDL_video.h"
#include "Brofiler.h"

#include "Leaks.h"

bool ModuleEditor::Init()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigWindowsMoveFromTitleBarOnly = true;

	return true;
}

bool ModuleEditor::Start()
{
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer->context);
	ImGui_ImplOpenGL3_Init(GLSL_VERSION);

	// Define Color Style
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 1.0f, 1.0f, 0.10f));
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, (ImVec4) ImColor(0.54f, 0.43f, 0.12f, 0.97f));

	panels.push_back(&panel_scene);
	panels.push_back(&panel_console);
	panels.push_back(&panel_configuration);
	panels.push_back(&panel_hierarchy);
	panels.push_back(&panel_inspector);
	panels.push_back(&panel_about);

	return true;
}

UpdateStatus ModuleEditor::PreUpdate()
{
	BROFILER_CATEGORY("ModuleEditor - PreUpdate", Profiler::Color::Azure)

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	return UpdateStatus::CONTINUE;
}

UpdateStatus ModuleEditor::Update()
{
	BROFILER_CATEGORY("ModuleEditor - Update", Profiler::Color::Azure)

	ImGui::CaptureMouseFromApp(true);
	ImGui::CaptureKeyboardFromApp(true);

	// TODO: Remove after test
	ImGui::ShowDemoWindow();

	// Main menu bar
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("New"))
		{
			modal_to_open = Modal::NEW_SCENE;
		}
		if (ImGui::MenuItem("Load"))
		{
			modal_to_open = Modal::LOAD_SCENE;
		}
		if (ImGui::MenuItem("Save"))
		{
			modal_to_open = Modal::SAVE_SCENE;
		}
		if (ImGui::MenuItem("Quit"))
		{
			modal_to_open = Modal::QUIT;
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("View"))
	{
		ImGui::MenuItem(panel_scene.name, "", &panel_scene.enabled);
		ImGui::MenuItem(panel_console.name, "", &panel_console.enabled);
		ImGui::MenuItem(panel_inspector.name, "", &panel_inspector.enabled);
		ImGui::MenuItem(panel_hierarchy.name, "", &panel_hierarchy.enabled);
		ImGui::MenuItem(panel_configuration.name, "", &panel_configuration.enabled);
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Help"))
	{
		if (ImGui::MenuItem("Repository"))
		{
			App->RequestBrowser("https://github.com/JaimeBea/RealBugEngine/wiki");
		}
		if (ImGui::MenuItem("Download latest"))
		{
			App->RequestBrowser("https://github.com/JaimeBea/RealBugEngine/releases");
		}
		if (ImGui::MenuItem("Report a bug"))
		{
			App->RequestBrowser("https://github.com/JaimeBea/RealBugEngine/issues");
		}
		ImGui::MenuItem(panel_about.name, "", &panel_about.enabled);
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();

	// Modals
	switch (modal_to_open)
	{
	case Modal::NEW_SCENE:
		ImGui::OpenPopup("New scene");
		break;
	case Modal::LOAD_SCENE:
		ImGui::OpenPopup("Load scene");
		break;
	case Modal::SAVE_SCENE:
		ImGui::OpenPopup("Save scene");
		break;
	case Modal::QUIT:
		ImGui::OpenPopup("Quit");
		break;
	}
	modal_to_open = Modal::NONE;

	ImGui::SetNextWindowSize(ImVec2(250, 100), ImGuiCond_FirstUseEver);
	if (ImGui::BeginPopupModal("New scene"))
	{
		ImGui::Text("Do you wish to create a new scene?");
		if (ImGui::Button("New scene"))
		{
			App->scene->CreateEmptyScene();
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	ImGui::SetNextWindowSize(ImVec2(250, 100), ImGuiCond_FirstUseEver);
	if (ImGui::BeginPopupModal("Load scene"))
	{
		ImGui::InputText("File name", file_name_buffer, sizeof(file_name_buffer));
		if (ImGui::Button("Load"))
		{
			SceneImporter::LoadScene(file_name_buffer);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	ImGui::SetNextWindowSize(ImVec2(250, 100), ImGuiCond_FirstUseEver);
	if (ImGui::BeginPopupModal("Save scene"))
	{
		ImGui::SetItemDefaultFocus();
		ImGui::InputText("File name", file_name_buffer, sizeof(file_name_buffer));
		if (ImGui::Button("Save"))
		{
			SceneImporter::SaveScene(file_name_buffer);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	ImGui::SetNextWindowSize(ImVec2(250, 100), ImGuiCond_FirstUseEver);
	if (ImGui::BeginPopupModal("Quit"))
	{
		ImGui::Text("Do you really want to quit?");
		if (ImGui::Button("Quit"))
		{
			return UpdateStatus::STOP;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	// Docking
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGuiID dock_space_id = ImGui::GetID("DockSpace");

	if (!ImGui::DockBuilderGetNode(dock_space_id))
	{
		ImGui::DockBuilderAddNode(dock_space_id);
		ImGui::DockBuilderSetNodeSize(dock_space_id, viewport->GetWorkSize());

		dock_main_id = dock_space_id;
		dock_left_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.25f, nullptr, &dock_main_id);
		dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.33f, nullptr, &dock_main_id);
		dock_down_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.3f, nullptr, &dock_main_id);
	}

	ImGui::SetNextWindowPos(viewport->GetWorkPos());
	ImGui::SetNextWindowSize(viewport->GetWorkSize());

	ImGuiWindowFlags dock_space_window_flags = 0;
	dock_space_window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
	dock_space_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", nullptr, dock_space_window_flags);
	ImGui::PopStyleVar(3);
	ImGui::DockSpace(dock_space_id);
	ImGui::End();

	// Panels
	for (Panel* panel : panels)
	{
		if (panel->enabled)
		{
			panel->Update();
		}
	}

	return UpdateStatus::CONTINUE;
}

UpdateStatus ModuleEditor::PostUpdate()
{
	BROFILER_CATEGORY("ModuleEditor - PostUpdate", Profiler::Color::Azure)

	// Draw to default frame buffer (main window)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Render main window
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Handle and render other windows
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
	SDL_GL_MakeCurrent(App->window->window, App->renderer->context);

	return UpdateStatus::CONTINUE;
}

bool ModuleEditor::CleanUp()
{
	panels.clear();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}
