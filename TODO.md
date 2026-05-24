# TODO

Issues identified during code review. To be addressed in detail.

## 1. PongGame data race (HIGH) — FIXED

`PongGame` now holds a `mutable critical_section_t cs_` (same pattern as
`Scene`). All cross-core accesses are protected: `animate()`, `render()`,
`reset()`, `is_finished()`, `move_left()`, and `move_right()` each enter/exit
`cs_`. `check_winner()` and `reset_ball()` are internal helpers always called
under the lock so they need no additional guarding.

## 2. Division by zero in display loop (MEDIUM) — FIXED

Guard in `lib/display.cpp:78` now reads:
`if (anim_hz > 0 && anim_hz <= REFRESH_HZ && frame % (REFRESH_HZ / anim_hz) == 0)`

## 2b. Corner-exit audio artefact — FIXED

When the ball exited in a corner, the top/bottom wall-bounce check (440 Hz)
fired in the same frame as the scoring check (220 Hz). The score sound
immediately overwrote the wall sound mid-PWM-cycle, producing an audible
artefact that was heard inconsistently (only on corner exits).

Fix: scoring is now checked *before* walls and paddles via `if/else if/else`.
If the ball has exited, wall and paddle checks are skipped entirely, so the
220 Hz score sound always plays cleanly.

## 3. Scene deadlock if a callback re-enters Scene (LATENT) — FIXED

`add()` and `remove()` now detect re-entry: if called on core 1 while
`in_animate_` is set (i.e. from within a child `animate()` or `on_collision()`
callback), the op is queued in `pending_` instead of trying to acquire `cs_`.
After all callbacks complete, `animate()` clears the flag and flushes `pending_`
while still holding `cs_`, so the mutation is applied atomically at the end of
the same frame. `do_remove()` is extracted as a private helper shared by the
normal path and the flush.

## 4. Buzzer::update() only called from PongGame (FRAGILE) — FIXED

`Buzzer::update()` moved to the top of the main loop in `main.cpp` so it runs
every 50 ms tick regardless of active screen. Removed from `PongGame::animate()`.
