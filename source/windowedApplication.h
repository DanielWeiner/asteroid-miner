#ifndef WINDOWED_APPLICATION_H_
#define WINDOWED_APPLICATION_H_

class Event;
class Window;

class WindowedApplication {
public:
    virtual ~WindowedApplication() {};
private:
    friend Window;
    virtual void handleEvent(const Event& event) = 0;
    virtual void render() = 0;
    virtual void init() = 0;
    virtual const char* getName() = 0;
    virtual float getWidth() = 0;
    virtual float getHeight() = 0;
    
};

#endif