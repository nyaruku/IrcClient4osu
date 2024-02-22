#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#pragma execution_character_set( "utf-8" )
#include <string>
#include <stdio.h>
#include <thread>
#include <filesystem>
#include <iostream>
#include <vector>
#include <WinSock2.h>
#include <Windows.h>
#include <codecvt>
#include "logger.h"
#include <future>
#pragma comment(lib,"WS2_32")

void connection_watcher();
void result_watcher();

#pragma region IMGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#include "imgui_stdlib.h"
#include <GLFW/glfw3.h>

#pragma region IRC
#include "libircclient.h"
#include "libirc_rfcnumeric.h"
#include "libirc_options.h"
#include "libirc_events.h"
#include "libirc_errors.h"
#include "irc.h"

#pragma comment(lib, "legacy_stdio_definitions")

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}
// Main code
HWND console_window;
bool console_display = false;
std::string spinner_s = "";

std::string connectionStatus = "";
std::future<int> result;
int main(int, char**)
{
#pragma region Console Inits
	HWND console_window;
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	// Set output mode to handle virtual terminal sequences
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE)
	{
		//return GetLastError();
	}

	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode))
	{
		//return GetLastError();
	}

	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	if (!SetConsoleMode(hOut, dwMode))
	{
		//return GetLastError();
	}
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(hOut, &info);
	//UTF-8
	SetConsoleOutputCP(65001);
	console_window = FindWindowA("ConsoleWindowClass", NULL);
	//HIDE ON STARTUP
	ShowWindow(console_window, console_display);

	// ANSI CODES
	//bold text - \033[1m
	//RGB Foreground Color - \033[38;2;255;255;255m
	//RGB Background Color - \033[48;2;255;0;0m
	//Reset all - \033[0m

#pragma endregion
#pragma region stuff
	const double fpsLimitFocused = 1.0 / 60.0; // 60 FPS when focused
	const double fpsLimitUnfocused = 1.0 / 5.0; // 5 FPS when not focused
	double lastUpdateTime = 0;
	double lastFrameTime = 0;
	// Get the value of the WINDIR environment variable
	wchar_t windir[MAX_PATH];
	GetEnvironmentVariable(L"WINDIR", windir, MAX_PATH);
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
	std::string strWindir = converter.to_bytes(windir);
	std::string consolaFontPath = strWindir + "\\Fonts\\consola.ttf";

	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow(900, 600, "IRC Client for osu!", nullptr, nullptr);
	if (window == nullptr)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(2); // Enable vsync
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL2_Init();
	//glfwSetWindowSizeLimits(window, 400, 400, GLFW_DONT_CARE, GLFW_DONT_CARE);
	//check if consola font exists
	ImFont* consola = NULL;
	if (std::filesystem::exists(strWindir)) {
		// all good
		consola = io.Fonts->AddFontFromFileTTF(consolaFontPath.c_str(), 16);
	}
	else {
		if (std::filesystem::exists("%WINDIR%/")) {
			MessageBox(NULL, L"Consola Font not found", L"Error", MB_OK | MB_TASKMODAL);
		}
		else {
			MessageBox(NULL, L"Couldn't get WINDIR Path", L"Error", MB_OK | MB_TASKMODAL);
		}
	}

	ImVec4 clear_color = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	int ui_toggle = 0;

