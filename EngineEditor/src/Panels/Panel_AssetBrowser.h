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
		void DoMenuPopupContext();
		void UpdateFileCount();
		void DrawParentPathText();
		void DrawBackButton();
		void DrawFileIcons(int icon_size);
		void DrawDirectoryIcons(int icon_size);
	private:
		std::vector<Ref<Texture2D>> image_asset_icons;
		std::size_t file_count = 0;
	};
};

