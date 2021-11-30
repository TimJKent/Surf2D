#include "sepch.h"
#include "Renderer2D.h"

#include "Shader.h"
#include "SurfEngine/Renderer/Material.h"
#include "SurfEngine/Platform/OpenGl/OpenGLShader.h"
#include "SurfEngine/Renderer/RenderCommand.h"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace SurfEngine{


	struct Renderer2DStorage {
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
		GridMaterial2D->SetShader(Shader::Create("res/shaders/background_grid_2d.glsl"));
		s_Data->MaterialCache.push_back(GridMaterial2D);

		Ref<Material> ColorMaterial = Material::Create();
		ColorMaterial->SetShader(Shader::Create("res/shaders/color.glsl"));
		s_Data->MaterialCache.push_back(ColorMaterial);

		Ref<Material> TextureMaterial = Material::Create();
		TextureMaterial->SetShader(Shader::Create("res/shaders/sprite.glsl"));
		s_Data->MaterialCache.push_back(TextureMaterial);

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
		s_Data->MaterialCache[1]->GetShader()->SetMat4("u_ViewProjection", camera->GetViewProjection());
		s_Data->MaterialCache[2]->Bind();
		s_Data->MaterialCache[2]->GetShader()->SetMat4("u_ViewProjection", camera->GetViewProjection());

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

	void Renderer2D::DrawBackgroundGrid(int amount) {
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

		s_Data->MaterialCache[0]->Bind();
		s_Data->MaterialCache[0]->GetShader()->SetFloat("u_GridSize", (float)amount);

		
		s_Data->VertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->VertexArray);
	}

	void Renderer2D::DrawQuad(glm::mat4 transform) {
		DrawQuad(transform, glm::vec4{1.0f,1.0f,1.0f,1.0f});
	}


	void Renderer2D::DrawQuad(glm::mat4 transform, glm::vec4 color) {
		//Quad Verticies x,y,z, texX, texY
		float SquareVertices[5 * 4] = {
			1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
		};

		uint32_t squareindices[6] = { 0, 1, 2, 2, 3, 0 };

		//Create VertexBuffer
		Ref<VertexBuffer> squareVB;
		squareVB.reset(VertexBuffer::Create(SquareVertices, sizeof(SquareVertices)));

		squareVB->SetLayout({
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float2, "a_TexCoord"},
			});

		//Create Index Buffer and Assign it
		std::shared_ptr<IndexBuffer> squareIB;
		squareIB.reset(IndexBuffer::Create(squareindices, 6));

		//Create VertexArray and assign VertexBuffer/ Index Buffer
		s_Data->VertexArray = VertexArray::Create();
		s_Data->VertexArray->AddVertexBuffer(squareVB);
		s_Data->VertexArray->SetIndexBuffer(squareIB);


		s_Data->MaterialCache[1]->Bind();
		s_Data->MaterialCache[1]->GetShader()->SetMat4("u_Transform", transform);
		s_Data->MaterialCache[1]->GetShader()->SetFloat4("u_Color", color);

		s_Data->VertexArray->Bind();

		RenderCommand::DrawIndexed(s_Data->VertexArray);
	}

	void Renderer2D::DrawQuad(glm::mat4 transform, Ref<SpriteRendererComponent> src) {
		//Quad Verticies x,y,z, texX, texY
		float layer = (float)src->Layer;
		float SquareVertices[5 * 4] = {
			1.0f,  0.0f, layer, 0.0f, 0.0f,
			0.0f,  0.0f, layer, 1.0f, 0.0f,
			0.0f, -1.0f, layer, 1.0f, 1.0f,
			1.0f, -1.0f, layer, 0.0f, 1.0f,
		};
		
		uint32_t squareindices[6] = { 0, 1, 2, 2, 3, 0 };
		
		//Create VertexBuffer
		Ref<VertexBuffer> squareVB;
		squareVB.reset(VertexBuffer::Create(SquareVertices, sizeof(SquareVertices)));
		
		squareVB->SetLayout({
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float2, "a_TexCoord"},
			});
		
		//Create Index Buffer and Assign it
		std::shared_ptr<IndexBuffer> squareIB;
		squareIB.reset(IndexBuffer::Create(squareindices, 6));
		
		//Create VertexArray and assign VertexBuffer/ Index Buffer
		s_Data->VertexArray = VertexArray::Create();
		s_Data->VertexArray->AddVertexBuffer(squareVB);
		s_Data->VertexArray->SetIndexBuffer(squareIB);
		
		
		s_Data->MaterialCache[2]->Bind();
		s_Data->MaterialCache[2]->GetShader()->SetMat4("u_Transform", transform);
		s_Data->MaterialCache[2]->GetShader()->SetFloat4("u_Color", src->Color);
		s_Data->MaterialCache[2]->GetShader()->SetInt("u_Texture", 0);
		src->Texture->Bind();
		
		s_Data->VertexArray->Bind();
		
		RenderCommand::DrawIndexed(s_Data->VertexArray);
	}
}
