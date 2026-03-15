#include "pch.h"
#include "Converter.h"

#include <iostream>
#include <filesystem>
#include <algorithm>
#include <vector>
#include <string>

int main(int argc, char* argv[])
{
	SetConsoleOutputCP(CP_UTF8);
	namespace fs = std::filesystem;

	fs::path resourcesDir;
	if (argc >= 2) resourcesDir = argv[1];
	else resourcesDir = fs::current_path() / "../Client/bin/resources";

	resourcesDir = fs::weakly_canonical(resourcesDir);

	if (!fs::exists(resourcesDir))
	{
		std::cerr << "[Error] Path not found : " << resourcesDir << "\n";
		std::cerr << "[CWD]                  : " << fs::current_path() << "\n";
		std::cerr << "Usage: ModelConverter.exe <resources_path>\n";
		std::cin.get();
		return -1;
	}

	std::cout << "========================================\n";
	std::cout << " ModelConverter\n";
	std::cout << " Scan: " << resourcesDir << "\n";
	std::cout << "========================================\n\n";

	const std::vector<std::string> validExts = { ".fbx", ".obj", ".gltf", ".glb" };
	int32 success = 0, fail = 0;

	for (const auto& entry : fs::recursive_directory_iterator(resourcesDir))
	{
		if (!entry.is_regular_file()) continue;

		std::string ext = entry.path().extension().string();
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

		Bool supported = std::any_of(validExts.begin(), validExts.end(),
			[&ext](const std::string& e) { return e == ext; });
		if (!supported) continue;

		fs::path outPath = entry.path().parent_path() / entry.path().stem();
		outPath += ".model";

		std::cout << "[Convert] " << entry.path().filename().string() << "\n";

		Tool::Converter conv;
		conv.Initialize();

		if (!conv.ReadAssetFile(entry.path().wstring()))
		{
			std::cerr << "  [FAIL] ReadAssetFile\n\n";
			++fail; continue;
		}

		// ★ 변환 결과 출력
		std::cout << "  Meshes		: " << conv.GetMeshCount() << "\n";
		std::cout << "  Materials		: " << conv.GetMaterialCount() << "\n";
		std::cout << "  Bones			: " << conv.GetBoneCount() << "\n";
		std::cout << "  Animations		: " << conv.GetAnimationCount() << "\n";

		if (!conv.ExportModel(outPath.wstring()))
		{
			std::cerr << "  [FAIL] ExportModel\n\n";
			++fail; continue;
		}

		std::cout << "  [OK] -> " << outPath.filename().string() << "\n\n";
		++success;
	}

	// ★ 최종 요약
	std::cout << "========================================\n";
	std::cout << " Result\n";
	std::cout << "   Files     : " << success << " OK / " << fail << " FAIL / " << " SKIP\n";
	std::cout << "========================================\n";
	std::cin.get();
	return (fail > 0) ? -1 : 0;
}
