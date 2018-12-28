// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#include <sqee/app/GuiWidgets.hpp>

#include <sqee/redist/imgui/imgui_internal.hpp>

//============================================================================//

using sq::Vec2F;

//============================================================================//

namespace { // anonymous

struct impl_InputTextCallbackUserData
{
    std::string& str;
    ImGuiInputTextCallback chainCallback;
    void* chainCallbackUserData;
};

int impl_input_text_callback(ImGuiInputTextCallbackData* data)
{
    auto userData = static_cast<impl_InputTextCallbackUserData*>(data->UserData);
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        IM_ASSERT(data->Buf == userData->str.data());
        userData->str.resize(static_cast<size_t>(data->BufTextLen));
        data->Buf = userData->str.data();
    }
    else if (userData->chainCallback != nullptr)
    {
        data->UserData = userData->chainCallbackUserData;
        return userData->chainCallback(data);
    }
    return 0;
}

} // anonymous namespace

//============================================================================//

bool ImGui::InputString(const char* label, std::string& str, ImGuiInputTextFlags flags,
                        ImGuiInputTextCallback callback, void* userData)
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    impl_InputTextCallbackUserData cbUserData = { str, callback, userData };
    return InputText(label, str.data(), str.capacity() + 1, flags, impl_input_text_callback, &cbUserData);
}

bool ImGui::InputStringMultiline(const char* label, std::string& str, ImVec2 size, ImGuiInputTextFlags flags,
                                 ImGuiInputTextCallback callback, void* userData)
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    impl_InputTextCallbackUserData cbUserData = { str, callback, userData };
    return InputTextMultiline(label, str.data(), str.capacity() + 1, size, flags, impl_input_text_callback, &cbUserData);
}

//----------------------------------------------------------------------------//

ImGui::DialogResult ImGui::DialogConfirmation(const char* title, const char* message)
{
    DialogResult result = DialogResult::None;

    if (ImGui::BeginPopupModal(title, nullptr, ImGuiWindowFlags_AlwaysAutoResize) == true)
    {
        ImGui::TextUnformatted(message);
        if (ImGui::Button("Confirm")) { result = DialogResult::Confirm; ImGui::CloseCurrentPopup(); }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) { result = DialogResult::Cancel; ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }
    else result = DialogResult::Cancel;

    return result;
}

//----------------------------------------------------------------------------//

bool ImGui::CloseButton(const char* label)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems == true)
        return false;

    const ImGuiContext& g = *GImGui;
    const ImGuiID id = window->GetID(label);

    const float frameHeight = GetFrameHeight();
    const ImRect bbox = { window->DC.CursorPos, Vec2F(window->DC.CursorPos) + frameHeight };

    ItemSize(bbox, 0.0f);
    if (ItemAdd(bbox, id) == false)
        return false;

    bool hovered, held;
    const bool pressed = ButtonBehavior(bbox, id, &hovered, &held);

    const ImU32 frameColour = GetColorU32((held) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
    RenderFrame(bbox.Min, bbox.Max, frameColour, true, g.Style.FrameRounding);

    const Vec2F centre = bbox.GetCenter();
    const float extent = (frameHeight - g.Style.FramePadding.y) * 0.25f;
    const ImU32 crossColour = GetColorU32(ImGuiCol_Text);
    window->DrawList->AddLine(centre + Vec2F(+extent,+extent), centre + Vec2F(-extent,-extent), crossColour);
    window->DrawList->AddLine(centre + Vec2F(+extent,-extent), centre + Vec2F(-extent,+extent), crossColour);

    return pressed;
}
