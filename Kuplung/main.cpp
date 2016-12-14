#include "kuplung/Kuplung.hpp"

/// @mainpage Kuplung - OpenGL 3D Viewer
///
/// @section intro Introduction
///
/// This application represents trials and errors collection -
/// adventure in C/C++ development and learning.<br />
/// It is by no means correct or exhaustive and is still work in progress.<br />
///
/// @section using Using
///
/// <ul>
/// <li><a href="https://github.com/ocornut/imgui" target="_blank">ImGui</a></li>
/// <li><a href="https://github.com/CedricGuillemet/ImGuizmo" target="_blank">ImGuizmo</a></li>
/// <li><a href="https://github.com/nothings/stb" target="_blank">STB libs</a></li>
/// <li><a href="http://www.boost.org/" target="_blank">Boost</a></li>
/// <li><a href="http://glm.g-truc.net/" target="_blank">GLM</a></li>
/// <li><a href="https://www.libsdl.org/" target="_blank">SDL2</a></li>
/// <li><a href=">http://libnoise.sourceforge.net/" target="_blank">libnoise</a></li>
/// </ul>
///
/// @section built Built with
///
/// <ul>
/// <li><a href="https://www.qt.io/ide/" target="_blank">Qt Creator</a></li>
/// <li><a href="https://developer.apple.com/xcode/" target="_blank">Xcode</a></li>
/// </ul>
///
/// @section contact Contact
///
/// Contact supudo for questions about everything.<br />
/// Send your questions and comments to supudo@gmail.com<br />
/// <a href="http://supudo.net" target="_blank">http://supudo.net</a>

int main() {
    //FIXME: Remove for prod
    setbuf(stdout, NULL);

    Kuplung app;
    return app.run();
}
