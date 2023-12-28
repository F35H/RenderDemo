#include <ConsoleUI/Facade.h>
#define STB_IMAGE_IMPLEMENTATION

int main() {
  //ConsoleUI* ui;
  try {
    while (UIValues::mainSelection != UIParams::Exit) {
      ConsoleUI::ConsoleUI();
    }; //while true;
  } //Try
  catch (const std::exception& ex) {
    std::cout << ex.what() << std::endl;
  }; //Catch
  return 0;
}