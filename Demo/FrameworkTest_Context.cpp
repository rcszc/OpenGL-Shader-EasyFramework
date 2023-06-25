// 测试Shader框架专用上下文.
#include <iostream>
#include <windows.h>

// OpenGL GLM Libray.
#include <glm.hpp>
#include <ext.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Test Framework.
#include "ShaderFramework/ShaderFramework.h"
#include "ShaderFramework/framework_ext/ShaderFramework_extend.h"

#include "ShaderFramework/framework_ext/ShaderFramework_model.h"

using namespace std;
using namespace LOGCONS;

// ################################ global define #############################

// MVP矩阵. Model View Projection.
GC_Matrix4 MatrixMVP = {};

glm::mat4 proj = {};

// #############################################################################
GCBIT EXIT_FALG = false;
GCBIT NO_CLOSE_GUIWIN = true;

GCFP32 DebugTransValue[2] = { 1.42f,0.1f };
GCFP32 FilterateValue = 0.2f;
GCFP32 TestFrameworkTimeTack = 0.0f;

void GUI_DEBUGUI() {
    static GCBIT windowMove = true;
    ImGuiWindowFlags windowFlags = NULL;

    if (windowMove) {
        windowFlags = ImGuiWindowFlags_NoMove;    // 固定位置
        windowFlags |= ImGuiWindowFlags_NoResize; // 固定大小
    }

    ImGui::Begin("ShaderFramework DEBUG", &NO_CLOSE_GUIWIN, windowFlags);

    EXIT_FALG = ImGui::Button("Exit");
    ImGui::SliderFloat("Shader.Blur", &DebugTransValue[0], 0.1f, 2.8f);
    ImGui::SliderFloat("Shader.Source", &DebugTransValue[1], 0.1f, 2.8f);
    ImGui::SliderFloat("Shader Filter", &FilterateValue, 0.0f, 1.0f);
    ImGui::Checkbox("Fixed", &windowMove);

    // 圆角边框.
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;
    style.FrameRounding = 4.0f;
    ImGui::End();
}


GCUI32 ParticleShader = NULL;
GC_MODELINDEX Particles = {};
void Particles_Init() {
    // 加载粒子着色器程序.
    GCS_LoadShader shader_Particle(true);
    shader_Particle.load_shader_vs("FrameworkShader/Particle.vs");
    shader_Particle.load_shader_fs("FrameworkShader/Particle.fs");

    ParticleShader = shader_Particle.Link_shader_program();
    shader_Particle.delete_shader();

    // 启用着色器程序.
    GC_ShaderEnable(ParticleShader); // 粒子.[离屏渲染]

    GCFP32 ptrdata[50 * 7] = {};
    ShaderExt::GC_DynamicPoints particles;
    particles.PointsDataLength = 50 * 7;
    particles.vertexData = ptrdata;

    ShaderExt::Load_PointsData data;
    data.Load_DPointsData(particles, Particles, 0);
}

GCUI32 TestFBOtexture = NULL;

GCUI32        ShaderProgram[2] = {};
GC_MODELINDEX ModelObject[2] = {};
GC_TEXINDEX   MainScene_Texture = {};
GC_TEXINDEX   MainWcube_Texture = {};
GCUI32        WindowFBO = NULL;
GC_TEXINDEX   FrameBuffer[2] = {};

