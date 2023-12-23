#version 450

//CONSTANTS
#define RADIAN  0.01745329
#define GOLD    1.61803
#define PI      3.14

#define DEFAULT               0

//COLORGAMUTS
#define RGB                   1 
#define HSL                   2
#define HSV                   3
#define HWB                   4
#define IHLS                  5
#define CMY                   6
#define YIQ                   7 
#define YDbDr                 8
#define YUV                   9
#define YUVHD                 10
#define BT709                 11

//SHADINGTYPES
#define FLAT                  1
#define GOURAND               2
#define PHONG                 3
#define BLINPHONG             4
#define GOOCH                 5
#define GOOCHPHONG            6
#define TOONNOEDGE            7
#define TOONFRESNALEDGE       8
#define TOONGOOCHEDGE         9

//FILTERS
#define DEUTERANOPIA          1
#define PROTENOPIA            2
#define TRITANOPIA            3
#define GRAYSCALE             4

//COLORGAMUT
#define COLORGAMUT            int(PC.matProp[0][0])           

//LIGHTING AND SHADING
#define POINTLIGHTCOLOR       vec3(1.0f, 0.0f, 0.0f)
#define POINTLIGHTPOS         vec3(0.0f, 2.0f, 2.0f)
#define POINTLIGHTINTENSITY   15.0f
#define AMBILIGHTCOLOR        vec3(0.1f, 0.1f, 0.1f)           
#define AMBIINTENSITY         0.1f           
#define SHADINGTYPE           int(PC.lighting[0][0])           

//FILTERS
#define FILTERTYPE             int(PC.lighting[0][1])
#define FILTERSEVERITY         1


layout(location = 0) in flat vec3 flatFragColor;
layout(location = 1) in smooth vec3 smoothFragColor;
layout(location = 2) in vec3 fragNorm;
layout(location = 3) in vec3 fragPos;

layout(location = 0) out vec3 outColor;

layout( push_constant ) uniform constants
{
	mat4 matProp;
	mat4 lighting;
} PC;

float lerp(float a, float b, float w) {
  return a + w*(b-a);
}

uvec3 pcg3d(uvec3 v) {

    v = v * 1664525u + 1013904223u;

    v.x += v.y*v.z;
    v.y += v.z*v.x;
    v.z += v.x*v.y;

    v ^= v >> 16u;

    v.x += v.y*v.z;
    v.y += v.z*v.x;
    v.z += v.x*v.y;

    return v;
} //pcg3d

