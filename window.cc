// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
// Required Notice: Copyright (c) 2026 George Zhang and the original Biquadris team (CS 246, University of Waterloo) — https://github.com/TheYellowDuck

// window.cc  —  SDL2 graphical display
#include "window.h"
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;

// ─── Public colour enum (kept for drawString/fillRectangle API) ──────────────
static const SDL_Color PALETTE[] = {
    {220, 225, 240, 255},  // 0 White  → near-white text
    { 15,  15,  28, 255},  // 1 Black  → dark bg
    {230,  55,  55, 255},  // 2 Red
    { 50, 210,  70, 255},  // 3 Green
    { 60, 130, 230, 255},  // 4 Blue
    {  0, 230, 230, 255},  // 5 Cyan
    {230, 220,  30, 255},  // 6 Yellow
    {170,  40, 220, 255},  // 7 Magenta
    {230, 150,  20, 255},  // 8 Orange
    {140, 140, 160, 255},  // 9 Grey
};

// ─── Internal theme colours ──────────────────────────────────────────────────
static constexpr SDL_Color C_BG     = { 15,  15,  28, 255};
static constexpr SDL_Color C_PANEL  = { 24,  24,  44, 255};
static constexpr SDL_Color C_GRID   = { 38,  38,  62, 255};
static constexpr SDL_Color C_BORDER = { 65,  65, 100, 255};
static constexpr SDL_Color C_TEXT   = {220, 225, 240, 255};
static constexpr SDL_Color C_DIM    = {110, 115, 145, 255};
static constexpr SDL_Color C_P1     = { 80, 160, 255, 255};
static constexpr SDL_Color C_P2     = {255,  90,  90, 255};
static constexpr SDL_Color C_GOLD   = {255, 215,   0, 255};
static constexpr SDL_Color C_FX     = {255, 190,  50, 255};

// ─── Block colours ───────────────────────────────────────────────────────────
static SDL_Color blockSDLColor(char type) {
    switch (type) {
        case 'I': return {  0, 230, 230, 255};
        case 'J': return { 40, 100, 255, 255};
        case 'L': return {255, 160,   0, 255};
        case 'O': return {240, 220,   0, 255};
        case 'S': return { 50, 230,  80, 255};
        case 'Z': return {230,  50,  50, 255};
        case 'T': return {180,  50, 240, 255};
        case '*': return {140, 140, 160, 255};
        default:  return { 80,  80, 100, 255};
    }
}

// ─── Layout constants ────────────────────────────────────────────────────────
static constexpr int CS   = 24;          // cell size (px)
static constexpr int BW   = 11 * CS;    // board pixel width  (264)
static constexpr int BH   = 18 * CS;    // board pixel height (432)
static constexpr int P1X  = 30;
static constexpr int P2X  = P1X + BW + 80;
static constexpr int BY   = 76;         // board top y

// ─── Render helpers ──────────────────────────────────────────────────────────
static void setC(SDL_Renderer* r, SDL_Color c) {
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
}
static void fillR(SDL_Renderer* r, int x, int y, int w, int h, SDL_Color c) {
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    SDL_Rect rect{x, y, w, h};
    SDL_RenderFillRect(r, &rect);
}
static void drawR(SDL_Renderer* r, int x, int y, int w, int h, SDL_Color c) {
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    SDL_Rect rect{x, y, w, h};
    SDL_RenderDrawRect(r, &rect);
}
static SDL_Color lighten(SDL_Color c, int a) {
    return {(Uint8)min(255,(int)c.r+a),(Uint8)min(255,(int)c.g+a),(Uint8)min(255,(int)c.b+a),c.a};
}
static SDL_Color darken(SDL_Color c, int d) {
    return {(Uint8)max(0,(int)c.r-d),(Uint8)max(0,(int)c.g-d),(Uint8)max(0,(int)c.b-d),c.a};
}

// Draw a single Tetris cell with highlight + shadow bevel
static void drawCell(SDL_Renderer* rdr, int px, int py, SDL_Color col) {
    fillR(rdr, px+1, py+1, CS-2, CS-2, col);
    SDL_Color hi = lighten(col, 80);
    fillR(rdr, px+1, py+1, CS-2, 3, hi);
    fillR(rdr, px+1, py+1, 3, CS-2, hi);
    SDL_Color sh = darken(col, 80);
    fillR(rdr, px+1, py+CS-4, CS-2, 3, sh);
    fillR(rdr, px+CS-4, py+1, 3, CS-2, sh);
}

