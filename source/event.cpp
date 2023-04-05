#include "event.h"

bool Event::isButtonPressed(const int targetButton) const
{
    return button & (1 << targetButton);
}

const Event Event::add(const PartialEvent&& other) const
{
    return {
        other.type.value_or(type),
        other.action.value_or(action),
        other.key.value_or(key),
        other.scancode.value_or(scancode),
        other.codepoint.value_or(codepoint),
        other.x.value_or(x),
        other.y.value_or(y),
        other.button.value_or(button),
        other.xoffset.value_or(xoffset),
        other.yoffset.value_or(yoffset),
        other.width.value_or(width),
        other.height.value_or(height),
        other.mods.value_or(mods)
    };
}