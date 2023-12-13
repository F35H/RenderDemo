#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <iostream>

#define clrscr(); system("cls");
//#define clrscr(); system("clear");

#include <ConsoleUI/UISingletons.h>
#include <ConsoleUI/Commands/UIMainMenu.h>
#include <ConsoleUI/Commands/UIConfigure.h>


static struct ConsoleUI {
  ConsoleUI() {
    clrscr();

    UICommands::UICommandInterface* menu = new UICommands::UICommandInterface();
    Vulkan::VulkanRender* vulkan;

    if (UIValues::menuStage == UIParams::MenuStage::MainMenuFirst) {
      switch (UIValues::mainSelection) {
      default:
        std::cout << "Not implemented yet dummy!" << "\n";
        std::cout << "UI Error Code:" << UIValues::mainSelection << "\n";
      case UIParams::MenuSelection::Exit:             UIValues::mainSelection = UIParams::Exit;           break;
      case UIParams::MenuSelection::MainMenu:         menu = new UICommands::MainMenu();                  break;
      case UIParams::MenuSelection::FilterParameters: menu = new UICommands::FilterParameters();          break;
      case UIParams::MenuSelection::GamutParameters:  menu = new UICommands::GamutParameters();           break;
      case UIParams::MenuSelection::ShadingParameters:menu = new UICommands::ShadingParameters();         break;
      case UIParams::MenuSelection::Render:           vulkan = new Vulkan::VulkanRender();
      }; //mainSelection
    }; //first 

    if (UIValues::menuStage == UIParams::MenuStage::MainMenuSecond) {
      switch (UIValues::mainSelection) {
        case UIParams::MenuSelection::FilterParameters:
          menu = new UICommands::FilterChoice();            
          UIValues::mainSelection = UIParams::MenuSelection::FilterParameters;
          break;
        case UIParams::MenuSelection::GamutParameters:  
          menu = new UICommands::GamutChoice();             
          UIValues::mainSelection = UIParams::MenuSelection::GamutParameters;
          break;
        case UIParams::MenuSelection::ShadingParameters:
          menu = new UICommands::ShadingChoice();
          UIValues::mainSelection = UIParams::MenuSelection::ShadingParameters;
          break;
      }; //mainSelection
    }; //First

    if (UIValues::menuStage == UIParams::MenuStage::MainMenuThird) {
      switch (UIValues::mainSelection) {
      case UIParams::MenuSelection::FilterParameters:
        menu = new UICommands::FilterInfo();
        UIValues::mainSelection = UIParams::MenuSelection::FilterParameters;
        break;
      case UIParams::MenuSelection::GamutParameters:
        menu = new UICommands::GamutInfo();
        UIValues::mainSelection = UIParams::MenuSelection::GamutParameters;
        break;
      case UIParams::MenuSelection::ShadingParameters:
        menu = new UICommands::ShadingInfo();
        //UIValues::mainSelection = UIParams::MenuSelection::ShadingParameters;
        break;
      }; //mainSelection
    }; //UIValues

    if (UIValues::menuStage == UIParams::MenuStage::Configure) {
      switch (UIValues::mainSelection) {
      case UIParams::MenuSelection::FilterParameters:
        menu = new UICommands::FilterConfigure();
        UIValues::mainSelection = UIParams::MenuSelection::FilterParameters;
        break;
      case UIParams::MenuSelection::GamutParameters:
        menu = new UICommands::GamutConfigure();
        UIValues::mainSelection = UIParams::MenuSelection::GamutParameters;
        break;
      case UIParams::MenuSelection::ShadingParameters:
        menu = new UICommands::ShadingConfigure();
        //UIValues::mainSelection = UIParams::MenuSelection::ShadingParameters;
        break;
      }; //mainSelection
    }; //COnfigure

    //if (UIValues::menuStage == UIParams::MenuStage:) {

    //}; //

    menu->Trigger();
  }; //ConsoleUI Ctor
}; //ConsoleUI