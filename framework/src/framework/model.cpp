#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <framework/model.hpp>
#include <libs/tiny_gltf/tiny_gltf.h>

namespace tests{
void printModel(Model const&model);
}

std::string componentType2Str(int i){
  switch(i){
    case TINYGLTF_COMPONENT_TYPE_FLOAT         :return "float"   ;
    case TINYGLTF_COMPONENT_TYPE_DOUBLE        :return "double"  ;
    case TINYGLTF_COMPONENT_TYPE_BYTE          :return "int8_t"  ;
    case TINYGLTF_COMPONENT_TYPE_SHORT         :return "int16_t" ;
    case TINYGLTF_COMPONENT_TYPE_INT           :return "int32_t" ;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT  :return "uint32_t";
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:return "uint16_t";
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE :return "uint8_t" ;
    default:break;
  }
  return "unknow";
}

std::string accesstorType2Str(int i){
  switch(i){
    case TINYGLTF_TYPE_SCALAR:return "v1";
    case TINYGLTF_TYPE_VEC2  :return "v2";
    case TINYGLTF_TYPE_VEC3  :return "v3";
    case TINYGLTF_TYPE_VEC4  :return "v4";
    default:break;
  }
  return "unknow";
}

class ModelDataImpl{
  public:
    ModelDataImpl(){}
    ~ModelDataImpl(){}
    void load(std::string const&fileName);
    void createModelView(Model&model);
    void createModelViewRoots   (Model&res);
    void createModelViewTextures(Model&res);
    void createModelViewBuffers (Model&res);
    void createModelViewMeshes  (Model&res);

    bool               wasModelLoaded = false;
    tinygltf::Model    model                 ;
    tinygltf::TinyGLTF loader                ;
};

void ModelDataImpl::load(std::string const&fileName){
  std::string err;
  std::string warn;
  if(fileName.find(".glb")==fileName.length()-4)
    wasModelLoaded = loader.LoadBinaryFromFile(&model, &err, &warn, fileName.c_str());

  if(fileName.find(".gltf")==fileName.length()-5)
    wasModelLoaded = loader.LoadASCIIFromFile(&model, &err, &warn, fileName.c_str());

  if(!wasModelLoaded)
    std::cerr << "model: " << fileName << "was not loaded" << std::endl;
}

void loadMatrix(Node*outNode,tinygltf::Node const&root){
  if(root.matrix.size() == 16){
    for(int i=0;i<16;++i)
      outNode->modelMatrix[i/4][i%4] = (float)root.matrix[i];
  }else{
    if(root.translation.size() == 3){
      auto*p = root.translation.data();
      outNode->modelMatrix = outNode->modelMatrix*glm::translate(glm::mat4(1.f),glm::vec3(p[0],p[1],p[2]));
    }
    if(root.rotation.size() == 4){
      auto*p = root.rotation.data();
      glm::quat q;
      q[0]=(float)p[0];
      q[1]=(float)p[1];
      q[2]=(float)p[2];
      q[3]=(float)p[3];
      outNode->modelMatrix = outNode->modelMatrix*glm::toMat4(q);
    }
    if(root.scale.size() == 3){
      auto*p = root.scale.data();
      outNode->modelMatrix = outNode->modelMatrix*glm::scale(glm::mat4(1.f),glm::vec3(p[0],p[1],p[2]));
    }
  }
}

void ModelDataImpl::createModelView(Model&res){
  if(!wasModelLoaded)return;
  createModelViewRoots   (res);
  createModelViewTextures(res);
  createModelViewBuffers (res);
  createModelViewMeshes  (res);
}

void createNode(Node*outNode,tinygltf::Node const&root,tinygltf::Model const&model){
  outNode->mesh = root.mesh;
  loadMatrix(outNode,root);
  outNode->nofChildren = root.children.size();
  outNode->children    = new Node[outNode->nofChildren];
  for(size_t i=0;i<outNode->nofChildren;++i){
    auto const&c = root.children[i];
    createNode(outNode->children+i,model.nodes.at(c),model);
  }
}

