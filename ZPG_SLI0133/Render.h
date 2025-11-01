#pragma once

class Application;

class Render {
private:
    Application& m_App;

public:
    Render(Application& app);
    void run();
};