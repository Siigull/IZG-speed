/*!
 * @file
 * @brief This file contains example implementation of student_GPU_run function
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <solutionInterface/gpu.hpp>

///\cond HIDDEN_SYMBOLS


//![exp_BindFramebufferCommand]
void student_GPU_run(GPUMemory&mem,CommandBuffer const&cb){
  // průchod všemi příkazy v CommandBufferu
  for(uint32_t i=0;i<cb.nofCommands;++i){
    // typ a data příkazu
    CommandType type = cb.commands[i].type;
    CommandData data = cb.commands[i].data;
    if(type == CommandType::BIND_FRAMEBUFFER){ // pokud je to BindFramebufferCommand
      //nastav aktivní framebuffer
      mem.activatedFramebuffer = data.bindFramebufferCommand.id;
    }
  }
}
//![exp_BindFramebufferCommand]

//![exp_clearCommands]
void clearColor(GPUMemory&mem,ClearColorCommand cmd){
  // ukázka čistícího příkazu

  // výběr framebufferu
  Framebuffer*fbo = mem.framebuffers+mem.activatedFramebuffer;

  // obsahuje framebuffer barevný buffer?
  if(fbo->color.data){
    for(uint32_t y=0;y<fbo->height;++y)
      for(uint32_t x=0;x<fbo->width;++x){
        void*pixelStart = getPixel(fbo->color,x,y);
        for(uint32_t i=0;i<fbo->color.channels;++i){
          //...
        }
      }
  //...
  }
}

void student_GPU_run(GPUMemory&mem,CommandBuffer const&cb){
  for(uint32_t i=0;i<cb.nofCommands;++i){
    CommandType type = cb.commands[i].type;
    CommandData data = cb.commands[i].data;
    if(type == CommandType::CLEAR_COLOR)
      clearColor(mem,data.clearColorCommand);
  }
}
//![exp_clearCommands]

//![student_GPU_run_gl_DrawID]
void student_GPU_run(GPUMemory&mem,CommandBuffer const&cb){
  // smyčka přes příkazy
  for(... commands ...){
    // vykreslovací příkaz
    if (commandType == CommandType::DRAW ){
      // kresli
      draw(mem, drawCommand);
      // počítadlo kreslících příkazů
      mem.gl_DrawID++;
    }
    if (commandType == CommandType::SET_DRAW_ID){
      mem.gl_DrawID = ...
    }
  }
}
//![student_GPU_run_gl_DrawID]

//![student_GPU_run_SubCommand]
void student_GPU_run(GPUMemory&mem,CommandBuffer const&cb){
  // smyčka přes příkazy
  for(... commands ...){
    // sub command
    if (commandType == CommandType::SUB_COMMAND){
      processSubCommandRecursive(commandData.subCommand.commandBuffer);
    }
  }
}
//![student_GPU_run_SubCommand]

//![index_buffer]
uint32_t getIndex(GPUMemory const&mem,uint32_t i){
  // aktivovaná tabulka nastavení pro vertex assembly jednotu
  auto vao = mem.vertexArrays[mem.activatedVertexArray];
  if(vao.indexBufferID >= 0){ // je použito indexování?
    Buffer indexBuffer = mem.buffers[vao.indexBufferID]; //indexový buffer
    if(vao.indexType == IndexType::U8){ // je typ indexu uint8_t
      uint8_t*ptr = (uint8_t*)indexBuffer.data; //ukazatel na data
      ptr += vao.indexOffset; // posun ukazatele o offset v bajtech
      return ...
    }
  }else
    // ....
}
//![index_buffer]

//![student_GPU_run_after_vs]
void vertexAssembly(){
  computeVertexID();
  readAttributes();
}

void primitiveAssembly(primitive,vertexArray,t,program){
  for(every vertex v in triangle){
    InVertex inVertex;
    vertexAssembly(inVertex,vertexArray,t+v);
    ShaderInterface si;
    if(program.vertexShader)
      program.vertexShader(primitive.vertex,inVertex,si);
  }
}

void rasterize(framebuffer,primitive,program){
  for(pixels in frame){
    if(pixels in primitive){
      InFragment inFragment;
      createFragment(inFragment,primitive,barycentrics,pixelCoord,program);
      OutFragment outFragment;
      ShaderInterface si;
      if(program.fragmentShader)
        program.fragmentShader(outFragment,inFragment,si);
    }
  }
}

void draw(GPUMemory&mem,DrawCommand const&cmd){
  for(every triangle t){
    Primitive primitive;
    primitiveAssembly(primitive,vertexArray,t,program)
    viewportTransformation(primitive,width,height)
    rasterize(framebuffer,primitive,program);
  }
}

void student_GPU_run(GPUMemory&mem,CommandBuffer const&cb){
  for(every command in cb){
    if(command.type == DRAW)
      draw(mem,command);
  }
}
//![student_GPU_run_after_vs]




//![student_GPU_run_pfo]
void runVertexAssembly(){
  computeVertexID()
  readVertexAttributes();
}

void runPrimitiveAssembly(primitive,VertexArray vao,t,vertexShader,shaderInterface){
  for(every vertex v in triangle){
    InVertex inVertex;
    runVertexAssembly(inVertex,vao,t+v);
    vertexShader(primitive.vertex,inVertex,shaderInterface);
  }
}

void rasterizeTriangle(framebuffer,primitive,fragmentShader){
  for(pixels in frame){
    if(pixels in primitive){
      InFragment inFragment;
      createFragment(inFragment,primitive,barycentrics,pixelCoord,prg);
      OutFragment outFragment;
      ShaderInterface si;
      si.uniforms = ...;
      si.textures = ...;
      
      fragmentShader(outFragment,inFragment,si);
      clampColor(outFragment,0,1);
      perFragmentOperations(framebuffer,outFragment,inFragment.gl_FragCoord.z)
    }
  }

}

void draw(GPUMemory&mem,DrawCommand const&cmd){
  for(every triangle t){
    Primitive primitive;
    runPrimitiveAssembly(primitive,vao,t,vertexShader,shaderInterface)

    runPerspectiveDivision(primitive)
    runViewportTransformation(primitive,width,height)
    rasterizeTriangle(framebuffer,primitive,fragmentShader);
  }
}

void student_GPU_student(...){
}

//![student_GPU_run_pfo]



//![student_GPU_run_clipping]
void draw(mem,drawCommand){
  for(every triangle t){
    Primitive primitive;
    runPrimitiveAssembly(primitive,vertexArray,t,vertexShader)

    ClippedPrimitive clipped;
    performeClipping(clipped,primitive);

    for(all clipped triangle c in clipped){
      runPerspectiveDivision(c)
      runViewportTransformation(c,width,height)
      rasterizeTriangle(framebuffer,c,fragmentShader);
    }
  }
}

void student_GPU_run(mem,commandBuffer){
  for(every command in commandBuffer){
    if(isDrawCommand )draw (mem,drawCommand)
  }
}
//![student_GPU_run_clipping]


//![student_prepareModel]
void student_prepareNode(GPUMemory&mem,CommandBuffer&cb,Node const&node,Model const&model,glm::mat4 const&prubeznaMatice,...){
  if(node.mesh>=0){
    Mesh mesh = model.meshes[node.mesh];

    drawCounter; // pocitadlo kreslicich prikazu

    // vytvoření vertex array
    VertexArray vao;
    vao.indexBufferID = mesh.indexBufferID;
    vao.indexOffset = ...;
    vao.indexType = ...;
    vao.vertexAttrib[0] = ...; // pozice
    vao.vertexAttrib[1] = ...; // normala
    vao.vertexAttrib[2] = ...; // texturovaci souradnice

    // vlozeni vao na spravne misto v pameti (aby jej bylo mozne najit 
    mem.vertexArrays[drawCounter] = vao;

    BindVertexArrayCommand bindVaoCmd;
    bindVaoCmd.id = drawCounter;

    DrawCommand drawCmd;
    drawCmd.backfaceCulling = ...; // pokud je double sided tak by se nemelo orezavat
    drawCmd.nofVertices = ...; // pocet vertexu

    // vlozeni bindVaoCmd a drawCmd do command buffer cb
    cb.commands[...    ] = bindVaoCmd;
    cb.commands[... +1 ] = setBackfaceCulling;
    cb.commands[... +2 ] = drawCmd   ;
    
    //zapis uniformnich dat do pameti
    ZKOBINUJ(prubeznaMatice,node.modelMatrix);
    vypocitej inverzni transponovanou matici pro normaly...

    mem.uniforms[getUniformLocation(cmdID,MODEL_MATRIX                  )].m4 = modelMatrix                   ;
    mem.uniforms[getUniformLocation(cmdID,INVERSE_TRANSPOSE_MODEL_MATRIX)].m4 = inverzniTransponovana Modelova;
    mem.uniforms[getUniformLocation(cmdID,DIFFUSE_COLOR                 )].v4 = difuzni barva                 ;
    mem.uniforms[getUniformLocation(cmdID,TEXTURE_ID                    )].i1 = id textury nebo -1 pokud neni ;
    mem.uniforms[getUniformLocation(cmdID,DOUBLE_SIDED                  )].v1 = double sided                  ;



    writeToMemory(mem);
  }

  for(size_t i=0;i<node.children.size();++i)
    prepareNode(mem,node.children[i],model,...); rekurze
}

void prepareModel(GPUMemory&mem,CommandBuffer&cb,Model const&model){
  mem.buffers  = ...;
  mem.textures = ...;

  glm::mat4 jednotkovaMatrice = glm::mat4(1.f);
  for(size_t i=0;i<model.roots.size();++i)
    prepareNode(mem,cb,model.roots[i],jednotkovaMatrice,...);
}
//![student_prepareModel]

///\endcond
