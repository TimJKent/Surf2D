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
			Ref<Texture2D> CameraGizmo;
			glm::vec4 GizmoColorActive = glm::vec4(1.0f,0.5f,0.0f,1.0f);
			glm::vec4 GizmoColorInActive = glm::vec4(1.0f, 1.0, 1.0f, 0.3f);
	};

	static Renderer2DStorage* s_Data;

	glm::vec4 Renderer2D::GetGizmoColorActive() {
		return s_Data->GizmoColorActive;
	}

	glm::vec4 Renderer2D::GetGizmoColorInActive() {
		return s_Data->GizmoColorInActive;
	}

	void Renderer2D::PushMaterial(const Ref<Material>& material) {
		s_Data->MaterialCache.push_back(material);
	}

	void Renderer2D::SetRenderSize(unsigned int x, unsigned int y) {
		s_Data->RenderTarget->Resize(x, y);
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

		Ref<Material> ReflectionMaterial = Material::Create();
		ReflectionMaterial->SetShader(Shader::Create("res/shaders/reflection.glsl"));
		s_Data->MaterialCache.push_back(ReflectionMaterial);

		Ref<Material> GizmoMaterial = Material::Create();
		GizmoMaterial->SetShader(Shader::Create("res/shaders/gizmo.glsl"));
		s_Data->MaterialCache.push_back(GizmoMaterial);

		s_Data->CameraGizmo = Texture2D::Create("res/gizmos/camera.png");
	}

	Ref<Texture2D> Renderer2D::GetGizmo() {
		return s_Data->CameraGizmo;
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

	glm::vec2 Renderer2D::GetRenderTargetSize() {
		return { s_Data->RenderTarget->GetSpecification().Width, s_Data->RenderTarget->GetSpecification().Height};
	}
	
	void Renderer2D::ClearRenderTarget() {
		s_Data->RenderTarget->Bind();
		s_Data->RenderTarget->ClearAttachment(1, -1);
		RenderCommand::SetClearColor(glm::vec4(0.25, 0.25, 0.25, 1.0));
		RenderCommand::Clear();
		s_Data->RenderTarget->Unbind();
	}

	void Renderer2D::BeginScene(const Camera* camera){
		
		s_Data->MaterialCache[0]->Bind();
		s_Data->MaterialCache[0]->GetShader()->SetMat4("u_ViewProjection", camera->GetViewProjection());
		s_Data->MaterialCache[1]->Bind();
		s_Data->MaterialCache[1]->GetShader()->SetMat4("u_ViewProjection", camera->GetViewProjection());
		s_Data->MaterialCache[2]->Bind();
		s_Data->MaterialCache[2]->GetShader()->SetMat4("u_ViewProjection", camera->GetViewProjection());
		s_Data->MaterialCache[3]->Bind();
		s_Data->MaterialCache[3]->GetShader()->SetMat4("u_ViewProjection", camera->GetViewProjection());
		s_Data->MaterialCache[4]->Bind();
		s_Data->MaterialCache[4]->GetShader()->SetMat4("u_ViewProjection", camera->GetViewProjection());
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
			0.50f, 0.5f, 0.0f, 0.0f, 0.0f,
			-0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
			-0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
			0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
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

	void Renderer2D::DrawGizmo(glm::mat4 transform, Ref<Texture2D> src, glm::vec4 color) {
		//Quad Verticies x,y,z, texX, texY

		float SquareVertices[5 * 4] = {
			0.50f, 0.5f, 0.0f, 0.0f, 0.0f,
			-0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
			-0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
			0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
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


		s_Data->MaterialCache[4]->Bind();
		s_Data->MaterialCache[4]->GetShader()->SetMat4("u_Transform", transform);
		s_Data->MaterialCache[4]->GetShader()->SetFloat4("u_Color", color);
		s_Data->MaterialCache[4]->GetShader()->SetInt("u_Texture", 0);
		src->Bind();

		s_Data->VertexArray->Bind();

		RenderCommand::DrawIndexed(s_Data->VertexArray);
	}

	void Renderer2D::DrawQuad(glm::mat4 transform, Ref<SpriteRendererComponent> src) {
		DrawQuad(transform, src, 1,1);
	}

	void Renderer2D::DrawQuad(glm::mat4 transform, Ref<SpriteRendererComponent> src, int frame, int totalFrames) {
		//Quad Verticies x,y,z, texX, texY
		float layer = src->Layer-98.f;


		glm::vec2 spriteUvs[4] = {
			{(float)frame / (float)totalFrames,0.0f},
			{((float)frame - 1.0f) / (float)totalFrames,0.0f},
			{((float)frame - 1.0f) / (float)totalFrames,1.0f},
			{(float)frame / (float)totalFrames,1.0f}
		};

	

		if (src->flipX) { 
			spriteUvs[0].x = std::abs(spriteUvs[0].x - 1.0f);
			spriteUvs[1].x = std::abs(spriteUvs[1].x - 1.0f);
			spriteUvs[2].x = std::abs(spriteUvs[2].x - 1.0f);
			spriteUvs[3].x = std::abs(spriteUvs[3].x - 1.0f);
		}

		float SquareVertices[5 * 4] = {
			0.5f, 0.5f, layer, spriteUvs[0].x, spriteUvs[0].y, // top right
		   -0.5f, 0.5f, layer, spriteUvs[1].x, spriteUvs[1].y, // top left
		   -0.5f, -0.5, layer, spriteUvs[2].x, spriteUvs[2].y, // bottom left
			0.5f, -0.5, layer, spriteUvs[3].x, spriteUvs[3].y, //bottom right
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

	void Renderer2D::DrawLine(glm::vec2 start, glm::vec2 end, glm::mat4 transform, glm::vec4 color) {
		float layer = 0.f;
		float SquareVertices[3 * 2] = {
			start.x, start.y, layer,
			end.x, end.y, layer
		};

		uint32_t indices[2] = { 0, 1 };

		//Create VertexBuffer
		Ref<VertexBuffer> squareVB;
		squareVB.reset(VertexBuffer::Create(SquareVertices, sizeof(SquareVertices)));

		squareVB->SetLayout({
			{ShaderDataType::Float3, "a_Position"},
			});

		//Create Index Buffer and Assign it
		std::shared_ptr<IndexBuffer> squareIB;
		squareIB.reset(IndexBuffer::Create(indices, 2));

		//Create VertexArray and assign VertexBuffer/ Index Buffer
		s_Data->VertexArray = VertexArray::Create();
		s_Data->VertexArray->AddVertexBuffer(squareVB);
		s_Data->VertexArray->SetIndexBuffer(squareIB);


		s_Data->MaterialCache[1]->Bind();
		s_Data->MaterialCache[1]->GetShader()->SetMat4("u_Transform", transform);
		s_Data->MaterialCache[1]->GetShader()->SetFloat4("u_Color", color);

		s_Data->VertexArray->Bind();

		RenderCommand::DrawLine(s_Data->VertexArray);
	}

	void Renderer2D::DrawBox(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec2 p4, glm::mat4 transform, glm::vec4 color) {
		DrawLine(p1, p2, transform, color);
		DrawLine(p2, p3, transform, color);
		DrawLine(p3, p4, transform, color);
		DrawLine(p4, p1, transform, color);
	}


}
