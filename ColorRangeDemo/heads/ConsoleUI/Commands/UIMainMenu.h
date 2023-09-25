#pragma once

#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/component_options.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

#include "../UISingletons.h"
#include "ConsoleUI/Commands/UICommands.h"

using namespace ftxui;

namespace UICommands {
  struct MainMenuCmd : UICommandInterface {

    MainMenuCmd() {
      auto style = ButtonOption::Animated(Color::Default, Color::White,
        Color::Blue, Color::Red);

      configBtn = Button("Configure", [&]() {terminate(); }, ButtonOption::Simple());
      renderBtn = Button("Render", [&]() {terminate(); }, ButtonOption::Simple());
    }; //CommandCtor

    void ShowButton() { showButton = true; };

    void HideButton() { showButton = false; };
  }; //Command

  struct MainMenu : MainMenuCmd {
    MainMenu() {
      hBox.emplace_back(
        Menu(&UIParams::menuChoice, &UIValues::mainSelection, menuOption)
      ); //hbox

    }; //MainMenuCtor
  }; //MainMenu


  //First Depth
  struct FilterParameters : MainMenu {
    FilterParameters() {
      hBox.emplace_back(
        Menu(&UIParams::filterLineage, &UIValues::filterLineage, menuOption)
      ); //hBox

      UIValues::menuStage = UIParams::MenuStage::MainMenuSecond;
    }; //FilterParameters
  }; //FilterParameters

  struct GamutParameters : MainMenu {
    GamutParameters() {
      hBox.emplace_back(
        Menu(&UIParams::gamutLineage, &UIValues::gamutLineage, menuOption)
      ); //hBox

      UIValues::menuStage = UIParams::MenuStage::MainMenuSecond;
    }; //GamutParameters
  }; //GamutParameters

  struct ShadingParameters : MainMenu {
    ShadingParameters() {
      hBox.emplace_back(
        Menu(&UIParams::shadingLineage, &UIValues::shadingLineage, menuOption)
      ); //hBox
      UIValues::menuStage = UIParams::MenuStage::MainMenuSecond;
    }; //ShadingParameters
  }; //GamutParameters


  //SecondDepth
  struct GamutChoice : GamutParameters {
    GamutChoice() {
      switch (UIValues::gamutLineage) {
      default:
      case UILineageEnums::GamutLineage::RGB: {
        hBox.emplace_back(
          Menu(&UIParams::gamutRGB, &UIValues::gamutChoice, menuOption)
        ); //hBox
      }; //RGB
      break;
      case UILineageEnums::GamutLineage::CIE: {
        hBox.emplace_back(
          Menu(&UIParams::gamutCIE, &UIValues::gamutChoice, menuOption)
        ); //hBox
      }; //CIE
      break;
      case UILineageEnums::GamutLineage::ComputerVision: {
        hBox.emplace_back(
          Menu(&UIParams::gamutComputerVision, &UIValues::gamutChoice, menuOption)
        ); //hBox
      }; //ComputerVision
      break;
      case UILineageEnums::GamutLineage::Television: {
        hBox.emplace_back(
          Menu(&UIParams::gamutTV, &UIValues::gamutChoice, menuOption)
        ); //hBox
      }; //Television
      break;

      }; //switch

      UIValues::menuStage = UIParams::MenuStage::MainMenuThird;
    }; //GamutChoice
  }; //GamutChoice


  struct ShadingChoice : ShadingParameters {
    ShadingChoice() {
      switch (UIValues::shadingLineage) {
      default:
      case UILineageEnums::ShadingLineage::NoShading:
        UIValues::mainSelection = UIParams::MenuSelection::MainMenu;
        return;
      case UILineageEnums::ShadingLineage::Traditional: {
        hBox.emplace_back(
          Menu(&UIParams::shadingTraditional, &UIValues::shadingChoice, menuOption)
        ); //hBox
      }; //Traditional
      break;
      case UILineageEnums::ShadingLineage::NPR: {
        hBox.emplace_back(
          Menu(&UIParams::shadingNPR, &UIValues::shadingChoice, menuOption)
        ); //hBox
      }; //NPR
      break;
      }; //switch

      UIValues::menuStage = UIParams::MenuStage::MainMenuThird;
    }; //GamutChoice
  }; //ShadingChoice

  struct FilterChoice : FilterParameters {
    FilterChoice() {
      switch (UIValues::filterLineage) {
      default:
      case UILineageEnums::FilterLineage::NoFilter:
        UIValues::mainSelection = UIParams::MenuSelection::MainMenu;
        return;
      case UILineageEnums::FilterLineage::ColorDificiency: {
        hBox.emplace_back(
          Menu(&UIParams::filterColorDeficiency, &UIValues::filterChoice, menuOption)
        ); //hBox
      }; //ColorDefficiency
      break;
      }; //switch

      UIValues::menuStage = UIParams::MenuStage::MainMenuThird;
    }; //FilterChoice
  }; //FilterChoice


