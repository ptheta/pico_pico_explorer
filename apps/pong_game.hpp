#pragma once

#include "screen.hpp"
#include "renderer.hpp"
#include "button.hpp"
#include "pico/critical_section.h"

class PongGame : public Screen {
public:
    PongGame();

    void     render(Renderer& r)  const override;
    void     animate()                  override;
    uint32_t animation_hz()       const override { return 30; }
    void     reset()                    override;

    void handle_buttons(const Buttons& btns);

    void move_left(int dir);   // -1 = up, +1 = down
    void move_right(int dir);

    bool is_finished() const;

private:
    void reset_ball();
    void check_winner();

    mutable critical_section_t cs_;

    int left_y_;
    int right_y_;
    int ball_x_, ball_y_;
    int ball_dx_, ball_dy_;
    int score_left_, score_right_;
    int winner_       = 0;   // 0 = playing, 1 = left won, 2 = right won
    int finish_timer_ = 0;   // counts down at 30 Hz after a win

    static constexpr int PADDLE_W      = 10;
    static constexpr int PADDLE_H      = 50;
    static constexpr int PADDLE_SPEED  = 6;
    static constexpr int LEFT_X        = 5;
    static constexpr int RIGHT_X       = 225;
    static constexpr int BALL_R        = 5;
    static constexpr int BALL_SPEED_X  = 4;
    static constexpr int BALL_SPEED_Y  = 3;
    static constexpr int WIN_SCORE     = 15;
    static constexpr int FINISH_FRAMES = 90;  // 3 s victory screen at 30 Hz
};
