
int Arrondi(float nombre_float) { 
	int nombre = int(nombre_float);
	if(nombre_float - nombre  >= 0.5) nombre++; 
	return nombre; 
}