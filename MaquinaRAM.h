/*
 * MaquinaRAM.h
 */

#ifndef MAQUINARAM_H_
#define MAQUINARAM_H_

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <limits>
#include "tratamientocadenas.cpp"

using std::vector;
using std::string;
using std::endl;
using std::cout;
using std::cin;
using std::ifstream;
using std::ofstream;
using std::streamsize;

/* Ficheros */
const char* NOMBRE_ENTRADA = "entrada.ram";
const char* NOMBRE_SALIDA = "salida.ram";
const char* NOMBRE_PROGRAMA = "ejemplo.ram";

/* Límite de caracteres por línea del fichero de programa de entrada */
const unsigned CARACTERES_POR_LINEA = 200000; 

/* Caracteres especiales del programa */
const char CARACTER_DELIMITADOR = ' ';
const char CARACTER_DIRECCIONAMIENTO_INDIRECTO = '*';
const char CARACTER_DIRECCIONAMIENTO_INMEDIATO = '=';

/* Códigos para la función que parsea el programa */
const int CODIGO_COMA = -2;
const int CODIGO_CADENA_VACIA = -4;
const int CODIGO_NUMERO = -1;
const int CODIGO_ETIQUETA = -3;
const int CODIGO_ERROR = -5;

/* Posición en el vector programa */
const int INSTRUCCION = 0;
const int ARGUMENTO = 1;
const int MODO = 2;

/* Tipos de instrucciones según su funcionamiento */
const int TIPO_LOAD = 0; //Direccionamiento directo, indirecto e inmediato
const int TIPO_STORE = 1; //Directo e indirecto
const int TIPO_ARITMETICO = 2; //Direccionamiento directo, indirecto e inmediato
const int TIPO_READ = 3; //Directo e indirecto
const int TIPO_WRITE = 4; //Directo, indirecto e inmediato
const int TIPO_SALTO = 5; //No tiene tipos de direccionamiento
const int TIPO_HALT = 6; //No tiene argumentos ni direccionamiento

/* Tipos de direccionamiento */
const int DIRECC_DIRECTO_INDIRECTO_INMEDIATO = 0;
const int DIRECC_DIRECTO_INDIRECTO = 1;
const int SIN_DIRECC = -1;

/* Etiqueta de la cual no se conoce la línea a la que hace referencia */
const int LINEA_DESCONOCIDA = -1;

/* Constantes usadas para el overflow */
const int MENOR_NEGATIVO = -10000;
const int MAYOR_POSITIVO = 10000;

class MaquinaRAM {
public:
	/* Códigos de instrucción */
	enum { LOAD, STORE, ADD, SUB, MULT, DIV, READ, WRITE, JUMP, JGTZ, JZERO, HALT } codigoInstruccion;

	/* Modos de operación */
	enum { INMEDIATO, DIRECTO, INDIRECTO } modosOperacion;

	MaquinaRAM(const char* nombreEntrada, const char* nombreSalida, const char* nombrePrograma);
	virtual ~MaquinaRAM();

private:
	/* Atributos del programa */
	vector<int> entrada;
	vector<int> salida;
	vector<vector<int> > programa; /*|instrucción|argumento|modo|*/
	vector<int> registro;
	string nombreSalida;
	vector<int> etiqueta; /* Guarda la línea a la que apunta la etiqueta */
	vector<string> nombreEtiqueta; /* vector auxiliar que guarda los nombres de las etiquetas */
	
	int PC;
	int posEntrada; /* Posición del puntero de entrada */
	bool traza;
	
	/* Ejecutar simulador */
	void ejecutarPrograma(void);
	void ejecutarInstruccion(void);
public:
	void prueba(void);
	
private:
	/* Instrucciones RAM */
	void load(void);
	void store(void);
	void add(void);
	void sub(void);
	void mult(void);
	void div(void);
	void read(void);
	void write(void);
	void jump(void);
	void jgtz(void);
	void jzero(void);
	void halt(void);
	
public:
	/* Debug */
	void debug(void);
	
private:
	/* Parsear programa */
	void parsearPrograma(const char* nombrePrograma);
	void parsearLinea(vector<string>&);
	int parsearPalabra(string&); /* Devuelve el código de la palabra. Si es un entero devuelve 0 */
	
	/* Parsear instrucciones de tipo entrada, salida, manipulación de registros y aritmética */
	void parsearArgumentoYModo(string& palabra, int codigo);
	int parsearArgumento(string& palabra);
	int parsearModo(string& palabra);
	
	/* Parsear etiquetas */
	void parsearSalto(string& palabra, int codigo);
	
	/* Parsear Entrada */
	void parsearEntrada(const char* nombreEntrada);
	
	/* Comprobación de errores */
	bool direccionamientoEsLegal(int codigo, int modo);
	bool argumentoEsLegal(int codigo, int argumento, int modo);
	//Se dividen las instrucciones RAM en varios tipos según su función
	inline int tipoInstruccion(int codigo);
	//Devuelve el tipo de direccionamiento de la instruccion codigo
	inline int tipoDireccionamiento(int codigo);
	bool existeEtiqueta(string);
	void etiquetaSinLinea(void);
	bool overFlow(int);
	
	/* Métodos para imprimir */
	void imprimirEntrada(void);
	//void imprimirLineaPrograma(const vector<string>& palabras);
	void imprimirSalida(void);
	void imprimirPrograma(void);
	void imprimirInstruccion(int);
	void imprimirRegistros(void);
	void imprimirEtiquetas(void);
	void imprimirTraza(int);
	const char* codigoAChar(int);
	const char modoAChar(int);
	
	/* Métodos get y set */
	inline void pushEntrada(int);
	inline int getTamanyoEntrada(void);
	inline int getEntrada(int);
	inline int getEntrada(void);
	inline void inicializarPosEntrada(void);
	inline void aumentarPosEntrada(void);
	inline int getPosEntrada(void);
	
	inline void pushSalida(int);
	inline int getTamanyoSalida(void);
	inline int getSalida(int);
	
	inline int getInstruccion(int); //devuelve argumento almacenado en el vector programa
	inline int getArgumento(int);
	inline int getModo(int);
	inline int getNumeroLineas(void);
	inline void pushInstruccion(int, int, int);
	
	inline void setRegistro(int, int);
	inline int getRegistro(int);
	inline int getNumeroRegistros(void);
	inline void nuevoRegistro(int = 0);
	inline void insertarValorEnRegistro(int valor, unsigned registro); /* Usar este */
	
	inline void actualizarEtiquetas(string, int);
	inline void nuevaEtiqueta(string, int);
	inline int getNumeroEtiquetas();
	inline string getNombreEtiqueta(int);
	inline int getLineaEtiqueta(int);
	inline int getPosicionEtiqueta(string);
	inline void setLineaEtiqueta(int, int);
	
	inline void inicializarPC(void);
	inline void aumentarPC(void);
	inline void reducirPC(void);
	inline void setPC(int);
	inline int getPC(void);
	
	/* Escribir salida en fichero */
	void escribirSalida(void);
};

#endif /* MAQUINARAM_H_ */