void ModelDataImpl::createModelViewRoots(Model&res){
  auto const&scene = model.scenes.at(0);
  res.nofRoots = scene.nodes.size();
  res.roots    = new Node[res.nofRoots];
  for(size_t i=0;i<scene.nodes.size();++i){
    auto node_id = scene.nodes[i];
    auto const&root = model.nodes.at(node_id);
    createNode(res.roots+i,root,model);
  }
}

void createModelViewTexture(Texture&tex,tinygltf::Image const&img){
  tex.width             = img.width;
  tex.height            = img.height;
  tex.img.channels      = img.component;
  tex.img.data          = (void*)img.image.data();
  tex.img.bytesPerPixel = img.component;
  tex.img.format        = Image::U8;
  tex.img.pitch         = img.width*img.component;
}

void ModelDataImpl::createModelViewTextures(Model&res){
  res.nofTextures = model.images.size();
  res.textures    = new Texture[res.nofTextures];
  for(size_t i=0;i<res.nofTextures;++i)
    createModelViewTexture(res.textures[i],model.images[i]);
}

void ModelDataImpl::createModelViewBuffers(Model&res){
  res.nofBuffers = model.buffers.size();
  res.buffers = new Buffer[res.nofBuffers];
  for(size_t i=0;i<res.nofBuffers;++i){
    auto const&buf    = model.buffers[i];
    auto      &buffer = res  .buffers[i];
    buffer.data = (void const*)buf.data.data();
    buffer.size = buf.data.size();
  }
}

