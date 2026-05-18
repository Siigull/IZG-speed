#include <studentSolution/gpu.hpp>
#include <cstring>

static void clearColor(GPUMemory&mem,ClearColorCommand const&cmd){
  Framebuffer*f=mem.framebuffers+mem.activatedFramebuffer;
  if(!f->color.data)return;
  glm::vec4 v=cmd.value*255.f;
  for(uint32_t y=0;y<f->height;++y)
    for(uint32_t x=0;x<f->width;++x){
      auto p=(uint8_t*)getPixel(f->color,x,y);
      for(uint32_t c=0;c<f->color.channels;++c)p[f->color.channelTypes[c]]=(uint8_t)(v[c]);
    }
}
static void clearDepth(GPUMemory&mem,ClearDepthCommand const&cmd){
  Framebuffer*f=mem.framebuffers+mem.activatedFramebuffer;
  if(!f->depth.data)return;
  float v=cmd.value;
  for(uint32_t y=0;y<f->height;++y)
    for(uint32_t x=0;x<f->width;++x)*(float*)getPixel(f->depth,x,y)=v;
}
static void clearStencil(GPUMemory&mem,ClearStencilCommand const&cmd){
  Framebuffer*f=mem.framebuffers+mem.activatedFramebuffer;
  if(!f->stencil.data)return;uint8_t v=cmd.value;
  for(uint32_t y=0;y<f->height;++y)
    for(uint32_t x=0;x<f->width;++x)*(uint8_t*)getPixel(f->stencil,x,y)=v;
}

static glm::vec3 ndc(glm::vec4 const&v){return glm::vec3(v)/v.w;}
static float edge(glm::vec2 const&a,glm::vec2 const&b,glm::vec2 const&c){
  return (c.x-a.x)*(b.y-a.y)-(c.y-a.y)*(b.x-a.x);
}
static float min3(float a,float b,float c){return a<b?(a<c?a:c):( b<c?b:c);}
static float max3(float a,float b,float c){return a>b?(a>c?a:c):( b>c?b:c);}

static bool stencilTest(uint8_t s,uint32_t ref,StencilFunc f){
  switch(f){
    case StencilFunc::NEVER:    return false;
    case StencilFunc::LESS:     return s<ref;
    case StencilFunc::LEQUAL:   return s<=ref;
    case StencilFunc::GREATER:  return s>ref;
    case StencilFunc::GEQUAL:   return s>=ref;
    case StencilFunc::EQUAL:    return s==ref;
    case StencilFunc::NOTEQUAL: return s!=ref;
    case StencilFunc::ALWAYS:   return true;
  default: return true;
  }
}
static uint8_t applyStencilOp(uint8_t v,StencilOp op,uint32_t ref){
  switch(op){
    case StencilOp::KEEP:      return v;
    case StencilOp::ZERO:      return 0;
    case StencilOp::REPLACE:   return (uint8_t)ref;
    case StencilOp::INCR:      return v<255?v+1:255;
    case StencilOp::INCR_WRAP: return v+1;
    case StencilOp::DECR:      return v>0?v-1:0;
    case StencilOp::DECR_WRAP: return v-1;
    case StencilOp::INVERT:    return ~v;
  default: return v;
  }
}
static glm::vec4 getBlendFactor(BlendFunc f,glm::vec4 const&src,glm::vec4 const&dst){
  switch(f){
    case BlendFunc::ZERO:                return glm::vec4(0.f);
    case BlendFunc::ONE:                 return glm::vec4(1.f);
    case BlendFunc::SRC_COLOR:           return src;
    case BlendFunc::ONE_MINUS_SRC_COLOR: return glm::vec4(1.f)-src;
    case BlendFunc::DST_COLOR:           return dst;
    case BlendFunc::ONE_MINUS_DST_COLOR: return glm::vec4(1.f)-dst;
    case BlendFunc::SRC_ALPHA:           return glm::vec4(src.a);
    case BlendFunc::ONE_MINUS_SRC_ALPHA: return glm::vec4(1.f-src.a);
    case BlendFunc::DST_ALPHA:           return glm::vec4(dst.a);
    case BlendFunc::ONE_MINUS_DST_ALPHA: return glm::vec4(1.f-dst.a);
  default: return glm::vec4(1.f);
  }
}
static glm::vec4 applyBlendEquation(BlendEquation e,glm::vec4 const&s,glm::vec4 const&d){
  switch(e){
    case BlendEquation::ADD:              return s+d;
    case BlendEquation::SUBTRACT:         return s-d;
    case BlendEquation::REVERSE_SUBTRACT: return d-s;
    case BlendEquation::MIN:              return glm::min(s,d);
    case BlendEquation::MAX:              return glm::max(s,d);
  default: return s+d;
  }
}

