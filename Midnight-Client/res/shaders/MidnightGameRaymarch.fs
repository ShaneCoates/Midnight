#version 150

smooth in vec2 TexCoord;

out vec4 outColor;

struct DistObjectIndexPair
{
	float dist;
	int colorIndex;
};

vec4[3] m_objectColors = vec4[3](
	vec4(0.75f, 0.25f, 0.25f, 1), //WALL
	vec4(135.0f / 255.0f, 179.0f / 255.0f, 122.0f / 255.0f, 1), //PLAYER
	vec4(119.0f / 255.0f, 134.0f / 255.0f, 127.0f / 255.0f, 1) // GUN 
);


// Camera
uniform vec2 m_resolution;
uniform vec3 m_camUp;
uniform vec3 m_camRight;
uniform vec3 m_camForward;
uniform vec3 m_eye;
uniform float m_focalLength;
uniform float m_zNear;
uniform float m_zFar;
uniform float m_aspectRatio;

// Raymarch parameters
uniform int m_rmSteps; // Max steps
uniform float m_rmEpsilon; // Distance threshold

// Scene
uniform vec4 m_skyColor;
uniform vec4 m_ambient;
uniform vec3 m_directionalLightDir;
uniform vec4 m_directionalLightColor;
uniform vec3 m_pointLightPos;
uniform vec4 m_pointLightColor;

uniform float m_time;

//Players
uniform vec4 m_player1Pos;
uniform vec4 m_player1Data;

//Debug options
uniform int m_MSAA;
uniform int m_AO;

// Rotates a point t radians around the y-axis
vec3 rotateY(vec3 v, float t)
{
	float cost = cos(t); float sint = sin(t);
	return vec3(v.x * cost + v.z * sint, v.y, -v.x * sint + v.z * cost);
}

// Rotates a point t radians around the x-axis
vec3 rotateX(vec3 v, float t)
{
	float cost = cos(t); float sint = sin(t);
	return vec3(v.x, v.y * cost - v.z * sint, v.y * sint + v.z * cost);
}

// Maps x from the range [minX, maxX] to the range [minY, maxY]
// The function does not clamp the result, as it may be useful
float mapTo(float x, float minX, float maxX, float minY, float maxY)
{
	float a = (maxY - minY) / (maxX - minX);
	float b = minY - a * minX;
	return a * x + b;
}

// Returns the signed distance to a sphere at the origin
float sdSphere(vec3 p, float radius)
{
	return length(p) - radius;
}

// Returns the unsigned distance estimate to a box at the origin of the given size
float udBox(vec3 p, vec3 size)
{
	return length(max(abs(p) - size, vec3(0.0f)));
}

// Returns the signed distance estimate to a box at the origin of the given size
float sdBox(vec3 p, vec3 size)
{
	vec3 d = abs(p) - size;
	return min(max(d.x, max(d.y, d.z)), 0.0f) + udBox(p, size);
}

// Subtracts d1 from d0, assuming d1 is a signed distance
float opSubtract(float d0, float d1)
{
	return max(d0, -d1);
}

mat4 matRotate( in vec3 xyz )
{
    vec3 si = sin(xyz);
    vec3 co = cos(xyz);

	return mat4( co.y*co.z,                co.y*si.z,               -si.y,       0.0,
                 si.x*si.y*co.z-co.x*si.z, si.x*si.y*si.z+co.x*co.z, si.x*co.y,  0.0,
                 co.x*si.y*co.z+si.x*si.z, co.x*si.y*si.z-si.x*co.z, co.x*co.y,  0.0,
			     0.0,                      0.0,                      0.0,        1.0 );
}

mat4 matTranslate( float x, float y, float z )
{
    return mat4( 1.0, 0.0, 0.0, 0.0,
				 0.0, 1.0, 0.0, 0.0,
				 0.0, 0.0, 1.0, 0.0,
				 x,   y,   z,   1.0 );
}

mat4 matInverse( in mat4 m )
{
	return mat4(
        m[0][0], m[1][0], m[2][0], 0.0,
        m[0][1], m[1][1], m[2][1], 0.0,
        m[0][2], m[1][2], m[2][2], 0.0,
        -dot(m[0].xyz,m[3].xyz),
        -dot(m[1].xyz,m[3].xyz),
        -dot(m[2].xyz,m[3].xyz),
        1.0 );
}

DistObjectIndexPair opU(DistObjectIndexPair d1, DistObjectIndexPair d2)
{
	return d1.dist < d2.dist ? d1 : d2;
}



