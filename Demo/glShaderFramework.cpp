// glShaderFramework.
// Test Framework. v1.0.0 Debug MSVC170.
// opengl 3.2, shader 4.6
// 2023_5_8 

#include <iostream>
#include <windows.h>
#include <GLFW/glfw3.h>
#include <GL/GL.h>

#include "FrameworkTest_Context.h"
#include "ShaderFramework/ShaderFramework.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#if defined(_MSV_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib,"legacy_stdio_definitons.lib")
#endif

using namespace std;
// 使用Nvidia显卡.
extern "C" { _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001; }

static void _glfw_ErrorCallback(int error, const char* description) {
    // EventLoop: print GLfwError.
    fprintf(stderr, u8"[FrameTest] GLFW Error %d: %s\n", error, description);
}

int main() {

    LOGCONS::Log << LOGCONS::Log.SET(CLOG_TRACE) << "glShander Framework Test." << LogEnd;
    LOGCONS::Log.OUT_SEATE(true);

    glfwSetErrorCallback(_glfw_ErrorCallback);
    if (!glfwInit()) {
        LOGCONS::Log << LOGCONS::Log.SET(CLOG_ERROR) << "Init Window Failed." << LogEnd;
        exit(-0xEEFF);
    }
    // opengl version 3.3.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE); // 兼容模式.
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // glfw.version 3.2+ [profile].
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);             // glfw.version 3.0+ 
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // Fixed window size
    glfwWindowHint(GLFW_SAMPLES, 4);          // 4x Samples MSAA 

    // create window. 
    GLFWwindow* _MainWindowObj = glfwCreateWindow(896, 896, "Framework Test.", nullptr, nullptr);

    // create context.
    glfwMakeContextCurrent(_MainWindowObj);
    glfwSwapInterval(1); // 垂直同步.

    // 创建上下文以后开始TestInit阶段.
    FreameworkTest_Init();

    glEnable(GL_BLEND);
    // RGBA = Srgba * GL_SRC_ALPHA + Drgba * GL_ONE_MINUS_SRC_ALPHA
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 深度测试.
    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);

    // IMGUI.
    // Setup Dear ImGui context.
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& GUIIO = ImGui::GetIO(); (void)GUIIO;
    GUIIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls.
    GUIIO.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls.

    // Setup De ar ImGui style.
    ImGui::StyleColorsDark();
    {
        ImGuiStyle* style = &ImGui::GetStyle();
        ImVec4* colors = style->Colors;

        colors[ImGuiCol_Text] = ImVec4(0.00f, 0.72f, 0.72f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.00f, 0.08f, 0.55f);
    }

    // Init set font.
    auto SetFonts = ImGui::GetIO().Fonts;
    SetFonts->AddFontFromFileTTF(
        "testui_imgui/imgui_fonts/MSYH_Bold.ttf",
        20.0f, NULL, SetFonts->GetGlyphRangesChineseFull());

    // Setup platform / Renderer backends.
    ImGui_ImplGlfw_InitForOpenGL(_MainWindowObj, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    // Main WindowLoop. [Test]
    GCI32 buffersizeTemp[2] = {};
    GC_VEC4f _Winodw_BGCOLOR = GC_VEC4f(1.0f, 1.0f, 1.0f, 0.0f);

    while (!glfwWindowShouldClose(_MainWindowObj)) {
        glfwPollEvents();
        glfwGetFramebufferSize(_MainWindowObj, &buffersizeTemp[0], &buffersizeTemp[1]);
        // background color.
        glClearColor(_Winodw_BGCOLOR.vector_x * _Winodw_BGCOLOR.vector_w,
            _Winodw_BGCOLOR.vector_y * _Winodw_BGCOLOR.vector_w,
            _Winodw_BGCOLOR.vector_z * _Winodw_BGCOLOR.vector_w, _Winodw_BGCOLOR.vector_w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // start the Dear ImGui frame.
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 框架测试主循环.
        if (FrameworkTest_Loop())
            glfwSetWindowShouldClose(_MainWindowObj, true);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        //glDepthMask(GL_TRUE);
        glfwMakeContextCurrent(_MainWindowObj);
        glfwSwapBuffers(_MainWindowObj);
        //break;
    }

    // 框架测试结束释放.
    FrameworkTest_EndFree();

    glfwDestroyWindow(_MainWindowObj);
    glfwTerminate();
    LOGCONS::Log << LOGCONS::Log.SET(CLOG_INFO) << "glShander Framework Test_End." << LogEnd;

    return NULL;
}