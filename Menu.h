#include "stdafx.h"
#include <fstream>
#include <sstream>
class CMenu : public std::Singleton <CMenu>
{
private:

	struct Color {
		float color[3]{ 1.0f, 1.0f, 1.0f };
		bool rainbow{ false };
		float rainbowSpeed{ 0.6f };
	};
	struct ColorToggle : public Color {
		bool enabled{ false };
	};

private:
	static void ShowHelpMarker(const char* desc)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	void colorPicker(const char* name, float color[3], bool* enable, bool* rainbow, float* rainbowSpeed) noexcept
	{
		ImGui::PushID(name);
		if (enable) {
			ImGui::Checkbox("##check", enable);
			ImGui::SameLine(0.0f, 5.0f);
		}
		bool openPopup = ImGui::ColorButton("##btn", ImVec4(color[0], color[1], color[2],0.8), ImGuiColorEditFlags_NoTooltip);
		ImGui::SameLine(0.0f, 5.0f);
		ImGui::TextUnformatted(name);

		if (openPopup)
			ImGui::OpenPopup("##popup");

		if (ImGui::BeginPopup("##popup")) {
			ImGui::ColorPicker3("##picker", color, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoSidePreview);

			if (rainbow && rainbowSpeed) {
				ImGui::SameLine();

				if (ImGui::BeginChild("##child", { 100.0f, 0.0f })) {
					ImGui::Checkbox("Rainbow", rainbow);
					ImGui::SetNextItemWidth(50.0f);
					ImGui::InputFloat("Speed", rainbowSpeed, 0.0f, 0.0f, "%.1f");
					ImGui::EndChild();
				}
			}
			ImGui::EndPopup();
		}
		ImGui::PopID();
	}
	void colorPicker(const char* name, ColorToggle& colorConfig) noexcept
	{
		colorPicker(name, colorConfig.color, &colorConfig.enabled, &colorConfig.rainbow, &colorConfig.rainbowSpeed);
	}

public:
	bool Open = true;
public:
	void DrawMeun()
	{
		if (Open)
		{//ImGuiWindowFlags_NoCollapse |
			ImGui::Begin(XorStr(u8"Menu"), NULL, ImVec2(355, 305), 0.8f, ImGuiWindowFlags_NoResize |  ImGuiWindowFlags_NoSavedSettings);
			{//本地时间
				ImGui::Spacing();
				SYSTEMTIME systime;
				GetLocalTime(&systime);
				ImGui::SameLine(); ImGui::Text(XorStr(u8"当前时间 : %.2d:%.2d:%.2d  %.4d年%.2d月%.2d日"), systime.wHour, systime.wMinute, systime.wSecond, systime.wYear, systime.wMonth, systime.wDay);
				ImGui::Spacing();
				ImGui::SameLine(); ImGui::Text(XorStr(u8"\t\t\t["));
				ImGui::SameLine(); ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), XorStr(u8"\tHOME"));
				ImGui::SameLine(); ImGui::Text(XorStr(u8"\t]"));
				ImGui::SameLine(); ImGui::Text(XorStr(u8"\t显隐"));
				ImGui::Spacing();
				ImGui::SameLine(); ImGui::Text(XorStr(u8"\t\t\t["));
				ImGui::SameLine(); ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), XorStr(u8"\tEND "));
				ImGui::SameLine(); ImGui::Text(XorStr(u8"\t]"));
				ImGui::SameLine(); ImGui::Text(XorStr(u8"\t退出"));

			}

			ImGui::BeginChild(XorStr("保存图标"), ImVec2(160, 192), true);
			{
				ImGui::TextColored(ImVec4(0.0f, 255.0f, 255.0f, 1.0f), XorStr(u8"[ 保存图标 ]"));
				ImGui::Separator();

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 255.0f, 255.0f, 0.3f));
				if (ImGui::Button(u8"保存当前桌面图标位置", ImVec2(130, 60)))
				{
					HWND hDeskTop;
					DWORD dwProcessId;
					hDeskTop = ::FindWindow("progman", NULL);
					hDeskTop = ::FindWindowEx(hDeskTop, 0, "shelldll_defview", NULL);
					hDeskTop = ::FindWindowEx(hDeskTop, 0, "syslistview32", NULL); //获取桌面窗口句柄;

					int Icon_value = ListView_GetItemCount(hDeskTop);
					DWORD dwStyle = (DWORD)GetWindowLong(hDeskTop, GWL_STYLE);
					DWORD dwExStyle = (DWORD)ListView_GetExtendedListViewStyle(hDeskTop, GWL_EXSTYLE);
					if (dwStyle & LVS_AUTOARRANGE || dwExStyle & LVS_EX_SNAPTOGRID)
					{
						char arr[256] = {};
						sprintf(arr,"当前检索到桌面共有%d个图标,\r\n 将会自动取消图标对齐至网格,\r\n是否继续(稍后可以右键桌面-查看进行更改)",Icon_value);
						if (IDYES == MessageBox(0, arr, "Information", MB_ICONINFORMATION | MB_YESNO))
						{
							SetWindowLong(hDeskTop, GWL_STYLE, dwStyle & ~LVS_AUTOARRANGE);
							ListView_SetExtendedListViewStyle(hDeskTop, dwExStyle & ~LVS_EX_SNAPTOGRID);
						}
						GetWindowThreadProcessId(hDeskTop, &dwProcessId);//通过桌面窗口句柄获取此窗口所在进程的PID,其实就是explorer进程
						HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);//打开指定PID进程,取得进程句柄
						LPVOID lpvPt = VirtualAllocEx(hProcess, NULL, sizeof(POINT), MEM_COMMIT, PAGE_READWRITE);//在指定进程里面申请一个POINI结构大小的空间.
						POINT pt;
						std::string str;
						for (int i = 0; i < Icon_value; ++i)
						{
							ListView_GetItemPosition(hDeskTop, i, lpvPt);//获取第一个图标的坐标,存入lpvPt
							ReadProcessMemory(hProcess, lpvPt, &pt, sizeof(POINT), NULL);
							str += std::to_string(pt.x) + ' ' + std::to_string(pt.y) + "\n";
						}
						std::ofstream OpenFile("D:\\IconInfo.txt");
						OpenFile << str << "\n";
						OpenFile.close();
						MessageBox(0, "保存成功,文件路径: D:\\IconInfo.txt", "ok", 0);
					}
					else
					{
						char arr[256] = {};
						sprintf(arr, "当前检索到桌面共有%d个图标 是否自动备份\r\n", Icon_value);
						if (IDYES == MessageBox(0, arr, "Information", MB_ICONINFORMATION | MB_YESNO))
						{
							GetWindowThreadProcessId(hDeskTop, &dwProcessId);//通过桌面窗口句柄获取此窗口所在进程的PID,其实就是explorer进程
							HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);//打开指定PID进程,取得进程句柄
							LPVOID lpvPt = VirtualAllocEx(hProcess, NULL, sizeof(POINT), MEM_COMMIT, PAGE_READWRITE);//在指定进程里面申请一个POINI结构大小的空间.
							POINT pt;
							std::string str;
							for (int i = 0; i < Icon_value; ++i)
							{
								ListView_GetItemPosition(hDeskTop, i, lpvPt);//获取第一个图标的坐标,存入lpvPt
								ReadProcessMemory(hProcess, lpvPt, &pt, sizeof(POINT), NULL);
								str += std::to_string(pt.x) + ' ' + std::to_string(pt.y) + "\n";
							}
							std::ofstream OpenFile("D:\\IconInfo.txt");
							OpenFile << str << "\n";
							OpenFile.close();
							MessageBox(0, "保存成功,文件路径: D:\\IconInfo.txt", "ok", 0);
						}
						
					}
				}
				ImGui::PopStyleColor();
			}
			ImGui::EndChild();

			ImGui::SameLine(180);

			ImGui::BeginChild(XorStr("恢复图标"), ImVec2(160, 192), true);
			{
				ImGui::TextColored(ImVec4(0.0f, 255.0f, 255.0f, 1.0f), XorStr(u8"[ 恢复图标 ]"));
				ImGui::Separator();

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 255.0f, 255.0f, 0.3f));
				if (ImGui::Button(u8"恢复当前桌面图标位置", ImVec2(130, 60)))
				{
					HWND hDeskTop;
					DWORD dwProcessId;
					hDeskTop = ::FindWindow("progman", NULL);
					hDeskTop = ::FindWindowEx(hDeskTop, 0, "shelldll_defview", NULL);
					hDeskTop = ::FindWindowEx(hDeskTop, 0, "syslistview32", NULL); //获取桌面窗口句柄;
					
					int Icon_value = ListView_GetItemCount(hDeskTop);
					//GetWindowThreadProcessId(hDeskTop, &dwProcessId);//通过桌面窗口句柄获取此窗口所在进程的PID,其实就是explorer进程
					//HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);//打开指定PID进程,取得进程句柄
					//LPVOID lpvPt = VirtualAllocEx(hProcess, NULL, sizeof(POINT), MEM_COMMIT, PAGE_READWRITE);//在指定进程里面申请一个POINI结构大小的空间.
					//POINT pt;
					//std::string str;
					//for (int i = 0; i < Icon_value; ++i)
					//{
					//	ListView_GetItemPosition(hDeskTop, i, lpvPt);//获取第一个图标的坐标,存入lpvPt
					//	ReadProcessMemory(hProcess, lpvPt, &pt, sizeof(POINT), NULL);
					//	str += std::to_string(pt.x) + ' ' + std::to_string(pt.y) + "\n";
					//}

					char *filename = "D:\\IconInfo.txt";
					std::ifstream inf(filename);
					if (!inf.is_open())
					{
						MessageBox(0, "请先进行备份", "Error", NULL);
					}
					else
					{
						if (IDNO == MessageBox(0, "找到以前备份, 是否需要从新备份", "Succeed", MB_ICONINFORMATION | MB_YESNO))
						{
							std::vector<std::string> vec;
							std::string temp;
							std::ifstream myfile("D:\\IconInfo.txt");
							while (getline(myfile, temp))  //利用getline读取每一行，并按照行为单位放入到vector
							{
								vec.push_back(temp);
							}
							std::vector <int> myvec;
							for (auto it = vec.begin(); it != vec.end(); it++)
							{
								std::istringstream is(*it);                    //用每一行的数据初始化一个字符串输入流；
								std::string s;
								int pam = 0;
								while (is >> s)                          //以空格为界，把istringstream中数据取出放入到依次s中
								{
									int r = atoi(s.c_str());     //做数据类型转换，将string类型转换成float
									myvec.push_back(r);
								}
							}

							int IconX = 0;
							int IconY = 1;
							for (int i = 0; i < Icon_value; ++i)
							{
								::SendMessage(hDeskTop, LVM_SETITEMPOSITION, i, MAKELPARAM(myvec[IconX], myvec[IconY]));
								IconX += 2;
								IconY += 2;
							}
							ListView_RedrawItems(hDeskTop, 0, ListView_GetItemCount(hDeskTop) - 1);
							::UpdateWindow(hDeskTop);

							MessageBox(0, "恢复完毕", "ok", 0);
						}
					}
				}

				ImGui::PopStyleColor();
			}


			ImGui::EndChild();

			ImGui::End();
		}
	}

};



