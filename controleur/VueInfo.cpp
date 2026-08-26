#include "VueInfo.h"
#include "ScenePlateau.h"

VueInfo::VueInfo(ScenePlateau* scene) : plateau(scene) {}

void VueInfo::add_info(const std::string& info) {
    if (plateau != nullptr) plateau->journal(info);
}
