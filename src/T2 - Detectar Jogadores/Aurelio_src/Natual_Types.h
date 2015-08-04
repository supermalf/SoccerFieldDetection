#ifndef NATURAL_TYPES_H
#define NATURAL_TYPES_H

#include <string>

/**
* Imprime o valor de um enumerator como string.
*/
#define GET_ENUM_NAME(bla) #bla

typedef enum dataType;
typedef enum dataGroup;
typedef enum samplesData;

#define NUM_DATATYPE 13
#define NUM_DATAGROUP 13
#define NUM_SAMPLESDATA 22


/**
* Enumerator. Define tipos de dados que serão utilizados nas amostras.
*/
enum dataType{
  D_UNDEF_DATA,     ///< Dado indefinido.
  D__R__VECTOR,     ///< Componente R.
  D__G__VECTOR,     ///< Componente G.
  D__B__VECTOR,     ///< Componente B
  D_MEAN_R_CMP,     ///< Média das componentes R do vetor.
  D_MEAN_G_CMP,     ///< Média das componentes G do vetor.
  D_MEAN_B_CMP,     ///< Média das componentes B do vetor.
  D_VAR__R_CMP,     ///< Variância das componentes R do vetor.
  D_VAR__G_CMP,     ///< Variância das componentes G do vetor.
  D_VAR__B_CMP,     ///< Variância das componentes B do vetor.
  D_CENTRAL__R,     ///< Componente R do pixel central.
  D_CENTRAL__G,     ///< Componente G do pixel central.
  D_CENTRAL__B      ///< Componente B do pixel central.
};


/**
* Imprime a string referente ao valor contido no enumerador recebido.
* @param type Enumerador recebido como parâmetro (portanto, contém um número inteiro). 
* @return Irá retornar a string referente a esse número.
*/
char* GetDataTypeChar( dataType type )
{
  char*  charName = new char[32];
  char** dataTypeName = new char*[NUM_DATATYPE];
  for( int i=0 ; i<NUM_DATATYPE ; i++ )
    dataTypeName[i] = new char[40];
  strcpy(dataTypeName[0], "D_UNDEFINED_DTA");
  strcpy(dataTypeName[1], "D__R__VECTOR");
  strcpy(dataTypeName[2], "D__G__VECTOR");
  strcpy(dataTypeName[3], "D__B__VECTOR");
  strcpy(dataTypeName[4], "D_MEAN_R_CMP");
  strcpy(dataTypeName[5], "D_MEAN_G_CMP");
  strcpy(dataTypeName[6], "D_MEAN_B_CMP");
  strcpy(dataTypeName[7], "D_VAR__R_CMP");
  strcpy(dataTypeName[8], "D_VAR__G_CMP");
  strcpy(dataTypeName[9], "D_VAR__B_CMP");
  strcpy(dataTypeName[10], "D_CENTRAL__R");
  strcpy(dataTypeName[11], "D_CENTRAL__G");
  strcpy(dataTypeName[12], "D_CENTRAL__B");

  sprintf( charName, "%s", dataTypeName[(int)type] );
  return charName;
}

/**
* Enumerator. Define grupos de dados, dados de mesmo grupo são normalizados juntos.
*/
enum dataGroup{
  UNDEF__GRP,     ///< Grupo de escalonamento indefinido.
  R___VECTOR,     ///< Componente R.
  G___VECTOR,     ///< Componente G.
  B___VECTOR,     ///< Componente B
  MEAN_R_CMP,     ///< Média das componentes R do vetor.
  MEAN_G_CMP,     ///< Média das componentes G do vetor.
  MEAN_B_CMP,     ///< Média das componentes B do vetor.
  VAR__R_CMP,     ///< Variância das componentes R do vetor.
  VAR__G_CMP,     ///< Variância das componentes G do vetor.
  VAR__B_CMP,     ///< Variância das componentes B do vetor.
  CENTRAL__R,     ///< Componente R do pixel central.
  CENTRAL__G,     ///< Componente G do pixel central.
  CENTRAL__B      ///< Componente B do pixel central.
};