static void raster(GPUMemory&mem,OutVertex const v[3]);

static bool insideNear(glm::vec4 const&p){return p.z>=-p.w;}
static OutVertex vtxLerp(OutVertex const&a,OutVertex const&b,float t){
  OutVertex r;r.gl_Position=a.gl_Position+t*(b.gl_Position-a.gl_Position);
  for(uint32_t i=0;i<maxAttribs;++i)r.attributes[i].v4=a.attributes[i].v4+t*(b.attributes[i].v4-a.attributes[i].v4);
  return r;
}
static float nearT(glm::vec4 const&a,glm::vec4 const&b){
  float av=a.z+a.w,bv=b.z+b.w;
  float d=av-bv;if(fabsf(d)<1e-6f)return 0.f;
  return av/d;
}
static void clipRaster(GPUMemory&mem,OutVertex const vin[3]){
  bool in[3];for(int i=0;i<3;++i)in[i]=insideNear(vin[i].gl_Position);
  int cnt=in[0]+in[1]+in[2];
  if(cnt==0)return;
  OutVertex v[4];int n=0;
  for(int i=0;i<3;++i){
    int j=(i+1)%3;
    if(in[i]){
      v[n++]=vin[i];
      if(!in[j])v[n++]=vtxLerp(vin[i],vin[j],nearT(vin[i].gl_Position,vin[j].gl_Position));
    }else if(in[j]){
      v[n++]=vtxLerp(vin[j],vin[i],nearT(vin[j].gl_Position,vin[i].gl_Position));
    }
  }
  if(n<3)return;
  raster(mem,v);
  if(n==4){
    OutVertex t2[3]={v[0],v[2],v[3]};
    raster(mem,t2);
  }
}

