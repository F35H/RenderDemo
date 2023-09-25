#pragma once

#include "UICommands.h"

using namespace ftxui;

namespace UICommands {
  struct Configure : UICommandInterface {
    Configure() {
    }; //MainMenuCtor
  }; //MainMenu


  struct FilterConfigure : Configure {
    FilterConfigure() {
      switch (UIValues::filterLineage) {
      default:
      case UILineageEnums::FilterLineage::NoFilter:
        return;
      case UILineageEnums::FilterLineage::ColorDificiency:
        goto switch_ColorDificiency;
      }; //switch

    switch_ColorDificiency:
      switch (UIValues::filterChoice) {
      default:
      case UIRenderEnums::Filter::GRAYSCALE:
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
    }; //FilterConfigure
  }; //FilterConfigure

  struct GamutConfigure : Configure {
    GamutConfigure() {
      auto backBtn = Button("Go Back", [&]() {
        UIValues::menuStage = UIParams::MenuStage::MainMenuFirst; 
        UIValues::tempColorVec[0] = UIValues::permColorVec[0];
        UIValues::tempColorVec[1] = UIValues::permColorVec[1];
        UIValues::tempColorVec[2] = UIValues::permColorVec[2];
        return screen.Exit(); 
        }, ButtonOption::Simple());
      auto noneBtn = Button("No Color", [&]() {
        UIValues::menuStage = UIParams::MenuStage::MainMenuFirst; 
        UIValues::permColorVec[0] = NULL;
        UIValues::permColorVec[1] = NULL;
        UIValues::permColorVec[2] = NULL;

        UIValues::tempColorVec[0] = UIValues::permColorVec[0];
        UIValues::tempColorVec[1] = UIValues::permColorVec[1];
        UIValues::tempColorVec[2] = UIValues::permColorVec[2];
        return screen.Exit(); 
        }, ButtonOption::Simple());
      auto colorBtn = Button("Set Color", [&]() {
        UIValues::menuStage = UIParams::MenuStage::MainMenuFirst; 
        UIValues::permColorVec[0] = UIValues::tempColorVec[0];
        UIValues::permColorVec[1] = UIValues::tempColorVec[1];
        UIValues::permColorVec[2] = UIValues::tempColorVec[2];
        return screen.Exit(); 
        }, ButtonOption::Simple());
      auto renderBtn = Button("Render", [&]() {
        UIValues::menuStage = UIParams::MenuStage::MainMenuFirst;
        UIValues::mainSelection = UIParams::MenuSelection::Render;
        UIValues::permColorVec[0] = UIValues::tempColorVec[0];
        UIValues::permColorVec[1] = UIValues::tempColorVec[1];
        UIValues::permColorVec[2] = UIValues::tempColorVec[2];
        return screen.Exit(); 
        }, ButtonOption::Simple());

      hBox.emplace_back(backBtn);
      hBox.emplace_back(noneBtn);
      hBox.emplace_back(colorBtn);
      hBox.emplace_back(renderBtn);


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
        auto slider = Slider("Red", &UIValues::tempColorVec[0], 0, 255, 1);
        vBox.emplace_back(slider);
        auto sliderGreen = Slider("Green", &UIValues::tempColorVec[1], 0, 255, 1);
        vBox.emplace_back(sliderGreen);
        auto sliderBlue = Slider("Blue", &UIValues::tempColorVec[2], 0, 255, 1);
        vBox.emplace_back(sliderBlue);
      }; //RGB
      return;
      case UIRenderEnums::Gamuts::CMY: {
        auto slider = Slider("Cyan", &UIValues::tempColorVec[0], 0, 255, 1);
        vBox.emplace_back(slider);
        slider = Slider("Magenta", &UIValues::tempColorVec[1], 0, 255, 1);
        vBox.emplace_back(slider);
        slider = Slider("Yellow", &UIValues::tempColorVec[2], 0, 255, 1);
        vBox.emplace_back(slider);
      }; //CMY
      return;
      case UIRenderEnums::Gamuts::HSL: {
        auto slider = Slider("Hue", &UIValues::tempColorVec[0], 0, 360, 1);
        vBox.emplace_back(slider);
        slider = Slider("Saturation", &UIValues::tempColorVec[1], 0, 255, 1);
        vBox.emplace_back(slider);
        slider = Slider("Light", &UIValues::tempColorVec[2], 0, 255, 1);
        vBox.emplace_back(slider);
      }; //HSL
      return;
      case UIRenderEnums::Gamuts::HSV: {
        auto slider = Slider("Hue", &UIValues::tempColorVec[0], 0, 360, 1);
        vBox.emplace_back(slider);
        slider = Slider("Saturation", &UIValues::tempColorVec[1], 0, 255, 1);
        vBox.emplace_back(slider);
        slider = Slider("Value", &UIValues::tempColorVec[2], 0, 255, 1);
        vBox.emplace_back(slider);
      }; //HSV
      return;
      case UIRenderEnums::Gamuts::HWB: {
        auto slider = Slider("Hue", &UIValues::tempColorVec[0], 0, 360, 1);
        vBox.emplace_back(slider);
        slider = Slider("White", &UIValues::tempColorVec[1], 0, 1, 1);
        vBox.emplace_back(slider);
        slider = Slider("Black", &UIValues::tempColorVec[2], 0, 1, 1);
        vBox.emplace_back(slider);
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

    }; //GamutConfigure
  }; //GamutConfigure

 struct ShadingConfigure : Configure {
    ShadingConfigure() {
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
    }; //ShadingParameters
  }; //GamutParameters
}; //UICommandss