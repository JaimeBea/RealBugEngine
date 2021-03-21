#include "pch.h"

#include "SampleTest.h"

#include <string>
#include <fstream>
#include <regex>


int Add(const int a, const int b) {
	return a + b;
}

int Mul(const int a, const int b) {
	return a * b;
}

void CreateFileVS(const char* name, const char* ext, const char* fName) {

	std::ifstream infile(fName);
	std::string str((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
	infile.close();

	std::regex rgx("@+([A-Z]|[a-z])+@+");
	std::string result = std::regex_replace(str, rgx, name);

	std::ofstream outfile(std::string(name) + ext);
	outfile << result;
	outfile.close();

}

void CreateSolution(const char* name)
{

	CreateFileVS(name, ".sln", "Test/TEMPLATEVC.sln");
	CreateFileVS(name, ".vcxproj", "Test/TEMPLATEVC.vcxproj");

	CopyFile(L"Test/pch.h", L"pch.h", false);
	CopyFile(L"Test/pch.cpp", L"pch.cpp", false);
	CopyFile(L"Test/framework.h", L"framework.h", false);
	CopyFile(L"Test/dllmain.cpp", L"dllmain.cpp", false);

}
