#pragma once
#define EXTENSION_NAME ".mech"
#include "MechEngine/Log.h"
#include <string>
#include <iostream>
#include <fstream>

namespace MechEngine {
	class Serialization {
	private:
		static bool fileIsOpen;
		static FILE* file;
	public:
	

		static void Init() {
			fileIsOpen = false;
			file = { 0 };
		}

		static void CreateFile(const std::string& parentDirPath, const std::string& fileName) {
			if (fileIsOpen) {
				ME_CORE_ERROR("ERROR - SERIALIZATION(0): Could not create file at {0} : Another File is currently in use!",parentDirPath+""+fileName+EXTENSION_NAME);
				return;
			}
			std::string filePath = "";

			if (!parentDirPath.empty()) {
				filePath += (parentDirPath + "\\");
			}
			filePath += (fileName + EXTENSION_NAME);

			std::ofstream File(filePath);
			File.close();
		}

		static bool FileExists(const std::string& parentDirPath, const std::string& fileName) {
			std::string filePath = "";

			if (!parentDirPath.empty()) {
				filePath += (parentDirPath + "\\");
			}

			filePath += (fileName + EXTENSION_NAME);
			std::ifstream File(filePath);
			return File.good();
		}

		static void OpenFileForReadAndWrite(const std::string& parentDirPath, const std::string& fileName) {
			//Error Checks
			if (fileIsOpen) {
				ME_CORE_ERROR("ERROR - SERIALIZATION(1): Could not open file at {0} : Another File is currently in use!", parentDirPath + "" + fileName + EXTENSION_NAME);
				return;
			}
			if (!FileExists(parentDirPath, fileName)) {
				ME_CORE_ERROR("ERROR - SERIALIZATION(2): Could not open file at {0} : File Does Not Exist!", parentDirPath + "" + fileName + EXTENSION_NAME);
				return;
			}

			//Open the file
			std::string filePath = "";
			if (!parentDirPath.empty()) {
				filePath += (parentDirPath + "\\");
			}
			filePath += (fileName + EXTENSION_NAME);

			fopen_s(&file,filePath.c_str(),"r+");

			//More Error Checks
			if (file == NULL) {
				ME_CORE_ERROR("ERROR - SERIALIZATION(3): Could not open file at {0} : Error Opening File!", parentDirPath + "" + fileName + EXTENSION_NAME);
				return;
			}

			fileIsOpen = true;
		}

		static void CloseFile() {
			if(file)
				fclose(file);
			fileIsOpen = false;
		}

		static void SetCursorPosition(int position) {
			if (file == NULL) {
				ME_CORE_ERROR("ERROR - SERIALIZATION(4): Could not Move Cursor Position to {0} : File does not exist!", position);
				return;
			}
			fseek(file,position,SEEK_SET);
		}

		static FILE* GetFile() { return file; }

		static bool ReadyForWrite() { return fileIsOpen; }

		static bool ReadyForRead() { return fileIsOpen; }
	};
}