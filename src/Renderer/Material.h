#ifndef MATERIAL_H
#define MATERIAL_H


class Material 
{
public:
    Material();
    explicit Material(const ShaderPtr& shader);

    template <typename InputType>
    void SetInput(const std::string& name, const InputType& value) {
        m_inputs.insert({name, std::any});
    }

    void ApplyUniforms() const;

private:
    // UniformBufferPtr m_uniforms;
    std::unordered_map<std::string, std::any> m_inputs;
    ShaderPtr m_shader;
};


#endif  // MATERIAL_H