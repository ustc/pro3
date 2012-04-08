varying vec3 ec_vnormal, ec_vposition;
uniform sampler2D mytexture;
uniform sampler2D mynormalmap;

void main() 
{
	vec3 P, N, L, V, H, tcolor;
	vec4 diffuse_color = gl_FrontMaterial.diffuse;
	vec4 specular_color = gl_FrontMaterial.specular;
	float shininess = gl_FrontMaterial.shininess;
	P = ec_vposition;
	N = normalize(ec_vnormal);
	L = normalize(gl_LightSource[0].position.xyz - P);
	V = normalize(-P);
	H = normalize(L+V);
	
	tcolor = vec3(texture2D(mynormalmap, gl_TexCoord[0].st));
	diffuse_color = 0.5*diffuse_color+0.5*vec4(tcolor,1.0);
	diffuse_color *= max(dot(N,L),0.0);
	specular_color *= pow(max(dot(H,N),0.0),shininess);
	gl_FragColor = diffuse_color + specular_color;
}
