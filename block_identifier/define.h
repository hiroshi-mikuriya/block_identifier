#pragma once

/// カメラ横サイズ
const int CAMERA_WIDTH = 1280;
/// カメラ縦サイズ
const int CAMERA_HEIGHT = 720;
/// 画像縮尺率
const double IMAGE_RATIO = 0.5;
/// IMAGE_RATIOをかけたあとのブロックサイズ（高さ）
const int BLOCK_SIZE = static_cast<int>(102 * IMAGE_RATIO);
/// IMAGE_RATIOをかけたあとのブロックサイズ（幅）
const int BLOCK_SIZE_WIDTH = static_cast<int>(150 * IMAGE_RATIO);
