#pragma once

#include "pointLight.h"

class SpotLight : public PointLight {
public:
    
    SpotLight();
    virtual ~SpotLight();

    /*  Set the direction of the light */
    void setDirection(const glm::vec3 &_dir);
    
    /*  Set the properties of the cutoff light cone */
    void setCutOff(float _lightConeRadiants, float _lieghtExponent);
    
    /*  GLSL #defines with the NUMBER of lights of this type */
    static std::string getArrayDefinesBlock(int _numberOfLights);

    /*  GLSL #defines with the NUMBER of lights of this type */
    static std::string getArrayUniformBlock();

    /*  GLSL block code with structs and need functions for this light type */
    static std::string getClassBlock();

    virtual std::string getInstanceDefinesBlock();

    virtual void setupProgram( ShaderProgram &_program );
    
protected:
   	glm::vec3 m_direction;

   	float m_spotExponent;
    float m_spotCutoff;
    float m_spotCosCutoff;
};