#pragma endregion

	switch (init_irc()) {
	case 0:
		break;
	case 1:
		break;
	case 2:
		break;
	}

	// Create a std::future object that will hold the result of the computation
	std::future<void> void_connection_watcher = std::async(std::launch::async, connection_watcher);
	std::future<void> void_result_watcher;
	std::string artText =
		"                       ________  ______\n"
		"  ____  _______  __   /  _/ __ \\/ ____/\n"
		" / __ \\/ ___/ / / /   / // /_/ / /     \n"
		"/ /_/ (__  ) /_/ /  _/ // _, _/ /___   \n"
		"\\____/____/\\__,_/  /___/_/ |_|\\____/   ";

	std::cout << "\033[1m\033[38;2;0;170;255m###########################################" << std::endl;
	std::cout << "\033[1m\033[38;2;0;170;255m# \033[38;2;255;0;187m                       ________  ______\033[38;2;0;170;255m #" << std::endl;
	std::cout << "\033[38;2;0;170;255m# \033[38;2;255;0;187m  ____  _______  __   /  _/ __ \\/ ____/\033[38;2;0;170;255m #" << std::endl;
	std::cout << "\033[38;2;0;170;255m# \033[38;2;255;0;187m / __ \\/ ___/ / / /   / // /_/ / /     \033[38;2;0;170;255m #" << std::endl;
	std::cout << "\033[38;2;0;170;255m# \033[38;2;255;0;187m/ /_/ (__  ) /_/ /  _/ // _, _/ /___   \033[38;2;0;170;255m #" << std::endl;
	std::cout << "\033[38;2;0;170;255m# \033[38;2;255;0;187m\\____/____/\\__,_/  /___/_/ |_|\\____/   \033[38;2;0;170;255m #" << std::endl;
	std::cout << "\033[1m\033[38;2;0;170;255m# \033[38;2;255;0;187m                                       \033[38;2;0;170;255m #" << std::endl;
	std::cout << "\033[1m\033[38;2;0;170;255m###########################################\033[0m" << std::endl;
	std::cout << "\033[38;2;0;170;255mosu!\033[38;2;255;255;255m IRC Client in \033[38;2;255;255;0mC++\033[38;2;255;255;255m by \033[38;2;0;170;255m\033[1m_Railgun_\033[0m" << std::endl;
	std::cout << "IRC Library: '\033[38;2;255;255;0mlibircclient\033[0m' | \033[38;2;0;170;255mhttps://github.com/shaoner/libircclient\033[0m" << std::endl;
	std::cout << "Frontend: '\033[38;2;255;255;0mImGui\033[0m' | \033[38;2;0;170;255mOpenGL 2\033[0m" << std::endl << std::endl;

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		double now = glfwGetTime();
		double deltaTime = now - lastUpdateTime;
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		if (consola != NULL) {
			ImGui::PushFont(consola);
		}

		ImGui::BeginMainMenuBar();
		ImGui::SetCursorPosX(0);
		if (ImGui::Button("Login", ImVec2(80, 21))) {
			ui_toggle = 0;
		}
		ImGui::SetCursorPosX(81);
		if (ImGui::Button("Chat", ImVec2(80, 21))) {
			ui_toggle = 1;
		}
		ImGui::SetCursorPosX(81 * 2);
		if (ImGui::Button("Settings", ImVec2(80, 21))) {
			ui_toggle = 2;
		}
		ImGui::SetCursorPosX(81 * 3);
		if (ImGui::Button("About", ImVec2(80, 21))) {
			ui_toggle = 3;
		}
		ImGui::SetCursorPosX(81 * 4);
		const char* logText = "";
		if (console_display) {
			logText = "Hide Console";
		}
		else {
			logText = "Show Console";
		}
		if (ImGui::Button(logText, ImVec2(120, 21))) {
			console_display = !console_display;
			ShowWindow(console_window, console_display);
		}
		ImGui::Text(connectionNotifier.c_str());
		ImGui::EndMainMenuBar();
		ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetFrameHeight()));
		ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y - ImGui::GetFrameHeight()));

		switch (ui_toggle) {
		case 0:
			ImGui::Begin("windowMain", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse);
			ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(artText.c_str()).x) * 0.5f);
			ImGui::SetCursorPosY((ImGui::GetWindowSize().y - ImGui::CalcTextSize(artText.c_str()).y) * 0.3f);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 255, 255, 1));
			ImGui::Text(artText.c_str());
			ImGui::PopStyleColor(1);
			ImGui::NewLine();
			ImGui::SetNextItemWidth(300);
			ImGui::SetCursorPosX((ImGui::GetWindowSize().x - (300 + ImGui::CalcTextSize("IRC Username").x)) * 0.5f);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(96.0f / 255.0f, 96.0f / 255.0f, 96.0f / 255.0f, 1));
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(25.0f / 255.0f, 25.0f / 255.0f, 25.0f / 255.0f, 1.00f));
			ImGui::InputText("Server", &con.server, ImGuiInputTextFlags_ReadOnly);
			ImGui::SetNextItemWidth(300);
			ImGui::SetCursorPosX((ImGui::GetWindowSize().x - (300 + ImGui::CalcTextSize("IRC Username").x)) * 0.5f);
			ImGui::InputText("Port", &con.port_string, ImGuiInputTextFlags_ReadOnly);
			ImGui::PopStyleColor(2);

			ImGui::SetNextItemWidth(300);
			ImGui::SetCursorPosX((ImGui::GetWindowSize().x - (300 + ImGui::CalcTextSize("IRC Username").x)) * 0.5f);
			ImGui::InputText("IRC Username", &con.username);
			ImGui::SetNextItemWidth(300);
			ImGui::SetCursorPosX((ImGui::GetWindowSize().x - (300 + ImGui::CalcTextSize("IRC Username").x)) * 0.5f);
			ImGui::InputText("IRC Password", &con.password, ImGuiInputTextFlags_Password);
			ImGui::SetCursorPosX((ImGui::GetWindowSize().x - (300 + ImGui::CalcTextSize("IRC Username").x)) * 0.5f);

			if (ImGui::Button("Connect", ImVec2(300, 21))) {
				result = std::async(std::launch::async, connect_irc, con.username.c_str(), con.password.c_str());
				void_result_watcher = std::async(std::launch::async, result_watcher);
			}
			ImGui::SetCursorPosX((ImGui::GetWindowSize().x - (300 + ImGui::CalcTextSize("IRC Username").x)) * 0.5f);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255, 0, 0, 1));
			ImGui::Text(connectionStatus.c_str());
			ImGui::PopStyleColor(1);
			ImGui::SetCursorPosX(4);
			ImGui::SetCursorPosY((ImGui::GetWindowSize().y - (ImGui::CalcTextSize("_Railgun_").y) - 4));
			ImGui::Text("_Railgun_");
			ImGui::End();
			break;
		case 1:
			// Chat
			break;
		case 2:
			// Settings
			break;
		case 3:
			// About
			break;
		}
		//Stack Underflow
		//ImGui::PopStyleColor(9999);
		ImGui::PopFont();
		ImGui::EndFrame();
