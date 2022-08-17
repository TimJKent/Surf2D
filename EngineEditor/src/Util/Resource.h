#pragma once
#include "SurfEngine.h"
#include "SurfEngine/Renderer/Texture.h"
#include <filesystem>

namespace Resource {

	enum ResourceType {
		DIRECTORY, PROJECT, SCENE, IMAGE, SCRIPT, ASSET, PHYSMAT, OTHER
	};

	static SurfEngine::Ref<SurfEngine::Texture2D> m_DirectoryIcon;
	static SurfEngine::Ref<SurfEngine::Texture2D> m_FileIcon;
	static SurfEngine::Ref<SurfEngine::Texture2D> m_ImageIcon;
	static SurfEngine::Ref<SurfEngine::Texture2D> m_SceneIcon;
	static SurfEngine::Ref<SurfEngine::Texture2D> m_ScriptIcon;
	static SurfEngine::Ref<SurfEngine::Texture2D> m_PhysMaterialIcon;
	static SurfEngine::Ref<SurfEngine::Texture2D> m_ProjectIcon;

	static void InitIcons() {
		m_DirectoryIcon = SurfEngine::Texture2D::Create("res\\textures\\icon_folder.png");
		m_FileIcon = SurfEngine::Texture2D::Create("res\\textures\\icon_file.png");
		m_ImageIcon = SurfEngine::Texture2D::Create("res\\textures\\icon_image.png");
		m_SceneIcon = SurfEngine::Texture2D::Create("res\\textures\\icon_globe.png");
		m_ScriptIcon = SurfEngine::Texture2D::Create("res\\textures\\icon_script.png");
		m_PhysMaterialIcon = SurfEngine::Texture2D::Create("res\\textures\\icon_phys_material.png");
		m_ProjectIcon = SurfEngine::Texture2D::Create("res\\textures\\icon_project.png");
	}

	

	ResourceType GetResourceType(const std::filesystem::path& path) {
		std::string ext = path.extension().string();

		if (ext.compare(".surf") == 0) { return ResourceType::PROJECT; }
		if (ext.compare(".scene") == 0) { return ResourceType::SCENE; }
		if (ext.compare(".png") == 0) { return ResourceType::IMAGE; }
		if (ext.compare(".phys") == 0) { return ResourceType::PHYSMAT; }
		if (ext.compare(".jpg") == 0) { return ResourceType::IMAGE; }
		if (ext.compare(".cs") == 0) { return ResourceType::SCRIPT; }
		if (ext.compare(".asset") == 0) { return ResourceType::ASSET; }

		return ResourceType::OTHER;
	}

	ResourceType GetResourceType(const std::string& path) {
		return GetResourceType(std::filesystem::path(path));
	}

	static SurfEngine::Ref<SurfEngine::Texture2D> GetResourceIcon(ResourceType rtype) {
		switch (rtype) {
		case DIRECTORY: return m_DirectoryIcon;
		case PROJECT:  return m_ProjectIcon;
		case SCENE: return m_SceneIcon;
		case IMAGE: return m_ImageIcon;
		case SCRIPT: return m_ScriptIcon;
		case PHYSMAT: return m_PhysMaterialIcon;
		case ASSET: return m_FileIcon;
		case OTHER: return m_FileIcon;
		default : return m_FileIcon;
		}
	}
}