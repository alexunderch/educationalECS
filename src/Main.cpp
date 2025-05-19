#include <iostream>
#include "./Game/Game.h"
#include <sol/sol.hpp>

int nativeCubeFunction(int n) {
    return (n * n * n);
}

void TestLua() {
    //declaring the state
    sol::state lua;
    //including the libraries
    lua.open_libraries(sol::lib::base);

    //exposing and binding native c++ to lua
    lua["cube"] = nativeCubeFunction;

    lua.script_file("./assets/scripts/myscript.lua");
    //getting value of a global variable from the script
    int someVariable = lua["some_variable"];

    std::cout << "The value of the variable in C++ is " << someVariable << std::endl;
    // bool isFullScreen = lua["config"]["fullscreen"];
    
    //reading the config 
    sol::table config = lua["config"];
    int height = config["resolution"]["height"];
    int width = config["resolution"]["width"];

    std::cout << "The resolution is (" << width  << ", " <<  height << ")" << std::endl;

    sol::function funFactorial = lua["factorial"];
    int inputValue = 5;
    int result = funFactorial(inputValue);
    std::cout << "The value of the result of factorial of "<< inputValue << "in C++ is " << result << std::endl;

}

int main(int argc, char* argv[]) {
    Game game;
    
    game.Initialise();
    game.Run();
    game.Destroy();
    return 0;
}
