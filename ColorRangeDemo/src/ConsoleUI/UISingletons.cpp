#include <ConsoleUI/UISingletons.h>

//GAMUT INFO
std::string UIParams::GamutInfo::RGB            = "TODO";
std::string UIParams::GamutInfo::CMY            = "TODO";
std::string UIParams::GamutInfo::HSL            = "TODO";
std::string UIParams::GamutInfo::HSV            = "TODO";
std::string UIParams::GamutInfo::HWB            = "TODO";
std::string UIParams::GamutInfo::IHLS           = "TODO";
std::string UIParams::GamutInfo::YIQ            = "TODO";
std::string UIParams::GamutInfo::YDbDr          = "TODO";
std::string UIParams::GamutInfo::YUV            = "Butter";
std::string UIParams::GamutInfo::YUVHD          = "TODO";
std::string UIParams::GamutInfo::BT709          = "TODO";

//SHADING INFO
std::string UIParams::ShadingInfo::NoShading    = "TODO";
std::string UIParams::ShadingInfo::Flat         = "TODO";
std::string UIParams::ShadingInfo::Gourand      = "TODO";
std::string UIParams::ShadingInfo::Phong        = "TODO";
std::string UIParams::ShadingInfo::Gooch        = "TODO";
std::string UIParams::ShadingInfo::GoochPhong   = "TODO";


//FILTER INFO
std::string UIParams::FilterInfo::NoFilter      = "TODO";
std::string UIParams::FilterInfo::Grayscale     = "TODO";
std::string UIParams::FilterInfo::Deuteranopia  = "TODO";
std::string UIParams::FilterInfo::Protenopia    = "TODO";
std::string UIParams::FilterInfo::Tritanopia    = "TODO";


//MENU VECTORS
std::vector<std::string> UIParams::menuChoice = {
  "MainMenu",
  "Render",
  "ColorSelection",
  "ShadingParameters",
  "FilterParameters",
  "Exit"
}; //MenuChoice

//LINEAGE VECTORS
std::vector<std::string> UIParams::gamutLineage = {
  "RGB",
  "CIE",
  "Television",
  "ComputerVision"
}; //GamutLineage
std::vector<std::string> UIParams::shadingLineage = {
  "None",
  "Traditional",
  "Non-Photorealistic [NPR]"
}; //GamutLineage
std::vector<std::string> UIParams::filterLineage = {
  "None",
  "ColorDifficiency"
}; //GamutLineage

//GAMUT VECTORS
std::vector<std::string> UIParams::gamutRGB = {
  "RGB",
  "CMY",
  "HSL [HSI, HSB, etc.]",
  "HSV",
  "HWB"
}; //gamutRGB
std::vector<std::string> UIParams::gamutCIE = {

}; //gamutCIE
std::vector<std::string> UIParams::gamutComputerVision = {
  "IHLS"
}; //ComputerVision
std::vector<std::string> UIParams::gamutTV = {
  "YIQ",
  "YDbDr",
  "YUV",
  "YUVHD",
  "BT709"
}; //gamutTV

//SHADING VECTORS
std::vector<std::string> UIParams::shadingTraditional = {
  "Flat",
  "Gourand",
  "Phong",
  "Blinphong"
}; //ShadingTraditional
std::vector<std::string> UIParams::shadingNPR = {
  "Gooch",
  "Gooch-Phong"
}; //shadingNPR

//FILTER VECTORS
std::vector<std::string> UIParams::filterColorDeficiency = {
  "DEUTERANOPIA",
  "PROTENOPIA",
  "TRITANOPIA",
  "GRAYSCALE"
}; //ColorDeficiency

int UIValues::mainSelection   =       UIParams::MenuSelection::MainMenu;
int UIValues::menuStage       =       UIParams::MenuStage::MainMenuFirst;

int UIValues::gamutChoice     =       UIRenderEnums::Gamuts::RGB;
int UIValues::shadingChoice   =       -1;
int UIValues::filterChoice    =       -1;

int  UIValues::gamutLineage   =       UILineageEnums::RGB;
int  UIValues::shadingLineage =       UILineageEnums::ShadingLineage::NoShading;
int  UIValues::filterLineage  =       UILineageEnums::FilterLineage::NoFilter;

int UIValues::tempColorVec[3] = {NULL, NULL, NULL};
int UIValues::permColorVec[3] = {NULL, NULL, NULL};
//std::array<float, 3>  UIValues::colorVec  = NULL;
//std::array<float, 3>  UIValues::colorVec  = NULL; 