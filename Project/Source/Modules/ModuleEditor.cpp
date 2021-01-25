#include "ModuleEditor.h"

#include "Globals.h"
#include "Application.h"
#include "FileSystem/SceneImporter.h"
#include "Modules/ModuleWindow.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleScene.h"

#include "ImGuizmo.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_utils.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "IconsFontAwesome5.h"
#include "IconsForkAwesome.h"
#include "GL/glew.h"
#include "SDL_video.h"
#include "Brofiler.h"

#include "Utils/Leaks.h"

static void ApplyCustomStyle() {
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	colors[ImGuiCol_Text] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.500f, 0.500f, 0.500f, 1.000f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.280f, 0.280f, 0.280f, 0.000f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
	colors[ImGuiCol_Border] = ImVec4(0.266f, 0.266f, 0.266f, 1.000f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.200f, 0.200f, 0.200f, 1.000f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.280f, 0.280f, 0.280f, 1.000f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.277f, 0.277f, 0.277f, 1.000f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.300f, 0.300f, 0.300f, 1.000f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_CheckMark] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_Button] = ImVec4(1.000f, 1.000f, 1.000f, 0.000f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
	colors[ImGuiCol_ButtonActive] = ImVec4(1.000f, 1.000f, 1.000f, 0.391f);
	colors[ImGuiCol_Header] = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(1.000f, 1.000f, 1.000f, 0.250f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.670f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_Tab] = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.352f, 0.352f, 0.352f, 1.000f);
	colors[ImGuiCol_TabActive] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	colors[ImGuiCol_DockingPreview] = ImVec4(1.000f, 0.391f, 0.000f, 0.781f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.000f, 0.373f, 0.000f, 1.000f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_NavHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
	colors[ImGuiCol_UrlText] = ImVec4(0.000f, 0.451f, 1.0f, 1.0f);

	style->ChildRounding = 4.0f;
	style->FrameBorderSize = 1.0f;
	style->FrameRounding = 2.0f;
	style->GrabMinSize = 7.0f;
	style->PopupRounding = 2.0f;
	style->ScrollbarRounding = 12.0f;
	style->ScrollbarSize = 13.0f;
	style->TabBorderSize = 1.0f;
	style->TabRounding = 0.0f;
	style->WindowRounding = 0.0f;

	// Merge in icons from Font Awesome and Fork Awesome
	ImGui::GetIO().Fonts->AddFontDefault();
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;

	static const ImWchar icons_ranges_fa[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
	ImGui::GetIO().Fonts->AddFontFromFileTTF("Fonts/" FONT_ICON_FILE_NAME_FAS, 12.0f, &icons_config, icons_ranges_fa);

	static const ImWchar icons_ranges_fk[] = {ICON_MIN_FK, ICON_MAX_FK, 0};
	ImGui::GetIO().Fonts->AddFontFromFileTTF("Fonts/" FONT_ICON_FILE_NAME_FK, 12.0f, &icons_config, icons_ranges_fk);
}

bool ModuleEditor::Init() {
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigWindowsMoveFromTitleBarOnly = true;

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		io.ConfigViewportsNoAutoMerge = false;
		io.ConfigViewportsNoTaskBarIcon = true;
	}

	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
		io.ConfigDockingTransparentPayload = true;
	}

	ApplyCustomStyle();

	return true;
}

bool ModuleEditor::Start() {
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

UpdateStatus ModuleEditor::PreUpdate() {
	BROFILER_CATEGORY("ModuleEditor - PreUpdate", Profiler::Color::Azure)

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	return UpdateStatus::CONTINUE;
}

UpdateStatus ModuleEditor::Update() {
	BROFILER_CATEGORY("ModuleEditor - Update", Profiler::Color::Azure)

	ImGui::CaptureMouseFromApp(true);
	ImGui::CaptureKeyboardFromApp(true);

	// Main menu bar
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("File")) {
		if (ImGui::MenuItem("New")) {
			modal_to_open = Modal::NEW_SCENE;
		}
		if (ImGui::MenuItem("Load")) {
			modal_to_open = Modal::LOAD_SCENE;
		}
		if (ImGui::MenuItem("Save")) {
			modal_to_open = Modal::SAVE_SCENE;
		}
		if (ImGui::MenuItem("Quit")) {
			modal_to_open = Modal::QUIT;
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("View")) {
		ImGui::MenuItem(panel_scene.name, "", &panel_scene.enabled);
		ImGui::MenuItem(panel_console.name, "", &panel_console.enabled);
		ImGui::MenuItem(panel_inspector.name, "", &panel_inspector.enabled);
		ImGui::MenuItem(panel_hierarchy.name, "", &panel_hierarchy.enabled);
		ImGui::MenuItem(panel_configuration.name, "", &panel_configuration.enabled);
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Help")) {
		if (ImGui::MenuItem("Repository")) {
			App->RequestBrowser("https://github.com/TBD-org/TBD-Engine/wiki");
		}
		if (ImGui::MenuItem("Download latest")) {
			App->RequestBrowser("https://github.com/TBD-org/TBD-Engine/releases");
		}
		if (ImGui::MenuItem("Report a bug")) {
			App->RequestBrowser("https://github.com/TBD-org/TBD-Engine/issues");
		}
		ImGui::MenuItem(panel_about.name, "", &panel_about.enabled);
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();

	// Modals
	switch (modal_to_open) {
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
	if (ImGui::BeginPopupModal("New scene")) {
		ImGui::Text("Do you wish to create a new scene?");
		if (ImGui::Button("New scene")) {
			App->scene->CreateEmptyScene();
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	ImGui::SetNextWindowSize(ImVec2(250, 100), ImGuiCond_FirstUseEver);
	if (ImGui::BeginPopupModal("Load scene")) {
		ImGui::InputText("File name", file_name_buffer, sizeof(file_name_buffer));
		if (ImGui::Button("Load")) {
			SceneImporter::LoadScene(file_name_buffer);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	ImGui::SetNextWindowSize(ImVec2(250, 100), ImGuiCond_FirstUseEver);
	if (ImGui::BeginPopupModal("Save scene")) {
		ImGui::SetItemDefaultFocus();
		ImGui::InputText("File name", file_name_buffer, sizeof(file_name_buffer));
		if (ImGui::Button("Save")) {
			SceneImporter::SaveScene(file_name_buffer);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	ImGui::SetNextWindowSize(ImVec2(250, 100), ImGuiCond_FirstUseEver);
	if (ImGui::BeginPopupModal("Quit")) {
		ImGui::Text("Do you really want to quit?");
		if (ImGui::Button("Quit")) {
			return UpdateStatus::STOP;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	// Docking
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGuiID dock_space_id = ImGui::GetID("DockSpace");

	if (!ImGui::DockBuilderGetNode(dock_space_id)) {
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
	for (Panel* panel : panels) {
		if (panel->enabled) {
			panel->Update();
		}
	}

	return UpdateStatus::CONTINUE;
}

UpdateStatus ModuleEditor::PostUpdate() {
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

bool ModuleEditor::CleanUp() {
	panels.clear();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}
