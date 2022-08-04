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
			std::unordered_map<std::string, Ref<Material>> MaterialCache;
			Ref<VertexArray> VertexArray;
			Ref<Framebuffer> RenderTarget;
			Ref<Texture2D> CameraGizmo;
			glm::vec4 GizmoColorActive = glm::vec4(1.0f,0.5f,0.0f,1.0f);
			glm::vec4 GizmoColorInActive = glm::vec4(1.0f, 1.0, 1.0f, 0.3f);
	};

	static Renderer2DStorage* s_Data;

	void Renderer2D::Init() {
		s_Data = new Renderer2DStorage();

		//Add Mandatory Shaders
		PushMaterial("SurfMaterial_BackgrounGridShader", Shader::Create("res/shaders/background_grid_2d.glsl"));

		PushMaterial("SurfMaterial_Color", Shader::Create("res/shaders/color.glsl"));

		PushMaterial("SurfMaterial_Sprite", Shader::Create("res/shaders/sprite.glsl"));

		PushMaterial("SurfMaterial_Circle", Shader::Create("res/shaders/circle.glsl"));

		PushMaterial("SurfMaterial_Gizmo", Shader::Create("res/shaders/gizmo.glsl"));

		s_Data->CameraGizmo = Texture2D::Create("res/gizmos/camera.png");
	}

	void Renderer2D::Shutdown() {
		delete s_Data;
	}

	void Renderer2D::BeginScene(const Camera* camera) {

		for (auto& mat : s_Data->MaterialCache) {
			mat.second->Bind();
			mat.second->GetShader()->SetMat4("u_ViewProjection", camera->GetViewProjection());
		}

		s_Data->RenderTarget->Bind();
		s_Data->RenderTarget->ClearAttachment(1, -1);
		RenderCommand::SetClearColor(glm::vec4(0.25, 0.25, 0.25, 1.0));
		RenderCommand::Clear();
	}

	void Renderer2D::EndScene() {
		s_Data->RenderTarget->Unbind();
	}

	bool Renderer2D::PushMaterial(const std::string& name, const Ref<Shader> shader) {
		//Return false if material already exist under that name
		if (s_Data->MaterialCache.count(name) != 0) {
			return false;
		}

		//Otherwise Create Material and return true
		Ref<Material> material = Material::Create();
		material->SetShader(shader);
		s_Data->MaterialCache[name] = material;
		return true;
	}

	Ref<Material> Renderer2D::GetMaterial(const std::string& name) {
		return s_Data->MaterialCache[name];
	}

	void Renderer2D::SetRenderTarget(Ref<Framebuffer> frameBuffer) {
		s_Data->RenderTarget = frameBuffer;
	}

	void Renderer2D::SetRenderSize(unsigned int x, unsigned int y) {
		s_Data->RenderTarget->Resize(x, y);
	}

	void Renderer2D::ResizeRenderTarget(uint32_t width, uint32_t height) { 
		s_Data->RenderTarget->Resize(width, height);
	}

	glm::vec2 Renderer2D::GetRenderTargetSize() {
		return { s_Data->RenderTarget->GetSpecification().Width, s_Data->RenderTarget->GetSpecification().Height};
	}
	
	uint32_t Renderer2D::GetOutputAsTextureId() {
		return s_Data->RenderTarget->GetColorAttachmentRendererID();
	}

	void Renderer2D::ClearRenderTarget() {
		s_Data->RenderTarget->Bind();
		s_Data->RenderTarget->ClearAttachment(1, -1);
		RenderCommand::SetClearColor(glm::vec4(0.25, 0.25, 0.25, 1.0));
		RenderCommand::Clear();
		s_Data->RenderTarget->Unbind();
	}

	Ref<Texture2D> Renderer2D::GetGizmo() {
		return s_Data->CameraGizmo;
	}

	glm::vec4 Renderer2D::GetGizmoColorActive() {
		return s_Data->GizmoColorActive;
	}

	glm::vec4 Renderer2D::GetGizmoColorInActive() {
		return s_Data->GizmoColorInActive;
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


		s_Data->MaterialCache["SurfMaterial_Color"]->Bind();
		s_Data->MaterialCache["SurfMaterial_Color"]->GetShader()->SetMat4("u_Transform", transform);
		s_Data->MaterialCache["SurfMaterial_Color"]->GetShader()->SetFloat4("u_Color", color);

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

		float outputX[4];
		if (src->flipX) {
			outputX[0] = spriteUvs[1].x;
			outputX[1] = spriteUvs[0].x;
			outputX[2] = spriteUvs[3].x;
			outputX[3] = spriteUvs[2].x;
		}
		else {
			outputX[0] = spriteUvs[0].x;
			outputX[1] = spriteUvs[1].x;
			outputX[2] = spriteUvs[2].x;
			outputX[3] = spriteUvs[3].x;
		}

		float SquareVertices[5 * 4] = {
			0.5f, 0.5f, layer, outputX[0], spriteUvs[0].y, // top right
		   -0.5f, 0.5f, layer, outputX[1], spriteUvs[1].y, // top left
		   -0.5f, -0.5, layer, outputX[2], spriteUvs[2].y, // bottom left
			0.5f, -0.5, layer, outputX[3], spriteUvs[3].y, //bottom right
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


		s_Data->MaterialCache["SurfMaterial_Sprite"]->Bind();
		s_Data->MaterialCache["SurfMaterial_Sprite"]->GetShader()->SetMat4("u_Transform", transform);
		s_Data->MaterialCache["SurfMaterial_Sprite"]->GetShader()->SetFloat4("u_Color", src->Color);
		s_Data->MaterialCache["SurfMaterial_Sprite"]->GetShader()->SetFloat2("u_Scale", src->scaling);
		s_Data->MaterialCache["SurfMaterial_Sprite"]->GetShader()->SetFloat2("u_Offset", src->offset);
		s_Data->MaterialCache["SurfMaterial_Sprite"]->GetShader()->SetInt("u_Texture", 0);
		src->Texture->Bind();

		s_Data->VertexArray->Bind();

		RenderCommand::DrawIndexed(s_Data->VertexArray);
	}

	void Renderer2D::DrawCircle(glm::mat4 transform, glm::vec4 color) {
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


		s_Data->MaterialCache["SurfMaterial_Circle"]->Bind();
		s_Data->MaterialCache["SurfMaterial_Circle"]->GetShader()->SetMat4("u_Transform", transform);
		s_Data->MaterialCache["SurfMaterial_Circle"]->GetShader()->SetFloat4("u_Color", color);

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


		s_Data->MaterialCache["SurfMaterial_Color"]->Bind();
		s_Data->MaterialCache["SurfMaterial_Color"]->GetShader()->SetMat4("u_Transform", transform);
		s_Data->MaterialCache["SurfMaterial_Color"]->GetShader()->SetFloat4("u_Color", color);

		s_Data->VertexArray->Bind();

		RenderCommand::DrawLine(s_Data->VertexArray);
	}

	void Renderer2D::DrawBox(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec2 p4, glm::mat4 transform, glm::vec4 color) {
		DrawLine(p1, p2, transform, color);
		DrawLine(p2, p3, transform, color);
		DrawLine(p3, p4, transform, color);
		DrawLine(p4, p1, transform, color);
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


		s_Data->MaterialCache["SurfMaterial_Gizmo"]->Bind();
		s_Data->MaterialCache["SurfMaterial_Gizmo"]->GetShader()->SetMat4("u_Transform", transform);
		s_Data->MaterialCache["SurfMaterial_Gizmo"]->GetShader()->SetFloat4("u_Color", color);
		s_Data->MaterialCache["SurfMaterial_Gizmo"]->GetShader()->SetInt("u_Texture", 0);
		src->Bind();

		s_Data->VertexArray->Bind();

		RenderCommand::DrawIndexed(s_Data->VertexArray);
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

		s_Data->MaterialCache["SurfMaterial_BackgrounGridShader"]->Bind();
		s_Data->MaterialCache["SurfMaterial_BackgrounGridShader"]->GetShader()->SetFloat("u_GridSize", (float)amount);


		s_Data->VertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data->VertexArray);
	}
}
