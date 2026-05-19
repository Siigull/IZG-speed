# IZG GPU Performance Optimization Loop

**Goal:** Optimize the student GPU implementation as much as possible while keeping all 64 conformance tests passing.

**Baseline:** ~8.6e-02 seconds per frame (method 0, 50 frames)
**Current Best:** ~4.15e-02 seconds per frame

## Build & Test Commands
- Build: `cd /home/sigull/izgProject/build && ninja`
- Performance: `cd /home/sigull/izgProject/build && ./izgProject --method 0 -p -f 50 2>&1 | grep "Student\] Seconds"`
- Conformance: `cd /home/sigull/izgProject && timeout 120 ./build/izgProject -c 2>&1 | tail -5`
- Git commit after each successful optimization, push to origin/master

## Key Source Files
- `studentSolution/src/studentSolution/gpu.cpp` — rasterizer core
- `studentSolution/src/studentSolution/prepareModel.cpp` — VS/FS
- `studentSolution/src/studentSolution/shaderFunctions.hpp` — texture fetch
- `studentSolution/CMakeLists.txt` — build flags

## Profiling Data (via inline rdtsc instrumentation — approximate)
Per-frame split for ~50ms total frame:
- `draw()` (vertex assembly + VS + clipRaster): ~18ms
- `raster()` (edge test + depth + interp + blend): ~16ms
- Fragment shader execution: ~4ms
- Command buffer / other overhead: ~12ms

## Successful Commits
1. Fast clears + active attr precompute
2. Incrementing framebuffer pointers in raster inner loop
3. Precompute edge coefficients + incremental stepping
4. Texture sampling fast paths (inline getPixel, U8-RGBA/F32-R fast returns)
5. Fast blending paths (unrolled RGBA dst read + inline SRC_ALPHA/ONE_MINUS_SRC_ALPHA)
6. VS cache `m*pos` in drawModel vertex shader
7. Skip no-op `createShadowMap_fs` call in shadow map raster
8. Switch statement in raster attribute interpolation loop
9. Switch statement in `draw()` vertex attribute assembly loop
10. Dedicated depth-only raster fast path for shadow map (skip InFragment/OutFragment)
11. Raster: CSE perspective-correct barycentric computation to use single divide
12. Fragment shader: inline texture fetch by calling `__student_texelFetch_inline` directly
13. CMake: add `-march=native` for AVX2 auto-vectorization
14. Raster: ultra-fast depth-only shadow path (no stencil, precompute depthCoeff*invA)
15. ClipRaster: skip vertex copy when all 3 vertices inside near plane; move VS/FS to header; precompute invA*depthCoeff in scene raster
16. Raster: 8x8 tile coarse raster for shadow-map depth-only fast path
17. VS/FS: add `__attribute__((always_inline))` to force inlining into raster inner loop (marginal/neutral; kept for cleaner semantics)
18. Raster: force-inline all static helpers (`stencilTest`, `edge`, `blend`, `vtxLerp`, etc.) — reduces call overhead and improves icache; also added SSE pixel loop in shadow-map full tiles (neutral correctness)
    - Performance before: ~4.24e-02
    - Performance after: ~4.15e-02 (~2% faster)
    - Tests passing: 64/64

## Attempted & Reverted
- Fragment shader raw-float math (no measurable gain)
- LTO (`-flto`) — broke `finalImageTest`
- Deferred `zInv`/`lP` computation in raster (no gain)
- `vtxLerp` active-attribute-only interpolation (slightly slower)
- Packed uint32_t clearColor (slower than row loop)
- Direct-calling known VS/FS from `gpu.cpp` instead of function pointers (no gain; branch predictor handles constant targets well)
- Moving VS/FS to header for cross-TU inlining (marginal unless `always_inline` is used)
- Scene pass tile raster with lambda (slower; overhead exceeds benefit for small triangles)
- Precomputing `scr_z*invA` and `hInv*invA` in scene raster (marginal; compiler already CSE'd the multiply)

## Next Targets
- **Draw/VS batching** — `draw()` is ~18ms. Find ways to reduce attribute assembly overhead or batch VS execution.
- **Raster inner loop SIMD** — The scene pass raster inner loop does edge test, depth test, 4x attribute interpolation, blend, color write per pixel. Use SSE/AVX to process 4 pixels at once.
- **Fragment shader math** — `normalize`, `dot`, texture coordinate wrap/clamp can be micro-optimized further.
- **Tile-based raster or half-space block stepping** — Reduce per-triangle setup relative to per-pixel work.

## Recent Exploration (Iteration 8 - no effective commit)
Tried several optimizations; none produced measurable improvement over the ~4.15e-02 baseline:
1. **Restructured shadow VS to `vp*(model*pos)`** — compiler already optimized `vp*model*pos` to the same mat-vec pattern; no improvement.
2. **Restructured stencil loads inside `ss.enabled` blocks only** — the compiler already eliminates dead stencil loads when `ss.enabled==false`.
3. **Checked `v%3` in draw loop** — compiler already hoists the modulo via magic multiply.
4. **Considered hoisting InVertex/OutFragment default construction** — default members are already kept in registers, no memory traffic.

### Updated Insights
- The compiler (`-O3 -march=native`) is doing an excellent job on scalar code. Most "obvious" optimizations are already applied.
- Remaining time is split roughly: raster inner loop ~14ms (of which FS ~4ms), draw/VS/clipRaster ~4ms, overhead ~4ms.
- To make significant further progress, we likely need: SIMD pixel processing in the scene raster (very complex), or reducing FS cost (e.g. approximating `normalize`).
- Next attempt: try fast `rsqrt` approximation in FS `normalize` calls to save ~2 `vsqrtss` instructions per visible pixel.
Explored several micro-optimizations; all were reverted after testing showed no improvement or regression:
1. **Defer stencil load until `ss.enabled` branch** — compiler already eliminated dead loads.
2. **Raw `uint32_t` texture coordinates in `texelFetch`** — `glm::uvec2` temp was already optimized away by compiler.
3. **`channels==3` hardcoded blend read + color write in raster** — compiler already unrolled 3-iter loop effectively.
4. **`-ffast-math`** — 64/64 tests passed, but no measurable perf change.
5. **Span-based shadow map raster (skip-then-process loops)** — no improvement; compiler already handled biased branch.
6. **Shadow VS mat-mat→mat-vec restructuring** — compiler already did it.
7. **Stencil load restructuring** — compiler already dead-code eliminated it.

### Insights
- Compiler with `-O3 -march=native` is extremely aggressive on scalar code. Micro-optimizations provide diminishing returns.
- **Algorithmic wins matter**: 8x8 tile coarse raster for shadow map reduced per-frame by ~10% (~4.55e-02 → ~4.15e-02).
- Scene raster inner loop is still the main bottleneck (~16ms). Tile raster for scene pass was attempted but overhead exceeded benefit for small triangles.
- `always_inline` on VS/FS gave ~2-3% win by eliminating per-pixel function call overhead in raster.
- To reach the ~2ms best solution on this PC, we need more algorithmic improvements: SIMD pixel processing, hierarchical Z, or further tile optimizations.
