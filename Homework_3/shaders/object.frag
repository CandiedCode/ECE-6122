#version 330 core
out vec4 FragColor;
in vec3 FragPos; in vec3 Normal; in vec2 TexCoords;
struct DirLight { vec3 direction, ambient, diffuse, specular; };
struct PointLight {
    vec3 position, ambient, diffuse, specular;
    float constant, linear, quadratic;
};

uniform DirLight sun;
uniform PointLight lantern;
uniform vec3 viewPos;
uniform sampler2D diffuseMap;
uniform float shininess;

vec3 CalcDirLight(DirLight l, vec3 n, vec3 vd, vec3 tc) {
    vec3 ld = normalize(l.direction);
    float d = max(dot(n, ld), 0.0);
    float s = pow(max(dot(vd, reflect(-ld, n)), 0.0), shininess);
    return (l.ambient + d*l.diffuse + s*l.specular) * tc;
}

vec3 CalcPointLight(PointLight l, vec3 n, vec3 fp, vec3 vd, vec3 tc) {
    vec3 ld = normalize(l.position - fp);
    float d = max(dot(n, ld), 0.0);
    float s = pow(max(dot(vd, reflect(-ld, n)), 0.0), shininess);
    float dst = length(l.position - fp);
    float att = 1.0 / (l.constant + l.linear*dst + l.quadratic*dst*dst);
    return att * (l.ambient + d*l.diffuse + s*l.specular) * tc;
}

void main() {
    vec3 n = normalize(Normal);
    vec3 vd = normalize(viewPos - FragPos);
    vec3 tc = vec3(texture(diffuseMap, TexCoords));
    FragColor = vec4(
        CalcDirLight(sun, n, vd, tc) +
        CalcPointLight(lantern, n, FragPos, vd, tc), 1.0);
}