  //Third Depth
  struct GamutInfo : GamutChoice {
    GamutInfo() {
      ShowButton();

      switch (UIValues::gamutLineage) {
      default:
      case UILineageEnums::GamutLineage::RGB:
        goto switch_RGB;
      case UILineageEnums::GamutLineage::CIE:
        goto switch_CIE;
      case UILineageEnums::GamutLineage::ComputerVision:
        goto switch_ComputerVision;
      case UILineageEnums::GamutLineage::Television:
        goto switch_TV;
      }; //switch


    switch_RGB:
      switch (UIValues::gamutChoice) {
      case UIRenderEnums::Gamuts::RGB: {
        pText = UIParams::GamutInfo::RGB;
      }; //RGB
                                     return;
      case UIRenderEnums::Gamuts::CMY: {
        pText = UIParams::GamutInfo::CMY;
      }; //CMY
                                     return;
      case UIRenderEnums::Gamuts::HSL: {
        pText = UIParams::GamutInfo::HSL;
      }; //HSL
                                     return;
      case UIRenderEnums::Gamuts::HSV: {
        pText = UIParams::GamutInfo::HSV;
      }; //HSV
                                     return;
      case UIRenderEnums::Gamuts::HWB: {
        pText = UIParams::GamutInfo::HWB;
      }; //HWB
                                     return;
      }; //switchRGB


    switch_CIE:


    switch_ComputerVision:
      switch (UIValues::gamutChoice) {
      case UIRenderEnums::Gamuts::IHLS: {
        pText = UIParams::GamutInfo::IHLS;
      }; //IHLS
                                      return;
      }; //gamutChoice


    switch_TV:
      switch (UIValues::gamutChoice) {
      case UIRenderEnums::Gamuts::YIQ: {
        pText = UIParams::GamutInfo::YIQ;
      }; //YIQ
                                     return;
      case UIRenderEnums::Gamuts::YDbDr: {
        pText = UIParams::GamutInfo::YDbDr;
      }; //YIQ
                                       return;
      case UIRenderEnums::Gamuts::YUV: {
        pText = UIParams::GamutInfo::YUV;
      }; //YUV
                                     return;
      case UIRenderEnums::Gamuts::YUVHD: {
        pText = UIParams::GamutInfo::YUVHD;
      }; //YUVHD
                                       return;
      case UIRenderEnums::Gamuts::BT709: {
        pText = UIParams::GamutInfo::BT709;
      }; //BT709
                                       return;
      }; //gamutChoice
    }; //GamutInfo
  }; //GamutInfo

  struct FilterInfo : FilterChoice {
    FilterInfo() {
      switch (UIValues::filterLineage) {
      default:
      case UILineageEnums::FilterLineage::NoFilter:
        pText = UIParams::FilterInfo::NoFilter;
        return;
      case UILineageEnums::FilterLineage::ColorDificiency:
        ShowButton();
        goto switch_ColorDificiency;
      }; //switch

    switch_ColorDificiency:
      switch (UIValues::filterChoice) {
      default:
      case UIRenderEnums::Filter::GRAYSCALE:
        pText = UIParams::FilterInfo::Grayscale;
        return;
      case UIRenderEnums::Filter::DEUTERANOPIA:
        pText = UIParams::FilterInfo::Deuteranopia;
        return;
      case UIRenderEnums::Filter::PROTENOPIA:
        pText = UIParams::FilterInfo::Protenopia;
        return;
      case UIRenderEnums::Filter::TRITANOPIA:
        pText = UIParams::FilterInfo::Tritanopia;
        return;

      }; //switch
    }; //FilterInfo
  }; //FilterInfo

  struct ShadingInfo : ShadingChoice {
    ShadingInfo() {
      switch (UIValues::shadingLineage) {
      default:
      case UILineageEnums::ShadingLineage::NoShading:
        pText = UIParams::ShadingInfo::NoShading;
        return;
      case UILineageEnums::ShadingLineage::NPR:
        ShowButton();
        goto switch_NPR;
        return;
      case UILineageEnums::ShadingLineage::Traditional:
        ShowButton();
        goto switch_Traditional;
        return;
      };// switch

    switch_Traditional:
      switch (UIValues::shadingChoice) {
      default:
      case UIRenderEnums::Shading::FLAT:
        pText = UIParams::ShadingInfo::Flat;
        return;
      case UIRenderEnums::Shading::GOURAND:
        pText = UIParams::ShadingInfo::Gourand;
        return;
      case UIRenderEnums::Shading::PHONG:
        pText = UIParams::ShadingInfo::Phong;
        return;
      }; //switch Traditional

    switch_NPR:
      switch (UIValues::shadingChoice) {
      default:
      case UIRenderEnums::Shading::GOOCH:
        pText = UIParams::ShadingInfo::Gooch;
        return;
      case UIRenderEnums::Shading::GOOCHPHONG:
        pText = UIParams::ShadingInfo::GoochPhong;
        return;
      }; //switch NPR


    }; //ShadingChoice
  }; //ShadingInfo

}; //UICommands