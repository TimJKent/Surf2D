#include "mepch.h"
#include "OpenGLRendererAPI.h"
#include <glad/glad.h>

namespace MechEngine {

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
	//	 glEnable(GL_DEPTH_CLAMP);
		glEnable(GL_DEPTH_TEST);
	 }

	 void  OpenGLRendererAPI::EnableBlending() {
		 glEnable(GL_BLEND);
		 glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	 }

	 void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray){
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
		// glDrawElements(GL_LINES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	 }

	 void OpenGLRendererAPI::DrawIndexedPoints(const Ref<VertexArray>& vertexArray) {
		 glPointSize(5.f);
		 glEnable(GL_POINT_SMOOTH);
		 glDrawElements(GL_POINTS, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
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