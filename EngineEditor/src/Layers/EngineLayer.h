#pragma once
#include "SurfEngine.h"

#include "../Util/Project.h"
#include "../Util/ProjectManager.h"
#include "../Util/SceneManager.h"

#include "../Panels/Panel_Viewport.h"


#include "imgui/imgui.h"
#include "imgui/imconfig.h"
#include "imgui/imgui_internal.h" //

#include <glfw/include/GLFW/glfw3.h>

#include "stb_image/stb_image.h"
#include "SurfEngine/Scripting/ScriptEngine.h"

using namespace SurfEngine;

class EngineLayer : public Layer {

	struct RuntimeSettings {
		Ref<OrthographicCamera> DebugCamera;
		bool DrawGrid;
		bool UpdateCamera;
	};

public:
	EngineLayer() : Layer("Example") {

	}

	void OnAttach() override {

		//Enable Textures, Important for FBO to work correctly - probably should be moved
		RenderCommand::EnableTextures();
		RenderCommand::EnableBlending();
		RenderCommand::EnableMSAA();
		Renderer2D::Init();
		ScriptEngine::Init();
		settings.DebugCamera.reset(new OrthographicCamera());
		

		//Create the FrameBuffer
		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8 , FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.Width = 1920;
		fbSpec.Height = 1080;
		Renderer2D::SetRenderTarget(Framebuffer::Create(fbSpec));
	}

	void OnUpdate(Timestep timestep) override {
		//RenderScene
		if (SceneManager::IsActiveScene()) {
			auto& scene = SceneManager::GetActiveScene();
			if (scene->IsPlaying()) {
				scene->OnUpdateRuntime(timestep);
			}
			else {
				scene->OnUpdateEditor(timestep, settings.DebugCamera, settings.DrawGrid, SceneManager::GetSelectedObject());
				settings.DebugCamera->RecalculateProjection();
				if (settings.UpdateCamera) {
					settings.DebugCamera->OnUpdate(timestep);
				}
			}
		}
	}

	void OnEvent(Event& event) override {
		auto& scene = SceneManager::GetActiveScene();
		if (scene) {
			if(settings.UpdateCamera){
				settings.DebugCamera->OnEvent(event);
			}
		}
	}

public:
	RuntimeSettings settings;
};