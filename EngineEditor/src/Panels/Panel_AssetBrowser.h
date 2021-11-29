#pragma once
#include "SurfEngine/Renderer/Texture.h"

namespace SurfEngine {
	class Panel_AssetBrowser
	{
	public:
		Panel_AssetBrowser(const std::string& root_path);

		void OnImGuiRender();

	private:
		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;
		Ref<Texture2D> m_ImageIcon;
		Ref<Texture2D> m_SceneIcon;
	};
};

