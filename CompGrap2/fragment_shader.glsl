#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;

struct Light {
    vec3 Position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Light light_1;
uniform Material material;

void main()
{
    // ambient
    vec3 ambient = light_1.ambient * material.ambient;
    
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light_1.Position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light_1.diffuse * (diff * material.diffuse);
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light_1.specular * (spec * material.specular);
    
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}