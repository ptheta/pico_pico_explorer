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

## 3. Scene deadlock if a callback re-enters Scene (LATENT)

`Scene::animate()` holds `cs_` (non-reentrant hardware spinlock) while making
virtual calls to `animate()` and `on_collision()` on child screens. No current
child calls back into the scene, so this is safe today. Any future
`on_collision` handler that calls `scene_.add()` or `scene_.remove()` will
deadlock the rendering core.

Fix: document the constraint in `screen.hpp`, or restructure so callbacks queue
deferred actions rather than calling scene methods directly.

## 4. Buzzer::update() only called from PongGame (FRAGILE)

`Buzzer::update()` is only called from `PongGame::animate()`, so the buzzer
only auto-stops while Pong is the active screen. Currently safe because all Pong
exit paths call `Buzzer::stop()` explicitly. If sounds are ever added to `Demo`
or `MenuScreen` they will never auto-stop.

Fix: move `Buzzer::update()` into the main loop so it runs every tick regardless
of active screen.
