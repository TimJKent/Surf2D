#pragma once
#include "SurfEngine.h"

#include "../Panels/Project.h"
#include "../Panels/ProjectManager.h"

#include "../Panels/Panel_Viewport.h"


#include "imgui/imgui.h"
#include "imgui/imconfig.h"
#include "imgui/imgui_internal.h"

#include <glfw/include/GLFW/glfw3.h>

#include "stb_image/stb_image.h"

using namespace SurfEngine;

class EngineLayer : public Layer {

public:
	EngineLayer() : Layer("Example") {

	}

	void OnAttach() override {

		//Enable Textures, Important for FBO to work correctly - probably should be moved
		RenderCommand::EnableTextures();
		RenderCommand::EnableBlending();
		RenderCommand::EnableMSAA();
		Renderer2D::Init();

		s_EditorCamera.reset(new OrthographicCamera());
		

		//Create the FrameBuffer
		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8 , FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.Width = 1920;
		fbSpec.Height = 1080;
		Renderer2D::SetRenderTarget(Framebuffer::Create(fbSpec));
	}

	void OnUpdate(Timestep timestep) override {
		//RenderScene
		if (ProjectManager::IsActiveScene()) {
			auto& scene = ProjectManager::GetActiveScene();
			if (scene->IsPlaying()) {

				scene->OnUpdateRuntime(timestep);
			}
			else {
				scene->OnUpdateEditor(timestep, s_EditorCamera, true, ProjectManager::GetSelectedObject());



				s_EditorCamera->RecalculateProjection();
				if (UpdateCamera) {
					s_EditorCamera->OnUpdate(timestep);
				}
			}
		}
	}

	void OnEvent(Event& event) override {
		auto& scene = ProjectManager::GetActiveScene();
		if (scene) {
			if(UpdateCamera){
				s_EditorCamera->OnEvent(event);
			}
		}
	}

	
private:
	Ref<OrthographicCamera> s_EditorCamera;
	bool mouseCheck = true;
public:
	bool UpdateCamera = false;
	bool DrawGrid = false;
};