void FreameworkTest_Init() {
    RectModel::Model_LoadObj TestLoad("TestModelC.obj", NULL);

    // 打印系统信息.
    GC_LoadInformation Test0("sysinfo GPU");
    GC_LoadInformation Test1("sysinfo MEMcpiy");
    GC_LoadInformation Test2("sysinfo MEMsize");
    GC_LoadInformation Test3("sysinfo Attribs");
    GC_LoadInformation Test4("sysinfo Texture");

    // 加载着色器程序.
    GCS_LoadShader shader_MainScene(true);
    shader_MainScene.load_shader_vs("FrameworkShader/BloomShader/MainScene.vs");
    shader_MainScene.load_shader_fs("FrameworkShader/BloomShader/MainScene.fs");

    GCS_LoadShader shader_BloomRender(true);
    shader_BloomRender.load_shader_vs("FrameworkShader/BloomShader/BloomRender.vs");
    shader_BloomRender.load_shader_fs("FrameworkShader/BloomShader/BloomRender.fs");

    ShaderProgram[0] = shader_MainScene.Link_shader_program();
    ShaderProgram[1] = shader_BloomRender.Link_shader_program();
    shader_MainScene.delete_shader();
    shader_BloomRender.delete_shader();

    // 启用着色器程序.
    GC_ShaderEnable(ShaderProgram[0]); // 主场景.[离屏渲染]
    GC_ShaderEnable(ShaderProgram[1]); // 辉光渲染.[渲染到屏幕]

    // 加载主场景贴图&模型.
    // vortex.png
    GCS_LoadTexture LoadScene_Texture("Texture/vortex.png");
    MainScene_Texture = LoadScene_Texture.get_textureindex(ShaderProgram[0]);

    // 窗口长宽比例 height / width.
    GCFP32 window_xy = 896.0f / 896.0f;

    // 加载主场景模型 + 传输数据.
    // 主场景.
    GC_DynamicModel MainScene_Model;
    MainScene_Model.vertexData = ShaderTestCube;
    MainScene_Model.ModelDataLength = ShaderTestCubeLen;

    GCSF_TransmitData.Load_DModelData(MainScene_Model, ModelObject[0], 0);

    // 渲染到屏幕.
    GC_DynamicModel MainWcube_Model;
    MainWcube_Model.vertexData = FBO_RDwindowCube(GC_VEC2f(20.0f, 20.0f * window_xy));
    MainWcube_Model.ModelDataLength = FBO_RDwindowCubeLen;

    GCSF_TransmitData.Load_DModelData(MainWcube_Model, ModelObject[1], 0);

    // 创建1个FBO两个纹理,一个用于原图一个用于模糊.
    {
        // 2 * Texture.
        GCS_CreateTexture SceneTexFBOa(GC_TEXTURE_RGBA, GC_VEC2f(896.0f, 896.0f));
        GCS_CreateTexture SceneTexFBOb(GC_TEXTURE_HDRRGBA, GC_VEC2f(896.0f, 896.0f));

        GCS_LoadFrameBuffer LoadFrameBuffer;
        // TexA => FBO, TexB => FBO.
        LoadFrameBuffer.bind_framebuffer(SceneTexFBOa.get_textureindex());
        LoadFrameBuffer.bind_framebuffer(SceneTexFBOb.get_textureindex());

        GCUI32 FBOtextureA = SceneTexFBOa.get_textureindex();
        GCUI32 FBOtextureB = SceneTexFBOb.get_textureindex();

        WindowFBO = LoadFrameBuffer.get_framebufferindex();

        SceneTexFBOa.unbind_texture();
        SceneTexFBOb.unbind_texture();

        FrameBuffer[0].Program = ShaderProgram[1];
        FrameBuffer[0].Texture = FBOtextureA;

        FrameBuffer[1].Program = ShaderProgram[1];
        FrameBuffer[1].Texture = FBOtextureB;
    }

    // 正交视图矩阵. +- 20.0f.
    glm::mat4 proj = glm::ortho(-20.0f, 20.0f, -20.0f * window_xy, 20 * window_xy, -20.0f, 20.0f);
    GCI32 MatCount = 0;
    for (GCI32 j = 0; j < 4; j++)
        for (GCI32 i = 0; i < 4; i++) {
            MatrixMVP.Matrix[MatCount] = proj[j][i];
            MatCount++;
        }

    Particles_Init();
    ShaderSetPointSize();
    ShaderExt::ShaderRTX::GC_ShaderTestRTX();
}


GC_VEC2f WindowSize = GC_VEC2f(896.0f, 896.0f);
GCBIT FrameworkTest_Loop() {

    //****************************** 离屏渲染 ******************************
    GC_FrameBuffer_Above(WindowFBO);
    GC_ClearFrameBuffer();

    GC_ShaderEnable(ShaderProgram[0]);
    GCSF_GlobalValue.sUniformMat4(ShaderProgram[0], "MatrixMVP", MatrixMVP);
    GCSF_GlobalValue.sUniform1f(ShaderProgram[0], "Time", TestFrameworkTimeTack); // TimeTick.

    GC_ShaderTexture(0, MainScene_Texture, "TextureA");

    GC_ShaderDrawMD(ModelObject[0]); // 绘制主场景模型.

    // 绘制粒子场景.
    GC_ShaderEnable(ParticleShader);
    ShaderExt::GC_ShaderDrawPS(Particles);

    GC_FrameBuffer_Below();
    GC_ClearFrameBuffer();

    //****************************** 渲染到屏幕 ******************************

    GC_ShaderEnable(ShaderProgram[1]);
    GCSF_GlobalValue.sUniformMat4(ShaderProgram[1], "MatrixMVP", MatrixMVP);
    GCSF_GlobalValue.sUniform2f(ShaderProgram[1], "screenSize", WindowSize);

    GCSF_GlobalValue.sUniform1f(ShaderProgram[1], "BloomBlur", DebugTransValue[0]);
    GCSF_GlobalValue.sUniform1f(ShaderProgram[1], "BloomSource", DebugTransValue[1]);
    GCSF_GlobalValue.sUniform1f(ShaderProgram[1], "TestFilter", FilterateValue); // 纹理过滤色.

    GC_ShaderTexture(1, FrameBuffer[0], "TextureWinA");
    GC_ShaderTexture(1, FrameBuffer[1], "TextureWinB");

    GC_ShaderDrawMD(ModelObject[1]);

    // 绘制Debug GUI.
    GUI_DEBUGUI();
    TestFrameworkTimeTack += 0.05f;
    return EXIT_FALG;
}

void FrameworkTest_EndFree() {

    GCSF_DeleteIndex.Delete_ModelObject(ModelObject[0]);
    GCSF_DeleteIndex.Delete_ModelObject(ModelObject[1]);
    GCSF_DeleteIndex.Delete_ModelObject(Particles);

    GCSF_DeleteIndex.Delete_TextureObject(MainScene_Texture);
    GCSF_DeleteIndex.Delete_TextureObject(FrameBuffer[0]);
    GCSF_DeleteIndex.Delete_TextureObject(FrameBuffer[1]);

    GCSF_DeleteIndex.Delete_FrameBufferObject(WindowFBO);

    GCSF_DeleteIndex.Delete_ShaderProgram(ShaderProgram[0]);
    GCSF_DeleteIndex.Delete_ShaderProgram(ShaderProgram[1]);
    GCSF_DeleteIndex.Delete_ShaderProgram(ParticleShader);
}