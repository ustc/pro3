varying vec3 ec_vnormal, ec_vposition, ec_vtangent, ec_vbitangent;
uniform sampler2D mytexture;
uniform sampler2D mynormalmap;
uniform sampler2D background;
uniform sampler2D myenvmap;

void main() 
{
	vec2 d_irr_index, s_irr_index;
	vec4 d_irr, s_irr,t_color;
	vec3 P, N, L, V, H, R, mapN;
	mat3 tform;
	vec4 diffuse_color = gl_FrontMaterial.diffuse;
	vec4 specular_color = gl_FrontMaterial.specular;
	float shininess = gl_FrontMaterial.shininess;
	P = ec_vposition;
	N = normalize(ec_vnormal);
	L = normalize(gl_LightSource[0].position.xyz - P);
	V = normalize(-P);
	H = normalize(L+V);

	N = 0.99*N; 
	R = 0.99*normalize(-V+2*(dot(V, N))*N);
        d_irr_index = 0.5*(N.st + vec2(1.0,1.0)); 
        s_irr_index = 0.5*(R.st + vec2(1.0,1.0)); 
        d_irr = vec4(texture2D(myenvmap, d_irr_index), 1.0);
        s_irr = vec4(texture2D(myenvmap, s_irr_index), 1.0);
    
    //normal map
    mapN = normalize(2.0*vec3(texture2D(mynormalmap, gl_TexCoord[0].st))
                   -vec3(1.0, 1.0, 1.0));
    tform = mat3(normalize(ec_vtangent), normalize(ec_vbitangent),
              N);
    //N = normalize(tform*mapN);
    
    t_color = vec4(texture2D(mytexture,  gl_TexCoord[0].st), 1.0);
    diffuse_color = 0.5*diffuse_color+0.5*t_color;
	diffuse_color = t_color;
	diffuse_color *= max(dot(N,L), 0.0)*d_irr;
	specular_color *= pow(max(dot(H,N), 0.0), shininess)*s_irr;
	
	gl_FragColor = diffuse_color + specular_color;
}
