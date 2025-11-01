#pragma once

class Light; // Dopøedná deklarace

class ILightObserver {
public:
    virtual ~ILightObserver() = default;

    // Metoda, kterou Subject zavolá, když se zmìní
    virtual void onLightChanged(const Light* light) = 0;
};