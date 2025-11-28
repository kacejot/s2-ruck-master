#pragma once

template <typename T>
class singleton
{
    singleton(const singleton&) = delete;
    singleton& operator=(const singleton&) = delete;

public:
    static T& instance()
    {
        static T s_instance;
        return s_instance;
    }

protected:
    singleton() = default;
    ~singleton() = default;
};

struct global_context : public singleton<global_context>
{
    uintptr_t game_base = 0;
};
