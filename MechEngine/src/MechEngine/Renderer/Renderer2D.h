#pragma once
#include "Camera.h"
#include "FrameBuffer.h"
#include "Texture.h"
#include "Mesh.h"
#include "Material.h"
#include "VertexArray.h" 
#include "MechEngine/Core/Transform.h"

namespace MechEngine{


	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void PushMaterial(const Ref<Material>& material);
		static Ref<Material> GetMaterial(int slot);

		static void BeginScene(const Ref<Camera>& camera);
		static void EndScene();

		static void SetRenderTarget(Ref<Framebuffer> frameBuffer);
		static void ResizeRenderTarget(uint32_t width, uint32_t height);

		static uint32_t GetOutputAsTextureId();

		static void DrawBackgroundGrid(int amount, bool renderIn3d);

		static void DrawMesh(const Transform transform, const Ref<Mesh>& mesh, int Material);
		static void DrawMeshWireFrame(const Transform transform, const Ref<Mesh>& mesh, int material);
	};
}