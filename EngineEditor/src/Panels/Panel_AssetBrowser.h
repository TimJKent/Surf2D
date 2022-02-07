#pragma once
#include "SurfEngine/Renderer/Texture.h"
#include <vector>

namespace SurfEngine {
	class Panel_AssetBrowser
	{
	public:
		Panel_AssetBrowser(const std::string& root_path);

		void OnImGuiRender();

		void OnRefresh();

	private:
		void UpdateFileCount();
		void DrawParentPathText();
		void DrawBackButton();
		void DrawFileIcons(int icon_size);
		void DrawDirectoryIcons(int icon_size);
	private:
		std::vector<Ref<Texture2D>> image_asset_icons;
		int file_count = 0;

		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;
		Ref<Texture2D> m_ImageIcon;
		Ref<Texture2D> m_SceneIcon;
		Ref<Texture2D> m_ScriptIcon;
		Ref<Texture2D> m_ProjectIcon;
	};
};

