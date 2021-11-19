#pragma once
#include "SurfEngine/Renderer/Texture.h"

namespace SurfEngine {
	class Panel_AssetBrowser
	{
	public:
		Panel_AssetBrowser(const std::string& root_path);

		void OnImGuiRender();

		void SetPath(const std::string& path);
		void SetHighestPath(const std::string& path);
	private:
		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;
		Ref<Texture2D> m_ImageIcon;
		Ref<Texture2D> m_SceneIcon;
		std::string m_RootPath;
		std::string m_HighestDirectory;
	};
};