void ModelDataImpl::createModelViewMeshes(Model&res){
  res.nofMeshes=0;
  for(auto const&mesh:model.meshes){
    for(auto const&primitive:mesh.primitives){
      if(primitive.mode != TINYGLTF_MODE_TRIANGLES)continue;
      res.nofMeshes++;
    }
  }
  res.meshes = new Mesh[res.nofMeshes];

  size_t meshCounter = 0;
  for(auto const&mesh:model.meshes){
    
    //std::cerr <<__FILE__ << "/" << __LINE__ << std::endl;

    for(auto const&primitive:mesh.primitives){
      if(primitive.mode != TINYGLTF_MODE_TRIANGLES)continue;

      auto&m_mesh = res.meshes[meshCounter++];

      if (primitive.material >= 0) {
          //std::cerr << "material: " << primitive.material << std::endl;
          auto const& mat = model.materials.at(primitive.material);
          m_mesh.doubleSided = mat.doubleSided;
          auto baseColorTextureIndex = mat.pbrMetallicRoughness.baseColorTexture.index;
          //std::cerr << "baseColorTexture.Index: " << baseColorTextureIndex << std::endl;
          for (size_t i = 0; i < mat.pbrMetallicRoughness.baseColorFactor.size(); ++i)
              m_mesh.diffuseColor[(uint32_t)i] = (float)mat.pbrMetallicRoughness.baseColorFactor.at(i);
        if(baseColorTextureIndex<0){
          if(mat.extensions.find("KHR_materials_pbrSpecularGlossiness")!=std::end(mat.extensions)){
            auto&specGlo = mat.extensions.at("KHR_materials_pbrSpecularGlossiness");
            if(specGlo.Has("diffuseTexture")){
              m_mesh.diffuseTexture = specGlo.Get("diffuseTexture").Get("index").GetNumberAsInt();
            }else
              m_mesh.diffuseTexture = -1;
          }else
            m_mesh.diffuseTexture = -1;
        }else{
          //std::cerr << "texIndex: " << model.materials.at(primitive.material).pbrMetallicRoughness.baseColorTexture.index << std::endl;
          m_mesh.diffuseTexture = model.textures.at(mat.pbrMetallicRoughness.baseColorTexture.index).source;
        }
      }else
        m_mesh.diffuseTexture = -1;

      //std::cerr << "diffuseTex: " << m_mesh.diffuseTexture << std::endl;

      //m_mesh.texture.width    = model.images.at(0).width;
      //m_mesh.texture.height   = model.images.at(0).height;
      //m_mesh.texture.channels = model.images.at(0).component;
      //m_mesh.texture.data     = model.images.at(0).image.data();

      if(primitive.indices >= 0){
          auto const&ia  = model.accessors.at(primitive.indices);
          auto const&ibv = model.bufferViews.at(ia.bufferView);
          m_mesh.nofIndices = (uint32_t)ia.count;
          m_mesh.indexBufferID = ibv.buffer;
          m_mesh.indexOffset   = ibv.byteOffset + ia.byteOffset;
          //m_mesh.indices = model.buffers.at(ibv.buffer).data.data() + ibv.byteOffset + ia.byteOffset;
          //std::cerr << "  ibv : " << ia.bufferView ;
          //std::cerr << "  iNof: " << ia.count      ;
          //std::cerr << "  ibuf: " << ibv.buffer    ;
          //std::cerr << "  ioff: " << ibv.byteOffset;
          //std::cerr << std::endl;
          if(ia.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT  )m_mesh.indexType = IndexType::U32;
          if(ia.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)m_mesh.indexType = IndexType::U16;
          if(ia.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE )m_mesh.indexType = IndexType::U8 ;
      }else{
        //std::cerr << "dont have indices" << std::endl;
      }
    //std::cerr <<__FILE__ << "/" << __LINE__ << std::endl;

      //uint32_t nofVertices = 0;
      for(auto const&attrib:primitive.attributes){

        VertexAttrib*att = nullptr;
        auto const&accessor = model.accessors.at(attrib.second);

        //std::cerr << attrib.first << std::endl;
        //std::cerr << " sparse    : " << accessor.sparse.isSparse << std::endl;
        //std::cerr << " count     : " << accessor.count << std::endl;
        //std::cerr << " comp_type : " << componentType2Str(accessor.componentType) << std::endl;
        //std::cerr << " offset    : " << accessor.byteOffset << std::endl;
        //std::cerr << " bufferView: " << accessor.bufferView << std::endl;
        //std::cerr << " components: " << accesstorType2Str(accessor.type) << std::endl;
        if(std::string(attrib.first) == "POSITION"){
          att = &m_mesh.position;


          //m_mesh.nofIndices = accessor.count;


        }
        if(std::string(attrib.first) == "NORMAL"){
          att = &m_mesh.normal;
        }

        if(std::string(attrib.first) == "TEXCOORD_0"){
          att = &m_mesh.texCoord;
        }
    //std::cerr << __LINE__ << std::endl;

        if(att){
          auto const&bufferView = model.bufferViews.at(accessor.bufferView);
          auto bufId  = bufferView.buffer;
          auto stride = bufferView.byteStride;
          auto offset = bufferView.byteOffset;
          //auto size   = bufferView.byteLength;
          //auto bptr   = model.buffers.at(bufId).data.data() + accessor.byteOffset;

          att->bufferID   = bufId;
          att->offset     = offset + accessor.byteOffset;
          att->stride     = stride;

          if(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT){
            if(accessor.type == TINYGLTF_TYPE_SCALAR)att->type = AttribType::FLOAT;
            if(accessor.type == TINYGLTF_TYPE_VEC2  )att->type = AttribType::VEC2 ;
            if(accessor.type == TINYGLTF_TYPE_VEC3  )att->type = AttribType::VEC3 ;
            if(accessor.type == TINYGLTF_TYPE_VEC4  )att->type = AttribType::VEC4 ;
            if(att->stride == 0)att->stride = sizeof(float)*(uint32_t)att->type;
          }
          //std::cerr << "  bufId : " << bufId       << std::endl;
          //std::cerr << "  stride: " << att->stride << std::endl;
          //std::cerr << "  offset: " << att->offset << std::endl;
          //std::cerr << "  type  : " << (uint32_t)att->type  << std::endl;
          //float*p = (float*)(((uint8_t*)att->bufferData)+att->offset);
          //for(int i=0;i<4*(uint32_t)att->type;++i)
          //  std::cerr << p[i] << std::endl;
        }
      }
    //std::cerr << __LINE__ << std::endl;

      

    }
    //std::cerr << __LINE__ << std::endl;
    
  }
}


void ModelData::load(std::string const&fileName){
  impl->load(fileName);
}

ModelData::ModelData(){
  impl = new ModelDataImpl();
}

ModelData::~ModelData(){
  delete impl;
}

void ModelData::createModelView(Model&model){
  impl->createModelView(model);
}
