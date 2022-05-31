#pragma once
#include "Camera.h"
#include "FrameBuffer.h"
#include "Texture.h"
#include "Material.h"
#include "VertexArray.h" 
#include "SurfEngine/Scenes/Components.h"

namespace SurfEngine{


	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void PushMaterial(const Ref<Material>& material);
		static Ref<Material> GetMaterial(int slot);

		static void BeginScene(const Camera* camera);
		static void EndScene();

		static void SetRenderTarget(Ref<Framebuffer> frameBuffer);
		static void SetRenderSize(unsigned int x, unsigned int y);

		static void ResizeRenderTarget(uint32_t width, uint32_t height);
		static glm::vec2 GetRenderTargetSize();

		static uint32_t GetOutputAsTextureId();

		static void ClearRenderTarget();

		static void DrawBackgroundGrid(int amount);
		
		static void DrawQuad(glm::mat4 transform);
		static void DrawQuad(glm::mat4 transform, glm::vec4 color);
		static void DrawQuad(glm::mat4 transform, Ref<SpriteRendererComponent> src);
		static void DrawQuad(glm::mat4 transform, Ref<SpriteRendererComponent> src, int frame, int totalFrames);
		static void DrawLine(glm::vec2 start, glm::vec2 end, glm::mat4 transform, glm::vec4 color);
		static void DrawBox(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec2 p4, glm::mat4 transform, glm::vec4 color);
		
		static void DrawGizmo(glm::mat4 transform, Ref<Texture2D> src, glm::vec4 color);


		static Ref<Texture2D> GetGizmo();
		static glm::vec4 GetGizmoColorActive();
		static glm::vec4 GetGizmoColorInActive();
	};
}