#pragma region Performance
		// Check if the window is focused
		double fpsLimit = 0;
		int focused = glfwGetWindowAttrib(window, GLFW_FOCUSED);
		int minimized = glfwGetWindowAttrib(window, GLFW_ICONIFIED);
		if (minimized == GLFW_TRUE) {
			// The window is minimized
			fpsLimit = fpsLimitUnfocused;
			Sleep(250);
		}
		else {
			if (focused == GLFW_TRUE) {
				// in focus
				fpsLimit = fpsLimitFocused;
			}
			else {
				// not focused
				fpsLimit = fpsLimitUnfocused;
				Sleep(250);
			}
		}
		// This if-statement only executes once every 60th of a second when focused,
		// and once every 5th of a second when not focused
		if ((now - lastFrameTime) >= fpsLimit) {
			// Rendering
			ImGui::Render();
			int display_w, display_h;
			glfwGetFramebufferSize(window, &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
			glClear(GL_COLOR_BUFFER_BIT);

			ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

			glfwMakeContextCurrent(window);
			glfwSwapBuffers(window);

			// Only set lastFrameTime when you actually draw something
			lastFrameTime = now;
		}

		// Set lastUpdateTime every iteration
		lastUpdateTime = now;
#pragma endregion
	}
	run_threads = false;
	irc_disconnect(session);

	// Cleanup
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void connection_watcher() {
	while (run_threads) {
		if (irc_is_connected(session) == 1) {
			connectionNotifier = "Connected";
			if (!init) {
				// commands for one time execution
		//		irc_cmd_join(session, "#", "");
				if (irc_cmd_whois(session, con.username.c_str()) == 0) {
					init = true;
				}
			}
		}
		else {
			connectionNotifier = "Not Connected";
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
}

void result_watcher() {
	if (result.get() == 0) {
		connectionStatus = "";
	}
	else {
		connectionStatus = "Failed to Connect, Error:\n" + std::string(irc_strerror(irc_errno(session)));
	}
}