static void raster(GPUMemory&mem,OutVertex const v[3]){
  auto f=mem.framebuffers+mem.activatedFramebuffer;
  auto&p=mem.programs[mem.activatedProgram];
  uint32_t w=f->width,h=f->height;

  for(int i=0;i<3;++i)if(v[i].gl_Position.w<=0.f)return;
  glm::vec3 pos[3];for(int i=0;i<3;++i)pos[i]=ndc(v[i].gl_Position);
  glm::vec3 scr[3];for(int i=0;i<3;++i)scr[i]=glm::vec3((glm::vec2(pos[i].x,pos[i].y)*.5f+.5f)*glm::vec2(w,h),pos[i].z);
  glm::vec2 sp[3];for(int i=0;i<3;++i)sp[i]=glm::vec2(scr[i]);
  float A=edge(sp[0],sp[1],sp[2]); if(A==0.f)return;
  bool cw=A<0.f;
  bool refCW=A>0.f;
  bool refCCW=!refCW;
  bool isFrontFace=refCCW;

  if(mem.backfaceCulling.enabled){
    bool frontCCW=mem.backfaceCulling.frontFaceIsCounterClockWise;
    isFrontFace=(frontCCW?refCCW:refCW);
    if(!isFrontFace)return;
  }

  int x0=(int)glm::clamp(min3(scr[0].x,scr[1].x,scr[2].x),0.f,(float)w-1);
  int x1=(int)glm::clamp(max3(scr[0].x,scr[1].x,scr[2].x),0.f,(float)w-1);
  int y0=(int)glm::clamp(min3(scr[0].y,scr[1].y,scr[2].y),0.f,(float)h-1);
  int y1=(int)glm::clamp(max3(scr[0].y,scr[1].y,scr[2].y),0.f,(float)h-1);

  ShaderInterface si;si.uniforms=mem.uniforms;si.textures=mem.textures;si.gl_DrawID=mem.gl_DrawID;
  float invA=1.f/A;float hInv[3]={1.f/v[0].gl_Position.w,1.f/v[1].gl_Position.w,1.f/v[2].gl_Position.w};
  auto&ss=mem.stencilSettings;
  auto&bs=mem.blendingSettings;
  StencilOps ops=isFrontFace?ss.frontOps:ss.backOps;

  for(int y=y0;y<=y1;++y){
    for(int x=x0;x<=x1;++x){
      glm::vec2 pc(x+.5f,y+.5f);
      float l0=edge(sp[1],sp[2],pc);
      float l1=edge(sp[2],sp[0],pc);
      float l2=edge(sp[0],sp[1],pc);
      if(cw){l0=-l0;l1=-l1;l2=-l2;}
      if(l0<=0.f||l1<=0.f||l2<=0.f)continue;

      l0*=invA;l1*=invA;l2*=invA;
      float zInv=l0*hInv[0]+l1*hInv[1]+l2*hInv[2];
      float lP[3]={l0*hInv[0]/zInv,l1*hInv[1]/zInv,l2*hInv[2]/zInv};
      float depthZ=l0*scr[0].z+l1*scr[1].z+l2*scr[2].z;

      uint8_t stencilVal=0;
      if(f->stencil.data)stencilVal=*(uint8_t*)getPixel(f->stencil,x,y);
      if(ss.enabled&&f->stencil.data){
        if(!stencilTest(stencilVal,ss.refValue,ss.func)){
          if(!mem.blockWrites.stencil)
            *(uint8_t*)getPixel(f->stencil,x,y)=applyStencilOp(stencilVal,ops.sfail,ss.refValue);
          continue;
        }
      }
      bool depthPass=true;
      float oldDepth=0.f;
      if(f->depth.data){
        oldDepth=*(float*)getPixel(f->depth,x,y);
        depthPass=depthZ<oldDepth;
      }
      if(!depthPass){
        if(ss.enabled&&f->stencil.data&&!mem.blockWrites.stencil)
          *(uint8_t*)getPixel(f->stencil,x,y)=applyStencilOp(stencilVal,ops.dpfail,ss.refValue);
        continue;
      }
      if(ss.enabled&&f->stencil.data&&!mem.blockWrites.stencil)
        *(uint8_t*)getPixel(f->stencil,x,y)=applyStencilOp(stencilVal,ops.dppass,ss.refValue);

      InFragment inF;inF.gl_FragCoord=glm::vec4(pc.x,pc.y,depthZ,1.f);
      for(uint32_t a=0;a<maxAttribs;++a){
        if(p.vs2fs[a]==AttribType::EMPTY)continue;
        AttribType tp=p.vs2fs[a];
        if(tp==AttribType::FLOAT)inF.attributes[a].v1=v[0].attributes[a].v1*lP[0]+v[1].attributes[a].v1*lP[1]+v[2].attributes[a].v1*lP[2];
        else if(tp==AttribType::VEC2)inF.attributes[a].v2=v[0].attributes[a].v2*lP[0]+v[1].attributes[a].v2*lP[1]+v[2].attributes[a].v2*lP[2];
        else if(tp==AttribType::VEC3)inF.attributes[a].v3=v[0].attributes[a].v3*lP[0]+v[1].attributes[a].v3*lP[1]+v[2].attributes[a].v3*lP[2];
        else if(tp==AttribType::VEC4)inF.attributes[a].v4=v[0].attributes[a].v4*lP[0]+v[1].attributes[a].v4*lP[1]+v[2].attributes[a].v4*lP[2];
        else if(tp==AttribType::UINT)inF.attributes[a].u1=(uint32_t)round(v[0].attributes[a].u1*lP[0]+v[1].attributes[a].u1*lP[1]+v[2].attributes[a].u1*lP[2]);
      }
      OutFragment outF;
      if(p.fragmentShader)p.fragmentShader(outF,inF,si);
      if(outF.discard)continue;

      if(!mem.blockWrites.color&&f->color.data){
        uint8_t*px=(uint8_t*)getPixel(f->color,x,y);
        glm::vec4 fragColor=outF.gl_FragColor;
        if(bs.enabled){
          glm::vec4 dstColor;
          for(uint32_t c=0;c<f->color.channels;++c){
            float ch=float(px[f->color.channelTypes[c]])/255.f;
            if(c==0)dstColor.r=ch;
            else if(c==1)dstColor.g=ch;
            else if(c==2)dstColor.b=ch;
            else if(c==3)dstColor.a=ch;
          }
          if(f->color.channels<4){
            if(f->color.channels<=0)dstColor.r=0.f;
            if(f->color.channels<=1)dstColor.g=0.f;
            if(f->color.channels<=2)dstColor.b=0.f;
            dstColor.a=1.f;
          }
          if(bs.equation==BlendEquation::MIN)fragColor=glm::min(fragColor,dstColor);
          else if(bs.equation==BlendEquation::MAX)fragColor=glm::max(fragColor,dstColor);
          else{
            glm::vec4 sf=getBlendFactor(bs.sFactor,fragColor,dstColor);
            glm::vec4 df=getBlendFactor(bs.dFactor,fragColor,dstColor);
            fragColor=applyBlendEquation(bs.equation,fragColor*sf,dstColor*df);
          }
          // no clamp - let wraparound happen for negative values
        }
        for(uint32_t c=0;c<f->color.channels;++c)
          px[f->color.channelTypes[c]]=(uint8_t)(fragColor[c]*255.f);
      }
      if(!mem.blockWrites.depth&&f->depth.data)*(float*)getPixel(f->depth,x,y)=depthZ;
    }
  }
}

