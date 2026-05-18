# IZG GPU Performance Optimization Loop

**Goal:** Optimize the student GPU implementation as much as possible while keeping all 64 conformance tests passing.

**Baseline:** ~8.6e-02 seconds per frame (method 0, 50 frames)

## Build & Test Commands
- Build: `cd /home/sigull/izgProject/build && ninja`
- Performance: `cd /home/sigull/izgProject/build && ./izgProject --method 0 -p -f 50 2>&1 | grep "Student\] Seconds"`
- Conformance: `cd /home/sigull/izgProject && timeout 120 ./build/izgProject -c 2>&1 | tail -5`
- Git commit after each successful optimization

## Candidate Optimizations (prioritized by estimated impact)

### High Impact
1. **[ ] Fast clear commands** — Replace per-pixel `getPixel` in `clearColor/ClearDepth/ClearStencil` with row pointer arithmetic or `memset`. `clearStencil` can be a single `memset` over `pitch*height`.
2. **[ ] Pointer arithmetic in raster inner loop** — Replace repeated `getPixel` calls per pixel with pointer increments. Precompute `colorPtr`, `depthPtr`, `stencilPtr` per row, advance by `bytesPerPixel` per x step.
3. **[ ] Skip attribute interpolation when no vs2fs** — For program 0 (shadow map), all `vs2fs` are EMPTY. Skip the interpolation loop entirely when `nActiveAttribs == 0`.
4. **[ ] Unroll/simplify attribute interpolation** — Precompute active attribute list per program at top of `raster()`. Replace switch-in-loop with direct type dispatch or fewer branches.

### Medium Impact
5. **[ ] Simplify edge function** — Use raw float math instead of `glm::vec2` in `edge()` and `raster()`. Precompute edge constants per triangle.
6. **[ ] Optimize texture sampling** — `student_read_textureClamp` does double `glm::fract` and `glm::vec2` ops. Use raw float math. `student_texelFetch` does format switch per pixel; inline or simplify common paths.
7. **[ ] Batch vertex shader outputs** — Cache `m * pos` in vertex shader to avoid recomputation.
8. **[ ] Fast blending path for common 4-channel RGBA** — Replace per-channel switch with direct RGBA extraction.

### Low Impact / Micro
9. **[ ] Hoist `ShaderInterface` construction** — Already mostly hoisted; minor.
10. **[ ] Remove redundant `glm::clamp` in bounding box** — `clamp(min3(...), 0, w-1)` may be redundant if we just use `std::max/min`.

## Optimization Log

### Commit 1: TBD
- Optimization:
- Performance before:
- Performance after:
- Tests passing:
