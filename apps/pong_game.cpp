#include "pong_game.hpp"
#include "buzzer.hpp"
#include "screen_dims.hpp"
#include <algorithm>
#include <cstdlib>
#include <string>

PongGame::PongGame() {
    critical_section_init(&cs_);
    reset();
}

void PongGame::reset() {
    critical_section_enter_blocking(&cs_);
    left_y_       = SCREEN_H / 2 - PADDLE_H / 2;
    right_y_      = SCREEN_H / 2 - PADDLE_H / 2;
    score_left_   = 0;
    score_right_  = 0;
    winner_       = 0;
    finish_timer_ = 0;
    reset_ball();
    critical_section_exit(&cs_);
}

void PongGame::reset_ball() {
    ball_x_  = SCREEN_W / 2;
    ball_y_  = SCREEN_H / 2;
    ball_dx_ = rand() % 2 ? BALL_SPEED_X : -BALL_SPEED_X;
    ball_dy_ = rand() % 2 ? BALL_SPEED_Y : -BALL_SPEED_Y;
}

void PongGame::check_winner() {
    if (winner_ != 0) return;
    if (score_left_ >= WIN_SCORE)  { winner_ = 1; ball_dx_ = ball_dy_ = 0; finish_timer_ = FINISH_FRAMES; Buzzer::play(1760, 600); }
    if (score_right_ >= WIN_SCORE) { winner_ = 2; ball_dx_ = ball_dy_ = 0; finish_timer_ = FINISH_FRAMES; Buzzer::play(1760, 600); }
}

void PongGame::animate() {
    critical_section_enter_blocking(&cs_);

    if (finish_timer_ > 0) {
        --finish_timer_;
    } else if (winner_ == 0) {
        ball_x_ += ball_dx_;
        ball_y_ += ball_dy_;

        // Scoring — checked before walls so a corner exit never triggers a
        // wall-bounce sound in the same frame, which would produce an audio
        // artefact when the score sound immediately overwrites it.
        if (ball_x_ - BALL_R <= 0)        { ++score_right_; reset_ball(); Buzzer::play(400, 300); check_winner(); }
        else if (ball_x_ + BALL_R >= SCREEN_W) { ++score_left_;  reset_ball(); Buzzer::play(400, 300); check_winner(); }
        else {
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
        }
    }

    critical_section_exit(&cs_);
}

void PongGame::render(Renderer& r) const {
    critical_section_enter_blocking(&cs_);

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

    // Victory overlay
    if (winner_ != 0) {
        r.set_pen({255, 220, 0});
        const char* msg = (winner_ == 1) ? "LEFT WINS!" : "RIGHT WINS!";
        r.text(msg, 20, 100, 200, 2.5f);
    }

    critical_section_exit(&cs_);
}

bool PongGame::is_finished() const {
    critical_section_enter_blocking(&cs_);
    bool result = finish_timer_ == 0 && winner_ != 0;
    critical_section_exit(&cs_);
    return result;
}

void PongGame::handle_buttons(const Buttons& btns) {
    if (btns.a.held()) move_left(-1);
    if (btns.b.held()) move_left(+1);
    if (btns.x.held()) move_right(-1);
    if (btns.y.held()) move_right(+1);
}

void PongGame::move_left(int dir) {
    critical_section_enter_blocking(&cs_);
    left_y_ = std::max(0, std::min(SCREEN_H - PADDLE_H, left_y_ + dir * PADDLE_SPEED));
    critical_section_exit(&cs_);
}

void PongGame::move_right(int dir) {
    critical_section_enter_blocking(&cs_);
    right_y_ = std::max(0, std::min(SCREEN_H - PADDLE_H, right_y_ + dir * PADDLE_SPEED));
    critical_section_exit(&cs_);
}