static void draw(GPUMemory&mem,DrawCommand const&cmd){
  auto&prg=mem.programs[mem.activatedProgram];
  auto&vao=mem.vertexArrays[mem.activatedVertexArray];
  int ibo=vao.indexBufferID;
  ShaderInterface si;si.uniforms=mem.uniforms;si.textures=mem.textures;si.gl_DrawID=mem.gl_DrawID;

  OutVertex ov[3];
  for(uint32_t v=0;v<cmd.nofVertices;++v){
    uint32_t vid=v;
    if(ibo>=0){auto*bf=(uint8_t*)mem.buffers[ibo].data+vao.indexOffset;
      if(vao.indexType==IndexType::U8)vid=((uint8_t*)bf)[v];
      else if(vao.indexType==IndexType::U16)vid=((uint16_t*)bf)[v];
      else vid=((uint32_t*)bf)[v];}
    InVertex inV;inV.gl_VertexID=vid;
    for(uint32_t a=0;a<maxAttribs;++a){
      auto&att=vao.vertexAttrib[a];
      if(att.bufferID<0||att.type==AttribType::EMPTY)continue;
      auto ad=(uint8_t*)mem.buffers[att.bufferID].data+att.offset+vid*att.stride;
      if(att.type==AttribType::FLOAT)inV.attributes[a].v1=*(float*)ad;
      else if(att.type==AttribType::VEC2)inV.attributes[a].v2=*(glm::vec2*)ad;
      else if(att.type==AttribType::VEC3)inV.attributes[a].v3=*(glm::vec3*)ad;
      else if(att.type==AttribType::VEC4)inV.attributes[a].v4=*(glm::vec4*)ad;
      else if(att.type==AttribType::UINT)inV.attributes[a].u1=*(uint32_t*)ad;
      else if(att.type==AttribType::UVEC2)inV.attributes[a].u2=*(glm::uvec2*)ad;
      else if(att.type==AttribType::UVEC3)inV.attributes[a].u3=*(glm::uvec3*)ad;
      else if(att.type==AttribType::UVEC4)inV.attributes[a].u4=*(glm::uvec4*)ad;
    }
    auto vs=prg.vertexShader;
    if(!vs)vs=[](OutVertex&o,InVertex const&,ShaderInterface const&){o.gl_Position=glm::vec4(0,0,0,1);};
    vs(ov[v%3],inV,si);
    if(v%3==2)clipRaster(mem,ov);
  }
}

void student_GPU_run(GPUMemory&mem,CommandBuffer const&cb){
  mem.gl_DrawID=0;
  for(uint32_t i=0;i<cb.nofCommands;++i){
    auto t=cb.commands[i].type;
    auto&d=cb.commands[i].data;
    if(t==CommandType::BIND_FRAMEBUFFER)mem.activatedFramebuffer=d.bindFramebufferCommand.id;
    else if(t==CommandType::BIND_PROGRAM)mem.activatedProgram=d.bindProgramCommand.id;
    else if(t==CommandType::BIND_VERTEXARRAY)mem.activatedVertexArray=d.bindVertexArrayCommand.id;
    else if(t==CommandType::BLOCK_WRITES_COMMAND)mem.blockWrites=d.blockWritesCommand.blockWrites;
    else if(t==CommandType::SET_BACKFACE_CULLING_COMMAND)mem.backfaceCulling.enabled=d.setBackfaceCullingCommand.enabled;
    else if(t==CommandType::SET_FRONT_FACE_COMMAND)mem.backfaceCulling.frontFaceIsCounterClockWise=d.setFrontFaceCommand.frontFaceIsCounterClockWise;
    else if(t==CommandType::SET_STENCIL_COMMAND)mem.stencilSettings=d.setStencilCommand.settings;
    else if(t==CommandType::SET_BLENDING_COMMAND)mem.blendingSettings=d.setBlendingCommand.settings;
    else if(t==CommandType::SET_DRAW_ID)mem.gl_DrawID=d.setDrawIdCommand.id;
    else if(t==CommandType::USER_COMMAND){auto&u=d.userCommand;if(u.callback)u.callback(u.data);}
    else if(t==CommandType::DRAW){draw(mem,d.drawCommand);mem.gl_DrawID++;}
    else if(t==CommandType::SUB_COMMAND){auto sub=d.subCommand.commandBuffer;if(sub)student_GPU_run(mem,*sub);}
    else if(t==CommandType::CLEAR_COLOR)clearColor(mem,d.clearColorCommand);
    else if(t==CommandType::CLEAR_DEPTH)clearDepth(mem,d.clearDepthCommand);
    else if(t==CommandType::CLEAR_STENCIL)clearStencil(mem,d.clearStencilCommand);
  }
}