/**
* A função irá retornar uma string referente ao valor desse número no enumerator.
* @param type Valor do enumerador.
* @return Irá retornar a string referente a esse número.
*/
char* GetDataGroupChar( dataGroup group )
{
  char*  charName = new char[32];
  char** dataGroupName = new char*[NUM_DATAGROUP];
  for( int i=0 ; i<NUM_DATAGROUP ; i++ )
    dataGroupName[i] = new char[40];
  strcpy(dataGroupName[0], "UNDEF__GRP");
  strcpy(dataGroupName[1], "R___VECTOR");
  strcpy(dataGroupName[2], "G___VECTOR");
  strcpy(dataGroupName[3], "B___VECTOR");
  strcpy(dataGroupName[4], "MEAN_R_CMP");
  strcpy(dataGroupName[5], "MEAN_G_CMP");
  strcpy(dataGroupName[6], "MEAN_B_CMP");
  strcpy(dataGroupName[7], "VAR__R_CMP");
  strcpy(dataGroupName[8], "VAR__G_CMP");
  strcpy(dataGroupName[9], "VAR__B_CMP");
  strcpy(dataGroupName[10], "CENTRAL__R");
  strcpy(dataGroupName[11], "CENTRAL__G");
  strcpy(dataGroupName[12], "CENTRAL__B");

  sprintf( charName, "%s", dataGroupName[(int)group] );
  return charName;
}


/**
* Enumerator. Define grupos de dados, dados de mesmo grupo são normalizados juntos.
*/
enum samplesData
{
  S_UNDEF__GRP,     ///< Tipo de amostra indefinido.
  S_R___VECTOR,     ///< Componente R.
  S_G___VECTOR,     ///< Componente G.
  S_B___VECTOR,     ///< Componente B
  S_MEAN_R_CMP,     ///< Média das componentes R do vetor.
  S_MEAN_G_CMP,     ///< Média das componentes G do vetor.
  S_MEAN_B_CMP,     ///< Média das componentes B do vetor.
  S_VAR__R_CMP,     ///< Variância das componentes R do vetor.
  S_VAR__G_CMP,     ///< Variância das componentes G do vetor.
  S_VAR__B_CMP,     ///< Variância das componentes B do vetor.
  S_ETP__R_CMP,     ///< Entropia das componentes R do vetor.
  S_ETP__G_CMP,     ///< Entropia das componentes G do vetor.
  S_ETP__B_CMP,     ///< Entropia das componentes B do vetor.
  S_ASM__R_CMP,     ///< Assimetria das componentes R do vetor.
  S_ASM__G_CMP,     ///< Assimetria das componentes G do vetor.
  S_ASM__B_CMP,     ///< Assimetria das componentes B do vetor.
  S_CVAR_R_CMP,     ///< Coeficiente de variação das componentes R do vetor.
  S_CVAR_G_CMP,     ///< Coeficiente de variação das componentes G do vetor.
  S_CVAR_B_CMP,     ///< Coeficiente de variação das componentes B do vetor.
  S_CENTRAL__R,     ///< Componente R do pixel central.
  S_CENTRAL__G,     ///< Componente G do pixel central.
  S_CENTRAL__B      ///< Componente B do pixel central.
};