// Draw a ghost (landing preview) cell — outline only with low-alpha fill
static void drawGhost(SDL_Renderer* rdr, int px, int py, SDL_Color col) {
    SDL_SetRenderDrawBlendMode(rdr, SDL_BLENDMODE_BLEND);
    SDL_Color fill = {col.r, col.g, col.b, 45};
    fillR(rdr, px+1, py+1, CS-2, CS-2, fill);
    SDL_Color border = {col.r, col.g, col.b, 140};
    SDL_SetRenderDrawColor(rdr, border.r, border.g, border.b, border.a);
    SDL_Rect r{px+1, py+1, CS-2, CS-2};
    SDL_RenderDrawRect(rdr, &r);
    SDL_SetRenderDrawBlendMode(rdr, SDL_BLENDMODE_NONE);
}

// ─── Font loading ────────────────────────────────────────────────────────────
static TTF_Font* loadSystemFont(int size) {
    const char* paths[] = {
        "/System/Library/Fonts/Helvetica.ttc",
        "/Library/Fonts/Arial.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
        "C:\\Windows\\Fonts\\arial.ttf",
        nullptr
    };
    for (int i = 0; paths[i]; ++i) {
        TTF_Font* f = TTF_OpenFont(paths[i], size);
        if (f) return f;
    }
    return nullptr;
}

// ─── Constructor / Destructor ────────────────────────────────────────────────
Xwindow::Xwindow(int w, int h) : width(w), height(h) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "SDL_Init failed: " << SDL_GetError() << endl; exit(1);
    }
    if (TTF_Init() < 0) {
        cerr << "TTF_Init failed: " << TTF_GetError() << endl; SDL_Quit(); exit(1);
    }
    sdlWindow = SDL_CreateWindow("Biquadris",
                                 SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                 w, h, SDL_WINDOW_SHOWN);
    if (!sdlWindow) {
        cerr << "SDL_CreateWindow failed: " << SDL_GetError() << endl;
        TTF_Quit(); SDL_Quit(); exit(1);
    }
    sdlRenderer = SDL_CreateRenderer(sdlWindow, -1,
                                     SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!sdlRenderer) sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, 0);
    if (!sdlRenderer) {
        cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << endl;
        SDL_DestroyWindow(sdlWindow); TTF_Quit(); SDL_Quit(); exit(1);
    }
    font      = loadSystemFont(13);
    fontLarge = loadSystemFont(22);
    if (!font) cerr << "Warning: could not load a system font.\n";
}

Xwindow::~Xwindow() {
    if (fontLarge)   TTF_CloseFont(fontLarge);
    if (font)        TTF_CloseFont(font);
    if (sdlRenderer) SDL_DestroyRenderer(sdlRenderer);
    if (sdlWindow)   SDL_DestroyWindow(sdlWindow);
    TTF_Quit(); SDL_Quit();
}

// ─── Public drawing primitives (legacy API) ──────────────────────────────────
void Xwindow::setColor(int color) {
    const SDL_Color& c = PALETTE[color];
    SDL_SetRenderDrawColor(sdlRenderer, c.r, c.g, c.b, c.a);
}

void Xwindow::fillRectangle(int x, int y, int w, int h, int color) {
    setColor(color);
    SDL_Rect r{x, y, w, h};
    SDL_RenderFillRect(sdlRenderer, &r);
}

void Xwindow::renderText(int x, int y, const string& msg, SDL_Color color, TTF_Font* f) {
    if (!f || msg.empty()) return;
    SDL_Surface* surf = TTF_RenderText_Blended(f, msg.c_str(), color);
    if (!surf) return;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(sdlRenderer, surf);
    if (tex) {
        SDL_Rect dst{x, y, surf->w, surf->h};
        SDL_RenderCopy(sdlRenderer, tex, nullptr, &dst);
        SDL_DestroyTexture(tex);
    }
    SDL_FreeSurface(surf);
}

void Xwindow::drawString(int x, int y, const string& msg, int color) {
    renderText(x, y, msg, PALETTE[color], font);
}

void Xwindow::clear() {
    setC(sdlRenderer, C_BG);
    SDL_RenderClear(sdlRenderer);
}

int Xwindow::blockColor(char type) const {
    switch (type) {
        case 'I': return Cyan;    case 'J': return Blue;
        case 'L': return Orange;  case 'O': return Yellow;
        case 'S': return Green;   case 'Z': return Red;
        case 'T': return Magenta; case '*': return Grey;
        default:  return Black;
    }
}

