#ifndef COLLISION_TESTER_H_
#define COLLISION_TESTER_H_

#include "../spriteFactory.h"
#include "../window.h"
#include "../windowedApplication.h"
#include "../event.h"

class CollisionTester : public WindowedApplication {
public:
    CollisionTester(Window& window);
    void handleEvent(const Event& event) override;
    void render() override;
    void init() override;
private:
    Window& _window;
};

#endif