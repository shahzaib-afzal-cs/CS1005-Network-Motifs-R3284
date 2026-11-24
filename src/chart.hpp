
#pragma once

#include <string>
#include "nullmodel.hpp"

void write_chart(const std::string& path, const std::string& title,
                 const std::string& nameA, const std::string& nameB,
                 const ZScores& zA, const ZScores& zB);
