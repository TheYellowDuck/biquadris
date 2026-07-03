// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
// Required Notice: Copyright (c) 2026 George Zhang and the original Biquadris team (CS 246, University of Waterloo) — https://github.com/TheYellowDuck

// effect.h
#ifndef EFFECT_H
#define EFFECT_H

#include <memory>
#include <vector>
#include <string>

// Forward declaration
class Board;

// Base effect class
class Effect {
public:
    virtual ~Effect() = default;

    // Apply the effect when first activated
    virtual void apply(Board& board) = 0;

    // Called after each drop to update effect state
    virtual void onDrop(Board& board) = 0;

    // Check if effect has expired and should be removed
    virtual bool isExpired() const = 0;

    // Get effect name for display
    virtual std::string getName() const = 0;
};

// Blind effect - covers board with question marks
class BlindEffect : public Effect {
public:
    BlindEffect() = default;

    void apply(Board& board) override;
    void onDrop(Board& board) override;
    bool isExpired() const override;
    std::string getName() const override { return "Blind"; }

private:
    bool expired = false;
};

// Heavy effect - blocks drop extra rows after movement
class HeavyEffect : public Effect {
public:
    HeavyEffect() = default;

    void apply(Board& board) override;
    void onDrop(Board& board) override;
    bool isExpired() const override;
    std::string getName() const override { return "Heavy"; }

private:
    bool expired = false;
};

// Force effect - changes next block (immediate effect, no persistence)
class ForceEffect : public Effect {
public:
    ForceEffect(char blockType) : blockType(blockType) {}

    void apply(Board& board) override;
    void onDrop(Board& board) override;
    bool isExpired() const override;
    std::string getName() const override { return "Force"; }

private:
    char blockType;
    bool expired = false;
};

// Effect manager for a board
class EffectManager {
    std::vector<std::unique_ptr<Effect>> activeEffects;

public:
    void addEffect(std::unique_ptr<Effect> effect, Board& board);
    void onDrop(Board& board);
    void clear();
    std::vector<std::string> getActiveEffectNames() const;
};

#endif
