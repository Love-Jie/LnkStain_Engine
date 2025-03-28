#version 450

layout(location=1) in vec2 v_Texcoord;

struct TextureParam{
    bool  enable;
    float uvRotation;
    vec4  uvTransform;   // x,y --> scale, z,w --> translation
};

//根据传进来的纹理参数对纹理的UV坐标进行变换
vec2 getTextureUV(TextureParam param, vec2 inUV){
    vec2 retUV = inUV * param.uvTransform.xy;           // scale
    retUV = vec2(                                       // rotation
    retUV.x * sin(param.uvRotation) + retUV.y * cos(param.uvRotation),
    retUV.y * sin(param.uvRotation) + retUV.x * cos(param.uvRotation)
    );
    inUV = retUV + param.uvTransform.zw;                // translation
    return inUV;
}

layout(set=0, binding=0, std140) uniform FrameUbo{
    mat4  projMat;
    mat4  viewMat;
    ivec2 resolution;
    uint  frameId;
    float time;
} frameUbo;

//定义材质参数
layout(set=1, binding=0, std140) uniform MaterialUbo{
    vec3 baseColor0;   //基础颜色值
    vec3 baseColor1;   //基础颜色值
    float mixValue;    //两个颜色的混合度
    TextureParam textureParam0;     //采样贴图的纹理参数
    TextureParam textureParam1;     //采样贴图的纹理参数
} materialUbo;

layout(set=2, binding=0) uniform sampler2D texture0;
layout(set=2, binding=1) uniform sampler2D texture1;

layout(location=0) out vec4 fragColor;

void main(){
    vec3 color0 = materialUbo.baseColor0;
    vec3 color1 = materialUbo.baseColor1;

    //判断是否需要开启纹理采样
    if(materialUbo.textureParam0.enable){
        TextureParam param = materialUbo.textureParam0;
        param.uvTransform.w = -frameUbo.time;
        color0 = texture(texture0, getTextureUV(param, v_Texcoord)).rgb;
    }
    //判断是否需要开启纹理采样
    if(materialUbo.textureParam1.enable){
        color1 = texture(texture1, getTextureUV(materialUbo.textureParam1, v_Texcoord)).rgb;
    }

    fragColor = vec4(mix(color0, color1, materialUbo.mixValue), 1.0);
}
