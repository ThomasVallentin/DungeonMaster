#include "Core/Application.h"


int main(int argc, char* argv[]) {
    Application& app = Application::Init(argc, argv);
    
    app.Run();
    return app.GetExitCode();
}
