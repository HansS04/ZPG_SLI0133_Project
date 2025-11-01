#pragma once

class Camera;

class ICameraObserver {
public:
    virtual ~ICameraObserver() = default;
    virtual void update(Camera* cam) = 0;
};