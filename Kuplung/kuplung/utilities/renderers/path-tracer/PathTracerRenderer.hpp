//
//  PathTracerRenderer.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef PathTracerRenderer_hpp
#define PathTracerRenderer_hpp

#include "kuplung/utilities/gl/GLUtils.hpp"
#include "kuplung/meshes/scene/ModelFaceBase.hpp"
#include "kuplung/meshes/scene/ModelFaceData.hpp"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/utilities/renderers/KuplungRendererBase.hpp"

class PathTracerRenderer: public KuplungRendererBase {
public:
    explicit PathTracerRenderer(ObjectsManager &managerObjects);
    ~PathTracerRenderer();
    void init();
    std::string renderImage(FBEntity file, std::vector<ModelFaceBase*> *meshModelFaces);

private:
    FBEntity fileOutputImage;

    std::unique_ptr<GLUtils> glUtils;
    ObjectsManager &managerObjects;

    GLuint renderFBO, renderRBO, renderTextureColorBuffer;
    void createFBO();
    void generateAttachmentTexture(GLboolean depth, GLboolean stencil);
    void renderSceneToFBO(std::vector<ModelFaceBase*> *meshModelFaces);
};

#endif /* PathTracerRenderer_hpp */
