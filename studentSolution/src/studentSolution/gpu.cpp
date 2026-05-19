#include <studentSolution/gpu.hpp>
#include <cstring>

namespace izg2026{
  void createShadowMap_fs(OutFragment&,InFragment const&,ShaderInterface const&);
}

static void clearColor(GPUMemory&mem,ClearColorCommand const&cmd){
  Framebuffer*f=mem.framebuffers+mem.activatedFramebuffer;
  if(!f->color.data)return;
  glm::vec4 v=cmd.value*255.f;
  for(uint32_t y=0;y<f->height;++y){
    uint8_t* row=(uint8_t*)f->color.data+y*f->color.pitch;
    for(uint32_t x=0;x<f->width;++x){
      uint8_t* p=row+x*f->color.bytesPerPixel;
      for(uint32_t c=0;c<f->color.channels;++c)p[f->color.channelTypes[c]]=(uint8_t)(v[c]);
    }
  }
}
static void clearDepth(GPUMemory&mem,ClearDepthCommand const&cmd){
  Framebuffer*f=mem.framebuffers+mem.activatedFramebuffer;
  if(!f->depth.data)return;
  float v=cmd.value;
  for(uint32_t y=0;y<f->height;++y){
    uint8_t* row=(uint8_t*)f->depth.data+y*f->depth.pitch;
    for(uint32_t x=0;x<f->width;++x)*(float*)(row+x*f->depth.bytesPerPixel)=v;
  }
}
static void clearStencil(GPUMemory&mem,ClearStencilCommand const&cmd){
  Framebuffer*f=mem.framebuffers+mem.activatedFramebuffer;
  if(!f->stencil.data)return;
  uint8_t v=cmd.value;
  memset(f->stencil.data,v,f->stencil.pitch*f->height);
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
  float hInv[3]={1.f/v[0].gl_Position.w,1.f/v[1].gl_Position.w,1.f/v[2].gl_Position.w};
  glm::vec3 scr[3];
  for(int i=0;i<3;++i){
    float ndcX=v[i].gl_Position.x*hInv[i];
    float ndcY=v[i].gl_Position.y*hInv[i];
    scr[i].x=(ndcX*.5f+.5f)*w;
    scr[i].y=(ndcY*.5f+.5f)*h;
    scr[i].z=v[i].gl_Position.z*hInv[i];
  }
  float A=edge(glm::vec2(scr[0]),glm::vec2(scr[1]),glm::vec2(scr[2])); if(A==0.f)return;
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
  float invA=1.f/A;
  auto&ss=mem.stencilSettings;
  auto&bs=mem.blendingSettings;
  StencilOps ops=isFrontFace?ss.frontOps:ss.backOps;

  uint32_t activeAttribs[maxAttribs];
  uint32_t nActiveAttribs=0;
  bool allFloatAttribs=true;
  for(uint32_t a=0;a<maxAttribs;++a){
    if(p.vs2fs[a]!=AttribType::EMPTY){
      activeAttribs[nActiveAttribs++]=a;
      if(static_cast<int>(p.vs2fs[a])>static_cast<int>(AttribType::VEC4))allFloatAttribs=false;
    }
  }

  bool hasStencil=f->stencil.data!=nullptr;
  bool hasDepth=f->depth.data!=nullptr;
  bool hasColor=f->color.data!=nullptr;

  float l0_dx = scr[2].y - scr[1].y;
  float l0_dy = scr[1].x - scr[2].x;
  float l0_c  = scr[1].y * scr[2].x - scr[1].x * scr[2].y;
  float l1_dx = scr[0].y - scr[2].y;
  float l1_dy = scr[2].x - scr[0].x;
  float l1_c  = scr[2].y * scr[0].x - scr[2].x * scr[0].y;
  float l2_dx = scr[1].y - scr[0].y;
  float l2_dy = scr[0].x - scr[1].x;
  float l2_c  = scr[0].y * scr[1].x - scr[0].x * scr[1].y;
  if(cw){
    l0_dx=-l0_dx;l0_dy=-l0_dy;l0_c=-l0_c;
    l1_dx=-l1_dx;l1_dy=-l1_dy;l1_c=-l1_c;
    l2_dx=-l2_dx;l2_dy=-l2_dy;l2_c=-l2_c;
  }

  if(nActiveAttribs==0&&!hasColor&&p.fragmentShader==izg2026::createShadowMap_fs){
    for(int y=y0;y<=y1;++y){
      uint8_t* stencilRow=hasStencil?((uint8_t*)f->stencil.data+y*f->stencil.pitch):nullptr;
      uint8_t* depthRow=hasDepth?((uint8_t*)f->depth.data+y*f->depth.pitch):nullptr;
      uint8_t* stencilPx=hasStencil?(stencilRow+x0*f->stencil.bytesPerPixel):nullptr;
      uint8_t* depthPx=hasDepth?(depthRow+x0*f->depth.bytesPerPixel):nullptr;
      float yc = y + 0.5f;
      float l0_row = l0_dx * (x0 + 0.5f) + l0_dy * yc + l0_c;
      float l1_row = l1_dx * (x0 + 0.5f) + l1_dy * yc + l1_c;
      float l2_row = l2_dx * (x0 + 0.5f) + l2_dy * yc + l2_c;
      for(int x=x0;x<=x1;++x){
        float l0=l0_row;
        float l1=l1_row;
        float l2=l2_row;
        if(l0<=0.f||l1<=0.f||l2<=0.f){
          l0_row+=l0_dx;l1_row+=l1_dx;l2_row+=l2_dx;
          if(stencilPx)stencilPx+=f->stencil.bytesPerPixel;
          if(depthPx)depthPx+=f->depth.bytesPerPixel;
          continue;
        }
        l0*=invA;l1*=invA;l2*=invA;
        float depthZ=l0*scr[0].z+l1*scr[1].z+l2*scr[2].z;
        uint8_t stencilVal=0;
        if(stencilPx)stencilVal=*stencilPx;
        if(ss.enabled&&stencilPx){
          if(!stencilTest(stencilVal,ss.refValue,ss.func)){
            if(!mem.blockWrites.stencil)*stencilPx=applyStencilOp(stencilVal,ops.sfail,ss.refValue);
            l0_row+=l0_dx;l1_row+=l1_dx;l2_row+=l2_dx;
            stencilPx+=f->stencil.bytesPerPixel;
            if(depthPx)depthPx+=f->depth.bytesPerPixel;
            continue;
          }
        }
        bool depthPass=true;
        float oldDepth=0.f;
        if(depthPx){
          oldDepth=*(float*)depthPx;
          depthPass=depthZ<oldDepth;
        }
        if(!depthPass){
          if(ss.enabled&&stencilPx&&!mem.blockWrites.stencil)*stencilPx=applyStencilOp(stencilVal,ops.dpfail,ss.refValue);
          l0_row+=l0_dx;l1_row+=l1_dx;l2_row+=l2_dx;
          if(stencilPx)stencilPx+=f->stencil.bytesPerPixel;
          if(depthPx)depthPx+=f->depth.bytesPerPixel;
          continue;
        }
        if(ss.enabled&&stencilPx&&!mem.blockWrites.stencil)*stencilPx=applyStencilOp(stencilVal,ops.dppass,ss.refValue);
        if(!mem.blockWrites.depth&&depthPx)*(float*)depthPx=depthZ;
        l0_row+=l0_dx;l1_row+=l1_dx;l2_row+=l2_dx;
        if(stencilPx)stencilPx+=f->stencil.bytesPerPixel;
        if(depthPx)depthPx+=f->depth.bytesPerPixel;
      }
    }
    return;
  }

  for(int y=y0;y<=y1;++y){
    uint8_t* stencilRow=hasStencil?((uint8_t*)f->stencil.data+y*f->stencil.pitch):nullptr;
    uint8_t* depthRow=hasDepth?((uint8_t*)f->depth.data+y*f->depth.pitch):nullptr;
    uint8_t* colorRow=hasColor?((uint8_t*)f->color.data+y*f->color.pitch):nullptr;

    uint8_t* stencilPx=hasStencil?(stencilRow+x0*f->stencil.bytesPerPixel):nullptr;
    uint8_t* depthPx=hasDepth?(depthRow+x0*f->depth.bytesPerPixel):nullptr;
    uint8_t* colorPx=hasColor?(colorRow+x0*f->color.bytesPerPixel):nullptr;

    float yc = y + 0.5f;
    float l0_row = l0_dx * (x0 + 0.5f) + l0_dy * yc + l0_c;
    float l1_row = l1_dx * (x0 + 0.5f) + l1_dy * yc + l1_c;
    float l2_row = l2_dx * (x0 + 0.5f) + l2_dy * yc + l2_c;

    for(int x=x0;x<=x1;++x){
      float l0=l0_row;
      float l1=l1_row;
      float l2=l2_row;
      if(l0<=0.f||l1<=0.f||l2<=0.f){
        l0_row+=l0_dx;l1_row+=l1_dx;l2_row+=l2_dx;
        if(stencilPx)stencilPx+=f->stencil.bytesPerPixel;
        if(depthPx)depthPx+=f->depth.bytesPerPixel;
        if(colorPx)colorPx+=f->color.bytesPerPixel;
        continue;
      }

      l0*=invA;l1*=invA;l2*=invA;
      float depthZ=l0*scr[0].z+l1*scr[1].z+l2*scr[2].z;
      float p0=l0*hInv[0];
      float p1=l1*hInv[1];
      float p2=l2*hInv[2];
      float zInv=p0+p1+p2;
      float invZ=1.0f/zInv;
      float lP[3]={p0*invZ,p1*invZ,p2*invZ};

      uint8_t stencilVal=0;
      if(stencilPx)stencilVal=*stencilPx;
      if(ss.enabled&&stencilPx){
        if(!stencilTest(stencilVal,ss.refValue,ss.func)){
          if(!mem.blockWrites.stencil)*stencilPx=applyStencilOp(stencilVal,ops.sfail,ss.refValue);
          l0_row+=l0_dx;l1_row+=l1_dx;l2_row+=l2_dx;
          stencilPx+=f->stencil.bytesPerPixel;
          if(depthPx)depthPx+=f->depth.bytesPerPixel;
          if(colorPx)colorPx+=f->color.bytesPerPixel;
          continue;
        }
      }
      bool depthPass=true;
      float oldDepth=0.f;
      if(depthPx){
        oldDepth=*(float*)depthPx;
        depthPass=depthZ<oldDepth;
      }
      if(!depthPass){
        if(ss.enabled&&stencilPx&&!mem.blockWrites.stencil)*stencilPx=applyStencilOp(stencilVal,ops.dpfail,ss.refValue);
        l0_row+=l0_dx;l1_row+=l1_dx;l2_row+=l2_dx;
        if(stencilPx)stencilPx+=f->stencil.bytesPerPixel;
        if(depthPx)depthPx+=f->depth.bytesPerPixel;
        if(colorPx)colorPx+=f->color.bytesPerPixel;
        continue;
      }
      if(ss.enabled&&stencilPx&&!mem.blockWrites.stencil)*stencilPx=applyStencilOp(stencilVal,ops.dppass,ss.refValue);

      InFragment inF;inF.gl_FragCoord=glm::vec4(x+.5f,y+.5f,depthZ,1.f);
      if(allFloatAttribs){
        if(p.vs2fs[0]!=AttribType::EMPTY)inF.attributes[0].v4=v[0].attributes[0].v4*lP[0]+v[1].attributes[0].v4*lP[1]+v[2].attributes[0].v4*lP[2];
        if(p.vs2fs[1]!=AttribType::EMPTY)inF.attributes[1].v4=v[0].attributes[1].v4*lP[0]+v[1].attributes[1].v4*lP[1]+v[2].attributes[1].v4*lP[2];
        if(p.vs2fs[2]!=AttribType::EMPTY)inF.attributes[2].v4=v[0].attributes[2].v4*lP[0]+v[1].attributes[2].v4*lP[1]+v[2].attributes[2].v4*lP[2];
        if(p.vs2fs[3]!=AttribType::EMPTY)inF.attributes[3].v4=v[0].attributes[3].v4*lP[0]+v[1].attributes[3].v4*lP[1]+v[2].attributes[3].v4*lP[2];
      }else{
        for(uint32_t ai=0;ai<nActiveAttribs;++ai){
          uint32_t a=activeAttribs[ai];
          switch(p.vs2fs[a]){
            case AttribType::FLOAT: inF.attributes[a].v1=v[0].attributes[a].v1*lP[0]+v[1].attributes[a].v1*lP[1]+v[2].attributes[a].v1*lP[2]; break;
            case AttribType::VEC2:  inF.attributes[a].v2=v[0].attributes[a].v2*lP[0]+v[1].attributes[a].v2*lP[1]+v[2].attributes[a].v2*lP[2]; break;
            case AttribType::VEC3:  inF.attributes[a].v3=v[0].attributes[a].v3*lP[0]+v[1].attributes[a].v3*lP[1]+v[2].attributes[a].v3*lP[2]; break;
            case AttribType::VEC4:  inF.attributes[a].v4=v[0].attributes[a].v4*lP[0]+v[1].attributes[a].v4*lP[1]+v[2].attributes[a].v4*lP[2]; break;
            case AttribType::UINT:  inF.attributes[a].u1=(uint32_t)round(v[0].attributes[a].u1*lP[0]+v[1].attributes[a].u1*lP[1]+v[2].attributes[a].u1*lP[2]); break;
            default: break;
          }
        }
      }
      OutFragment outF;
      if(p.fragmentShader&&p.fragmentShader!=izg2026::createShadowMap_fs)p.fragmentShader(outF,inF,si);
      if(outF.discard){
        l0_row+=l0_dx;l1_row+=l1_dx;l2_row+=l2_dx;
        if(stencilPx)stencilPx+=f->stencil.bytesPerPixel;
        if(depthPx)depthPx+=f->depth.bytesPerPixel;
        if(colorPx)colorPx+=f->color.bytesPerPixel;
        continue;
      }

      if(!mem.blockWrites.color&&colorPx){
        uint8_t* px=colorPx;
        glm::vec4 fragColor=outF.gl_FragColor;
        if(bs.enabled){
          glm::vec4 dstColor;
          if(f->color.channels==4){
            dstColor.r=float(px[f->color.channelTypes[0]])/255.f;
            dstColor.g=float(px[f->color.channelTypes[1]])/255.f;
            dstColor.b=float(px[f->color.channelTypes[2]])/255.f;
            dstColor.a=float(px[f->color.channelTypes[3]])/255.f;
          }else{
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
          }
          if(bs.equation==BlendEquation::MIN)fragColor=glm::min(fragColor,dstColor);
          else if(bs.equation==BlendEquation::MAX)fragColor=glm::max(fragColor,dstColor);
          else if(bs.equation==BlendEquation::ADD && bs.sFactor==BlendFunc::SRC_ALPHA && bs.dFactor==BlendFunc::ONE_MINUS_SRC_ALPHA){
            float a=fragColor.a;
            fragColor=fragColor*a+dstColor*(1.f-a);
          }else{
            glm::vec4 sf=getBlendFactor(bs.sFactor,fragColor,dstColor);
            glm::vec4 df=getBlendFactor(bs.dFactor,fragColor,dstColor);
            fragColor=applyBlendEquation(bs.equation,fragColor*sf,dstColor*df);
          }
          // no clamp - let wraparound happen for negative values
        }
        for(uint32_t c=0;c<f->color.channels;++c)
          px[f->color.channelTypes[c]]=(uint8_t)(fragColor[c]*255.f);
      }
      if(!mem.blockWrites.depth&&depthPx)*(float*)depthPx=depthZ;

      l0_row+=l0_dx;l1_row+=l1_dx;l2_row+=l2_dx;
      if(stencilPx)stencilPx+=f->stencil.bytesPerPixel;
      if(depthPx)depthPx+=f->depth.bytesPerPixel;
      if(colorPx)colorPx+=f->color.bytesPerPixel;
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
      switch(att.type){
        case AttribType::FLOAT: inV.attributes[a].v1=*(float*)ad; break;
        case AttribType::VEC2:  inV.attributes[a].v2=*(glm::vec2*)ad; break;
        case AttribType::VEC3:  inV.attributes[a].v3=*(glm::vec3*)ad; break;
        case AttribType::VEC4:  inV.attributes[a].v4=*(glm::vec4*)ad; break;
        case AttribType::UINT:  inV.attributes[a].u1=*(uint32_t*)ad; break;
        case AttribType::UVEC2: inV.attributes[a].u2=*(glm::uvec2*)ad; break;
        case AttribType::UVEC3: inV.attributes[a].u3=*(glm::uvec3*)ad; break;
        case AttribType::UVEC4: inV.attributes[a].u4=*(glm::uvec4*)ad; break;
        default: break;
      }
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
