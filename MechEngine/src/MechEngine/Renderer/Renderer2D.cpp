#include "mepch.h"
#include "Renderer2D.h"

#include "Shader.h"
#include "MechEngine/Renderer/Material.h"
#include "MechEngine/Platform/OpenGl/OpenGLShader.h"
#include "MechEngine/Renderer/RenderCommand.h"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "MechEngine/Core/Transform.h"

namespace MechEngine{


	struct Renderer2DStorage {
			bool viewModeIn3d = true;
			std::vector<Ref<Material>> MaterialCache;
			Ref<VertexArray> VertexArray;
			Ref<Framebuffer> RenderTarget;
		};

	static Renderer2DStorage* s_Data;

	void Renderer2D::PushMaterial(const Ref<Material>& material) {
		s_Data->MaterialCache.push_back(material);
	}

	Ref<Material> Renderer2D::GetMaterial(int slot) {
		return s_Data->MaterialCache[slot];
	}

	void Renderer2D::Init() {
		s_Data = new Renderer2DStorage();

		//Add Mandatory Shaders
		Ref<Material> GridMaterial2D = Material::Create();
		Ref<Material> GridMaterial3D = Material::Create();
		GridMaterial2D->SetShader(Shader::Create("assets/shaders/background_grid_2d.glsl"));
		GridMaterial3D->SetShader(Shader::Create("assets/shaders/grid.glsl"));

		s_Data->MaterialCache.push_back(GridMaterial2D);
		s_Data->MaterialCache.push_back(GridMaterial3D);
	}

	void Renderer2D::Shutdown(){
		delete s_Data;
	}

	void Renderer2D::SetRenderTarget(Ref<Framebuffer> frameBuffer) {
		s_Data->RenderTarget = frameBuffer;
	}
	
	void Renderer2D::ResizeRenderTarget(uint32_t width, uint32_t height) { 
		s_Data->RenderTarget->Resize(width, height);
	}
	


	void Renderer2D::BeginScene(const Ref<Camera>& camera){

		s_Data->MaterialCache[0]->Bind();
		s_Data->MaterialCache[0]->GetShader()->SetMat4("u_ViewProjection", camera->GetViewProjection());
		s_Data->MaterialCache[1]->Bind();
		s_Data->MaterialCache[1]->GetShader()->SetMat4("u_View", camera->GetView());
		s_Data->MaterialCache[1]->GetShader()->SetMat4("u_Proj", camera->GetProjection());

		for (int i = 2; i < s_Data->MaterialCache.size(); i++) {
			s_Data->MaterialCache[i]->Bind();
			s_Data->MaterialCache[i]->GetShader()->SetMat4("u_ViewProjection", camera->GetViewProjection());
		}

		s_Data->RenderTarget->Bind();
		s_Data->RenderTarget->ClearAttachment(1, -1);
		RenderCommand::SetClearColor(glm::vec4(0.25, 0.25, 0.25, 1.0));
		RenderCommand::Clear();
	}

	uint32_t Renderer2D::GetOutputAsTextureId() {
		return s_Data->RenderTarget->GetColorAttachmentRendererID();
	}

	void Renderer2D::EndScene(){
		s_Data->RenderTarget->Unbind();
	}

	void Renderer2D::DrawBackgroundGrid(int amount, bool renderIn3d) {
		s_Data->VertexArray = VertexArray::Create();

		//Quad Verticies x,y,z, texX, texY
		float SquareVertices[5 * 4] = {
			1.0, 1.0, 0.0f, 0.0f, 0.0f,
			-1.0, 1.0, 0.0f, 1.0f, 0.0f,
			-1.0, -1.0, 0.0f, 1.0f, 1.0f,
			1.0, -1.0, 0.0f, 0.0f, 1.0f,
		};
		//Indexs
		uint32_t squareindices[6] = { 0, 1, 2 ,2,3,0 };

		//Create VertexBuffer
		Ref<VertexBuffer> squareVB;
		squareVB.reset(VertexBuffer::Create(SquareVertices, sizeof(SquareVertices)));
		squareVB->SetLayout({
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float2, "a_TexCoord"}
			});

