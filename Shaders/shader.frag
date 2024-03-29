#version 330

out vec4 colour;
in vec2 TexCoord;
in vec4 vCol;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture2D;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
float ambientStrength = 0.2f; 

vec3 specularLight() 
{ 
    float specularStrength = 0.8f; 
    float shininess = 64.0f; 
 
    vec3 lightDir = normalize(lightPos - FragPos); 
    vec3 norm = normalize(Normal); 
    vec3 reflectDir = reflect(-lightDir, norm); 
    vec3 viewDir = normalize(viewPos - FragPos); 
 
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), shininess); 
 
    vec3 specular = lightColor * spec * specularStrength; 
 
    return specular;
}

vec3 ambientLight (){
    vec3 ambient = lightColor* ambientStrength;
    return ambient;
}

vec3 diffuseLight() 
{ 
    float diffuseStrength = 1.0f; 
 
    vec3 lightDir = normalize(lightPos - FragPos); 
    vec3 norm = normalize(Normal); 
 
    float diff = max(dot(norm, lightDir), 0.0f); 
    vec3 diffuse = lightColor * diff * diffuseStrength; 
    return diffuse; 
}

void main()
{
    colour =  texture(texture2D, TexCoord) * vec4(ambientLight() + diffuseLight() + specularLight(),1.0f);
    // colour = vCol; 

}