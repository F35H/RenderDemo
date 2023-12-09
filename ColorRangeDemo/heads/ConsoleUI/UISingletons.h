#pragma once

#include <vector>
#include <string>

static struct UILineageEnums {
  enum GamutLineage : unsigned char {
    RGB = 0,
    CIE,
    Television,
    ComputerVision
  }; //GamutLineage

  enum FilterLineage : unsigned char {
    NoFilter = 0,
    ColorDificiency
  }; //FilterLineage

  enum ShadingLineage : unsigned char {
    NoShading = 0,
    Traditional,
    NPR
  }; //ShadingLanguage
}; //Lineage

static struct UIRenderEnums {
  static struct Gamuts {
    enum RGBLineage : unsigned char {
      RGB = 0,
      CMY,
      HSL,
      HSV,
      HWB,
    }; //RGBLineage

    enum CVLineage : unsigned char {
      IHLS
    }; //CVLineage

    enum TVLineage : unsigned char {
      YIQ = 0,
      YDbDr,
      YUV,
      YUVHD,
      BT709
    }; //TVLineage
  }; //Gamuts

  static struct Shading {
    enum Traditional : unsigned char {
      FLAT = 0,
      GOURAND,
      PHONG,
      BLINPHONG,
    }; //Traditional

    enum NPR : unsigned char {
      GOOCH = 0,
      GOOCHPHONG,
      TOON
    }; //NPR
  }; //Shading

  static struct Filter {
    enum ColorDefficiency : unsigned char {
      DEUTERANOPIA,
      PROTENOPIA,
      TRITANOPIA,
      GRAYSCALE
    }; //ColorDefficiency
  }; //Filter
}; //RenderParams


static struct UIParams {
  enum MenuSelection : unsigned char {
    MainMenu = 0,
    Render,
    GamutParameters,
    ShadingParameters,
    FilterParameters,
    Exit
  }; //MenuSelection

  enum MenuStage : unsigned char {
    MainMenuFirst,
    MainMenuSecond,
    MainMenuThird,
    Configure
  }; //MenuStage


  static struct GamutInfo {
    static std::string RGB;
    static std::string CMY;
    static std::string HSL;
    static std::string HSV;
    static std::string HWB;
    static std::string IHLS;
    static std::string YIQ;
    static std::string YDbDr;
    static std::string YUV;
    static std::string YUVHD;
    static std::string BT709;
  }; //GamutInfo

  static struct FilterInfo {
    static std::string NoFilter;
    static std::string Grayscale;
    static std::string Deuteranopia;
    static std::string Protenopia;
    static std::string Tritanopia;
  }; //FilterInfo

  static struct ShadingInfo {
    static std::string NoShading;
    static std::string Flat;
    static std::string Gourand;
    static std::string Phong;
    static std::string Blinphong;
    static std::string Gooch;
    static std::string GoochPhong;
    static std::string Toon;
  }; //FilterInfo

  //MENU
  static std::vector<std::string> menuChoice;

  //LINEAGE
  static std::vector<std::string> gamutLineage;
  static std::vector<std::string> filterLineage;
  static std::vector<std::string> shadingLineage;

  //GAMUTS
  static std::vector<std::string> gamutRGB;
  static std::vector<std::string> gamutCIE;
  static std::vector<std::string> gamutComputerVision;
  static std::vector<std::string> gamutTV;
  
  //SHADING
  static std::vector<std::string> shadingTraditional;
  static std::vector<std::string> shadingNPR;

  //FILTER
  static std::vector<std::string> filterColorDeficiency;

}; //UIParams

static struct UIValues {
  static int mainSelection;
  static int menuStage;
  
  static int gamutChoice;
  static int shadingChoice;
  static int filterChoice;

  static int gamutLineage;
  static int shadingLineage;
  static int filterLineage;

  static int tempColorVec[3];
  static int permColorVec[3];
}; //UIValues