		//Create Index Buffer and Assign it
		std::shared_ptr<IndexBuffer> squareIB;
		squareIB.reset(IndexBuffer::Create(squareindices, sizeof(squareindices) / sizeof(uint32_t)));

		//Create VertexArray and assign VertexBuffer/ Index Buffer
		s_Data->VertexArray->AddVertexBuffer(squareVB);
		s_Data->VertexArray->SetIndexBuffer(squareIB);


		if (!renderIn3d) {
			s_Data->MaterialCache[0]->Bind();
			s_Data->MaterialCache[0]->GetShader()->SetFloat("u_GridSize", amount);
		}											 
		else {										 
			s_Data->MaterialCache[1]->Bind();		 
			s_Data->MaterialCache[1]->GetShader()->SetFloat("u_GridSize", amount);
		}
		
		s_Data->VertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->VertexArray);
	}
	
	void Renderer2D::DrawMesh(const Transform transform, const Ref<Mesh>& mesh, int material) {
		//Create VertexBuffer
		Ref<VertexBuffer> squareVB;
		squareVB.reset(VertexBuffer::Create(mesh->GetVertices(), mesh->GetStride() * mesh->Size() *sizeof(float) ));
		squareVB->SetLayout({
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float2, "a_TexCoord"},
			{ShaderDataType::Float4, "a_Color"}
			});

		//Create Index Buffer and Assign it
		std::shared_ptr<IndexBuffer> squareIB;
		squareIB.reset(IndexBuffer::Create(mesh->GetIndices(), mesh->IndicesSize()));

		//Create VertexArray and assign VertexBuffer/ Index Buffer
		s_Data->VertexArray = VertexArray::Create();
		s_Data->VertexArray->AddVertexBuffer(squareVB);
		s_Data->VertexArray->SetIndexBuffer(squareIB);


		s_Data->MaterialCache[material]->Bind();
		s_Data->MaterialCache[material]->GetShader()->SetMat4("u_Transform", transform.GetTransformMatrix());

		s_Data->VertexArray->Bind();
		
		RenderCommand::DrawIndexed(s_Data->VertexArray);
	}
	
	void Renderer2D::DrawMeshWireFrame(const Transform transform, const Ref<Mesh>& mesh, int material) {

		//Create VertexBuffer
		Ref<VertexBuffer> squareVB;
		squareVB.reset(VertexBuffer::Create(mesh->GetVertices(), mesh->GetStride() * mesh->Size() * sizeof(float)));
		squareVB->SetLayout({
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float2, "a_TexCoord"},
			{ShaderDataType::Float4, "a_Color"}
			});

		//Create Index Buffer and Assign it
		std::shared_ptr<IndexBuffer> squareIB;
		squareIB.reset(IndexBuffer::Create(mesh->GetIndices(), mesh->IndicesSize()));

		//Create VertexArray and assign VertexBuffer/ Index Buffer
		s_Data->VertexArray = VertexArray::Create();
		s_Data->VertexArray->AddVertexBuffer(squareVB);
		s_Data->VertexArray->SetIndexBuffer(squareIB);

		s_Data->MaterialCache[material]->Bind();
		s_Data->MaterialCache[material]->GetShader()->SetMat4("u_Transform", transform.GetTransformMatrix());

		RenderCommand::SetWireFrameMode(RendererAPI::WireFrameMode::On);
		RenderCommand::DrawIndexed(s_Data->VertexArray);


		RenderCommand::DrawIndexedPoints(s_Data->VertexArray);
		RenderCommand::SetWireFrameMode(RendererAPI::WireFrameMode::Off);
	}

	bool Renderer2D::GetRenderMode() {
		return s_Data->viewModeIn3d;
	}

	void Renderer2D::SetRenderMode(bool value) {
		s_Data->viewModeIn3d = value;
	}
}
