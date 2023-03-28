#include "event.h"

bool Event::isButtonPressed(const int targetButton) const
{
    return button & (1 << targetButton);
}