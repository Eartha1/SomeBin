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
			{//����ʱ��
				ImGui::Spacing();
				SYSTEMTIME systime;
				GetLocalTime(&systime);
				ImGui::SameLine(); ImGui::Text(XorStr(u8"��ǰʱ�� : %.2d:%.2d:%.2d  %.4d��%.2d��%.2d��"), systime.wHour, systime.wMinute, systime.wSecond, systime.wYear, systime.wMonth, systime.wDay);
				ImGui::Spacing();
				ImGui::SameLine(); ImGui::Text(XorStr(u8"\t\t\t["));
				ImGui::SameLine(); ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), XorStr(u8"\tHOME"));
				ImGui::SameLine(); ImGui::Text(XorStr(u8"\t]"));
				ImGui::SameLine(); ImGui::Text(XorStr(u8"\t����"));
				ImGui::Spacing();
				ImGui::SameLine(); ImGui::Text(XorStr(u8"\t\t\t["));
				ImGui::SameLine(); ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), XorStr(u8"\tEND "));
				ImGui::SameLine(); ImGui::Text(XorStr(u8"\t]"));
				ImGui::SameLine(); ImGui::Text(XorStr(u8"\t�˳�"));

			}

			ImGui::BeginChild(XorStr("����ͼ��"), ImVec2(160, 192), true);
			{
				ImGui::TextColored(ImVec4(0.0f, 255.0f, 255.0f, 1.0f), XorStr(u8"[ ����ͼ�� ]"));
				ImGui::Separator();

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 255.0f, 255.0f, 0.3f));
				if (ImGui::Button(u8"���浱ǰ����ͼ��λ��", ImVec2(130, 60)))
				{
					HWND hDeskTop;
					DWORD dwProcessId;
					hDeskTop = ::FindWindow("progman", NULL);
					hDeskTop = ::FindWindowEx(hDeskTop, 0, "shelldll_defview", NULL);
					hDeskTop = ::FindWindowEx(hDeskTop, 0, "syslistview32", NULL); //��ȡ���洰�ھ��;

					int Icon_value = ListView_GetItemCount(hDeskTop);
					DWORD dwStyle = (DWORD)GetWindowLong(hDeskTop, GWL_STYLE);
					DWORD dwExStyle = (DWORD)ListView_GetExtendedListViewStyle(hDeskTop, GWL_EXSTYLE);
					if (dwStyle & LVS_AUTOARRANGE || dwExStyle & LVS_EX_SNAPTOGRID)
					{
						char arr[256] = {};
						sprintf(arr,"��ǰ���������湲��%d��ͼ��,\r\n �����Զ�ȡ��ͼ�����������,\r\n�Ƿ����(�Ժ�����Ҽ�����-�鿴���и���)",Icon_value);
						if (IDYES == MessageBox(0, arr, "Information", MB_ICONINFORMATION | MB_YESNO))
						{
							SetWindowLong(hDeskTop, GWL_STYLE, dwStyle & ~LVS_AUTOARRANGE);
							ListView_SetExtendedListViewStyle(hDeskTop, dwExStyle & ~LVS_EX_SNAPTOGRID);
						}
						GetWindowThreadProcessId(hDeskTop, &dwProcessId);//ͨ�����洰�ھ����ȡ�˴������ڽ��̵�PID,��ʵ����explorer����
						HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);//��ָ��PID����,ȡ�ý��̾��
						LPVOID lpvPt = VirtualAllocEx(hProcess, NULL, sizeof(POINT), MEM_COMMIT, PAGE_READWRITE);//��ָ��������������һ��POINI�ṹ��С�Ŀռ�.
						POINT pt;
						std::string str;
						for (int i = 0; i < Icon_value; ++i)
						{
							ListView_GetItemPosition(hDeskTop, i, lpvPt);//��ȡ��һ��ͼ�������,����lpvPt
							ReadProcessMemory(hProcess, lpvPt, &pt, sizeof(POINT), NULL);
							str += std::to_string(pt.x) + ' ' + std::to_string(pt.y) + "\n";
						}
						std::ofstream OpenFile("D:\\IconInfo.txt");
						OpenFile << str << "\n";
						OpenFile.close();
						MessageBox(0, "����ɹ�,�ļ�·��: D:\\IconInfo.txt", "ok", 0);
					}
					else
					{
						char arr[256] = {};
						sprintf(arr, "��ǰ���������湲��%d��ͼ�� �Ƿ��Զ�����\r\n", Icon_value);
						if (IDYES == MessageBox(0, arr, "Information", MB_ICONINFORMATION | MB_YESNO))
						{
							GetWindowThreadProcessId(hDeskTop, &dwProcessId);//ͨ�����洰�ھ����ȡ�˴������ڽ��̵�PID,��ʵ����explorer����
							HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);//��ָ��PID����,ȡ�ý��̾��
							LPVOID lpvPt = VirtualAllocEx(hProcess, NULL, sizeof(POINT), MEM_COMMIT, PAGE_READWRITE);//��ָ��������������һ��POINI�ṹ��С�Ŀռ�.
							POINT pt;
							std::string str;
							for (int i = 0; i < Icon_value; ++i)
							{
								ListView_GetItemPosition(hDeskTop, i, lpvPt);//��ȡ��һ��ͼ�������,����lpvPt
								ReadProcessMemory(hProcess, lpvPt, &pt, sizeof(POINT), NULL);
								str += std::to_string(pt.x) + ' ' + std::to_string(pt.y) + "\n";
							}
							std::ofstream OpenFile("D:\\IconInfo.txt");
							OpenFile << str << "\n";
							OpenFile.close();
							MessageBox(0, "����ɹ�,�ļ�·��: D:\\IconInfo.txt", "ok", 0);
						}
						
					}
				}
				ImGui::PopStyleColor();
			}
			ImGui::EndChild();

			ImGui::SameLine(180);

			ImGui::BeginChild(XorStr("�ָ�ͼ��"), ImVec2(160, 192), true);
			{
				ImGui::TextColored(ImVec4(0.0f, 255.0f, 255.0f, 1.0f), XorStr(u8"[ �ָ�ͼ�� ]"));
				ImGui::Separator();

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 255.0f, 255.0f, 0.3f));
				if (ImGui::Button(u8"�ָ���ǰ����ͼ��λ��", ImVec2(130, 60)))
				{
					HWND hDeskTop;
					DWORD dwProcessId;
					hDeskTop = ::FindWindow("progman", NULL);
					hDeskTop = ::FindWindowEx(hDeskTop, 0, "shelldll_defview", NULL);
					hDeskTop = ::FindWindowEx(hDeskTop, 0, "syslistview32", NULL); //��ȡ���洰�ھ��;
					
					int Icon_value = ListView_GetItemCount(hDeskTop);
					//GetWindowThreadProcessId(hDeskTop, &dwProcessId);//ͨ�����洰�ھ����ȡ�˴������ڽ��̵�PID,��ʵ����explorer����
					//HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);//��ָ��PID����,ȡ�ý��̾��
					//LPVOID lpvPt = VirtualAllocEx(hProcess, NULL, sizeof(POINT), MEM_COMMIT, PAGE_READWRITE);//��ָ��������������һ��POINI�ṹ��С�Ŀռ�.
					//POINT pt;
					//std::string str;
					//for (int i = 0; i < Icon_value; ++i)
					//{
					//	ListView_GetItemPosition(hDeskTop, i, lpvPt);//��ȡ��һ��ͼ�������,����lpvPt
					//	ReadProcessMemory(hProcess, lpvPt, &pt, sizeof(POINT), NULL);
					//	str += std::to_string(pt.x) + ' ' + std::to_string(pt.y) + "\n";
					//}

					char *filename = "D:\\IconInfo.txt";
					std::ifstream inf(filename);
					if (!inf.is_open())
					{
						MessageBox(0, "���Ƚ��б���", "Error", NULL);
					}
					else
					{
						if (IDNO == MessageBox(0, "�ҵ���ǰ����, �Ƿ���Ҫ���±���", "Succeed", MB_ICONINFORMATION | MB_YESNO))
						{
							std::vector<std::string> vec;
							std::string temp;
							std::ifstream myfile("D:\\IconInfo.txt");
							while (getline(myfile, temp))  //����getline��ȡÿһ�У���������Ϊ��λ���뵽vector
							{
								vec.push_back(temp);
							}
							std::vector <int> myvec;
							for (auto it = vec.begin(); it != vec.end(); it++)
							{
								std::istringstream is(*it);                    //��ÿһ�е����ݳ�ʼ��һ���ַ�����������
								std::string s;
								int pam = 0;
								while (is >> s)                          //�Կո�Ϊ�磬��istringstream������ȡ�����뵽����s��
								{
									int r = atoi(s.c_str());     //����������ת������string����ת����float
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

							MessageBox(0, "�ָ����", "ok", 0);
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



