#version 430

out vec4 color;
uniform bool use_texture;
uniform float k;

// параметры точечного источника освещения
uniform vec4 light_position;
uniform vec4 light_ambient;
uniform vec4 light_diffuse;
uniform vec4 light_specular;
uniform vec3 light_attenuation;

// параметры материала
uniform sampler2D material_texture;
uniform vec4 material_ambient;
uniform vec4 material_diffuse;
uniform vec4 material_specular;
uniform vec4 material_emission;
uniform float material_shininess;

// параметры, полученные из вершинного шейдера
in vec2 vert_texcoord;   // текстурные координаты
in vec3 vert_normal;     // нормаль
in vec3 vert_lightDir;   // направление на источник освещения
in vec3 vert_viewDir;    // направление от вершины к наблюдателю
in float vert_distance;  // расстояние от вершины до источника освещения
in vec3 vert_color;

void main(){
	vec3 normal = normalize(vert_normal);
	vec3 lightDir = normalize(vert_lightDir);
	vec3 viewDir = normalize(vert_viewDir);
	
	color = material_ambient * light_ambient;
	color += light_diffuse * material_diffuse * pow (max ( dot ( normal, lightDir ), 0.0 ), 1.0 + k) * pow ( 1.0 - dot ( normal, viewDir ), 1.0 - k );
	
	
	if(use_texture)
		color *= texture(material_texture, vert_texcoord);
}