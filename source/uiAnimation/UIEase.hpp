/**
 * @file UIEase.hpp
 * @brief Easing mode alias for UI property animations.
 *
 * Maps onto MathManager::EaseMode so curves stay fixed-point and shared.
 * @author Oles Gedz (olesgedz)
 */

#pragma once

#include "managers/MathManager.hpp"

/**
 * @brief Easing curves available to UI tweens.
 */
using UIEase = MathManager::EaseMode;
