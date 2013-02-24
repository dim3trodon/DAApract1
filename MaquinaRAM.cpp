/*
 * MaquinaRAM.cpp

 */

#include "MaquinaRAM.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                     Constructor y destructor  //
///////////////////////////////////////////////////////////////////////////////////////////////////
MaquinaRAM::MaquinaRAM(const char* nombreEntrada, const char* nombreSalida, 
		       const char* nombrePrograma) {
  this->nombreSalida = nombreSalida;
  parsearEntrada(nombreEntrada); 
  ifstream ficheroPrograma(nombrePrograma);
  char* linea = new char[CARACTERES_POR_LINEA];
  const string dosPuntos = ":";
  const string espacioDosPuntos = " :";
  const string dosPuntosEspacio = ": ";
  const string puntoComa = ";";
  const string puntoComaEspacio = "; ";
  string lineaString;
  vector<string> palabras;
  inicializarPC();
  inicializarPosEntrada();
  if(ficheroPrograma.fail()) {
    cout << "Fichero del programa no encontrado." << endl;
    halt();
  }
  else {
      while(ficheroPrograma.getline(linea, CARACTERES_POR_LINEA)) {
	lineaString = linea;
	/* Cambiar "etiqueta : " por "etiqueta: " */
	lineaString = replaceAll(lineaString, dosPuntos, dosPuntosEspacio);
	lineaString = replaceAll(lineaString, espacioDosPuntos, dosPuntosEspacio);
	lineaString = eliminarComentarios(lineaString);
	palabras = split(lineaString, CARACTER_DELIMITADOR, 0);
	parsearLinea(palabras);
    }
  }
  etiquetaSinLinea();
  ficheroPrograma.close();
}