/**
* A função irá retornar uma string referente ao valor desse número no enumerator.
  FAZER DE UMA MANEIRA MENOS PORCA DEPOIS!!!
* @param type Valor do enumerador.
* @return Irá retornar a string referente a esse número.
*/
char* GetSamplesDataChar( samplesData sampleType )
{
  char*  charName = new char[32];
  char** samplesDataName = new char*[NUM_SAMPLESDATA];
  for( int i=0 ; i<NUM_SAMPLESDATA ; i++ )
    samplesDataName[i] = new char[40];
  strcpy(samplesDataName[0], "S_UNDEF__GRP");
  strcpy(samplesDataName[1], "S_R___VECTOR");
  strcpy(samplesDataName[2], "S_G___VECTOR");
  strcpy(samplesDataName[3], "S_B___VECTOR");
  strcpy(samplesDataName[4], "S_MEAN_R_CMP");
  strcpy(samplesDataName[5], "S_MEAN_G_CMP");
  strcpy(samplesDataName[6], "S_MEAN_B_CMP");
  strcpy(samplesDataName[7], "S_VAR__R_CMP");
  strcpy(samplesDataName[8], "S_VAR__G_CMP");
  strcpy(samplesDataName[9], "S_VAR__B_CMP");
  strcpy(samplesDataName[10], "S_ETP__R_CMP");
  strcpy(samplesDataName[11], "S_ETP__G_CMP");
  strcpy(samplesDataName[12], "S_ETP__B_CMP");
  strcpy(samplesDataName[13], "S_ASM__R_CMP");
  strcpy(samplesDataName[14], "S_ASM__G_CMP");
  strcpy(samplesDataName[15], "S_ASM__B_CMP");
  strcpy(samplesDataName[16], "S_CVAR_R_CMP");
  strcpy(samplesDataName[17], "S_CVAR_G_CMP");
  strcpy(samplesDataName[18], "S_CVAR_B_CMP");
  strcpy(samplesDataName[19], "S_CENTRAL__R");
  strcpy(samplesDataName[20], "S_CENTRAL__G");
  strcpy(samplesDataName[21], "S_CENTRAL__B");

  sprintf( charName, "%s", samplesDataName[(int)sampleType] );
  delete samplesDataName;
  return charName;
}

















dataGroup inGroupsVector[92] =
{
  R___VECTOR,  
  R___VECTOR,  
  R___VECTOR,  
  R___VECTOR,  
  R___VECTOR,
  ///////////////////////
  R___VECTOR,  
  R___VECTOR,  
  R___VECTOR,  
  R___VECTOR,  
  R___VECTOR,  
  ///////////////////////
  R___VECTOR,  
  R___VECTOR,  
  R___VECTOR,  
  R___VECTOR,  
  R___VECTOR,  
  ///////////////////////
  R___VECTOR,  
  R___VECTOR,  
  R___VECTOR,  
  R___VECTOR,  
  R___VECTOR,  
  ///////////////////////
  R___VECTOR,  
  R___VECTOR,  
  R___VECTOR,  
  R___VECTOR,  
  R___VECTOR,  
  ///////////////////////
  ///////////////////////
  G___VECTOR,  
  G___VECTOR,  
  G___VECTOR,  
  G___VECTOR,  
  G___VECTOR,  
  ///////////////////////
  G___VECTOR,  
  G___VECTOR,  
  G___VECTOR,  
  G___VECTOR,  
  G___VECTOR,  
  ///////////////////////
  G___VECTOR,  
  G___VECTOR,  
  G___VECTOR,  
  G___VECTOR,  
  G___VECTOR,  
  ///////////////////////
  G___VECTOR,  
  G___VECTOR,  
  G___VECTOR,  
  G___VECTOR,  
  G___VECTOR,  
  ///////////////////////
  G___VECTOR,  
  G___VECTOR,  
  G___VECTOR,  
  G___VECTOR,  
  G___VECTOR,  
  ///////////////////////
  ///////////////////////
  B___VECTOR,  
  B___VECTOR,  
  B___VECTOR,  
  B___VECTOR,  
  B___VECTOR,  
  ///////////////////////
  B___VECTOR,  
  B___VECTOR,  
  B___VECTOR,  
  B___VECTOR,  
  B___VECTOR,  
  ///////////////////////
  B___VECTOR,  
  B___VECTOR,  
  B___VECTOR,  
  B___VECTOR,  
  B___VECTOR,  
  ///////////////////////
  B___VECTOR,  
  B___VECTOR,  
  B___VECTOR,  
  B___VECTOR,  
  B___VECTOR,  
  ///////////////////////
  B___VECTOR,  
  B___VECTOR,  
  B___VECTOR,  
  B___VECTOR,  
  B___VECTOR,  
  ///////////////////////
  ///////////////////////
  MEAN_R_CMP, 
  MEAN_R_CMP,
  ///////////////////////
  MEAN_G_CMP, 
  MEAN_G_CMP, 
  ///////////////////////
  MEAN_B_CMP, 
  MEAN_B_CMP,
  ///////////////////////
  ///////////////////////
  VAR__R_CMP, 
  VAR__R_CMP,
  ///////////////////////
  VAR__G_CMP, 
  VAR__G_CMP, 
  ///////////////////////
  VAR__B_CMP, 
  VAR__B_CMP,
  ///////////////////////
  CENTRAL__R, 
  CENTRAL__G,
  CENTRAL__B
};