DistObjectIndexPair distPlayer(vec3 p, vec3 playerPos, float playerRot)
{
	mat4 rot = matRotate(vec3(0.0f, playerRot, 0.0f));
	mat4 leftTra = matTranslate(0f, 0f, 0.1f *	(1 - m_player1Data.x));
	mat4 rightTra = matTranslate(0f, 0f, 0.1f * (1 - m_player1Data.y));

    mat4 leftMat = matInverse(rot * leftTra);
	mat4 rightMat = matInverse(rot * rightTra);

	vec3 leftP = (leftMat*vec4(p - playerPos, 1.0)).xyz;
	vec3 rightP = (rightMat*vec4(p - playerPos, 1.0)).xyz;


	float guns =		sdBox(leftP + vec3(0.24f, 0, 0), vec3(0.02f, 0.05f, 0.25f));
	guns = min(guns,	sdBox(rightP - vec3(0.24f, 0, 0), vec3(0.02f, 0.05f, 0.25f)));

	return opU(DistObjectIndexPair(guns, 2), DistObjectIndexPair(sdSphere(p - playerPos, 0.25f), 1));
}



vec3 outerWall = vec3(10f, 0.5f, 10f);
vec3 innerWall = vec3(9f, 1f, 9f);
// Defines the distance field for the scene
DistObjectIndexPair distScene(vec3 p)
{
	//Walls
	vec3 wallPos = p - vec3(0f, 0.25f, 0f);
	DistObjectIndexPair dist = DistObjectIndexPair(opSubtract(sdBox(wallPos, outerWall), sdBox(wallPos, innerWall)), 0);

	//dist = opU(dist, DistObjectIndexPair(sdBox(wallPos, vec3(1f, 0.5f, 1f)), 0));

	//Fake player
	dist = opU(dist, distPlayer(p, vec3(m_player1Pos.xyz), m_player1Pos.w));
	
	//dist = min(dist, distPlayer(p, vec3(sin(m_time), 0.1f, 4), m_time));

	//dist = min(dist, distPlayer(p, vec3(sin(m_time), 0.1f, 0), m_time));

	//dist = min(dist, distPlayer(p, vec3(sin(m_time), 0.1f, -2), m_time));



	return dist;
}

// To improve performance we raytrace the floor
// n: floor normal
// o: floor position
float raytraceFloor(vec3 ro, vec3 rd, vec3 n, vec3 o)
{
	return dot(o - ro, n) / dot(rd, n);
}

// Calcs intersection and exit distances, and normal at intersection
//
// The box is axis aligned and at the origin, but has any size.
// For arbirarily oriented and located boxes, simply transform
// the ray accordingly and reverse the transformation for the
// returning normal(without translation)
//
vec2 boxIntersection( vec3 ro, vec3 rd, vec3 boxSize ) 
{
    vec3 m = 1.0/rd;
    vec3 n = m*ro;
    vec3 k = abs(m)*boxSize;
	
    vec3 t1 = -n - k;
    vec3 t2 = -n + k;

    float tN = max( max( t1.x, t1.y ), t1.z );
    float tF = min( min( t2.x, t2.y ), t2.z );
	
    if( tN > tF || tF < 0.0) return vec2(-1.0); // no intersection
    
    return vec2( tN, tF );
}


// Finds the closest intersecting object along the ray at origin ro, and direction rd.
// i: step count
// t: distance traveled by the ray
int raymarch(vec3 ro, vec3 rd, out int i, out float t)
{
	vec3 size = vec3(100f, 1.8f, 100f);
	vec2 intersect = boxIntersection(ro, rd, size);

	t = intersect.x;
	float maxT = intersect.y;
	if(t < 0)
	{
		t = 10000;
		return 0;
	}
	DistObjectIndexPair dist;
	for(i = 0; i < m_rmSteps; ++i)
	{
		dist = distScene(ro + rd * t);

		// We make epsilon proportional to t so that we drop accuracy the further into the scene we get
		// We also drop the ray as soon as it leaves the clipping volume as defined by g_zFar
		if(dist.dist < m_rmEpsilon * t * 2.0f || t > maxT)
			break;
		t += dist.dist;
	}
	return dist.colorIndex;
}

// Returns a value between [0, 1] depending on how visible p0 is from p1
// k: denotes the soft-shadow strength
// See http://www.iquilezles.org/www/articles/rmshadows/rmshadows.htm
float getVisibility(vec3 p0, vec3 p1, float k)
{
	vec3 rd = normalize(p1 - p0);
	float t = 10.0f * m_rmEpsilon;
	float maxt = length(p1 - p0);
	float f = 1.0f;
	while(t < maxt)
	{
		float d = distScene(p0 + rd * t).dist;

		// A surface was hit before we reached p1
		if(d < m_rmEpsilon)
			return 0.0f;

		// Penumbra factor
		f = min(f, k * d / t);

		t += d;
	}

	return f;
}

// Approximates the (normalized) gradient of the distance function at the given point.
// If p is near a surface, the function will approximate the surface normal.
vec3 getNormal(vec3 ro, vec3 rd, float t)
{
	float h = 0.0001f;
	vec3 p = ro + rd * t;
	return normalize(vec3(
		distScene(ro + vec3(h, 0, 0) + rd * t).dist - distScene(ro - vec3(h, 0, 0) + rd * t).dist,
		distScene(ro + vec3(0, h, 0) + rd * t).dist - distScene(ro - vec3(0, h, 0) + rd * t).dist,
		distScene(ro + vec3(0, 0, h) + rd * t).dist - distScene(ro - vec3(0, 0, h) + rd * t).dist));
}

