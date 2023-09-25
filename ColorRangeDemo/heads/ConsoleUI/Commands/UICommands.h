#pragma once

#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include "../UISingletons.h"

using namespace ftxui;

namespace UICommands {
  //UIConfigure.h
  struct UICommandInterface {
    ftxui::ScreenInteractive screen = ScreenInteractive::TerminalOutput();

    std::string pText;
    ftxui::Components vBox;
    ftxui::Components hBox;
    ftxui::MenuOption menuOption;
    ftxui::Component configBtn;
    ftxui::Component renderBtn;
    bool showButton = false;

    void ShowButton() { showButton = true; };

    void HideButton() { showButton = false; };

    UICommandInterface() {
      menuOption = MenuOption();
      menuOption.on_enter = screen.ExitLoopClosure();
    }; //CommandINterface

    void MainMenuActivate() {

      auto style = ButtonOption::Animated(Color::Default, Color::White,
        Color::Blue, Color::Red);

      configBtn = Button("Configure", [&]() {UIValues::menuStage = UIParams::MenuStage::Configure; return screen.Exit(); }, ButtonOption::Simple());
      renderBtn = Button("Render", [&]() {terminate(); }, ButtonOption::Simple());

      auto menu = Container::Horizontal(hBox);
      auto vBox = Container::Vertical({ configBtn,renderBtn });
      if (showButton) menu->Add(vBox);
      auto render = Renderer(menu, [&] {return hbox({
        menu->Render(),
        separator(),
        vbox({
          paragraphAlignCenter(pText),
          }) | frame,
        }) |
        border; });
      screen.Loop(render);
    }; //MainMenuCmd

    void ConfigureActivate() {
      auto menu = Container::Vertical(vBox);
      menu->Add(Container::Horizontal(hBox));
      auto render = Renderer(menu, [&] {return vbox({
        menu->Render(),
        }) |
        border; });
      screen.Loop(render);
    }; //ConfigureActivate

    void Trigger() {
      switch (UIValues::menuStage) {
      case UIParams::MenuStage::MainMenuFirst:
      case UIParams::MenuStage::MainMenuSecond:
      case UIParams::MenuStage::MainMenuThird:
        MainMenuActivate();
        return;
      case UIParams::MenuStage::Configure:
        ConfigureActivate();
        return;
      }; //switch
    }; //Trigger
  }; //ConfigureCmd

}; //UICommands

