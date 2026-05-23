#include "pong_game.hpp"
#include "buzzer.hpp"
#include "screen_dims.hpp"
#include <algorithm>
#include <cstdlib>
#include <string>

PongGame::PongGame() {
    reset();
}

void PongGame::reset() {
    left_y_      = SCREEN_H / 2 - PADDLE_H / 2;
    right_y_     = SCREEN_H / 2 - PADDLE_H / 2;
    score_left_  = 0;
    score_right_ = 0;
    reset_ball();
}

void PongGame::reset_ball() {
    ball_x_  = SCREEN_W / 2;
    ball_y_  = SCREEN_H / 2;
    ball_dx_ = rand() % 2 ? BALL_SPEED_X : -BALL_SPEED_X;
    ball_dy_ = rand() % 2 ? BALL_SPEED_Y : -BALL_SPEED_Y;
}

void PongGame::animate() {
    Buzzer::update();

    ball_x_ += ball_dx_;
    ball_y_ += ball_dy_;

    // Top / bottom walls
    if (ball_y_ - BALL_R <= 0)        { ball_y_ = BALL_R;            ball_dy_ = -ball_dy_; Buzzer::play(440, 50); }
    if (ball_y_ + BALL_R >= SCREEN_H) { ball_y_ = SCREEN_H - BALL_R; ball_dy_ = -ball_dy_; Buzzer::play(440, 50); }

    // Left paddle collision
    if (ball_dx_ < 0 &&
        ball_x_ - BALL_R <= LEFT_X + PADDLE_W &&
        ball_y_ + BALL_R >= left_y_ &&
        ball_y_ - BALL_R <= left_y_ + PADDLE_H) {
        ball_x_  = LEFT_X + PADDLE_W + BALL_R;
        ball_dx_ = -ball_dx_;
        Buzzer::play(880, 60);
    }

    // Right paddle collision
    if (ball_dx_ > 0 &&
        ball_x_ + BALL_R >= RIGHT_X &&
        ball_y_ + BALL_R >= right_y_ &&
        ball_y_ - BALL_R <= right_y_ + PADDLE_H) {
        ball_x_  = RIGHT_X - BALL_R;
        ball_dx_ = -ball_dx_;
        Buzzer::play(880, 60);
    }

    // Scoring
    if (ball_x_ - BALL_R <= 0)        { ++score_right_; reset_ball(); Buzzer::play(220, 300); }
    if (ball_x_ + BALL_R >= SCREEN_W) { ++score_left_;  reset_ball(); Buzzer::play(220, 300); }
}

void PongGame::render(Renderer& r) const {
    r.set_pen({0, 0, 0});
    r.clear();

    // Dashed centre line
    r.set_pen({60, 60, 60});
    for (int y = 4; y < SCREEN_H; y += 16)
        r.fill_rect(119, y, 2, 8);

    // Paddles
    r.set_pen({255, 255, 255});
    r.fill_rect(LEFT_X,  left_y_,  PADDLE_W, PADDLE_H);
    r.fill_rect(RIGHT_X, right_y_, PADDLE_W, PADDLE_H);

    // Ball
    r.set_pen({255, 220, 0});
    r.fill_circle(ball_x_, ball_y_, BALL_R);

    // Scores
    r.set_pen({200, 200, 200});
    r.text(std::to_string(score_left_),  70, 8, 40, 2.0f);
    r.text(std::to_string(score_right_), 155, 8, 40, 2.0f);
}

void PongGame::move_left(int dir) {
    left_y_ = std::max(0, std::min(SCREEN_H - PADDLE_H, left_y_ + dir * PADDLE_SPEED));
}

void PongGame::move_right(int dir) {
    right_y_ = std::max(0, std::min(SCREEN_H - PADDLE_H, right_y_ + dir * PADDLE_SPEED));
}
