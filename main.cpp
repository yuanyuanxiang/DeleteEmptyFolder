#include <stdint.h>
#include <io.h>
#include <string>
#include <vector>
#include <windows.h>
#include <iostream>

// 如果当前目录为空，则删除空目录
bool IsEmptyFolder(const std::string &folder_path)
{
	bool ret = true;
	//文件句柄
	intptr_t hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	std::string p;
	try
	{
		if ((hFile = _findfirst(p.assign(folder_path).append("\\*.*").c_str(), &fileinfo)) != -1)
		{
			do{
				if (0==strcmp(fileinfo.name, ".") || 0==strcmp(fileinfo.name, ".."))
					continue;
				if (FILE_ATTRIBUTE_DIRECTORY & fileinfo.attrib) //子目录
				{
					std::string child = folder_path + "\\" + fileinfo.name;
					if (IsEmptyFolder(child)){
						BOOL b = RemoveDirectory(child.c_str());
						if (FALSE == b) ret = false;
						printf("删除\"%s\"%s\n", child.c_str(), b ? "成功" : "失败");
					}else ret = false;
				}else if (ret) // 非空目录
				{
					ret = false;
				}
			} while (_findnext(hFile, &fileinfo) == 0);
			_findclose(hFile);
		}
	}catch (std::exception e){ if(hFile) _findclose(hFile); }

	return ret;
}

bool DeleteEmptyFolder(const char *folder) { return IsEmptyFolder(folder); }

// 删除当前目录下的空目录（递归删除其子空目录）
int main()
{
	char buf[64];
	printf("要删除当前目录下的空目录,并且递归删除其空子目录吗? y/n\n");
	std::cin >> buf;
	if (buf[0] == 'y' || buf[0] == 'Y')
	{
		char folder_path[_MAX_PATH], *p = folder_path;
		GetModuleFileNameA(NULL, folder_path, sizeof(folder_path));
		while (*p) ++p;
		while ('\\' != *p) --p;
		*p = 0;
		DeleteEmptyFolder(folder_path);
		system("PAUSE");
		return 0;
	}

	return -1;
}
