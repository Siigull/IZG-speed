# IZG Project GPU Implementation

Goal: Build working GPU by implementing `student_GPU_run` and helper functions in `/home/sigull/izgProject/studentSolution/src/studentSolution/gpu.cpp` to pass ALL 64 conformance tests.

## Current Status
- **64/64 tests passing** - ALL TESTS PASS!

## Build & Test
- Build: `cd /home/sigull/izgProject/build && ninja`
- Test all: `cd /home/sigull/izgProject && ./build/izgProject -c`
- Test specific: `cd /home/sigull/izgProject && ./build/izgProject -c --test N --up-to-test`

## Test Order (from doc/)
### Phase 1: Clear Commands (p04) - DONE
### Phase 2: Command types (p05) - DONE
### Phase 3: Pipeline vector part (p07) - DONE
### Phase 4: Rasterization (p09/p14) - DONE
- 22-28: rasterization, perspectiveDivision, backfaceCulling, barycentrics

### Phase 5: Per-fragment ops (p10/p12/p13) - DONE
- stencil_test, stencil_writes_sfail, depth_test, stencil_writes_dpfail, discardTest, stencil_writes_dppass, depth_writes, color_writes, blending

### Phase 6: Clipping (p14) - DONE
- clippingTests: triangles behind near plane

### Phase 7: Model (p15) - DONE
- traverseModelTests, drawModelTests, vertexShader, fragmentShader, finalImageTest

## Constraints
- Simple procedural/C-style code
- Fix tests one by one, no boilerplate for future tests
- Follow doc/ order

## Completed Implementation Summary
1. **gpu.cpp**: Implemented `student_GPU_run`, `draw()`, `raster()`, `clipRaster()` with proper vertex assembly, near-plane clipping, rasterization, stencil/depth tests, blending, color writes
2. **prepareModel.cpp**: Implemented `student_prepareModel` with model tree traversal (pre-order), VAO setup, command buffer population, uniform writing
3. **Shaders**: Implemented `student_drawModel_vertexShader` (MVP transform, world-space outputs) and `student_drawModel_fragmentShader` (Lambert lighting + shadow mapping)
4. **Critical fixes**:
   - Removed clamping from blending (negative blending results wrap around to uint8_t, matching reference behavior)
   - Proper alpha discard when diffuseColor.a < 0.5
   - Double-sided normal flipping
   - Shadow map lookup with bounds checking
