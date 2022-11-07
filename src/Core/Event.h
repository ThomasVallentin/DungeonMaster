#ifndef EVENT_H
#define EVENT_H

#define DEFINE_EVENT_TYPE(_type) EventType GetType() const override { return EventType::_type; }  \
                                 const char* GetName() const override { return #_type; }   


enum class EventType {
    None = 0,
    WindowResized, WindowClosed,
    KeyPressed, KeyReleased, 
    MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
};


class Event {
public:
    virtual EventType GetType() const = 0;
    virtual const char* GetName() const = 0;
};


// == Window Events ==


class WindowResizedEvent : public Event {
public:
    WindowResizedEvent(const float& width, const float& height) : 
            m_width(width), m_height(height) {}

    float GetWidth() const { return m_width; }
    float GetHeight() const { return m_height; }

    DEFINE_EVENT_TYPE(WindowResized)

private:
    float m_width;
    float m_height;
};


class WindowClosedEvent : public Event {
public:
    WindowClosedEvent() = default;

    DEFINE_EVENT_TYPE(WindowClosed)
};


// == Key Events ==


class KeyEvent : public Event {
public:
    explicit KeyEvent(const int& key, const int& modifiers) : 
            m_key(key), m_modifiers(modifiers) {}

    int GetKey() const { return m_key; }
    int GetModifiers() const { return m_modifiers; }

private:
    int m_key;
    int m_modifiers;
};


class KeyPressedEvent : public KeyEvent {
public:
    explicit KeyPressedEvent(const int& key, const int& modifiers) : 
            KeyEvent(key, modifiers) {}

    DEFINE_EVENT_TYPE(KeyPressed)

private:
    int m_key;
};


class KeyReleasedEvent : public KeyEvent {
public:
    explicit KeyReleasedEvent(const int& key, const int& modifiers) : 
            KeyEvent(key, modifiers) {}

    DEFINE_EVENT_TYPE(KeyReleased)

private:
    int m_key;
};


// == Mouse Events ==


class MouseButtonEvent : public Event {
public:
    explicit MouseButtonEvent(const int& button, const int& modifiers) : 
            m_button(button), m_modifiers(modifiers) {}

    int GetButton() const { return m_button; }
    int GetModifiers() const { return m_modifiers; }

private:
    int m_button;
    int m_modifiers;
};


class MouseButtonPressedEvent : public MouseButtonEvent {
public:
    explicit MouseButtonPressedEvent(const int& button, const int& modifiers) : 
            MouseButtonEvent(button, modifiers) {}

    DEFINE_EVENT_TYPE(MouseButtonPressed)

private:
    int m_button;
};


class MouseButtonReleasedEvent : public MouseButtonEvent {
public:
    explicit MouseButtonReleasedEvent(const int& button, const int& modifiers) : 
            MouseButtonEvent(button, modifiers) {}

    DEFINE_EVENT_TYPE(MouseButtonReleased)

private:
    int m_button;
};


class MouseMovedEvent : public Event {
public:
    MouseMovedEvent(const float& posX, const float& posY) : 
            m_posX(posX), m_posY(posY) {}

    float GetPosX() const { return m_posX; }
    float GetPosY() const { return m_posY; }

    DEFINE_EVENT_TYPE(MouseMoved)

private:
    float m_posX;
    float m_posY;
};


class MouseScrolledEvent : public Event {
public:
    MouseScrolledEvent(const float& offsetX, const float& offsetY) : 
            m_offsetX(offsetX), m_offsetY(offsetY) {}

    float GetOffsetX() const { return m_offsetX; }
    float GetOffsetY() const { return m_offsetY; }

    DEFINE_EVENT_TYPE(MouseScrolled)

private:
    float m_offsetX;
    float m_offsetY;
};


#endif  // EVENT_H
