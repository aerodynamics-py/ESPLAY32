#pragma once

#include <Arduino.h>
#include "../core/Game.h"
#include "../core/Types.h"

// =========================================================
// CIRCUIT PATH - Solo logic puzzle
// =========================================================
//
// Procedural version.
// The player chooses:
// - grid size
// - network length
//
// The generator now builds a real valid path first,
// then adds coherent decoy branches.
// =========================================================

class CircuitPathGame : public Game {
public:
  const char* getName() const override;
  const char* getSubtitle() const override;
  uint16_t getAccentColor() const override;

  void begin(lgfx::LGFX_Device& lcd, SoundManager& sound) override;
  void update() override;
  void handleTouch(int x, int y) override;

  bool wantsHome() const override;
  void clearHomeRequest() override;

private:
  enum Phase {
    PHASE_CONFIG,
    PHASE_PLAYING,
    PHASE_LEVEL_COMPLETE
  };

  enum TileType {
    TILE_EMPTY = 0,
    TILE_END,
    TILE_STRAIGHT,
    TILE_ELBOW,
    TILE_TEE,
    TILE_CROSS,
    TILE_SOURCE,
    TILE_LAMP
  };

  enum Direction {
    DIR_N = 0,
    DIR_E = 1,
    DIR_S = 2,
    DIR_W = 3
  };

  enum LengthMode {
    LENGTH_SHORT = 0,
    LENGTH_MEDIUM = 1,
    LENGTH_LONG = 2
  };

  lgfx::LGFX_Device* _lcd = nullptr;
  SoundManager* _sound = nullptr;

  bool _wantsHome = false;

  Phase _phase = PHASE_CONFIG;

  static const int MAX_COLS = 6;
  static const int MAX_ROWS = 5;
  static const int MAX_CELLS = MAX_COLS * MAX_ROWS;

  int _cols = 5;
  int _rows = 5;
  int _cellCount = 25;

  int _selectedGridOption = 1;     // 0=4x4, 1=5x5, 2=6x5
  int _selectedLengthMode = LENGTH_MEDIUM;

  uint8_t _type[MAX_CELLS];
  uint8_t _rotation[MAX_CELLS];

  uint8_t _initialType[MAX_CELLS];
  uint8_t _initialRotation[MAX_CELLS];

  bool _connections[MAX_CELLS][4];
  bool _visited[MAX_CELLS];
  bool _onSolutionPath[MAX_CELLS];

  int _path[MAX_CELLS];
  int _pathLength = 0;
  int _minPathLength = 10;
  int _dfsBudget = 0;

  int _moves = 0;
  int _sourceIndex = -1;
  int _lampIndex = -1;
  bool _connected = false;

  int _cellSize = 58;
  int _cellGap = 8;
  int _gridX = 0;
  int _gridY = 96;
  int _gridW = 0;
  int _gridH = 0;

  static const Rect BACK_BUTTON;
  static const Rect RESET_BUTTON;

  static const Rect GRID_4_BUTTON;
  static const Rect GRID_5_BUTTON;
  static const Rect GRID_6_BUTTON;

  static const Rect SHORT_BUTTON;
  static const Rect MEDIUM_BUTTON;
  static const Rect LONG_BUTTON;

  static const Rect START_BUTTON;

  static const Rect NEW_BUTTON;
  static const Rect CONFIG_BUTTON;
  static const Rect HOME_BUTTON;

  // Configuration
  void drawConfigScreen();
  void drawConfigCard(const Rect& r, const char* title, const char* subtitle, bool selected);
  void applySelectedGrid();

  // Generation
  void generateNewPuzzle();
  void clearAllGenerationData();
  void clearTilesAndConnections();
  void computeMinPathLength();

  bool generateSolutionPath();
  bool attemptGeneratePath();
  bool dfsBuildPath(int current, int length);
  void buildSnakeFallbackPath();

  void buildConnectionsFromPath();
  void addDecoyNetworks();
  bool addDecoyWalk(int startIndex, int maxSteps);
  int findRandomEmptyCell() const;
  int pickRandomOccupiedCell(bool allowEndpoints) const;
  int countOccupiedCells() const;
  int targetOccupiedCells() const;

  void convertConnectionsToTiles();
  void setTileFromConnections(int index);
  void scrambleRotations();
  void saveInitialBoard();
  void resetGeneratedPuzzle();

  void addConnectionBetween(int a, int b);
  bool hasAnyConnection(int index) const;
  int directionBetween(int fromIndex, int toIndex) const;
  void shuffleDirections(int dirs[4]);

  // Drawing
  void drawScreen();
  void drawHeader();
  void drawGrid();
  void drawTile(int index);
  void drawFooter(const char* text);
  void drawLevelCompleteScreen();

  Rect getTileRect(int index) const;
  int getTileAt(int x, int y) const;
  void computeGridLayout();

  // Interaction
  bool isRotatable(int index) const;
  void rotateTile(int index);

  // Connection logic
  bool checkConnected();
  bool hasConnection(uint8_t type, uint8_t rotation, int dir) const;
  int neighborIndex(int index, int dir) const;
  int oppositeDir(int dir) const;

  // Drawing helpers
  void drawWireSegment(const Rect& r, int dir, uint16_t color);
  void drawSourceIcon(const Rect& r);
  void drawLampIcon(const Rect& r, bool lit);

  // Helpers
  void changePhase(Phase phase);
};