// ─── Board rendering ─────────────────────────────────────────────────────────
void Xwindow::drawBoard(const Board& board, int ox, int oy, SDL_Color accent) {
    // Panel behind the board
    fillR(sdlRenderer, ox-4, oy-4, BW+8, BH+8, C_PANEL);
    // Double border: accent outer, dimmer inner
    drawR(sdlRenderer, ox-4, oy-4, BW+8, BH+8, accent);
    drawR(sdlRenderer, ox-3, oy-3, BW+6, BH+6, darken(accent, 70));

    const Block* cur    = board.getCurrentBlock();
    int ghostY          = cur ? board.getGhostY() : -1;

    for (int row = 0; row < 18; ++row) {
        for (int col = 0; col < 11; ++col) {
            int px = ox + col * CS;
            int py = oy + row * CS;

            // Subtle grid lines
            drawR(sdlRenderer, px, py, CS, CS, C_GRID);

            char ch = board.getCell(col, row);

            // Determine whether this cell holds the active or ghost block
            bool isCur   = false;
            bool isGhost = false;
            char btype   = ' ';
            if (cur) {
                const auto& shape = cur->getShape();
                int bx = cur->getX(), by = cur->getY();
                for (int i = 0; i < (int)shape.size(); ++i)
                    for (int j = 0; j < (int)shape[i].size(); ++j)
                        if (shape[i][j] && by+i == row && bx+j == col) {
                            isCur = true; btype = cur->getType();
                        }
                // Ghost (only in empty cells and when it's below current position)
                if (!isCur && ghostY > cur->getY()) {
                    for (int i = 0; i < (int)shape.size(); ++i)
                        for (int j = 0; j < (int)shape[i].size(); ++j)
                            if (shape[i][j] && ghostY+i == row && cur->getX()+j == col)
                                isGhost = true, btype = cur->getType();
                }
            }

            if (ch == '?') {
                // Blind: show a dark question-mark tile
                fillR(sdlRenderer, px+1, py+1, CS-2, CS-2, darken(C_PANEL, 5));
                SDL_Color qc = {55, 55, 85, 255};
                fillR(sdlRenderer, px+5, py+5, CS-10, CS-10, qc);
            } else if (isCur) {
                drawCell(sdlRenderer, px, py, blockSDLColor(btype));
            } else if (isGhost && ch == ' ') {
                drawGhost(sdlRenderer, px, py, blockSDLColor(btype));
            } else if (ch != ' ') {
                drawCell(sdlRenderer, px, py, blockSDLColor(ch));
            }
        }
    }
}

// Draw the game frame to the back buffer without presenting (used by overlays)
void Xwindow::drawBaseFrame() {
    if (!lastP1 || !lastP2) return;
    clear();

    TTF_Font* big = fontLarge ? fontLarge : font;

    // Title
    renderText(width/2 - 68, 10, "BIQUADRIS", C_TEXT, big);

    // Separator
    setC(sdlRenderer, C_BORDER);
    SDL_RenderDrawLine(sdlRenderer, P1X, BY-6, width-P1X, BY-6);

    // Player 1
    renderText(P1X + BW/2 - 36, BY - 38, "PLAYER 1", C_P1, font);
    { stringstream ss; ss << "LVL " << lastP1->getLevel()
                          << "   SCORE " << lastP1->getScore();
      renderText(P1X, BY-20, ss.str(), C_DIM, font); }
    drawBoard(*lastP1, P1X, BY, C_P1);

    // Player 2
    renderText(P2X + BW/2 - 36, BY - 38, "PLAYER 2", C_P2, font);
    { stringstream ss; ss << "LVL " << lastP2->getLevel()
                          << "   SCORE " << lastP2->getScore();
      renderText(P2X, BY-20, ss.str(), C_DIM, font); }
    drawBoard(*lastP2, P2X, BY, C_P2);

    // Next previews
    int ny = BY + BH + 12;
    renderText(P1X, ny, "NEXT", C_DIM, font);
    renderText(P2X, ny, "NEXT", C_DIM, font);

    auto drawNextBlock = [&](const Block* b, int ox) {
        if (!b) return;
        SDL_Color col = blockSDLColor(b->getType());
        const auto& shape = b->getShape();
        for (int i = 0; i < (int)shape.size(); ++i)
            for (int j = 0; j < (int)shape[i].size(); ++j)
                if (shape[i][j])
                    drawCell(sdlRenderer, ox + j*CS, ny + 18 + i*CS, col);
    };
    drawNextBlock(lastP1->getNextBlock(), P1X);
    drawNextBlock(lastP2->getNextBlock(), P2X);

    // Active effects
    auto drawFx = [&](const Board& bd, int ox) {
        auto fx = bd.getActiveEffects();
        if (fx.empty()) return;
        string line = "FX: ";
        for (size_t k = 0; k < fx.size(); ++k) { if (k) line += ", "; line += fx[k]; }
        renderText(ox, ny + 72, line, C_FX, font);
    };
    drawFx(*lastP1, P1X);
    drawFx(*lastP2, P2X);

    // Hint bar
    renderText(6, height - 18,
        "Arrows=Move  Space=Drop  Z/X=Rotate  +/-=Level  R=Restart  Q=Quit",
        C_DIM, font);
}

