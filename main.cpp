/*
 * main.cpp

 */

#include "MaquinaRAM.cpp"

using namespace std;

int main()
{

  
	MaquinaRAM maquina(NOMBRE_ENTRADA, NOMBRE_SALIDA, NOMBRE_PROGRAMA);
	maquina.debug();

	return 0;
}