// Calculate the light intensity with soft shadows
// p: point on surface
// lightPos: position of the light source
// lightColor: the radiance of the light source
// returns: the color of the point
vec4 getShading(vec3 p, vec3 normal, vec3 lightPos, vec4 lightColor)
{
	float intensity = 0.0f;
	float vis = getVisibility(p, lightPos, 16);
	if(vis > 0.0f)
	{
		vec3 lightDirection = normalize(lightPos - p);
		intensity = clamp(dot(normal, lightDirection), 0, 1) * vis;
	}


	return lightColor * intensity + m_ambient * (1.0f - intensity);
}

// Compute an ambient occlusion factor
// p: point on surface
// n: normal of the surface at p
// returns: a value clamped to [0, 1], where 0 means there were no other surfaces around the point,
// and 1 means that the point is occluded by other surfaces.
float ambientOcclusion(vec3 p, vec3 n)
{
	float stepSize = 0.01f;
	float t = stepSize;
	float oc = 0.0f;
	for(int i = 0; i < 10; ++i)
	{
		float d = distScene(p + n * t).dist;
		oc += t - d; // Actual distance to surface - distance field value
		t += stepSize;
	}

	return clamp(oc, 0, 1);
}

vec4 computeColor(vec3 ro, vec3 rd)
{
	float t0;
	int i;
	int c = raymarch(ro, rd, i, t0);

	vec3 floorNormal = vec3(0, 1, 0);
	float t1 = raytraceFloor(ro, rd, floorNormal, vec3(0));
	
	vec4 color = vec4(1);
	vec3 p; // Surface point
	vec3 normal; // Surface normal
	float t; // Distance traveled by ray from eye
	vec4 surfTexture = vec4(1.0); // Surface texture
	if(t1 < t0 && t1 >= m_zNear && t1 <= m_zFar) // The floor was closest
	{
		t = t1;
		p = ro + rd * t1;
		normal = floorNormal;
		surfTexture = vec4(0.1f, 0.1f, 0.1f, 1.0f);
	}
	else if(i < m_rmSteps && t0 >= m_zNear && t0 <= m_zFar) // Raymarching hit a surface
	{
		t = t0;
		p = ro + rd * t0;
		normal = getNormal(ro, rd, t0);

		color = m_objectColors[c];
	}
	else
	{
		return m_skyColor;
	}

	
	float z = mapTo(t, m_zNear, m_zFar, 1, 0); // Map depth to [0, 1]

	// Color based on depth
	//color = vec4(1.0f) * z;

	

	// Diffuse lighting
	color *= surfTexture * (
		getShading(p, normal, p - (5 * m_directionalLightDir), m_directionalLightColor) + // we use p - directionalLightDir to pretend it's a directional light
		getShading(p, normal, m_pointLightPos, m_pointLightColor)
	) / 2.0f;

	// Color based on surface normal
	//color = vec4(abs(normal), 1.0);

	if(c == 1)
	{
		color += (vec4(1, 0, 0, 1) * m_player1Data.z);
	}

	if(m_AO > 0)
	{
		// Blend in ambient occlusion factor
		float ao = ambientOcclusion(p, normal);
		color = color * (1.0f - ao);
	}

	// Blend the background color based on the distance from the camera
	float zSqrd = z * z;
	color = mix(m_skyColor, color, zSqrd * (3.0f - 2.0f * z)); // Fog

	return color;

}

void main()
{
	vec3 ro = m_eye;
	vec3 rd = normalize(m_camForward * m_focalLength + m_camRight * TexCoord.x * m_aspectRatio + m_camUp * TexCoord.y);

	vec4 color;
	if(m_MSAA == 0)
	{
		 color = computeColor(ro, rd);
	}
	else
	{
	// 4xAA
		vec2 hps = vec2(1.0) / (m_resolution * 2.0);

		vec3 rd0 = normalize(m_camForward * m_focalLength + m_camRight * (TexCoord.x - hps.x) * m_aspectRatio + m_camUp * TexCoord.y);
		vec3 rd1 = normalize(m_camForward * m_focalLength + m_camRight * (TexCoord.x + hps.x) * m_aspectRatio + m_camUp * TexCoord.y);
		vec3 rd2 = normalize(m_camForward * m_focalLength + m_camRight * TexCoord.x * m_aspectRatio + m_camUp * (TexCoord.y - hps.y));
		vec3 rd3 = normalize(m_camForward * m_focalLength + m_camRight * TexCoord.x * m_aspectRatio + m_camUp * (TexCoord.y + hps.y));
	
		color = (computeColor(ro, rd0) + computeColor(ro, rd1) + computeColor(ro, rd2) + computeColor(ro, rd3)) / 4.0;
	}
	outColor = vec4(color.xyz, 1.0f);
}

//void main()
//{
//	outColor = vec4(texture(tex, TexCoord));
//}