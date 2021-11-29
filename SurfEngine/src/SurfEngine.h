#pragma once

#include "SurfEngine/Core/Application.h"
#include "SurfEngine/Core/Layer.h"
#include "SurfEngine/Core/Log.h"
#include "SurfEngine/imgui/ImGuiLayer.h"

//Core
#include "SurfEngine/Core/Color.h"
#include "SurfEngine/Core/Timestep.h"
#include "SurfEngine/Core/UUID.h"
#include "SurfEngine/Core/PlatformUtils.h"

//Input
#include "SurfEngine/Core/MouseButtonCodes.h"
#include "SurfEngine/Core/KeyCodes.h"
#include "SurfEngine/Core/Input.h"

//Renderer
#include "SurfEngine/Renderer/renderer.h"
#include "SurfEngine/Renderer/RenderCommand.h"
#include "SurfEngine/Renderer/Renderer2D.h"

#include "SurfEngine/Renderer/Buffer.h"
#include "SurfEngine/Renderer/FrameBuffer.h"
#include "SurfEngine/Renderer/Shader.h"
#include "SurfEngine/Renderer/Texture.h"
#include "SurfEngine/Renderer/VertexArray.h"


//Cameras
#include "SurfEngine/Renderer/Camera.h"
#include "SurfEngine/Renderer/OrthographicCamera.h"


//Entt
#include "SurfEngine/Scenes/Scene.h"
#include "SurfEngine/Scenes/Components.h"
#include "SurfEngine/Scenes/Object.h"
#include "SurfEngine/Scenes/ScriptableObject.h"
#include "SurfEngine/Scenes/SceneSerializer.h"