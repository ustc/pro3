varying vec3 ec_vnormal, ec_vposition;
uniform sampler2D mytexture;
uniform sampler2D mynormalmap;
uniform sampler2D background;

void main() 
{
	vec2 d_irr_index, s_irr_index;
	vec3 d_irr, s_irr;
	vec3 P, N, L, V, H, R, tcolor;
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
        d_irr = vec3(texture2D(background, d_irr_index));
        s_irr = vec3(texture2D(mytexture, s_irr_index));
        diffuse_color *= vec4(d_irr, 1.0);
        specular_color *= vec4(s_irr, 1.0);
    	gl_FragColor = diffuse_color + 0.5*specular_color;

/*	tcolor = vec3(texture2D(background, gl_TexCoord[0].st));
	//diffuse_color = 0.5*diffuse_color+0.5*vec4(tcolor,1.0);
	diffuse_color = vec4(tcolor,1.0);
	diffuse_color *= max(dot(N,L),0.0);
	specular_color *= pow(max(dot(H,N),0.0),shininess);
	gl_FragColor = diffuse_color + specular_color;
*/
}
