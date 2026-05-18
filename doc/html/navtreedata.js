/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "Izg project", "index.html", [
    [ "00 Zadání projektu do předmětu IZG.", "index.html", null ],
    [ "01 Jak na projekt?", "p01_jakNaTo.html", null ],
    [ "02 Grafická karta, paměť a příkazy", "p02_gpuTheory.html", [
      [ "Teorie", "p02_gpuTheory.html#theory", null ]
    ] ],
    [ "03 Úkoly ohledně Command Bufferu", "p03_commandTasks.html", [
      [ "Teorie: Nastavování aktivních objektů GPU a zápisu nastavení", "p03_commandTasks.html#activateTheory", null ],
      [ "Úkol 0: Aktivování objektů a nastavení", "p03_commandTasks.html#BindTask", [
        [ "Test 0 – bindFramebuffer", "p03_commandTasks.html#bindFBO", null ],
        [ "Test 1 – bindProgram", "p03_commandTasks.html#bindProgram", null ],
        [ "Test 2 – bindVertexArray", "p03_commandTasks.html#bindVBO", null ],
        [ "Test 3 – blockWrites", "p03_commandTasks.html#blockWritesCMD", null ],
        [ "Test 4 – setBackface", "p03_commandTasks.html#setBackfaceCullingCMD", null ],
        [ "Test 5 – setFrontFace", "p03_commandTasks.html#setFrontFaceCMD", null ],
        [ "Test 6 – setStencil", "p03_commandTasks.html#setStencilCMD", null ],
        [ "Test 7 – setDrawID", "p03_commandTasks.html#setDrawIDCMD", null ]
      ] ]
    ] ],
    [ "04 Čistící příkazy", "p04_commands_clear_tasks.html", [
      [ "Teorie: Čistění framebufferu", "p04_commands_clear_tasks.html#ClearCommandTheory", null ],
      [ "Úkol 1: Čištění framebufferu", "p04_commands_clear_tasks.html#ClearCommandTask", [
        [ "Test 8 – čištění framebufferu", "p04_commands_clear_tasks.html#clear", null ],
        [ "Test 9 – čištění částečného framebufferu", "p04_commands_clear_tasks.html#clearPartial", null ],
        [ "Test 10 – zápis do vícero framebufferů", "p04_commands_clear_tasks.html#clearMultipleFBO", null ]
      ] ]
    ] ],
    [ "05 Uživatelský příkaz, číslování a podcommand buffer", "p05_commands_user_sub_draw_tasks.html", [
      [ "Úkol 2: – Uživatelský příkaz a číslování", "p05_commands_user_sub_draw_tasks.html#userCommand", [
        [ "Test 11 – UserCommand", "p05_commands_user_sub_draw_tasks.html#userCommandT", null ],
        [ "Test 12 – DrawCommand a počítání gl_DrawID", "p05_commands_user_sub_draw_tasks.html#drawIDTest", null ],
        [ "Test 13 – sub command", "p05_commands_user_sub_draw_tasks.html#subCmdPractice", null ]
      ] ]
    ] ],
    [ "06 Teorie o vektorové a rastrové části GPU", "p06_pipeline_vector_part_theory.html", [
      [ "Úvod", "p06_pipeline_vector_part_theory.html#GPU_vector_intro", null ],
      [ "Grafická karta", "p06_pipeline_vector_part_theory.html#GPU", null ],
      [ "Zobrazovací řetězec", "p06_pipeline_vector_part_theory.html#Pipeline", null ],
      [ "Vektorová část zobrazovacího řetězce", "p06_pipeline_vector_part_theory.html#vector_part_theory", null ]
    ] ],
    [ "07 Vektorová čast GPU: část vertexů", "p07_pipeline_vector_part_tasks.html", [
      [ "Vertexová část zobrazovacího řetězce", "p07_pipeline_vector_part_tasks.html#vector_part_theory_pipeline", [
        [ "Neindexované a indexované kreslení", "p07_pipeline_vector_part_tasks.html#index_draw", null ],
        [ "Vertex Assembly jednotka", "p07_pipeline_vector_part_tasks.html#VertexAssembly", null ],
        [ "Tabulka nastavení Vertex Array", "p07_pipeline_vector_part_tasks.html#ss_vertexArray", null ],
        [ "Vertex Processor", "p07_pipeline_vector_part_tasks.html#vector_part_theory_vs", null ]
      ] ],
      [ "Úkol 3: kreslící příkaz – vertexová část GPU", "p07_pipeline_vector_part_tasks.html#vector_part_task", [
        [ "Test 14 – spouštění vertex shaderu", "p07_pipeline_vector_part_tasks.html#drawCmdPractice", null ],
        [ "Test 15 – test číslování vykreslovacích příkazů", "p07_pipeline_vector_part_tasks.html#ShaderInterFace_gl_DrawID", null ],
        [ "Test 16 – test proloženého kreslení a čistění", "p07_pipeline_vector_part_tasks.html#ss_mixed_clear_draw", null ],
        [ "Test 17 – ověření ShaderInterface", "p07_pipeline_vector_part_tasks.html#ss_shaderInterface", null ],
        [ "Test 18 – číslování vrcholů s indexováním.", "p07_pipeline_vector_part_tasks.html#gl_VertexIDIndex32", null ],
        [ "Testy 19-21 – Vertex Atributy, Vertex Assembly jednotka", "p07_pipeline_vector_part_tasks.html#VertexPuller_Att", [
          [ "Vstupy:", "p07_pipeline_vector_part_tasks.html#sss_vertexAssembly_inputs", null ],
          [ "Výstupy:", "p07_pipeline_vector_part_tasks.html#sss_vertexAssembly_outputs", null ],
          [ "Úkol:", "p07_pipeline_vector_part_tasks.html#sss_vertexAssembly_attribTask", null ]
        ] ]
      ] ]
    ] ],
    [ "08 Vektorová část GPU: část přimitiv", "p08_pipeline_primitive_part_theory.html", [
      [ "Část primitiv", "p08_pipeline_primitive_part_theory.html#pipeline_primitive_part", [
        [ "Primitive Assembly", "p08_pipeline_primitive_part_theory.html#primitive_assembly", null ],
        [ "Perspektivní dělení", "p08_pipeline_primitive_part_theory.html#gpu_pd", null ],
        [ "Viewport transformace", "p08_pipeline_primitive_part_theory.html#gpu_Viewport", null ],
        [ "Culling / Backface Culling", "p08_pipeline_primitive_part_theory.html#gpu_Culling", null ]
      ] ]
    ] ],
    [ "09 Rasterizace", "p09_rasterization_theory.html", [
      [ "Rasterizace", "p09_rasterization_theory.html#gpu_rasterization", null ],
      [ "InFragment", "p09_rasterization_theory.html#ss_rasterization_InFragment", null ],
      [ "Interpolace atributů", "p09_rasterization_theory.html#ss_rasterization_interpolation", null ],
      [ "Výpočet 2D Barycentrických souřadnic pro interpolaci hloubky", "p09_rasterization_theory.html#sss_rasterization_interpolation_2Dlambda", null ],
      [ "Výpočet perspektivně korektních Barycentrických souřadnic pro interpolaci uživatelských atribů", "p09_rasterization_theory.html#sss_rasterization_interpolation_3Dlambda", null ],
      [ "Fragment processor", "p09_rasterization_theory.html#gpu_fragmentShader", null ],
      [ "Úkol 4 – naprogramovat Primitive Assembly jednotku, perspektivní dělení, zahazování odvrácených primitiv, rasterizaci a pouštění fragment shaderu", "p09_rasterization_theory.html#Raster", [
        [ "Test 22 – Ověření, že funguje základní rasterizace", "p09_rasterization_theory.html#ss_raster_test", null ],
        [ "Test 23 – Ověření, zda nerasterizujete mimo okno", "p09_rasterization_theory.html#ss_raster_test_2", null ],
        [ "Test 24 – Komprehenzivní testování rasterizace", "p09_rasterization_theory.html#ss_raster_test_3", null ],
        [ "Test 25 – Ověření, zda počítáte perspektivní dělení.", "p09_rasterization_theory.html#ss_raster_test_4", null ],
        [ "Test 26 – Ověření, zda vám funguje backface culling.", "p09_rasterization_theory.html#ss_raster_test_5", null ],
        [ "Test 27 – Ověření, zda se správně interpoluje hloubka fragmentů.", "p09_rasterization_theory.html#ss_raster_test_6", null ],
        [ "Testy 28-29 – Ověření, zda se správně interpolují vertex attributy.", "p09_rasterization_theory.html#ss_raster_test_7", null ]
      ] ]
    ] ],
    [ "10 Per Fragment Operace", "p10_per_fragment_operations.html", [
      [ "Per Fragment Operace", "p10_per_fragment_operations.html#per_fragment_operation", null ]
    ] ],
    [ "12 Brzké per fragment operace", "p12_pfo_early.html", [
      [ "Stencilový test", "p12_pfo_early.html#pfo_early_stencil_test", [
        [ "Stencilová porovnávací funce", "p12_pfo_early.html#pfo_early_stencil_test_func", null ],
        [ "Stencilová operace", "p12_pfo_early.html#pfo_early_stencil_test_op", null ]
      ] ],
      [ "Hloubkový test", "p12_pfo_early.html#pfo_early_depth_test", [
        [ "Pokud hloubkový test selže...", "p12_pfo_early.html#pfo_early_depth_test_fail", null ]
      ] ],
      [ "Úkol 5 – Naprogramovat brzké Per Fragment Operace", "p12_pfo_early.html#pfo_early_tasks", [
        [ "Test 30 – Stencil test", "p12_pfo_early.html#pfo_early_stencil_stencil_test", null ],
        [ "Test 31 – Stencil Operace při sfail", "p12_pfo_early.html#pfo_early_stencil_sfail", null ],
        [ "Test 32 – Depth test", "p12_pfo_early.html#pfo_early_depth_test_task", null ],
        [ "Test 33 – Depth test a modifikace stencilového buffer při dpfail", "p12_pfo_early.html#pfo_early_depth_dppas", null ]
      ] ]
    ] ],
    [ "13 Pozdní Per Fragment Operace", "p13_per_fragment_operations.html", [
      [ "Zahazování fragmentu (operace discard)", "p13_per_fragment_operations.html#pfo_late_discrad", null ],
      [ "Modifikace stencilového bufferu", "p13_per_fragment_operations.html#pfo_late_stencil", null ],
      [ "Modifikace hloubkového bufferu", "p13_per_fragment_operations.html#pfo_late_depth", null ],
      [ "Modifikace barevného bufferu", "p13_per_fragment_operations.html#pfo_late_color", [
        [ "Zápis barvy bez Blendingu", "p13_per_fragment_operations.html#pfo_late_color_write", [
          [ "Příklad zápisu barvy do barevného bufferu typu uint8_t", "p13_per_fragment_operations.html#pfo_late_color_write_example", null ],
          [ "Příklad zápisu barvy do barevného bufferu typu float", "p13_per_fragment_operations.html#pfo_late_color_write_example2", null ]
        ] ],
        [ "Zápis barvy s Blendingem", "p13_per_fragment_operations.html#pfo_late_blending", [
          [ "Příklad práce Blendingu", "p13_per_fragment_operations.html#pfo_late_blending_example", null ]
        ] ]
      ] ],
      [ "Úkol 6 – Naprogramovat pozdní Per Fragment Operace", "p13_per_fragment_operations.html#pfo_late_tasks", [
        [ "Test 34 – Discard", "p13_per_fragment_operations.html#pfo_late_discard_tasks", null ],
        [ "Test 35 – Modifikace stencilového bufferu při dppass", "p13_per_fragment_operations.html#pfo_late_stencil_tasks", null ],
        [ "Test 36 – Modifikace depth bufferu", "p13_per_fragment_operations.html#pfo_late_depth_tasks", null ],
        [ "Test 37 – Zápis barvy", "p13_per_fragment_operations.html#pfo_late_color_tasks", null ],
        [ "Test 38 – Blending", "p13_per_fragment_operations.html#pfo_late_blending_tasks", null ]
      ] ]
    ] ],
    [ "14 Ořez", "p14_pipeline_clipping.html", [
      [ "Teorie ořezu", "p14_pipeline_clipping.html#ClippingTheory", null ],
      [ "Úkol 7 – naprogramovat ořez trojúhelníků blízkou ořezovou rovinou", "p14_pipeline_clipping.html#clip", [
        [ "Test 39 – ořez celého CW trojúhelníku, který je příliš blízko kamery.", "p14_pipeline_clipping.html#clipping_all", null ],
        [ "Test 40 – ořez celého CCW trojúhelníku, který je příliš blízko kamery.", "p14_pipeline_clipping.html#clipping_0", null ],
        [ "Test 41 – Ořez trojúhelníku, když je 1 vrchol ořezán", "p14_pipeline_clipping.html#clipping_1", null ],
        [ "Test 42 – Ořez trojúhelníku, když jsou 2 vrcholy ořezány", "p14_pipeline_clipping.html#clipping_2", null ]
      ] ],
      [ "Hotová grafická karta", "p14_pipeline_clipping.html#finalGPU", null ]
    ] ],
    [ "15 Implementace vykreslování modelů se stíny – soubor student/prepareModel.cpp", "p15_drawModel.html", [
      [ "Úkol 8 – Vykreslování modelů – funkce student_prepareModel", "p15_drawModel.html#drawModel_fce", [
        [ "Testy 43-48 – Průchod modelem", "p15_drawModel.html#modelTraverse", null ],
        [ "Testy 48-56 – paměť", "p15_drawModel.html#modelMemory", null ]
      ] ],
      [ "Úkol 9 – Vykreslování modelů – vertex shader student_drawModel_vertexShader", "p15_drawModel.html#drawModel_vs", [
        [ "Test 57 – Vertex Shader", "p15_drawModel.html#vertex_task", null ]
      ] ],
      [ "Úkol 10 – Vykreslování modelů – fragment shader drawMode_fragmentShader", "p15_drawModel.html#drawModel_fs", [
        [ "Test 58-62 – Fragment Shader", "p15_drawModel.html#fragment_task", null ]
      ] ],
      [ "Úkol 11 – finální render", "p15_drawModel.html#finalCountDown", [
        [ "Test 63 – finální render", "p15_drawModel.html#finalCountDown_task", null ]
      ] ]
    ] ],
    [ "16 Rozdělení souborů a složek", "p16_rozdeleni.html", null ],
    [ "17 Sestavení", "p17_sestaveni.html", null ],
    [ "18 Spouštění", "p18_spousteni.html", null ],
    [ "19 Ovládání", "p19_ovladani.html", null ],
    [ "20 Testování", "p20_testovani.html", null ],
    [ "21 Odevzdávání", "p21_odevzdavani.html", null ],
    [ "22 Časté chyby, které nedělejte", "p22_caste_chyby.html", null ],
    [ "23 Hodnocení", "p23_hodnoceni.html", null ],
    [ "24 Soutěž", "p24_soutez.html", null ],
    [ "25 Závěrem", "p25_zaver.html", null ],
    [ "Todo List", "todo.html", null ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", null ],
        [ "Functions", "functions_func.html", null ],
        [ "Variables", "functions_vars.html", null ],
        [ "Enumerations", "functions_enum.html", null ],
        [ "Enumerator", "functions_eval.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", null ],
        [ "Functions", "globals_func.html", null ],
        [ "Variables", "globals_vars.html", null ],
        [ "Typedefs", "globals_type.html", null ],
        [ "Enumerations", "globals_enum.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"annotated.html",
"solutionInterface_2src_2solutionInterface_2gpu_8hpp.html#af046868f9f300b11282490852e003566a60cba2874369bd3790e14ea5f8a7bc58",
"unionCommandData.html#a5228b7555bf613231534ca8e414065ce"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';