MaquinaRAM::~MaquinaRAM() {
  entrada.clear();
  salida.clear();
  for(int i = 0; i < getNumeroLineas(); i++)
    programa.clear();
  registro.clear();
  etiqueta.clear();
  nombreEtiqueta.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                            Ejecutar simulador //
///////////////////////////////////////////////////////////////////////////////////////////////////
void MaquinaRAM::ejecutarPrograma(void) {
  if(getPC() < getNumeroLineas())
    while(getPC() < getNumeroLineas())
      ejecutarInstruccion();
  else
    cout << "Ya no hay más instrucciones que ejecutar." << endl;
}

void MaquinaRAM::ejecutarInstruccion(void) {
  int programCounter = getPC();
  if(programCounter < getNumeroLineas()) {
    if(traza) {
      cout << getPC() << " ";
      imprimirInstruccion(programCounter);
      imprimirTraza(programCounter);
    }
    switch(getInstruccion(programCounter)) {
      case LOAD: load(); break;
      case STORE: store(); break;
      case ADD: add(); break;
      case SUB: sub(); break;
      case MULT: mult(); break;
      case DIV: div(); break;
      case READ: read(); break;
      case WRITE: write(); break;
      case JUMP: jump(); break;
      case JGTZ: jgtz(); break;
      case JZERO: jzero(); break;
      case HALT: halt(); break;
    }
    if(traza)
      imprimirRegistros();
    cout << endl;
    aumentarPC();
  }
  else
    cout << "Ya no hay más instrucciones que ejecutar." << endl;
}

void MaquinaRAM::prueba(void) {
  insertarValorEnRegistro(5, 1);
  insertarValorEnRegistro(2, 2);
  insertarValorEnRegistro(7, 5);
  traza = true;
  cout << endl;
  ejecutarInstruccion();
  cout << endl;
  ejecutarInstruccion();
  cout << endl;
  ejecutarInstruccion();
  cout << endl;
  imprimirRegistros();
  cout << endl;
  imprimirSalida();
}
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                            Instrucciones RAM  //
///////////////////////////////////////////////////////////////////////////////////////////////////
void MaquinaRAM::load(void) {
  int programCounter = getPC();
  switch(getModo(programCounter)) {
    case DIRECTO: 
      insertarValorEnRegistro(getRegistro(getArgumento(programCounter)), 0);
      break;
    case INDIRECTO:
      insertarValorEnRegistro(getRegistro(getRegistro(getArgumento(programCounter))), 0);
      break;
    case INMEDIATO:
      insertarValorEnRegistro(getArgumento(programCounter), 0);
      break;
  }
}

void MaquinaRAM::store(void) {
  int programCounter = getPC();
  switch(getModo(programCounter)) {
    case DIRECTO:
      insertarValorEnRegistro(getRegistro(0), getArgumento(programCounter));
      break;
    case INDIRECTO:
      insertarValorEnRegistro(getRegistro(0), getRegistro(getArgumento(programCounter)));
      break;
  }
}

void MaquinaRAM::add(void) {
  int programCounter = getPC();
  int valor;
  switch(getModo(programCounter)) {
    case DIRECTO:
      valor = getRegistro(0) + getRegistro(getArgumento(programCounter));
      break;
    case INDIRECTO:
      valor = getRegistro(0) + getRegistro(getRegistro(getArgumento(programCounter)));
      break;
    case INMEDIATO:
      valor = getRegistro(0) + getArgumento(programCounter);
      break;
  }
  if(!overFlow(valor))
    insertarValorEnRegistro(valor, 0);
}

void MaquinaRAM::sub(void) {
  int programCounter = getPC();
  int valor;
  switch(getModo(programCounter)) {
    case DIRECTO:
      valor = getRegistro(0) - getRegistro(getArgumento(programCounter));
      break;
    case INDIRECTO:
      valor = getRegistro(0) - getRegistro(getRegistro(getArgumento(programCounter)));
      break;
    case INMEDIATO:
      valor = getRegistro(0) - getArgumento(programCounter);
      break;
  }
  if(!overFlow(valor))
    insertarValorEnRegistro(valor, 0);
}

void MaquinaRAM::mult(void) {
  int programCounter = getPC();
  int valor;
  switch(getModo(programCounter)) {
    case DIRECTO:
      valor = getRegistro(0) * getRegistro(getArgumento(programCounter));
      break;
    case INDIRECTO:
      valor = getRegistro(0) * getRegistro(getRegistro(getArgumento(programCounter)));
      break;
    case INMEDIATO:
      valor = getRegistro(0) * getArgumento(programCounter);
      break;
  }
  if(!overFlow(valor))
    insertarValorEnRegistro(valor, 0);
}

void MaquinaRAM::div(void) {
  int programCounter = getPC();
  int divisor;
  switch(getModo(programCounter)) {
    case DIRECTO:
      divisor = getRegistro(getArgumento(programCounter));
      break;
    case INDIRECTO:
      divisor = getRegistro(getRegistro(getArgumento(programCounter)));
      break;
    case INMEDIATO:
      divisor = getArgumento(programCounter);
      break;
  }
  if(divisor != 0)
      insertarValorEnRegistro(getRegistro(0) / getArgumento(programCounter), 0);
  else {
    cout << "Se ha intentado dividir por 0 en la línea " << getPC() + 1 << endl;
    cout << endl;
    halt();
  }
}

void MaquinaRAM::read(void) {
  int programCounter = getPC();
  switch(getModo(programCounter)) {
    case DIRECTO:
      insertarValorEnRegistro(getEntrada(), getArgumento(programCounter));
      break;
    case INDIRECTO:
      insertarValorEnRegistro(getEntrada(), getRegistro(getArgumento(programCounter)));
      break;
  }
}

void MaquinaRAM::write(void) {
  int programCounter = getPC();
  switch(getModo(programCounter)) {
    case DIRECTO:
      pushSalida(getRegistro(getArgumento(programCounter)));
      break;
    case INDIRECTO:
      pushSalida(getRegistro(getRegistro(getArgumento(programCounter))));
      break;
    case INMEDIATO:
      pushSalida(getArgumento(programCounter));
      break;
  }
}

void MaquinaRAM::jump(void) {
  int programCounter = getPC();
  setPC(getLineaEtiqueta(getArgumento(programCounter)));
  reducirPC();
}

void MaquinaRAM::jgtz(void) {
  int programCounter = getPC();
  if(getRegistro(0) > 0) {
    setPC(getLineaEtiqueta(getArgumento(programCounter)));
    reducirPC();
  }
}

void MaquinaRAM::jzero(void) {
  int programCounter = getPC();
  if(getRegistro(0) == 0) {
    setPC(getLineaEtiqueta(getArgumento(programCounter)));
    reducirPC();
  }
}

void MaquinaRAM::halt(void) {
  escribirSalida();
  cout << "HALT" << endl;
  exit(-1);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                         Debug //
///////////////////////////////////////////////////////////////////////////////////////////////////
void MaquinaRAM::debug(void) {
  char opcion = '0';
  traza = false;
  cout << " ############################### " << endl;
  cout << " # SIMULADOR DE LA MÁQUINA RAM #" << endl;
  cout << " ############################### " << endl;
  cout << endl;
  cout << " d: Ver / no ver traza" << endl;
  cout << " r: Ver los registros" << endl;
  cout << " p: Ver el programa" << endl;
  cout << " t: Ver etiquetas" << endl;
  cout << " g: Ejecutar paso a paso" << endl;
  cout << " f: Ejecutar hasta el final" << endl;
  cout << " s: Desensamblador" << endl;
  cout << " i: Ver cinta entrada" << endl;
  cout << " o: Ver cinta salida" << endl;
  cout << " x: Salir" << endl;
  while(opcion != 'x') {
    cout << endl;
    cout << " > ";
    cin >> opcion;
    cout << endl;
    switch(opcion) {
      case 'd':
	if(traza)
	  traza = false;
	else
	  traza = true;
	break;
      case 'r':
	imprimirRegistros();
	break;
      case 'p':
	imprimirPrograma();
	break;
      case 't':
	imprimirEtiquetas();
	break;
      case 'g':
	ejecutarInstruccion();
	break;
      case 'f':
	ejecutarPrograma();
	break;
      case 's':
	imprimirInstruccion(getPC());
	break;
      case 'i':
	imprimirEntrada();
	break;
      case 'o':
	imprimirSalida();
	break;
    }
    cout << endl;
  }
  halt();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                             Parsear programa  //
///////////////////////////////////////////////////////////////////////////////////////////////////
void MaquinaRAM::parsearPrograma(const char* nombrePrograma) {

}

void MaquinaRAM::parsearLinea(vector<string> &palabras) {
  int codigo;
  bool unaInstruccionParseada = false;
  bool unaEtiquetaParseada = false;
  bool coma = false;
  string instruccion;
  for(unsigned i = 0; (!coma)&&(i < palabras.size()); i++) {
    instruccion = palabras[i];
    codigo = parsearPalabra(palabras[i]);
    /*if(codigo == HALT) {
      pushInstruccion(HALT, 0, 0);
      unaInstruccionParseada = true;
    }*/
    if(codigo >= 0) {
      if(!unaInstruccionParseada) {
	if(i < palabras.size() - 1) {
	  i++;
	  if(tipoInstruccion(codigo) != TIPO_SALTO)
	    parsearArgumentoYModo(palabras[i], codigo);
	  else
	    parsearSalto(palabras[i], codigo);
	  unaInstruccionParseada = true;
	  i++;
	}
	else {
	  if(codigo == HALT)
	    pushInstruccion(HALT, 0, 1);
	  else {
	    cout << "Error: la instrucción " << instruccion << " no recibió argumento." << endl;
	    halt();
	  }
	}
      }
      else {
	cout << "Error, dos instrucciones por línea." << endl;
	halt();
      }
    }
    else if(codigo == CODIGO_ETIQUETA) {
      if((!unaEtiquetaParseada)&&(!unaInstruccionParseada)) {
	palabras[i].erase(palabras[i].size() -1); //Borrar el caracter ':'
	actualizarEtiquetas(palabras[i], getNumeroLineas());
	unaEtiquetaParseada = true;
      }
      else {
	if(unaEtiquetaParseada)
	  cout << "Error: se han parseado dos etiquetas en la misma línea." << endl;
	if(unaInstruccionParseada)
	  cout << "Error: se ha encontrado una etiqueta después de una instruccion." << endl;
	halt();
      }
    }
    else if(codigo == CODIGO_COMA) {
      
      cout << "coma" << endl;
      
      coma = true; //se desecha el resto de las palabras
    }
    else if(codigo == CODIGO_CADENA_VACIA)
      i++;
    else if(codigo == CODIGO_ERROR) {
      cout << "Error: no existe la instrucción \"" << instruccion << "\"" << endl;
      halt();
    }
    else if(codigo == CODIGO_NUMERO) {
      cout << "Error: se esperaba una instrucción y se recibió \"" << instruccion << "\"" << endl;
      halt();
    }
    else {
      cout << "Error desconocido." << endl;
      halt();
    }
  }
  palabras.clear();
}

int MaquinaRAM::parsearPalabra(string &palabra) {
  //Se pone en mayúscula para despreciar diferencias entre mayúscula y minúscula
  string palabraMayus = upcase(palabra);
  if(!palabraMayus.compare("LOAD"))
    return LOAD;
  if(!palabraMayus.compare("STORE"))
    return STORE;
  if(!palabraMayus.compare("ADD"))
    return ADD;
  if(!palabraMayus.compare("SUB"))
    return SUB;
  if(!palabraMayus.compare("MULT"))
    return MULT;
  if(!palabraMayus.compare("DIV"))
    return DIV;
  if(!palabraMayus.compare("READ"))
    return READ;
  if(!palabraMayus.compare("WRITE"))
    return WRITE;
  if(!palabraMayus.compare("JUMP"))
    return JUMP;
  if(!palabraMayus.compare("JGTZ"))
    return JGTZ;
  if(!palabraMayus.compare("JZERO"))
    return JZERO;
  if(!palabraMayus.compare("HALT"))
    return HALT;
  if(palabraMayus.at(0) == ';')
    return CODIGO_COMA;
  if(!palabraMayus.compare(""))
    return CODIGO_CADENA_VACIA;
  if((palabraMayus.size() > 0)&&(palabraMayus.at(palabraMayus.size() -1 ) == ':')) 
    return CODIGO_ETIQUETA;
  /*if(cadenaEsNumero(palabraMayus))
    return CODIGO_NUMERO;*/
  return CODIGO_ERROR;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//        Parsear instrucciones de tipo entrada, salida, manipulación de registros y aritmética  //
///////////////////////////////////////////////////////////////////////////////////////////////////
void MaquinaRAM::parsearArgumentoYModo(string &palabra, int codigo) {
  int modo;
  int argumento;
  modo = parsearModo(palabra);
  argumento = parsearArgumento(palabra);
  if(direccionamientoEsLegal(codigo, modo))
    if(argumentoEsLegal(codigo, argumento, modo))
      pushInstruccion(codigo, argumento, modo);
    else {
      cout << "Error: el argumento de la instrucción " << codigoAChar(codigo) << " ";
      cout << modoAChar(modo) << palabra << " es negativo." << endl;
      halt();
    }
  else {
    cout << "Error: el direccionamiento de la instrucción " << codigoAChar(codigo) << " ";
    cout << modoAChar(modo) << palabra << " es ilegal." << endl;
    halt();
  }
}

int MaquinaRAM::parsearArgumento(string& palabra) {
  if(palabra.size() > 0) {
    if(cadenaEsNumero(palabra))
      return StringToNumber(palabra);
    else {
      
      cout << palabra << endl;
      cout << "aqui" << endl;
      
      cout << "Error: el argumento \"" << palabra << "\" no es un número." << endl;
      halt();
      return -1;
    }
  }
  else
    return 0;
}

int MaquinaRAM::parsearModo(string &palabra) {
  char caracterDeModo;
  if(palabra.size() > 0) {
    caracterDeModo = palabra.at(0);
    if(caracterDeModo == CARACTER_DIRECCIONAMIENTO_INDIRECTO) {
      palabra.erase(palabra.begin());
      return INDIRECTO;
    }
    else if(caracterDeModo == CARACTER_DIRECCIONAMIENTO_INMEDIATO) {
      palabra.erase(palabra.begin());
      return INMEDIATO;
    }
    else if(caracterEsDigito(caracterDeModo))
      return DIRECTO;
    else
      return -1;
  }
  else
    return -1;
}

void MaquinaRAM::parsearSalto(string& palabra, int codigo) {
  int posEtiqueta;
  /* Si no existe la etiqueta, se añade una etiqueta de la que no se sabe su línea */
  if(!existeEtiqueta(palabra))
    actualizarEtiquetas(palabra, LINEA_DESCONOCIDA);
  /* En posEtiqueta se guarda la posición de la etiqueta en el vector */
  posEtiqueta = getPosicionEtiqueta(palabra);
  pushInstruccion(codigo, posEtiqueta, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                               Parsear Entrada //
///////////////////////////////////////////////////////////////////////////////////////////////////
void MaquinaRAM::parsearEntrada(const char* nombreEntrada) {
  ifstream ficheroEntrada;
  ficheroEntrada.open(nombreEntrada);
  int auxiliar;
  if(ficheroEntrada.fail())
    cout << "No se encontró el fichero." << endl;
  else {
    while(!ficheroEntrada.eof()) {
      ficheroEntrada >> auxiliar;
      pushEntrada(auxiliar);
    }
  }
  ficheroEntrada.close();
  //imprimirEntrada();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                      Comprobación de sintaxis //
///////////////////////////////////////////////////////////////////////////////////////////////////
bool MaquinaRAM::direccionamientoEsLegal(int codigo, int modo) {
  int direccionamiento = tipoDireccionamiento(codigo);
  if(direccionamiento == DIRECC_DIRECTO_INDIRECTO) {
    if(modo == INMEDIATO)
      return false;
    else
      return true;
  }
  else
    return true;
}

bool MaquinaRAM::argumentoEsLegal(int codigo, int argumento, int modo) {
  if((argumento < 0)&&(tipoInstruccion(codigo) != TIPO_ARITMETICO)&&(modo != INMEDIATO))
    return false;
  else
    return true;
}

int MaquinaRAM::tipoInstruccion(int codigo) {
  if(codigo == LOAD)
    return TIPO_LOAD;
  else if(codigo == STORE)
    return TIPO_STORE;
  else if((codigo >= ADD) && (codigo <= DIV))
    return TIPO_ARITMETICO;
  else if(codigo == READ)
    return TIPO_READ;
  else if(codigo == WRITE)
    return TIPO_WRITE;
  else if((codigo >= JUMP) && (codigo <= JZERO))
    return TIPO_SALTO;
  else if(codigo == HALT)
    return TIPO_HALT;
  else {
    cout << "Error en el código de la instrucción." << endl;
    halt();
  }
  return -1;
}

int MaquinaRAM::tipoDireccionamiento(int codigo) {
  int tipo = tipoInstruccion(codigo);
  if((tipo == TIPO_STORE)||(tipo == TIPO_READ))
    return DIRECC_DIRECTO_INDIRECTO;
  else if(tipo == TIPO_HALT)
    return SIN_DIRECC;
  else
    return DIRECC_DIRECTO_INDIRECTO_INMEDIATO;
}

bool MaquinaRAM::existeEtiqueta(string etiqueta) {
  for(int i = 0; i < getNumeroEtiquetas(); i++)
    if(!getNombreEtiqueta(i).compare(etiqueta))
      return true;
  return false;
}

void MaquinaRAM::etiquetaSinLinea(void) {
  for(int i = 0; i < getNumeroEtiquetas(); i++) {
    if(getLineaEtiqueta(i) == LINEA_DESCONOCIDA) {
      cout << "Error: alguna instrucción de salto hace referencia a una etiqueta sin línea.";
      cout << endl;
      halt();
    }
  }
}

bool MaquinaRAM::overFlow(int valor) {
  if((valor < MENOR_NEGATIVO)||(valor > MAYOR_POSITIVO)) {
    cout << "Error: overFlow en la línea " << getPC() + 1 << "." << endl;
    cout << endl;
    halt();
    return true;
  }
  else
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                         Métodos para imprimir //
///////////////////////////////////////////////////////////////////////////////////////////////////
void MaquinaRAM::imprimirEntrada(void) {
  for(int i = 0; i < getTamanyoEntrada(); i++)
    cout << "E[" << i << "] =  " << getEntrada(i) << endl;
}

void MaquinaRAM::imprimirSalida(void) {
  for(int i = 0; i < getTamanyoSalida(); i++)
    cout << "S[" << i << "] =  " << getSalida(i) << endl;
}

void MaquinaRAM::imprimirPrograma(void) {
  for(int i = 0; i < getNumeroLineas(); i++) {
    if(i < 10)
	cout << " ";
    if(i == getPC())
      cout << " > ";
    else
      cout << " " << i << " ";
    imprimirInstruccion(i);
  }
}

void MaquinaRAM::imprimirInstruccion(int i) {
  if(i < getNumeroLineas()) {
    int codigo = getInstruccion(i);
    cout << codigoAChar(getInstruccion(i));
    if(codigo != HALT) {
      cout << "     ";
      if((tipoInstruccion(codigo) != TIPO_SALTO))
	cout << modoAChar(getModo(i)) << getArgumento(i);
      else
	cout << " " << getNombreEtiqueta(getArgumento(i));
    }
    for(int j = 0; j < getNumeroEtiquetas(); j++)
      if(getLineaEtiqueta(j) == i)
	cout << "    :"<< getNombreEtiqueta(j);
      cout << endl;
  }
  else 
    imprimirInstruccion(getNumeroLineas() - 1);
}

void MaquinaRAM::imprimirRegistros(void) {
  for(int i = 0; i < getNumeroRegistros(); i++)
    cout << "[" << getRegistro(i) << "]";
  cout << endl;
}

void MaquinaRAM::imprimirEtiquetas(void) {
  for(int i = 0; i < getNumeroEtiquetas(); i++)
    cout << getNombreEtiqueta(i) << ": " << getLineaEtiqueta(i) << endl;
}

void MaquinaRAM::imprimirTraza(int i) {
  int codigo = getInstruccion(i);
  int argumento = getArgumento(i);
  int modo = getModo(i);
  switch(codigo) {
    case LOAD:
      switch(modo) {
	case DIRECTO:
	  cout << "R[" << getArgumento(i) << "] = " << getRegistro(getArgumento(i)) << " -> R[0]";
	  cout << endl;
	  break;
	case INDIRECTO:
	  cout << "R[" << getArgumento(i) << "] = " << getRegistro(getArgumento(i)) << " ->* R[";
	  cout << getRegistro(getArgumento(i)) << "] = " << getRegistro(getRegistro(getArgumento(i)));
	  cout << " -> R[0]" << endl;
	  break;
	case INMEDIATO:
	  cout << getArgumento(i) << " -> R[0]" << endl;
	  break;
      }
      break;
    case STORE:
      switch(modo) {
	case DIRECTO:
	  cout << "R[0] = " << getRegistro(0) << " -> R[" << getArgumento(i) << "]" << endl;
	  break;
	case INDIRECTO:
	  cout << "R[O] = " << getRegistro(0) << " ->* R[" << getArgumento(i) << "] = ";
	  cout << getRegistro(argumento) << " -> R[" << getRegistro(argumento) << "]" << endl;
	  break;
      }
      break;
    case ADD: case SUB: case MULT: case DIV:
      char operando;
      switch(codigo) {
	case ADD: operando = '+'; break;
	case SUB: operando = '-'; break;
	case MULT: operando = '*'; break;
	case DIV: operando = '/'; break;
      }
      cout << "R[0] <- R[0](" << getRegistro(0) << ") " << operando;
      switch(modo) {
	case DIRECTO:
	  cout << " R[" << getArgumento(i) << "](";
	  cout << getRegistro(getArgumento(i)) << ")" << endl;
	  break;
	case INDIRECTO:
	  cout << " R[" << getArgumento(i) << "](";
	  cout << getRegistro(getArgumento(i)) << ") ->* R[";
	  cout << getRegistro(getArgumento(i)) << "](";
	  cout << getRegistro(getRegistro(getArgumento(i))) << ")" << endl;
	  break;
	case INMEDIATO:
	  cout << " " << getArgumento(i) << endl;
	  break;
      }
      break;
    case READ:
      cout << "E[" << getPosEntrada() << "](" << getEntrada(getPosEntrada());
      cout << ") -> R[" << getArgumento(i) << "]";
      if(modo == INDIRECTO) {
	cout << "(" << getRegistro(getArgumento(i)) << ") ->* R[" << getRegistro(getArgumento(i));
	cout << "]";
      }
      cout << endl;
      break;
    case WRITE:
      switch(modo) {
	case DIRECTO:
	  cout << "R[" << getArgumento(i) << "](" << getRegistro(getArgumento(i)) << ")";
	  break;
	case INDIRECTO:
	  cout << "R[" << getArgumento(i) << "](" << getRegistro(getArgumento(i))<< ") ->* ";
	  cout << "R[" << getRegistro(getArgumento(i)) << "](";
	  cout << getRegistro(getRegistro(getArgumento(i))) << ")";
	  break;
	case INMEDIATO:
	  cout << getArgumento(i);
	  break;
      }
      cout << " -> S[" << getTamanyoSalida() << "]" << endl;
      break;
     case JUMP:
       cout << "PC <- " << getNombreEtiqueta(getArgumento(i)) << "(";
       cout << getLineaEtiqueta(getArgumento(i)) << ")" << endl;
       break;
     case JGTZ:
       cout << "R[0](" << getRegistro(0) << ") > 0? = ";
       switch(getRegistro(0) > 0) {
	 case true:
	   cout << "true" << endl;
	   cout << "PC <- " << getNombreEtiqueta(getArgumento(i)) << "(";
	   cout << getLineaEtiqueta(getArgumento(i)) << ")" << endl;
	   break;
	 default: 
	   cout << "false" << endl;
	   break;
       }
       break;
     case JZERO:
       cout << "R[0](" << getRegistro(0) << ") == 0? = ";
       switch(getRegistro(0) == 0) {
	 case true:
	   cout << "true" << endl;
	   cout << "PC <- " << getNombreEtiqueta(getArgumento(i)) << "(";
	   cout << getLineaEtiqueta(getArgumento(i)) << ")" << endl;
	   break;
	 default: 
	   cout << "false" << endl;
	   break;
       }
       break;
  }
}

const char* MaquinaRAM::codigoAChar(int codigo) {
  switch(codigo) {
    case LOAD: return "LOAD "; break;
    case STORE: return "STORE"; break;
    case ADD: return "ADD  "; break;
    case SUB: return "SUB  "; break;
    case MULT: return "MULT "; break;
    case DIV: return "DIV  "; break;
    case READ: return "READ "; break;
    case WRITE: return "WRITE"; break;
    case JUMP: return "JUMP "; break;
    case JGTZ: return "JGTZ "; break;
    case JZERO: return "JZERO"; break;
    case HALT: return "HALT "; break;
    default: return "ERROR"; break;
  }
}

const char MaquinaRAM::modoAChar(int modo) {
  switch(modo) {
    case INMEDIATO: return CARACTER_DIRECCIONAMIENTO_INMEDIATO; break;
    case DIRECTO: return ' '; break;
    case INDIRECTO: return CARACTER_DIRECCIONAMIENTO_INDIRECTO; break;
    default: return '#'; break;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                             Métodos get y set //
///////////////////////////////////////////////////////////////////////////////////////////////////

void MaquinaRAM::pushEntrada(int valor) {
  entrada.push_back(valor);
}

int MaquinaRAM::getTamanyoEntrada(void) {
  return entrada.size();
}

int MaquinaRAM::getEntrada(int i) {
  return entrada[i];
}

int MaquinaRAM::getEntrada(void) {
  if(posEntrada < getTamanyoEntrada()) {
    posEntrada++;
    return entrada[posEntrada - 1];
  }
  else {
    cout << "Aviso: no se ha podido leer en la entrada. Se trabajará con -1." << endl;
    return -1;
  }
}

void MaquinaRAM::inicializarPosEntrada(void) {
  posEntrada = 0;
}

void MaquinaRAM::aumentarPosEntrada(void) {
  posEntrada++;
}

int MaquinaRAM::getPosEntrada(void) {
  return posEntrada;
}

void MaquinaRAM::pushSalida(int valor) {
  salida.push_back(valor);
}
int MaquinaRAM::getTamanyoSalida(void) {
  return salida.size();
}
int MaquinaRAM::getSalida(int i) {
  return salida[i];
}

int MaquinaRAM::getInstruccion(int linea) {
  return programa[linea][INSTRUCCION];
}

int MaquinaRAM::getArgumento(int linea) {
  return programa[linea][ARGUMENTO];
}

int MaquinaRAM::getModo(int linea) {
  return programa[linea][MODO];
}

int MaquinaRAM::getNumeroLineas(void) {
  return programa.size();
}

void MaquinaRAM::pushInstruccion(int codigo, int argumento, int modo) {
  vector<int> linea;
  linea.push_back(codigo);
  linea.push_back(argumento);
  linea.push_back(modo);
  programa.push_back(linea);
}

void MaquinaRAM::setRegistro(int i, int valor) {
  registro[i] = valor;
}

int MaquinaRAM::getRegistro(int i) {
  if(i < getNumeroRegistros())
    return registro[i];
  else
    return 0;
}

int MaquinaRAM::getNumeroRegistros(void) {
  return registro.size();
}

void MaquinaRAM::nuevoRegistro(int valor/* = 0*/) {
  registro.push_back(valor);
}

void MaquinaRAM::insertarValorEnRegistro(int valor, unsigned i) {
  if(i < (getNumeroRegistros()))
    setRegistro(i, valor);
  else {
    unsigned numRegistros = getNumeroRegistros();
    while(numRegistros < i) {
      nuevoRegistro();
      numRegistros++;
    }
    nuevoRegistro(valor);
  }
}

void MaquinaRAM::actualizarEtiquetas(string etiqueta, int numLinea) {
  if(!existeEtiqueta(etiqueta))
    nuevaEtiqueta(etiqueta, numLinea);
  else {
    int posEtiqueta = getPosicionEtiqueta(etiqueta);
    /* Si la línea de la etiqueta es desconocida, se actualiza */
    if(getLineaEtiqueta(posEtiqueta) == LINEA_DESCONOCIDA)
      setLineaEtiqueta(posEtiqueta, numLinea);
    else {
      cout << "Error: hay dos líneas con la misma etiqueta." << endl;
      halt();
    }
  }
}

void MaquinaRAM::nuevaEtiqueta(string nombreEtiqueta_, int numLinea) {
  nombreEtiqueta.push_back(nombreEtiqueta_);
  etiqueta.push_back(numLinea);
}

int MaquinaRAM::getNumeroEtiquetas(void) {
  return etiqueta.size();
}

string MaquinaRAM::getNombreEtiqueta(int i) {
 return nombreEtiqueta[i]; 
}

int MaquinaRAM::getLineaEtiqueta(int i) {
  return etiqueta[i];
}

int MaquinaRAM::getPosicionEtiqueta(string nombreEtiqueta_) {
  for(int i = 0; i < getNumeroEtiquetas(); i++)
    if(!getNombreEtiqueta(i).compare(nombreEtiqueta_))
      return i;
  return -1;
}

void MaquinaRAM::setLineaEtiqueta(int i, int numLinea) {
  etiqueta[i] = numLinea;
}

void MaquinaRAM::inicializarPC(void) {
  PC = 0;
}

void MaquinaRAM::aumentarPC(void) {
  PC++;
}

void MaquinaRAM::reducirPC(void) {
  PC--;
}

void MaquinaRAM::setPC(int valor) {
  PC = valor;
}

int MaquinaRAM::getPC(void) {
  return PC;
}

void MaquinaRAM::escribirSalida(void) {
  ofstream ficheroSalida(nombreSalida.data());
  if(ficheroSalida.fail())
    cout << "Error al escribir en el fichero de salida." << endl;
  else
    for(int i = 0; i < getTamanyoSalida(); i++)
      ficheroSalida << getSalida(i) << endl;
  ficheroSalida.close();
}