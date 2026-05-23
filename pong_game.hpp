#pragma once

#include "screen.hpp"
#include "renderer.hpp"

class PongGame : public Screen {
public:
    PongGame();

    void     render(Renderer& r)  const override;
    void     animate()                  override;
    uint32_t animation_hz()       const override { return 30; }
    void     reset()                    override;

    void move_left(int dir);   // -1 = up, +1 = down
    void move_right(int dir);

private:
    void reset_ball();

    int left_y_;
    int right_y_;
    int ball_x_, ball_y_;
    int ball_dx_, ball_dy_;
    int score_left_, score_right_;

    static constexpr int PADDLE_W     = 10;
    static constexpr int PADDLE_H     = 50;
    static constexpr int PADDLE_SPEED = 6;
    static constexpr int LEFT_X       = 5;
    static constexpr int RIGHT_X      = 225;  // SCREEN_W - PADDLE_W - 5
    static constexpr int BALL_R       = 5;
    static constexpr int BALL_SPEED_X = 4;
    static constexpr int BALL_SPEED_Y = 3;
};
