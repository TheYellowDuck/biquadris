// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
// Required Notice: Copyright (c) 2026 George Zhang and the original Biquadris team (CS 246, University of Waterloo) — https://github.com/TheYellowDuck

// effect.cc
#include "effect.h"
#include "board.h"
#include <algorithm>

using namespace std;

// BlindEffect implementation
void BlindEffect::apply(Board& board) {
    board.setBlind(true);
}

void BlindEffect::onDrop(Board& board) {
    board.setBlind(false);
    expired = true;
}

bool BlindEffect::isExpired() const {
    return expired;
}

// HeavyEffect implementation
void HeavyEffect::apply(Board& board) {
    board.addHeavy();
}

void HeavyEffect::onDrop(Board& board) {
    // Heavy decrements itself in Board::drop()
    // Check if it's expired
    expired = true;
}

bool HeavyEffect::isExpired() const {
    return expired;
}

// ForceEffect implementation
void ForceEffect::apply(Board& board) {
    board.forceBlock(blockType);
    expired = true;  // Force is immediate
}

void ForceEffect::onDrop(Board& board) {
    // Force effect doesn't persist
}

bool ForceEffect::isExpired() const {
    return expired;
}

// EffectManager implementation
void EffectManager::addEffect(unique_ptr<Effect> effect, Board& board) {
    effect->apply(board);
    activeEffects.push_back(std::move(effect));
}

void EffectManager::onDrop(Board& board) {
    // Notify all effects of drop
    for (auto& effect : activeEffects) {
        effect->onDrop(board);
    }

    // Remove expired effects
    activeEffects.erase(
        remove_if(activeEffects.begin(), activeEffects.end(),
            [](const unique_ptr<Effect>& e) { return e->isExpired(); }),
        activeEffects.end()
    );
}

void EffectManager::clear() {
    activeEffects.clear();
}

vector<string> EffectManager::getActiveEffectNames() const {
    vector<string> names;
    for (const auto& effect : activeEffects) {
        if (!effect->isExpired()) {
            names.push_back(effect->getName());
        }
    }
    return names;
}
