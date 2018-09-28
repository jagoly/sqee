#include <sqee/app/GuiWidgets.hpp>

//#define IMGUI_DEFINE_MATH_OPERATORS
//#include <sqee/redist/imgui/imgui_internal.hpp>

namespace sq::gui::detail {

//============================================================================//

void display_input_label(const char* label, float width)
{
    const ImGuiStyle& style = get_style();

    const float cursorStart = imgui::GetCursorPosX();

    const float textWidth = imgui::CalcTextSize(label).x;
    const float padding = (width - textWidth) / 2.f;

    imgui::AlignTextToFramePadding();
    imgui::SetCursorPosX(cursorStart + padding);
    imgui::TextUnformatted(label);
    imgui::SameLine(cursorStart + width + style.ItemSpacing.x);
}

//============================================================================//

/*bool display_input_text(const char* label, uint maxChars, string& ref, FlagSet<InputText> flags, InputTextCallback callback)
{
    const size_t bufSize = maxChars + 1u;

    //SQASSERT(ref.size() <= maxChars, "");
    if (ref.size() > maxChars) ref.resize(maxChars);

    SQASSERT(callback == nullptr || flags & (InputText::CallbackAlways | InputText::CallbackFilter), "");

    ImGuiInputTextFlags imFlags = 0;

    if (!(flags & InputText::LiveEdit)) imFlags |= ImGuiInputTextFlags_EnterReturnsTrue;
    if (flags & InputText::CallbackAlways) imFlags |= ImGuiInputTextFlags_CallbackAlways;
    if (flags & InputText::CallbackFilter) imFlags |= ImGuiInputTextFlags_CallbackCharFilter;
    if (flags & InputText::DisableTab) imFlags |= ImGuiInputTextFlags_CallbackCompletion;

    // using imgui provided label for now
    //if (labelWidth != 0.f) detail::display_input_label(label.c_str(), labelWidth);

    //UniquePtr<char[]> buffer { new char[bufSize] };

    auto buffer = std::make_unique<char[]>(bufSize);

    //Array<char, BufSize> buffer;
    ref.copy(buffer.get(), maxChars);
    buffer[ref.size()] = '\0';

    float height = imgui::CalcTextSize(ref.c_str()).y;
    height += imgui::CalcTextSize("\n").y + get_style().FramePadding.y * 2.f;
    const Vec2F size = { 0.f, height };

    bool changed;

    //--------------------------------------------------------//

    if (callback != nullptr)
    {
        const auto cbWrapper = [](ImGuiTextEditCallbackData* imData) -> int
        {
            InputTextCallbackData data
            {
                string(imData->Buf), uint(imData->CursorPos),
                uint(imData->SelectionStart), uint(imData->SelectionEnd),
                uint32_t(imData->EventChar)
            };

            std::invoke(*static_cast<InputTextCallback*>(imData->UserData), data);

            if (data.text != string_view(imData->Buf))
            {
                const size_t maxChars = size_t(imData->BufSize) - 1u;

                SQASSERT(data.text.size() <= maxChars, "naughty callback!");

                data.text.copy(imData->Buf, maxChars);
                imData->Buf[data.text.size()] = '\0';

                imData->BufTextLen = int(data.text.size());
                imData->BufDirty = true;
            }

            imData->CursorPos = int(data.cursorPos);
            imData->SelectionStart = int(data.selectStart);
            imData->SelectionEnd = int(data.selectEnd);

            imData->EventChar = ImWchar(data.character);

            return 0; // not sure why imgui needs a result
        };

        changed = imgui::InputTextMultiline(label, buffer.get(), bufSize, size, imFlags, cbWrapper, &callback);
    }

    else if (flags & InputText::DisableTab)
    {
        const auto cbDummy = [](ImGuiTextEditCallbackData*) -> int { return 0; };
        changed = imgui::InputTextMultiline(label, buffer.get(), bufSize, size, imFlags, cbDummy);
    }

    else changed = imgui::InputTextMultiline(label, buffer.get(), bufSize, size, imFlags);

    //--------------------------------------------------------//

    // need to do our own string compare when EnterReturnsTrue is enabled
    if (changed && (flags & InputText::LiveEdit || ref != string_view(buffer.get())))
    {
        ref = string(buffer.get());
        return true;
    }

    return false;
}*/


} // namespace sq::gui::detail
