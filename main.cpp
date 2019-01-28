#include <stdint.h>
#include <io.h>
#include <string>
#include <vector>
#include <windows.h>
#include <iostream>
#include <time.h>

// 如果当前目录为空，则删除空目录
bool IsEmptyFolder(const std::string &folder_path, int &count)
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
					if (IsEmptyFolder(child, count)){
						BOOL b = RemoveDirectory(child.c_str());
						if (FALSE == b) ret = false;
						else ++count;
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

bool DeleteEmptyFolder(const char *folder, int &count) { return IsEmptyFolder(folder, count); }

// 删除当前目录下的空目录（递归删除其子空目录）
int main()
{
	// 获取当前目录
	char folder_path[_MAX_PATH], *p = folder_path;
	GetModuleFileNameA(NULL, folder_path, sizeof(folder_path));
	while (*p) ++p;
	while ('\\' != *p) --p;
	*p = 0;
	while (1)
	{
		// 获取用户输入目录
		std::string folder;
		do{
			folder = folder_path;
			char path[_MAX_PATH];
			printf("请输入目录:");
			std::cin >> path;
			_strlwr(path);
			if (path[0] == 'q' && strlen(path) == 1) // 输入q退出
				return -1;
			if (!('a' <= path[0] && path[0] <= 'z'))
				folder.append("\\").append(path);
			else
				folder = path;
		}while (_access(folder.c_str(), 0) == -1);
		// 进行二次确认
		char buf[64];
		printf("你选择了目录：%s\n", folder.c_str());
		printf("要删除该目录下的空目录,并且递归删除其空子目录吗? y/n\n");
		std::cin >> buf;
		if (buf[0] == 'y' || buf[0] == 'Y')
		{
			int count = 0;
			clock_t tick = clock();
			DeleteEmptyFolder(folder.c_str(), count);
			printf("删除空目录[%d个]完毕，耗时[%d]ms\n", count, int(clock()-tick));
			system("PAUSE");
		}
	}

	return 0;
}
