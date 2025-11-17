#include <UI/UIManager.h>
#include <Utils/Debug.h>

 ///ImGui includes
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>

UIManager::UIManager() {}
UIManager::~UIManager() {}



void UIManager::StartInvisibleWindow(ImVec2 pos){
	ImGui::SetNextWindowPos(pos); // (x, y) in screen coordinates
	ImGui::SetNextWindowBgAlpha(0.0f); // Fully opaque background (0 = invisible, 1 = opaque)
	ImGui::Begin("##HiddenLabel", nullptr, ImGuiWindowFlags_NoTitleBar 
										 | ImGuiWindowFlags_NoBackground 
										 | ImGuiWindowFlags_AlwaysAutoResize); // this basically sets the window to be invisible
}

void UIManager::EndWindow(){
	ImGui::End();
}
void UIManager::PushButtonStyle(ImVec4 normal, ImVec4 hovered, ImVec4 active, float rounding)
{
	ImGui::PushStyleColor(ImGuiCol_Button, normal);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hovered);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, active);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, rounding);
}

void UIManager::PushSliderStyle(ImVec4 normal, ImVec4 hovered, ImVec4 active, float rounding)
{
	ImVec4 bg_normal = normal;
	bg_normal.w = 0.5f;
	ImVec4 bg_hovered = hovered;
	bg_hovered.w = 0.5f;
	ImVec4 bg_active = active;
	bg_active.w = 0.5f;

	ImGui::PushStyleColor(ImGuiCol_FrameBg, bg_normal);
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, bg_hovered);
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, bg_active);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, rounding);

	ImGui::PushStyleColor(ImGuiCol_SliderGrab, normal);
	ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, active);
	ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, rounding);

}

void UIManager::PopButtonStyle()
{
	ImGui::PopStyleColor(3); // Pop 3 colors
	ImGui::PopStyleVar(1);   // Pop 1 style var
}

void UIManager::PopSliderStyle()
{
	ImGui::PopStyleColor(5); // Pop 5 colors
	ImGui::PopStyleVar(2);   // Pop 1 style var

}