void main() {
  vec3 convertedColor = smoothFragColor;
  if (SHADINGTYPE == FLAT) {
    convertedColor = flatFragColor;
  }; //SHADINGTYPE == FLAT

  switch (COLORGAMUT) {
    //RGB LINEAGE
    default:
      convertedColor = vec3(1.0,1.0,1.0);
      break;
    case CMY:
      convertedColor = vec3(1 - convertedColor[0], 1 - convertedColor[1], 1 - convertedColor[2]);
      break;
    case RGB:
      convertedColor = vec3(convertedColor);
      break;
    case HSV: {
      float hue = convertedColor[0] * 6;
      int i = int(hue);

      //Defined as n, m, & f are defined this way in the paper. No clue what they mean.
      //Bitwise Function to check for even
      float
      n = 
      ((i&1) == 0) ? (convertedColor[2] * (1 - convertedColor[1] * ( 1 - (hue - i))))
      : (convertedColor[2] * (1 - convertedColor[1] * (hue - i)));
      float m = convertedColor[2] * (1 - convertedColor[1]);
      

      switch(i) {
        case 6:
        case 0: convertedColor = vec3(convertedColor[2],n,m); break;
        case 1: convertedColor = vec3(n,convertedColor[2],m); break;
        case 2: convertedColor = vec3(m,convertedColor[2],n); break;
        case 3: convertedColor = vec3(m,n,convertedColor[2]); break;
        case 4: convertedColor = vec3(n,m,convertedColor[2]); break;
        case 5: convertedColor = vec3(convertedColor[2],m,n); break;
      }; //switch
    }
    break;
    case HSL: {
    vec3 rgb = clamp( abs(mod(convertedColor[0]*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0, 0.0, 1.0 );

    rgb = convertedColor[2] + convertedColor[1] * (rgb-0.5)*(1.0-abs(2.0*convertedColor[2]-1.0));

    convertedColor = vec3(rgb);

    } //HSL
    break;
    case HWB: {
      float hue = convertedColor[0] * 6;
      int i = int(hue);
      float v = 1 - convertedColor[2];

      //Defined as n, m, & f are defined this way in the paper. No clue what they mean.
      //Bitwise Function to check for even
      float
      n = 
      ((i&1) == 0) ? (convertedColor[1] + (hue - i) * (v - convertedColor[1]))
      : (convertedColor[1] + (1 - (hue - i)) * (v - convertedColor[1]));
      

      switch(i) {
        case 6:
        case 0: convertedColor = vec3(v,n,convertedColor[1]); break;
        case 1: convertedColor = vec3(n,v,convertedColor[1]); break;
        case 2: convertedColor = vec3(convertedColor[1],v,n); break;
        case 3: convertedColor = vec3(convertedColor[1],n,v); break;
        case 4: convertedColor = vec3(n,convertedColor[1],v); break;
        case 5: convertedColor = vec3(v,convertedColor[1],n); break;
      }; //switch
    } //HWB
    break;
    case IHLS: {
      float hue = 120 - ((convertedColor[0] * 360)/6);
      float chroma = (sqrt(3) * convertedColor[1])/(2 * sin(hue * RADIAN));

      float chromaPos = chroma * cos((convertedColor[0]*360) * RADIAN);
      float chromaNeg = -chroma * sin((convertedColor[0]*360) * RADIAN);

      mat3 finalCalc;
      finalCalc[0] = vec3(1.0f, 0.7875f, 0.3714f);
      finalCalc[1] = vec3(1.0f, -0.2125f, -0.2059f);
      finalCalc[2] = vec3(1.0f, -0.2125f, 0.9488f);

      vec3 rtrnColor = finalCalc * vec3(convertedColor[2],chromaPos, chromaNeg);
      convertedColor = vec3(rtrnColor);
    }
    break;



    //TELVISION LINEAGE
    case YIQ: {
      mat3 finalCalc;
      finalCalc[0] = vec3(1.0f, 0.956f, 0.619f);
      finalCalc[1] = vec3(1.0f, -0.272f, -0.647f);
      finalCalc[2] = vec3(1.0f, -1.106f, 1.703f);

      vec3 rtrnColor = finalCalc * vec3(convertedColor[0],(convertedColor[1] * 1.1914), (convertedColor[2] * 1.0456));
      convertedColor = vec3(rtrnColor);
    } 
    break;
    case YDbDr: {
      mat3 finalCalc;
      finalCalc[0] = vec3(1.0f, 0.000092f, -0.525912f);
      finalCalc[1] = vec3(1.0f, -0.129132, 0.267899f);
      finalCalc[2] = vec3(1.0f, 0.664679f, -0.000079f);

      vec3 rtrnColor = finalCalc * vec3(convertedColor[0],(convertedColor[1] * 2.666), (convertedColor[2] * 2.666));
      convertedColor = vec3(rtrnColor);
    } 
    break;
    case YUV: {
      mat3 finalCalc;
      finalCalc[0] = vec3(1.0f, 0.0f, 1.1398f);
      finalCalc[1] = vec3(1.0f, -0.39465, -0.5806f);
      finalCalc[2] = vec3(1.0f, 2.03211f, 0.0f);

      vec3 rtrnColor = finalCalc * vec3(convertedColor[0],(convertedColor[1]), (convertedColor[2]));
      convertedColor = vec3(rtrnColor);
    }
    break;
    case YUVHD: {
      mat3 finalCalc;
      finalCalc[0] = vec3(1.0f, 0.0f, 1.28033f);
      finalCalc[1] = vec3(1.0f, -0.21482, 0.38059f);
      finalCalc[2] = vec3(1.0f, 2.12798f, 0.0f);

      vec3 rtrnColor = finalCalc * vec3(convertedColor[0],(convertedColor[1]), (convertedColor[2]));
      convertedColor = vec3(rtrnColor);
    }
    break;
    case BT709: {
      mat3 finalCalc;
        finalCalc[0] = vec3(1.0f, 0.0f, 1.5748f);
        finalCalc[1] = vec3(1.0f, -0.1873, -0.4681f);
        finalCalc[2] = vec3(1.0f, 1.8556f, 0.0f);

        vec3 rtrnColor = finalCalc * vec3(convertedColor[0],(convertedColor[1]), (convertedColor[2]));
        convertedColor = vec3(rtrnColor);
    }

    break;
    //CIE LINEAGE

  } //COLORGAMUT


  //SHADING
  float diffuse = 1.0f;

  switch (SHADINGTYPE) {
    default:
      break;
    case FLAT: {
      //CREATE FLAT BY MOVING THIS INTO THE VERTEX SHADER TO USE THE FLAT MODIFIER
    } //FLAT
    break;
    case GOURAND: {
      //CREATE GOURAND BY MOVING PHONG INTO THE VERTEX SHADER
    }//GOURAND
     break;
    case PHONG: {
      vec3 lightDir = abs(POINTLIGHTPOS - fragPos);

      float distance = length(lightDir);
      distance = distance;
      lightDir = normalize(lightDir);
      float shininess = 16.0;

      vec3 viewDir = normalize(-fragPos);
      float lambertian = max(dot(lightDir, fragNorm), 0.0);
      float specular = 5.0;

      if (lambertian > 0.0f) {
        vec3 reflectDir = reflect(-lightDir, fragNorm);
        float specAngle = max(dot(reflectDir, viewDir), 0.0);
        specular = pow(specAngle, shininess/4.0);
      }; //if lambertian
      
      convertedColor = (AMBIINTENSITY * AMBILIGHTCOLOR) +
        (convertedColor * lambertian * POINTLIGHTCOLOR * POINTLIGHTINTENSITY) /
        (distance + vec3(1.0f,1.0f,1.0f) * specular * POINTLIGHTCOLOR * POINTLIGHTINTENSITY /distance);
    }
    break;
    case BLINPHONG: {
      vec3 lightDir = abs(POINTLIGHTPOS - fragPos);
      float distance = length(lightDir);
      distance = distance * distance;
      lightDir = normalize(lightDir);
      float shininess = 16.0;

      vec3 viewDir = normalize(-fragPos);
      float lambertian = max(dot(lightDir, fragNorm), 0.0);
      float specular = 5.0;

      if (lambertian > 0.0f) {
        vec3 halfDir = normalize(lightDir + viewDir);
        float specAngle = max(dot(halfDir, fragNorm), 0.0);
        specular = pow(specAngle, shininess);
      }; //if lambertian
      
      convertedColor = (AMBIINTENSITY * AMBILIGHTCOLOR) +
        (convertedColor * lambertian * POINTLIGHTCOLOR * POINTLIGHTINTENSITY) /
        (distance + vec3(1.0f,1.0f,1.0f) * specular * POINTLIGHTCOLOR * POINTLIGHTINTENSITY /distance);
    }
    break;
    case GOOCHPHONG: {
      vec3 lightDir = abs(POINTLIGHTPOS - fragPos);
      
      vec3 kBlue =   vec3(0.0f,0.0f,1.0f); 
      vec3 kYellow = vec3(0.0f,1.0f,1.0f);
      
      float alpha = 0.5f;
      float beta = 0.5f;

      float difRef = 1.0f;

      vec3 kCool = kBlue   + alpha * difRef;
      vec3 kWarm = kYellow + beta* difRef;
      convertedColor = (((1+dot(lightDir, fragNorm))/2) * (kCool)) + (((1-dot(lightDir, fragNorm))/2) * (kWarm));
      convertedColor/2;
      convertedColor + 0.5f;

      //lightDir = POINTLIGHTPOS - fragPos;
      float distance = length(lightDir);
      distance = distance * distance;
      lightDir = normalize(lightDir);
      float shininess = 16.0;

      vec3 viewDir = normalize(fragPos);
      float lambertian = max(dot(lightDir, fragNorm), 0.0);
      float specular = 5.0;

      if (lambertian > 0.0f) {
        vec3 halfDir = normalize(lightDir + viewDir);
        float specAngle = max(dot(halfDir, fragNorm), 0.0);
        specular = pow(specAngle, shininess);
      }; //if lambertian
      
      convertedColor = (AMBIINTENSITY * AMBILIGHTCOLOR) +
        (convertedColor * lambertian * POINTLIGHTCOLOR * POINTLIGHTINTENSITY) /
        (distance + (vec3(1.0f,1.0f,1.0f) * specular * POINTLIGHTCOLOR * POINTLIGHTINTENSITY) /distance);

    } //GOOCHPHONG
    break;
    case GOOCH: {
      vec3 lightDir = abs(POINTLIGHTPOS - fragPos);
      
      vec3 kBlue =   vec3(0.0f,0.0f,1.0f); 
      vec3 kYellow = vec3(0.0f,1.0f,1.0f);
      
      float alpha = 0.5f;
      float beta = 0.9f;

      float difRef = 0.5f;

      vec3 kCool = kBlue   + alpha * difRef;
      vec3 kWarm = kYellow + beta * difRef;
      convertedColor = (((1+dot(lightDir, fragNorm))/2) * (kCool)) + (((1-dot(lightDir, fragNorm))/2) * (kWarm));
      convertedColor/2;
      convertedColor + 0.5f;
    } //GOOCH
    break;
    case TOONNOEDGE: {    
      vec3 lightDir = POINTLIGHTPOS - fragPos;
      float diffuse = max(dot(lightDir, fragNorm), 0.0);
      vec4 color;

      if (diffuse > pow(0.95, POINTLIGHTINTENSITY)) {
        color = vec4(vec3(1.0), 1.0);
      } else if (diffuse > pow(0.7, POINTLIGHTINTENSITY)) {
        color = vec4(vec3(0.6), 1.0);
      } else if (diffuse > pow(0.4, POINTLIGHTINTENSITY)) {
        color = vec4(vec3(0.4), 1.0);
      } else {
        color = vec4(vec3(0.2), 1.0);
      }

      convertedColor = vec3(color) * POINTLIGHTCOLOR * convertedColor;

    } //TOON
    break;
    case TOONFRESNALEDGE: {
      vec3 lightDir = POINTLIGHTPOS - fragPos;
      float diffuse = max(dot(lightDir, fragNorm), 0.0);
      vec4 color;

      if (diffuse > pow(0.95, POINTLIGHTINTENSITY)) {
        color = vec4(vec3(1.0), 1.0);
      } else if (diffuse > pow(0.7, POINTLIGHTINTENSITY)) {
        color = vec4(vec3(0.6), 1.0);
      } else if (diffuse > pow(0.4, POINTLIGHTINTENSITY)) {
        color = vec4(vec3(0.4), 1.0);
      } else {
        color = vec4(vec3(0.2), 1.0);
      }

      convertedColor = (vec3(color)) * convertedColor;

      vec3 viewDir = normalize(-fragPos);
      float edge1 = 1 - 0.7; //width
      float edge2 = edge1 + 0.1; //Softness
      float fresnel = pow(1.0 - clamp(dot(fragNorm, viewDir), 0,1), 2);
      vec3 outline = lerp(1, smoothstep(edge1, edge2, fresnel), step(0, edge1)) * vec3(0.1,0.1,0.1);
      if ((convertedColor.r < outline.r) || (convertedColor.g < outline.g) || (convertedColor.b < outline.b)) {convertedColor *= outline; };
      
      viewDir = normalize(-fragPos);
      edge1 = 1 - 0.7; //width
      edge2 = edge1 + 0.1; //Softness
      fresnel = pow(1.0 - clamp(dot(fragNorm, viewDir), 0,1), 2);
      outline = lerp(1, smoothstep(edge1, edge2, fresnel), step(0, edge1)) * vec3(1.0,1.0,1.0);
      //if ((convertedColor.r > outline.r) || (convertedColor.g > outline.g) || (convertedColor.b > outline.b)) {convertedColor *= outline; };

      //convertedColor *= outline;
    } //TOONFRESNAL
    break;
    case TOONGOOCHEDGE: {    
      vec3 lightDir = abs(normalize(-fragPos) - fragPos);
      float diffuse = max(dot(lightDir, fragNorm), 0.0);
      vec4 color;

      if (diffuse > pow(0.95, POINTLIGHTINTENSITY)) {
        color = vec4(vec3(1.0), 1.0);
      } else if (diffuse > pow(0.7, POINTLIGHTINTENSITY)) {
        color = vec4(vec3(0.6), 1.0);
      } else if (diffuse > pow(0.4, POINTLIGHTINTENSITY)) {
        color = vec4(vec3(0.4), 1.0);
      } else {
        color = vec4(vec3(0.2), 1.0);
      }

      convertedColor = (vec3(color)) * (POINTLIGHTCOLOR) * convertedColor;
        
      vec3 kBlue =   convertedColor; 
      vec3 kYellow = vec3(0.1f,0.1f,0.1f);
      
      float alpha = 0.1f;
      float beta = 0.1;

      float difRef = 1.0f;

      vec3 kCool = kBlue   + alpha * difRef;
      vec3 kWarm = kYellow + beta* difRef;
      convertedColor = (((1+dot(lightDir, fragNorm))/2) * (kCool)) + (((1-dot(lightDir, fragNorm))/2) * (kWarm));
      convertedColor/2;
      convertedColor + 0.5f;

      lightDir = abs(POINTLIGHTPOS - fragPos);
      convertedColor = (((1+dot(lightDir, fragNorm))/2) * (kCool)) + (((1-dot(lightDir, fragNorm))/2) * (kWarm));
      convertedColor/2;
      convertedColor + 0.5f;

    } //TOONGOOCHEDGE
    break;
  };//SHADING

  //FILTERS
  switch(FILTERTYPE) {
    default:
    case DEFAULT:
    break;
    case DEUTERANOPIA: {      
      float s = 10*FILTERSEVERITY;
      float i1 = floor(s); 
      float i2 = ceil(s);

      mat3 values;
      switch(int(i1 + 1)){
        default:
        case 1: {
          values[0] = vec3(1.0f, 0.0f, -0.0f);
          values[1] = vec3(0.0f, 1.0f, 0.0f);
          values[2] = vec3(0.0f, 0.0f, 1.0f);
        } //case 1
        break;
        case 2: {
          values[0] = vec3(0.866435,  0.177704, -0.044139);
          values[1] = vec3(0.049567,  0.939063,  0.011370);
          values[2] = vec3(-0.003453,  0.007233,  0.996220);
        } //case 2
        break;
        case 3: {
          values[0] = vec3(0.760729,  0.319078, -0.079807);
          values[1] = vec3(0.090568,  0.889315,  0.020117);
          values[2] = vec3(-0.006027,  0.013325,  0.992702);
        } //case 3
        break;
        case 4: {
          values[0] = vec3(0.675425,  0.433850, -0.109275);
          values[1] = vec3(0.125303,  0.847755,  0.026942);
          values[2] = vec3(-0.007950,  0.018572,  0.989378);
        } //case 4
        break;
        case 5: {
          values[0] = vec3(0.605511,  0.528560, -0.134071);
          values[1] = vec3(0.155318,  0.812366,  0.032316);
          values[2] = vec3(-0.009376,  0.023176,  0.986200);
        } //case 5
        break;
        case 6: {
          values[0] = vec3(0.547494,  0.607765, -0.155259);
          values[1] = vec3(0.181692,  0.781742,  0.036566);
          values[2] = vec3(-0.010410,  0.027275,  0.983136);
        } //case 6
        break;
        case 7: {
          values[0] = vec3(0.498864,  0.674741, -0.173604);
          values[1] = vec3(0.205199,  0.754872,  0.039929);
          values[2] = vec3(-0.011131,  0.030969,  0.980162);
        } //case 7
        break;
        case 8: {
          values[0] = vec3(0.457771,  0.731899, -0.189670);
          values[1] = vec3(0.226409,  0.731012,  0.042579);
          values[2] = vec3(-0.011595,  0.034333,  0.977261);
        } //case 8
        break;
        case 9: {
          values[0] = vec3(0.422823,  0.781057, -0.203881);
          values[1] = vec3(0.245752,  0.709602,  0.044646);
          values[2] = vec3(-0.011843,  0.037423,  0.974421);
        } //case 9
        break;
        case 10: {
          values[0] = vec3(0.392952,  0.823610, -0.216562);
          values[1] = vec3(0.263559,  0.690210,  0.046232);
          values[2] = vec3(-0.011910,  0.040281,  0.971630);
        } //case 10
        break;        
        case 11: {
          values[0] = vec3(0.367322,  0.860646, -0.227968);
          values[1] = vec3(0.280085,  0.672501,  0.047413);
          values[2] = vec3(-0.011820,  0.042940,  0.968881);
        } //case 11
        break;
      }; //FilterSeverity
      if (i1 == i2){
        convertedColor *= values;
      } //if (i1 == i2)
      else {
        mat3 newVal = values * (i1 - s);
        newVal += values * (s - i2); 
        convertedColor *= newVal;
      } //if (i1 == i2)
    }; //DEUTERANOPIA
      break;
    case PROTENOPIA:{
      float s = 10*FILTERSEVERITY;
      float i1 = floor(s); 
      float i2 = ceil(s);

      mat3 values;
      switch(int(i1 + 1)){
        default:
        case 1: {
          values[0] = vec3(1.000000,  0.000000, -0.000000);
          values[1] = vec3(0.000000,  1.000000,  0.000000);
          values[2] = vec3(-0.000000, -0.000000,  1.000000);
        } //case 1
        break;
        case 2: {
          values[0] = vec3(0.856167,  0.182038, -0.038205);
          values[1] = vec3(0.029342,  0.955115,  0.015544);
          values[2] = vec3(-0.002880, -0.001563,  1.004443);
        } //case 2
        break;
        case 3: {
          values[0] = vec3(0.734766,  0.334872, -0.069637);
          values[1] = vec3(0.051840,  0.919198,  0.028963);
          values[2] = vec3(-0.004928, -0.004209,  1.009137);
        } //case 3
        break;
        case 4: {
          values[0] = vec3(0.630323,  0.465641, -0.095964);
          values[1] = vec3(0.069181,  0.890046,  0.040773);
          values[2] = vec3(-0.006308, -0.007724,  1.014032);
        } //case 4
        break;
        case 5: {
          values[0] = vec3(0.539009,  0.579343, -0.118352);
          values[1] = vec3(0.082546,  0.866121,  0.051332);
          values[2] = vec3(-0.007136, -0.011959,  1.019095);
        } //case 5
        break;
        case 6: {
          values[0] = vec3(0.458064,  0.679578, -0.137642);
          values[1] = vec3(0.092785,  0.846313,  0.060902);
          values[2] = vec3(-0.007494, -0.016807,  1.024301);
        } //case 6
        break;
        case 7: {
          values[0] = vec3(0.385450,  0.769005, -0.154455);
          values[1] = vec3(0.100526,  0.829802,  0.069673);
          values[2] = vec3(-0.007442, -0.022190,  1.029632);
        } //case 7
        break;
        case 8: {
          values[0] = vec3(1.0f, 0.0f, 1.5748f);
          values[1] = vec3(1.0f, -0.1873, -0.4681f);
          values[2] = vec3(1.0f, 1.8556f, 0.0f);
        } //case 8
        break;
        case 9: {
          values[0] = vec3(0.319627,  0.849633, -0.169261);
          values[1] = vec3(0.106241,  0.815969,  0.077790);
          values[2] = vec3(-0.007025, -0.028051,  1.035076);
        } //case 9
        break;
        case 10: {
          values[0] = vec3(0.203876,  0.990338, -0.194214);
          values[1] = vec3(0.112975,  0.794542,  0.092483);
          values[2] = vec3(-0.005222, -0.041043,  1.046265);
        } //case 10
        break;        
        case 11: {
          values[0] = vec3(0.152286,  1.052583, -0.204868);
          values[1] = vec3(0.114503,  0.786281,  0.099216);
          values[2] = vec3(-0.003882, -0.048116,  1.051998);
        } //case 11
        break;
      }; //FilterSeverity
      if (i1 == i2){
        convertedColor *= values;
      } //if (i1 == i2)
      else {
        mat3 newVal = values * (i1 - s);
        newVal += values * (s - i2); 
        convertedColor *= newVal;
      } //if (i1 == i2)
    } //PROTENOPIA
      break;
    case TRITANOPIA: {
      float s = 10*FILTERSEVERITY;
      float i1 = floor(s); 
      float i2 = ceil(s);

      mat3 values;
      switch(int(i1 + 1)){
        default:
        case 1: {
          values[0] = vec3(1.0f, 0.0f, -0.0f);
          values[1] = vec3(0.0f, 1.0f, 0.0f);
          values[2] = vec3(0.0f, 0.0f, 1.0f);
        } //case 1
        break;
        case 2: {
          values[0] = vec3(0.926670,  0.092514, -0.019184);
          values[1] = vec3(0.021191,  0.964503,  0.014306);
          values[2] = vec3(0.008437,  0.054813,  0.936750);
        } //case 2
        break;
        case 3: {
          values[0] = vec3(0.895720,  0.133330, -0.029050);
          values[1] = vec3(0.029997,  0.945400,  0.024603);
          values[2] = vec3(0.013027,  0.104707,  0.882266);
        } //case 3
        break;
        case 4: {
          values[0] = vec3(0.905871,  0.127791, -0.033662);
          values[1] = vec3(0.026856,  0.941251,  0.031893);
          values[2] = vec3(0.013410,  0.148296,  0.838294);
        } //case 4
        break;
        case 5: {
          values[0] = vec3(0.948035,  0.089490, -0.037526);
          values[1] = vec3(0.014364,  0.946792,  0.038844);
          values[2] = vec3(0.010853,  0.193991,  0.795156);
        } //case 5
        break;
        case 6: {
          values[0] = vec3(1.017277,  0.027029, -0.044306);
          values[1] = vec3(-0.006113,  0.958479,  0.047634);
          values[2] = vec3(0.006379,  0.248708,  0.744913);
        } //case 6
        break;
        case 7: {
          values[0] = vec3(1.104996, -0.046633, -0.058363);
          values[1] = vec3(-0.032137,  0.971635,  0.060503);
          values[2] = vec3(0.001336,  0.317922,  0.680742);
        } //case 7
        break;
        case 8: {
          values[0] = vec3(1.193214, -0.109812, -0.083402);
          values[1] = vec3(-0.058496,  0.979410,  0.079086);
          values[2] = vec3(-0.002346,  0.403492,  0.598854);
        } //case 8
        break;
        case 9: {
          values[0] = vec3(1.257728, -0.139648, -0.118081);
          values[1] = vec3(-0.078003,  0.975409,  0.102594);
          values[2] = vec3(-0.003316,  0.501214,  0.502102);
        } //case 9
        break;
        case 10: {
          values[0] = vec3(1.278864, -0.125333, -0.153531);
          values[1] = vec3(-0.084748,  0.957674,  0.127074);
          values[2] = vec3(-0.000989,  0.601151,  0.399838);
        } //case 10
        break;        
        case 11: {
          values[0] = vec3(1.255528, -0.076749, -0.178779);
          values[1] = vec3(-0.078411,  0.930809,  0.147602);
          values[2] = vec3(0.004733,  0.691367,  0.303900);
        } //case 11
        break;
      }; //FilterSeverity
      if (i1 == i2){
        convertedColor *= values;
      } //if (i1 == i2)
      else {
        mat3 newVal = values * (i1 - s);
        newVal += values * (s - i2); 
        convertedColor *= newVal;
      } //if (i1 == i2)
    } //TRITANOPIA
    break;
    case GRAYSCALE:
      float grayColor = convertedColor.x;
      grayColor += convertedColor.y;
      grayColor += convertedColor.z;
      grayColor /= 3;

      convertedColor.x = grayColor;
      convertedColor.y = grayColor;
      convertedColor.z = grayColor;
      break;
  }; //switch




  outColor = convertedColor;
} //main