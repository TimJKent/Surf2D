#include "sepch.h"
#include "OpenGLRendererAPI.h"
#include <glad/glad.h>

namespace SurfEngine {

	void OpenGLRendererAPI::SetClearColor(const glm::vec4 color) {
		glClearColor(color.r, color.g,color.b,color.a);
	}

	 void OpenGLRendererAPI::Clear(){
		 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	 }

	 void OpenGLRendererAPI::EnableTextures() {
		 glEnable(GL_TEXTURE_2D);
	 }

	 void OpenGLRendererAPI::EnableMSAA() {
		 glEnable(GL_MULTISAMPLE);
	 }
		
	 void OpenGLRendererAPI::EnableDepth() {
		//glEnable(GL_DEPTH_CLAMP);
		glEnable(GL_DEPTH_TEST);
	 }

	 void  OpenGLRendererAPI::EnableBlending() {
		 glEnable(GL_BLEND);
		 glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	 }

	 void OpenGLRendererAPI::BindTextureId(int slot, uint32_t id) {
		 glBindTextureUnit(slot, id);
	 }

	 void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray){
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
		 glDrawElements(GL_LINES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	 }

	 void OpenGLRendererAPI::DrawLine(const Ref<VertexArray>& vertexArray) {
		 glLineWidth(2.0f);
		 vertexArray->Bind();
		 glDrawArrays(GL_LINES, 0, 2);
	 }

	 void OpenGLRendererAPI::SetWireFrameMode(RendererAPI::WireFrameMode mode) {
		 glLineWidth(2.f);
		 if (mode == RendererAPI::WireFrameMode::On) {
			 glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		 }
		 else {
			 glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		 }
	 }
}