// ─── Full-frame render ───────────────────────────────────────────────────────
void Xwindow::render(const Board& p1, const Board& p2) {
    lastP1 = &p1; lastP2 = &p2;
    drawBaseFrame();
    SDL_RenderPresent(sdlRenderer);
}

// ─── Overlay helpers ─────────────────────────────────────────────────────────
static void drawPopup(SDL_Renderer* rdr, int cx, int cy, int bw, int bh) {
    int bx = cx - bw/2, by = cy - bh/2;
    // Dimming overlay
    SDL_SetRenderDrawBlendMode(rdr, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(rdr, 0, 0, 0, 150);
    SDL_Rect full{0, 0, 10000, 10000};
    SDL_RenderFillRect(rdr, &full);
    SDL_SetRenderDrawBlendMode(rdr, SDL_BLENDMODE_NONE);
    // Panel
    fillR(rdr, bx, by, bw, bh, {22, 22, 44, 255});
    drawR(rdr, bx, by, bw, bh, {80, 80, 130, 255});
    drawR(rdr, bx+2, by+2, bw-4, bh-4, {50, 50, 90, 255});
}

// ─── Game-over screen ────────────────────────────────────────────────────────
void Xwindow::renderGameOver(int loser, int winner,
                             int score1, int score2, int hi) {
    drawBaseFrame();

    int cx = width/2, cy = height/2;
    int bw = 420, bh = 310;
    int bx = cx - bw/2, by = cy - bh/2;
    drawPopup(sdlRenderer, cx, cy, bw, bh);

    TTF_Font* big = fontLarge ? fontLarge : font;
    renderText(cx - 78, by + 28, "GAME  OVER", {230, 55, 55, 255}, big);

    setC(sdlRenderer, C_BORDER);
    SDL_RenderDrawLine(sdlRenderer, bx+20, by+68, bx+bw-20, by+68);

    { stringstream ss; ss << "Player " << loser  << " loses!";
      renderText(cx - 70, by + 82, ss.str(), C_TEXT, font); }
    { stringstream ss; ss << "Player " << winner << " wins!";
      renderText(cx - 70, by + 106, ss.str(), {80, 230, 100, 255}, font); }

    setC(sdlRenderer, C_BORDER);
    SDL_RenderDrawLine(sdlRenderer, bx+20, by+132, bx+bw-20, by+132);

    renderText(cx - 60, by + 150, "Final Scores", C_DIM, font);
    { stringstream ss; ss << "Player 1:     " << score1;
      renderText(cx - 70, by + 174, ss.str(), C_TEXT, font); }
    { stringstream ss; ss << "Player 2:     " << score2;
      renderText(cx - 70, by + 198, ss.str(), C_TEXT, font); }
    { stringstream ss; ss << "High Score:  " << hi;
      renderText(cx - 70, by + 222, ss.str(), C_GOLD, font); }

    setC(sdlRenderer, C_BORDER);
    SDL_RenderDrawLine(sdlRenderer, bx+20, by+250, bx+bw-20, by+250);

    renderText(cx - 130, by + 270, "Press  R  to restart     Q  to quit", C_DIM, font);

    SDL_RenderPresent(sdlRenderer);
}

// ─── Special action prompt ───────────────────────────────────────────────────
void Xwindow::renderSpecialActionPrompt(int playerNum) {
    drawBaseFrame();

    int cx = width/2, cy = height/2;
    int bw = 520, bh = 220;
    int bx = cx - bw/2, by = cy - bh/2;
    drawPopup(sdlRenderer, cx, cy, bw, bh);

    TTF_Font* big = fontLarge ? fontLarge : font;
    { stringstream ss; ss << "Player " << playerNum << "  —  Special Action!";
      renderText(cx - 110, by + 24, ss.str(), C_GOLD, big); }

    setC(sdlRenderer, C_BORDER);
    SDL_RenderDrawLine(sdlRenderer, bx+20, by+60, bx+bw-20, by+60);

    renderText(bx + 30, by + 76,  "1 / B  —  Blind   Cover opponent's center rows", C_TEXT, font);
    renderText(bx + 30, by + 100, "2 / H  —  Heavy   Opponent's blocks drop on each move", C_TEXT, font);
    renderText(bx + 30, by + 124, "3 / F  —  Force   Replace opponent's current block", C_TEXT, font);

    setC(sdlRenderer, C_BORDER);
    SDL_RenderDrawLine(sdlRenderer, bx+20, by+154, bx+bw-20, by+154);

    renderText(cx - 100, by + 170, "Press the number or letter key", C_DIM, font);

    SDL_RenderPresent(sdlRenderer);
}

// ─── Block-selector screen (for force) ───────────────────────────────────────
void Xwindow::renderBlockSelector() {
    drawBaseFrame();

    int cx = width/2, cy = height/2;
    int bw = 580, bh = 360;
    int bx = cx - bw/2, by = cy - bh/2;
    drawPopup(sdlRenderer, cx, cy, bw, bh);

    TTF_Font* big = fontLarge ? fontLarge : font;
    renderText(cx - 120, by + 22, "Force Block — Choose Type", C_GOLD, big);

    setC(sdlRenderer, C_BORDER);
    SDL_RenderDrawLine(sdlRenderer, bx+20, by+60, bx+bw-20, by+60);

    // Draw block previews in two columns
    const struct { char t; const char* label; int col; int row; } items[] = {
        {'I',"1  /  I", 0, 0}, {'O',"4  /  O", 1, 0},
        {'J',"2  /  J", 0, 1}, {'S',"5  /  S", 1, 1},
        {'L',"3  /  L", 0, 2}, {'Z',"6  /  Z", 1, 2},
        {'T',"7  /  T", 0, 3},
    };
    const int col0x = bx + 30, col1x = bx + 310;
    const int rowY0 = by + 76, rowStep = 65;
    const int previewX0 = col0x + 75, previewX1 = col1x + 75;

    // I shape
    static const bool shpI[1][4] = {{1,1,1,1}};
    // J shape
    static const bool shpJ[2][3] = {{1,0,0},{1,1,1}};
    // L shape
    static const bool shpL[2][3] = {{0,0,1},{1,1,1}};
    // O shape
    static const bool shpO[2][2] = {{1,1},{1,1}};
    // S shape
    static const bool shpS[2][3] = {{0,1,1},{1,1,0}};
    // Z shape
    static const bool shpZ[2][3] = {{1,1,0},{0,1,1}};
    // T shape
    static const bool shpT[2][3] = {{1,1,1},{0,1,0}};

    const int ps = 16;  // preview cell size

    auto previewBlock = [&](char t, int ox, int ry) {
        SDL_Color col = blockSDLColor(t);
        auto drawPv = [&](const bool* shp, int rows, int cols) {
            for (int i = 0; i < rows; ++i)
                for (int j = 0; j < cols; ++j)
                    if (shp[i*cols+j]) {
                        fillR(sdlRenderer, ox+j*ps, ry+i*ps, ps-2, ps-2, col);
                        SDL_Color hi = lighten(col, 60);
                        fillR(sdlRenderer, ox+j*ps, ry+i*ps, ps-2, 2, hi);
                        fillR(sdlRenderer, ox+j*ps, ry+i*ps, 2, ps-2, hi);
                    }
        };
        switch (t) {
            case 'I': drawPv((bool*)shpI, 1, 4); break;
            case 'J': drawPv((bool*)shpJ, 2, 3); break;
            case 'L': drawPv((bool*)shpL, 2, 3); break;
            case 'O': drawPv((bool*)shpO, 2, 2); break;
            case 'S': drawPv((bool*)shpS, 2, 3); break;
            case 'Z': drawPv((bool*)shpZ, 2, 3); break;
            case 'T': drawPv((bool*)shpT, 2, 3); break;
            default: break;
        }
    };

    for (auto& it : items) {
        int lx = it.col == 0 ? col0x : col1x;
        int px = it.col == 0 ? previewX0 : previewX1;
        int ry = rowY0 + it.row * rowStep;
        renderText(lx, ry + 6, it.label, C_TEXT, font);
        previewBlock(it.t, px, ry);
    }

    setC(sdlRenderer, C_BORDER);
    SDL_RenderDrawLine(sdlRenderer, bx+20, by+bh-42, bx+bw-20, by+bh-42);
    renderText(cx - 170, by+bh-28, "Press the number key or block letter (I/J/L/O/S/T/Z)", C_DIM, font);

    SDL_RenderPresent(sdlRenderer);
}

// ─── Input ───────────────────────────────────────────────────────────────────
bool Xwindow::checkForInput() {
    return SDL_HasEvents(SDL_FIRSTEVENT, SDL_LASTEVENT) == SDL_TRUE;
}

string Xwindow::getSpecialActionInput() {
    SDL_Event e;
    while (true) {
        SDL_WaitEvent(&e);
        if (e.type == SDL_QUIT) return "blind";
        if (e.type != SDL_KEYDOWN) continue;
        switch (e.key.keysym.sym) {
            case SDLK_1: case SDLK_b: return "blind";
            case SDLK_2: case SDLK_h: return "heavy";
            case SDLK_3: case SDLK_f: return "force";
            default: break;
        }
    }
}

char Xwindow::getBlockTypeInput() {
    SDL_Event e;
    while (true) {
        SDL_WaitEvent(&e);
        if (e.type == SDL_QUIT) return 'I';
        if (e.type != SDL_KEYDOWN) continue;
        switch (e.key.keysym.sym) {
            case SDLK_1: case SDLK_i: return 'I';
            case SDLK_2: case SDLK_j: return 'J';
            case SDLK_3: case SDLK_l: return 'L';
            case SDLK_4: case SDLK_o: return 'O';
            case SDLK_5: case SDLK_s: return 'S';
            case SDLK_6: case SDLK_z: return 'Z';
            case SDLK_7: case SDLK_t: return 'T';
            default: break;
        }
    }
}

string Xwindow::getInput() {
    SDL_Event e;
    while (true) {
        SDL_WaitEvent(&e);

        if (e.type == SDL_QUIT) { inputBuffer.clear(); return "eof"; }
        if (e.type != SDL_KEYDOWN) continue;

        SDL_Keycode key = e.key.keysym.sym;

        auto flush = [&](const char* cmd) -> string {
            string result = inputBuffer + cmd;
            inputBuffer.clear();
            return result;
        };

        switch (key) {
            case SDLK_LEFT:                      return flush("left");
            case SDLK_RIGHT:                     return flush("right");
            case SDLK_DOWN:                      return flush("down");
            case SDLK_UP:                        return flush("clockwise");
            case SDLK_SPACE:                     return flush("drop");
            case SDLK_z:                         return flush("counterclockwise");
            case SDLK_x:                         return flush("clockwise");
            case SDLK_r: inputBuffer.clear();    return "restart";
            case SDLK_q: inputBuffer.clear();    return "eof";
            case SDLK_PLUS:
            case SDLK_EQUALS:
            case SDLK_KP_PLUS:  inputBuffer.clear(); return "levelup";
            case SDLK_MINUS:
            case SDLK_KP_MINUS: inputBuffer.clear(); return "leveldown";

            case SDLK_i: inputBuffer.clear(); return "I";
            case SDLK_j: inputBuffer.clear(); return "J";
            case SDLK_l: inputBuffer.clear(); return "L";
            case SDLK_o: inputBuffer.clear(); return "O";
            case SDLK_s: inputBuffer.clear(); return "S";
            case SDLK_t: inputBuffer.clear(); return "T";

            case SDLK_0: inputBuffer += '0'; break;
            case SDLK_1: inputBuffer += '1'; break;
            case SDLK_2: inputBuffer += '2'; break;
            case SDLK_3: inputBuffer += '3'; break;
            case SDLK_4: inputBuffer += '4'; break;
            case SDLK_5: inputBuffer += '5'; break;
            case SDLK_6: inputBuffer += '6'; break;
            case SDLK_7: inputBuffer += '7'; break;
            case SDLK_8: inputBuffer += '8'; break;
            case SDLK_9: inputBuffer += '9'; break;

            case SDLK_RETURN:
            case SDLK_KP_ENTER:
                if (!inputBuffer.empty()) {
                    string cmd = inputBuffer; inputBuffer.clear(); return cmd;
                }
                break;

            case SDLK_ESCAPE:
                inputBuffer.clear();
                break;

            default:
                if (key >= 32 && key < 127) inputBuffer += static_cast<char>(key);
                break;
        }
    }
}