dataType inTypesVector[92] =
{
  D__R__VECTOR,  
  D__R__VECTOR,  
  D__R__VECTOR,  
  D__R__VECTOR,  
  D__R__VECTOR,
  ///////////////////////
  D__R__VECTOR,  
  D__R__VECTOR,  
  D__R__VECTOR,  
  D__R__VECTOR,  
  D__R__VECTOR,
  ///////////////////////
  D__R__VECTOR,  
  D__R__VECTOR,  
  D__R__VECTOR,  
  D__R__VECTOR,  
  D__R__VECTOR,
  ///////////////////////
  D__R__VECTOR,  
  D__R__VECTOR,  
  D__R__VECTOR,  
  D__R__VECTOR,  
  D__R__VECTOR,
  ///////////////////////
  D__R__VECTOR,  
  D__R__VECTOR,  
  D__R__VECTOR,  
  D__R__VECTOR,  
  D__R__VECTOR,
  ///////////////////////
  ///////////////////////
  D__G__VECTOR,  
  D__G__VECTOR,  
  D__G__VECTOR,  
  D__G__VECTOR,  
  D__G__VECTOR,  
  ///////////////////////
  D__G__VECTOR,  
  D__G__VECTOR,  
  D__G__VECTOR,  
  D__G__VECTOR,  
  D__G__VECTOR,  
  ///////////////////////
  D__G__VECTOR,  
  D__G__VECTOR,  
  D__G__VECTOR,  
  D__G__VECTOR,  
  D__G__VECTOR,  
  ///////////////////////
  D__G__VECTOR,  
  D__G__VECTOR,  
  D__G__VECTOR,  
  D__G__VECTOR,  
  D__G__VECTOR,  
  ///////////////////////
  D__G__VECTOR,  
  D__G__VECTOR,  
  D__G__VECTOR,  
  D__G__VECTOR,  
  D__G__VECTOR,  
  ///////////////////////
  ///////////////////////
  D__B__VECTOR,  
  D__B__VECTOR,  
  D__B__VECTOR,  
  D__B__VECTOR,  
  D__B__VECTOR,  
  ///////////////////////
  D__B__VECTOR,  
  D__B__VECTOR,  
  D__B__VECTOR,  
  D__B__VECTOR,  
  D__B__VECTOR,  
  ///////////////////////
  D__B__VECTOR,  
  D__B__VECTOR,  
  D__B__VECTOR,  
  D__B__VECTOR,  
  D__B__VECTOR,  
  ///////////////////////
  D__B__VECTOR,  
  D__B__VECTOR,  
  D__B__VECTOR,  
  D__B__VECTOR,  
  D__B__VECTOR,  
  ///////////////////////
  D__B__VECTOR,  
  D__B__VECTOR,  
  D__B__VECTOR,  
  D__B__VECTOR,  
  D__B__VECTOR,  
  ///////////////////////
  ///////////////////////
  D_MEAN_R_CMP, 
  D_MEAN_R_CMP,
  ///////////////////////
  D_MEAN_G_CMP, 
  D_MEAN_G_CMP, 
  ///////////////////////
  D_MEAN_B_CMP, 
  D_MEAN_B_CMP,
  ///////////////////////
  ///////////////////////
  D_VAR__R_CMP, 
  D_VAR__R_CMP,
  ///////////////////////
  D_VAR__G_CMP, 
  D_VAR__G_CMP, 
  ///////////////////////
  D_VAR__B_CMP, 
  D_VAR__B_CMP,
  ///////////////////////
  D_CENTRAL__R, 
  D_CENTRAL__G,
  D_CENTRAL__B
};


#endif // NATURAL_TYPES