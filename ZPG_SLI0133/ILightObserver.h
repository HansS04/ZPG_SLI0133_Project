#pragma once

class Light;

class ILightObserver {
public:
    virtual ~ILightObserver() = default;

    virtual void onLightChanged(const Light* light) = 0;
};
