# TODO

Issues identified during code review. To be addressed in detail.

## 1. PongGame data race (HIGH)

`PongGame` has plain `int` members with no synchronisation. Core 0 writes
`left_y_`/`right_y_` via `handle_buttons()` and reads `winner_`/`finish_timer_`
via `is_finished()`. Core 1 writes and reads all of them in `animate()` and
`render()`. The most dangerous window is `is_finished()` on core 0 reading
`winner_` and `finish_timer_` while `check_winner()` on core 1 is writing both.

`Scene` already does this correctly with `critical_section_t`. `PongGame` needs
the same treatment, or `std::atomic` on `winner_` and `finish_timer_` at
minimum.

## 2. Division by zero in display loop (MEDIUM)

`lib/display.cpp:78`:
```cpp
frame % (REFRESH_HZ / anim_hz)
```
If any `Screen` returns `animation_hz() > REFRESH_HZ` (30), integer division
produces 0 and `frame % 0` is evaluated. On Cortex-M0+ this silently returns 0,
so `animate()` gets called every frame regardless of the requested rate.
`screen.hpp` has an advisory comment but no enforcement.

Fix: `if (anim_hz > 0 && anim_hz <= REFRESH_HZ && frame % (REFRESH_HZ / anim_hz) == 0)`.

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
